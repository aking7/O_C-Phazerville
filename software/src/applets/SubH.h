// Copyright (c) 2024, The Phazerville Project
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "../SubHLinker.h"

class SubH : public HemisphereApplet {
public:

    enum SubHCursor {
        SCALE,
        ROOT,
        CHORD,
        SEVENTH,
        SUS,
        INVERSION,
        HOME,
        SECDOM,
        DRIFT,
        HOLD,
        OCTAVE,
        SLEW,
        CLOCK_DIV,
        STAY,
        DEJAVU,
        BRANCH,
        LOOP_LEN,
        LOOP_LOCK,
        LAST_SETTING = LOOP_LOCK
    };

    // The Subharmonicon's SUB CV inputs read -5V..+5V as divisor 16 down
    // to 1 in 16 equal zones (see the MIDI CC table in the manual), with
    // the SUB FREQ knob centered. One zone = 10V/16 = 0.625V.
    static const int SUBH_ZONE = 960;     // 0.625V (1V = 1536)
    static const int SUBH_5V   = 5 * ONE_OCTAVE;

    const char* applet_name() {
        return "SubH";
    }
    const uint8_t* applet_icon() { return PhzIcons::chordinate; }

    void Start() {
        cursor = 0;
        top_row = 0;
        linker.Register(hemisphere);

        if (!linker.IsLinked()) {
            linker.scale = GetScale(0);
            linker.root_note = GetRootNote(0);
            linker.stay_prob = 80;
            linker.dejavu = 0;
            linker.branch_prob = 50;
            linker.home = 50;
            linker.seventh = true;
            linker.octave = 0;
            linker.inversion = SubHLinker::INV_AUTO;
            linker.use_sec_dom = false;
            linker.drift = 0;
            linker.sus_mode = 0;
            linker.hold = 0;
            linker.clock_div = 1;
        }

        for(int i=0; i<2; ++i) current_cv[i] = 0;
        slew_amount = 0;
        last_chord = -1;
        last_inv = -1;
        last_scale = -1;
        last_root = -1;
        last_secd = -1;
    }

    void Unload() {
        linker.Unload(hemisphere);
    }

