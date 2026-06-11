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
        INVERSION,
        SLEW,
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
        page = 0;
        linker.Register(hemisphere);

        if (!linker.IsLinked()) {
            linker.scale = GetScale(0);
            linker.root_note = GetRootNote(0);
            linker.stay_prob = 80;
            linker.dejavu = 0;
            linker.branch_prob = 50;
        }

        for(int i=0; i<2; ++i) current_cv[i] = 0;
        slew_amount = 0;
        last_chord = -1;
        last_inv = -1;
        last_scale = -1;
        last_root = -1;
    }

    void Unload() {
        linker.Unload(hemisphere);
    }

    void Controller() {
        linker.Register(hemisphere);

        // CV Modulation for Chord and Inversion
        int chord_cv = Proportion(DetentedIn(0), HEMISPHERE_MAX_INPUT_CV, 7);
        int inv_cv = Proportion(DetentedIn(1), HEMISPHERE_MAX_INPUT_CV, 4);

        if (Clock(0)) {
            // Only the "primary" registered side advances the shared sequencer
            if (!linker.registered[0] || hemisphere == 0) {
                linker.AdvanceSequencer();
            }
        }

        if (Clock(1)) {
            linker.seq_step = 0;
        }

        int current_chord = (linker.chord_index + chord_cv + 7) % 7;
        int current_inv = (linker.inversion + inv_cv + 4) % 4;

        // Efficiency: Only update divisions if state has changed
        if (current_chord != last_chord || current_inv != last_inv ||
            linker.scale != last_scale || linker.root_note != last_root) {
            linker.UpdateSubharmonics(current_chord, current_inv);
            last_chord = current_chord;
            last_inv = current_inv;
            last_scale = linker.scale;
            last_root = linker.root_note;
        }

        ForEachChannel(ch) {
            // Target the center of the CV zone for this divisor. Divisor 1
            // sits at +4.69V, 8 at +0.31V, 16 at -4.69V. Note: hardware with
            // a -3V floor clamps divisors above 13.
            int div = linker.divisions[ch + (hemisphere * 2)];
            int target = (16 - div) * SUBH_ZONE + (SUBH_ZONE / 2) - SUBH_5V;
            target = constrain(target, HEMISPHERE_MIN_CV, HEMISPHERE_MAX_CV);
            if (slew_amount == 0) {
                current_cv[ch] = target;
            } else {
                // Smoothly morph to target
                int32_t s = slew_amount;
                current_cv[ch] = (current_cv[ch] * (256 - s) + target * s) >> 8;
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
        } else {
            CursorToggle();
        }
    }

    void OnEncoderMove(int direction) {
        if (!EditMode()) {
            MoveCursor(cursor, direction, LAST_SETTING);
            page = (cursor >= STAY) ? 1 : 0;
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
            linker.inversion = (linker.inversion + direction + 4) % 4;
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

    uint64_t OnDataRequest() {
        uint64_t data = 0;
        Pack(data, PackLocation{0, 8}, linker.scale);
        Pack(data, PackLocation{8, 4}, (uint8_t)linker.root_note);
        Pack(data, PackLocation{12, 3}, (uint8_t)linker.chord_index);
        Pack(data, PackLocation{15, 2}, (uint8_t)linker.inversion);
        Pack(data, PackLocation{17, 7}, (uint8_t)linker.stay_prob);
        Pack(data, PackLocation{24, 5}, (uint8_t)(linker.loop_length - 1));
        Pack(data, PackLocation{29, 1}, linker.is_locked);
        Pack(data, PackLocation{30, 8}, (uint8_t)slew_amount);
        Pack(data, PackLocation{38, 7}, (uint8_t)linker.dejavu);
        Pack(data, PackLocation{45, 7}, (uint8_t)linker.branch_prob);
        return data;
    }

    void OnDataReceive(uint64_t data) {
        linker.scale = Unpack(data, PackLocation{0, 8});
        linker.root_note = Unpack(data, PackLocation{8, 4});
        linker.chord_index = Unpack(data, PackLocation{12, 3});
        linker.inversion = Unpack(data, PackLocation{15, 2});
        linker.stay_prob = Unpack(data, PackLocation{17, 7});
        linker.loop_length = Unpack(data, PackLocation{24, 5}) + 1;
        linker.is_locked = Unpack(data, PackLocation{29, 1});
        slew_amount = Unpack(data, PackLocation{30, 8});
        linker.dejavu = Unpack(data, PackLocation{38, 7});
        linker.branch_prob = Unpack(data, PackLocation{45, 7});
    }

protected:
    void SetHelp() {
        help[HELP_DIGITAL1] = "Clock";
        help[HELP_DIGITAL2] = "Reset";
        help[HELP_CV1]      = "Chord";
        help[HELP_CV2]      = "Invers";
        help[HELP_OUT1]     = "SUB x_1";
        help[HELP_OUT2]     = "SUB x_2";
        help[HELP_EXTRA1]   = "SubH Pairing App";
        help[HELP_EXTRA2]   = "Linked Modes";
    }

private:
    SubHLinker &linker = SubHLinker::get();
    int8_t cursor;
    uint8_t page;
    uint8_t slew_amount;
    int current_cv[2];

    int8_t last_chord, last_inv, last_scale, last_root;

    void DrawInterface() {
        gfxHeader(page == 0 ? "SubH Harm" : "SubH Rhy");

        if (page == 0) {
            // Short scale names are 4 chars (24px), root note up to 2 chars
            gfxPrint(1, 15, OC::scale_names_short[linker.scale]);
            if (cursor == SCALE) gfxCursor(1, 23, 24);

            gfxPrint(34, 15, OC::Strings::note_names_unpadded[linker.root_note]);
            if (cursor == ROOT) gfxCursor(34, 23, 12);

            gfxPrint(1, 25, "Chrd:");
            const char* chord_names[] = {"I", "ii", "iii", "IV", "V", "vi", "vii"};
            gfxPrint(chord_names[linker.chord_index]);
            if (cursor == CHORD) gfxCursor(31, 33, 18);

            gfxPrint(1, 35, "Inv:");
            gfxPrint(linker.inversion);
            if (cursor == INVERSION) gfxCursor(25, 43, 7);

            gfxPrint(1, 45, "Slew:");
            gfxPrint(slew_amount);
            if (cursor == SLEW) gfxCursor(31, 53, 18);
        } else {
            gfxPrint(1, 15, "Stay:");
            gfxPrint(linker.stay_prob);
            if (cursor == STAY) gfxCursor(31, 23, 18);

            gfxPrint(1, 25, "DjVu:");
            gfxPrint(linker.dejavu);
            if (cursor == DEJAVU) gfxCursor(31, 33, 18);

            gfxPrint(1, 35, "Brch:");
            gfxPrint(linker.branch_prob);
            if (cursor == BRANCH) gfxCursor(31, 43, 18);

            gfxPrint(1, 45, "Len:");
            gfxPrint(linker.loop_length);
            if (cursor == LOOP_LEN) gfxCursor(25, 53, 12);

            gfxPrint(40, 45, linker.is_locked ? "LOCK" : "EVOL");
            if (cursor == LOOP_LOCK) gfxCursor(40, 53, 24);
        }

        // This hemisphere's two divisors, e.g. "4/16"
        gfxPrint(1, 55, linker.divisions[0 + hemisphere*2]);
        gfxPrint("/");
        gfxPrint(linker.divisions[1 + hemisphere*2]);

        if (linker.IsLinked()) {
            gfxIcon(55, 55, LINK_ICON);
        }
    }
};