    void Controller() {
        linker.Register(hemisphere);
        const bool linked = linker.IsLinked();

        int chord_cv = 0, inv_cv = 0;
        if (linked && hemisphere == 1) {
            // Performance CVs on the linked right side: CV1 transposes the
            // whole chord (1V/oct, semitone-quantized), CV2 modulates Home
            linker.cv_transpose = (DetentedIn(0) / 128) * 128;
            linker.cv_home = Proportion(DetentedIn(1), HEMISPHERE_MAX_INPUT_CV, 100);
        } else {
            // CV Modulation for Chord and Inversion
            chord_cv = Proportion(DetentedIn(0), HEMISPHERE_MAX_INPUT_CV, 7);
            inv_cv = Proportion(DetentedIn(1), HEMISPHERE_MAX_INPUT_CV, 4);
            if (!linked) {
                linker.cv_transpose = 0;
                linker.cv_home = 0;
            }
        }

        if (Clock(0)) {
            // Only the "primary" registered side advances the shared
            // sequencer, every clock_div clocks
            if (!linker.registered[0] || hemisphere == 0) {
                if (++linker.clock_count >= linker.clock_div) {
                    linker.clock_count = 0;
                    linker.AdvanceSequencer();
                }
            }
        }

        if (Clock(1)) {
            // Reset the loop and cadence home to the I chord
            linker.seq_step = 0;
            linker.chord_index = 0;
            linker.sec_dom = false;
            linker.clock_count = 0;
        }

        int current_chord = (linker.chord_index + chord_cv + 7) % 7;
        int current_inv = (linker.inversion == SubHLinker::INV_AUTO)
            ? SubHLinker::INV_AUTO
            : (linker.inversion + inv_cv + 4) % 4;

        // Efficiency: Only update divisions if state has changed
        if (current_chord != last_chord || current_inv != last_inv ||
            (int8_t)linker.sec_dom != last_secd ||
            linker.scale != last_scale || linker.root_note != last_root) {
            linker.UpdateSubharmonics(current_chord, linker.sec_dom, current_inv);
            last_chord = current_chord;
            last_inv = current_inv;
            last_secd = (int8_t)linker.sec_dom;
            last_scale = linker.scale;
            last_root = linker.root_note;
        }

        // Each hemisphere drives one VCO (left = VCO 1, right = VCO 2):
        //   Output A -> VCO input:     the fundamental pitch (1V/oct)
        //   Output B -> VCO SUB input: the undertone divisor as a zone CV
        // A solo instance drives VCO 1, the pair that owns the bass.
        int vco = linker.IsLinked() ? hemisphere : 0;

        // Divisor -> CV: center of the zone for this divisor. Divisor 1 sits
        // at +4.69V, 8 at +0.31V, 16 at -4.69V. Note: hardware with a -3V
        // floor can't reach divisors above 13 (they fold onto ~13).
        int div = linker.divisions[vco];
        int div_target = (16 - div) * SUBH_ZONE + (SUBH_ZONE / 2) - SUBH_5V;
        div_target = constrain(div_target, HEMISPHERE_MIN_CV, HEMISPHERE_MAX_CV);

        int targets[2];
        // Oct and the transpose CV shift the whole chord: both VCO pitches
        // move, and the subs follow since each divisor is an interval below
        // its own VCO.
        targets[0] = constrain(linker.vco_pitch[vco] + linker.octave * ONE_OCTAVE
                                   + linker.cv_transpose,
                               HEMISPHERE_MIN_CV, HEMISPHERE_MAX_CV);
        targets[1] = div_target;              // undertone divisor

        ForEachChannel(ch) {
            if (slew_amount == 0) {
                current_cv[ch] = targets[ch];
            } else {
                // Smoothly morph/glide to target
                int32_t s = slew_amount;
                current_cv[ch] = (current_cv[ch] * (256 - s) + targets[ch] * s) >> 8;
            }
            Out(ch, current_cv[ch]);
        }
    }

    void View() {
        DrawInterface();
    }

    void OnButtonPress() {
        if (cursor == LOOP_LOCK) {
            linker.is_locked = !linker.is_locked;
        } else if (cursor == SEVENTH) {
            linker.seventh = !linker.seventh;
            last_chord = -1; // re-voice the chord
        } else if (cursor == SECDOM) {
            linker.use_sec_dom = !linker.use_sec_dom;
        } else {
            CursorToggle();
        }
    }

    void OnEncoderMove(int direction) {
        if (!EditMode()) {
            MoveCursor(cursor, direction, LAST_SETTING);
            return;
        }

        switch (cursor) {
        case SCALE:
            linker.scale = constrain(linker.scale + direction, 0, OC::Scales::NUM_SCALES - 1);
            break;
        case ROOT:
            linker.root_note = (linker.root_note + direction + 12) % 12;
            break;
        case CHORD:
            linker.chord_index = (linker.chord_index + direction + 7) % 7;
            break;
        case INVERSION:
            // 0-3, plus Auto (nearest-voicing voice-leading)
            linker.inversion = (linker.inversion + direction + 5) % 5;
            last_chord = -1; // re-voice
            break;
        case SUS:
            linker.sus_mode = constrain(linker.sus_mode + direction, 0, 3);
            break;
        case HOME:
            linker.home = constrain(linker.home + direction, 0, 100);
            break;
        case DRIFT:
            linker.drift = constrain(linker.drift + direction, 0, 100);
            break;
        case HOLD:
            linker.hold = constrain(linker.hold + direction, 0, 100);
            break;
        case OCTAVE:
            linker.octave = constrain(linker.octave + direction, -2, 2);
            break;
        case CLOCK_DIV:
            linker.clock_div = constrain(linker.clock_div + direction, 1, 8);
            break;
        case STAY:
            linker.stay_prob = constrain(linker.stay_prob + direction, 0, 100);
            break;
        case DEJAVU:
            linker.dejavu = constrain(linker.dejavu + direction, 0, 100);
            break;
        case BRANCH:
            linker.branch_prob = constrain(linker.branch_prob + direction, 0, 100);
            break;
        case LOOP_LEN:
            linker.loop_length = constrain(linker.loop_length + direction, 1, 32);
            break;
        case SLEW:
            slew_amount = constrain(slew_amount + direction, 0, 255);
            break;
        default:
            break;
        }
    }

    // 64 of 64 bits used. Stay/Home store in 4% steps; DjVu/Brch/Drft/Hold
    // in 7% steps; Slew in 16 steps. Booleans are stored inverted and
    // octave in two's complement, so all-zero data decodes to the defaults.
    uint64_t OnDataRequest() {
        uint64_t data = 0;
        Pack(data, PackLocation{0, 8}, linker.scale);
        Pack(data, PackLocation{8, 4}, (uint8_t)linker.root_note);
        Pack(data, PackLocation{12, 3}, (uint8_t)linker.chord_index);
        Pack(data, PackLocation{15, 3}, (uint8_t)linker.inversion);
        Pack(data, PackLocation{18, 5}, (uint8_t)(linker.stay_prob / 4));
        Pack(data, PackLocation{23, 5}, (uint8_t)(linker.home / 4));
        Pack(data, PackLocation{28, 4}, (uint8_t)(linker.dejavu / 7));
        Pack(data, PackLocation{32, 4}, (uint8_t)(linker.branch_prob / 7));
        Pack(data, PackLocation{36, 4}, (uint8_t)(linker.drift / 7));
        Pack(data, PackLocation{40, 4}, (uint8_t)(linker.hold / 7));
        Pack(data, PackLocation{44, 5}, (uint8_t)(linker.loop_length - 1));
        Pack(data, PackLocation{49, 1}, linker.is_locked);
        Pack(data, PackLocation{50, 4}, (uint8_t)(slew_amount / 16));
        Pack(data, PackLocation{54, 1}, !linker.seventh);
        Pack(data, PackLocation{55, 3}, (uint8_t)(linker.octave & 0x07));
        Pack(data, PackLocation{58, 1}, linker.use_sec_dom);
        Pack(data, PackLocation{59, 3}, (uint8_t)(linker.clock_div - 1));
        Pack(data, PackLocation{62, 2}, linker.sus_mode);
        return data;
    }

    void OnDataReceive(uint64_t data) {
        linker.scale = Unpack(data, PackLocation{0, 8});
        linker.root_note = Unpack(data, PackLocation{8, 4});
        linker.chord_index = Unpack(data, PackLocation{12, 3});
        linker.inversion = constrain(Unpack(data, PackLocation{15, 3}), 0, SubHLinker::INV_AUTO);
        linker.stay_prob = Unpack(data, PackLocation{18, 5}) * 4;
        linker.home = Unpack(data, PackLocation{23, 5}) * 4;
        linker.dejavu = constrain(Unpack(data, PackLocation{28, 4}) * 7, 0, 100);
        linker.branch_prob = constrain(Unpack(data, PackLocation{32, 4}) * 7, 0, 100);
        linker.drift = constrain(Unpack(data, PackLocation{36, 4}) * 7, 0, 100);
        linker.hold = constrain(Unpack(data, PackLocation{40, 4}) * 7, 0, 100);
        linker.loop_length = Unpack(data, PackLocation{44, 5}) + 1;
        linker.is_locked = Unpack(data, PackLocation{49, 1});
        slew_amount = Unpack(data, PackLocation{50, 4}) * 16;
        linker.seventh = !Unpack(data, PackLocation{54, 1});
        int oct = Unpack(data, PackLocation{55, 3});
        linker.octave = (oct <= 3) ? oct : oct - 8;
        linker.use_sec_dom = Unpack(data, PackLocation{58, 1});
        linker.clock_div = Unpack(data, PackLocation{59, 3}) + 1;
        linker.sus_mode = Unpack(data, PackLocation{62, 2});
    }

protected:
    void SetHelp() {
        help[HELP_DIGITAL1] = "Clock";
        help[HELP_DIGITAL2] = "Reset";
        help[HELP_CV1]      = "Chord";
        help[HELP_CV2]      = "Invers";
        help[HELP_OUT1]     = "VCO pitch";
        help[HELP_OUT2]     = "VCO SUB";
        help[HELP_EXTRA1]   = "L=VCO1 R=VCO2";
        help[HELP_EXTRA2]   = "Home: key gravity";
    }

private:
    SubHLinker &linker = SubHLinker::get();
    int8_t cursor;
    int8_t top_row;
    uint8_t slew_amount;
    int current_cv[2];

    int8_t last_chord, last_inv, last_scale, last_root, last_secd;

    static const int NUM_ROWS = 16;

    // Which display row each cursor stop lives on
    int CursorRow() {
        switch (cursor) {
        case SCALE: case ROOT: return 0;
        case CHORD:     return 1;
        case SEVENTH:   return 2;
        case SUS:       return 3;
        case INVERSION: return 4;
        case HOME:      return 5;
        case SECDOM:    return 6;
        case DRIFT:     return 7;
        case HOLD:      return 8;
        case OCTAVE:    return 9;
        case SLEW:      return 10;
        case CLOCK_DIV: return 11;
        case STAY:      return 12;
        case DEJAVU:    return 13;
        case BRANCH:    return 14;
        default:        return 15; // LOOP_LEN, LOOP_LOCK
        }
    }

    void DrawRow(int r, int y) {
        switch (r) {
        case 0:
            // Short scale names are 4 chars (24px), root note up to 2 chars
            gfxPrint(1, y, OC::scale_names_short[linker.scale]);
            if (cursor == SCALE) gfxCursor(1, y+8, 24);
            gfxPrint(34, y, OC::Strings::note_names_unpadded[linker.root_note]);
            if (cursor == ROOT) gfxCursor(34, y+8, 12);
            break;
        case 1: {
            const char* chord_names[] = {"I", "ii", "iii", "IV", "V", "vi", "vii"};
            gfxPrint(1, y, "Chrd:");
            gfxPrint(chord_names[linker.chord_index]);
            if (cursor == CHORD) gfxCursor(31, y+8, 18);
            break;
        }
        case 2:
            gfxPrint(1, y, "7th:");
            gfxPrint(linker.seventh ? "On" : "Off");
            if (cursor == SEVENTH) gfxCursor(25, y+8, 18);
            break;
        case 3: {
            const char* sus_names[] = {"Off", "2", "4", "Rnd"};
            gfxPrint(1, y, "Sus:");
            gfxPrint(sus_names[linker.sus_mode]);
            if (cursor == SUS) gfxCursor(25, y+8, 18);
            break;
        }
        case 4:
            gfxPrint(1, y, "Inv:");
            if (linker.inversion == SubHLinker::INV_AUTO) gfxPrint("Auto");
            else gfxPrint(linker.inversion);
            if (cursor == INVERSION) gfxCursor(25, y+8, 24);
            break;
        case 5:
            gfxPrint(1, y, "Home:");
            gfxPrint(linker.home);
            if (cursor == HOME) gfxCursor(31, y+8, 18);
            break;
        case 6:
            gfxPrint(1, y, "SecD:");
            gfxPrint(linker.use_sec_dom ? "On" : "Off");
            if (cursor == SECDOM) gfxCursor(31, y+8, 18);
            break;
        case 7:
            gfxPrint(1, y, "Drft:");
            gfxPrint(linker.drift);
            if (cursor == DRIFT) gfxCursor(31, y+8, 18);
            break;
        case 8:
            gfxPrint(1, y, "Hold:");
            gfxPrint(linker.hold);
            if (cursor == HOLD) gfxCursor(31, y+8, 18);
            break;
        case 9:
            gfxPrint(1, y, "Oct:");
            gfxPrint(linker.octave);
            if (cursor == OCTAVE) gfxCursor(25, y+8, 12);
            break;
        case 10:
            gfxPrint(1, y, "Slew:");
            gfxPrint(slew_amount);
            if (cursor == SLEW) gfxCursor(31, y+8, 18);
            break;
        case 11:
            gfxPrint(1, y, "Div:");
            gfxPrint(linker.clock_div);
            if (cursor == CLOCK_DIV) gfxCursor(25, y+8, 7);
            break;
        case 12:
            gfxPrint(1, y, "Stay:");
            gfxPrint(linker.stay_prob);
            if (cursor == STAY) gfxCursor(31, y+8, 18);
            break;
        case 13:
            gfxPrint(1, y, "DjVu:");
            gfxPrint(linker.dejavu);
            if (cursor == DEJAVU) gfxCursor(31, y+8, 18);
            break;
        case 14:
            gfxPrint(1, y, "Brch:");
            gfxPrint(linker.branch_prob);
            if (cursor == BRANCH) gfxCursor(31, y+8, 18);
            break;
        case 15:
            gfxPrint(1, y, "Len:");
            gfxPrint(linker.loop_length);
            if (cursor == LOOP_LEN) gfxCursor(25, y+8, 12);
            gfxPrint(40, y, linker.is_locked ? "LOCK" : "EVOL");
            if (cursor == LOOP_LOCK) gfxCursor(40, y+8, 24);
            break;
        }
    }

    void DrawInterface() {
        gfxHeader("SubH");

        // Current chord + link state in the header corner opposite the
        // title (the title right-aligns on the right hemisphere). A
        // secondary dominant shows as e.g. "V/ii".
        const char* chord_names[] = {"I", "ii", "iii", "IV", "V", "vi", "vii"};
        const int hx = (hemisphere & 1) ? 1 : 28;
        gfxPrint(hx, 2, linker.sec_dom ? "V/" : "");
        gfxPrint(chord_names[linker.chord_index]);
        if (linker.IsLinked()) {
            int ix = hx + (linker.sec_dom ? 12 : 0)
                        + 6 * strlen(chord_names[linker.chord_index]) + 3;
            gfxIcon(ix > 55 ? 55 : ix, 2, LINK_ICON);
        }

        // Scrolling settings list: 4 rows visible, cursor stays in view
        const int row = CursorRow();
        if (row < top_row) top_row = row;
        if (row > top_row + 3) top_row = row - 3;
        for(int i = 0; i < 4; ++i) DrawRow(top_row + i, 15 + i*10);

        // Scroll position indicator on the right edge
        const int bar_h = 40 * 4 / NUM_ROWS;
        gfxLine(63, 15 + top_row * 40 / NUM_ROWS,
                63, 15 + top_row * 40 / NUM_ROWS + bar_h);

        // The four sounding chord tones, low to high. The bass (leftmost) is
        // always made by a suboscillator; the key's root note is underlined
        // wherever it appears in the chord.
        for(int i = 0; i < 4; ++i) {
            const int pc = ((linker.sounding[i] % 12) + 12) % 12;
            const int x = 1 + i*16;
            const char *name = OC::Strings::note_names_unpadded[pc];
            gfxPrint(x, 55, name);
            if (pc == linker.root_note) gfxLine(x, 63, x + (name[1] ? 11 : 5), 63);
        }
    }
};
