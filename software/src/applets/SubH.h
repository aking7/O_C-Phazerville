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
        PAGE,
        SEQ_PROB,
        LOOP_LEN,
        LOOP_LOCK,
        SLEW,
        LAST_SETTING = SLEW
    };

    const char* applet_name() {
        return "SubH";
    }
    const uint8_t* applet_icon() { return PhzIcons::chordinate; }

    void Start() {
        page = 0;
        linker.Register(hemisphere);

        if (!linker.IsLinked()) {
            linker.scale = GetScale(0);
            linker.root_note = GetRootNote(0);
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
            if (!linker.registered[0] || hemisphere == 0) {
                AdvanceSequencer();
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
            UpdateSubharmonics(current_chord, current_inv);
            last_chord = current_chord;
            last_inv = current_inv;
            last_scale = linker.scale;
            last_root = linker.root_note;
        }

        ForEachChannel(ch) {
            int target = (linker.divisions[ch + (hemisphere * 2)] - 1) * 512;
            if (slew_amount == 0) {
                current_cv[ch] = target;
            } else {
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
        if (cursor == PAGE) {
            page = 1 - page;
        } else if (cursor == LOOP_LOCK) {
            linker.is_locked = !linker.is_locked;
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
            linker.inversion = (linker.inversion + direction + 4) % 4;
            break;
        case SEQ_PROB:
            prob_stay = constrain(prob_stay + direction, 0, 100);
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
        Pack(data, PackLocation{8, 4}, linker.root_note);
        Pack(data, PackLocation{12, 3}, linker.chord_index);
        Pack(data, PackLocation{15, 2}, linker.inversion);
        Pack(data, PackLocation{17, 7}, prob_stay);
        Pack(data, PackLocation{24, 5}, linker.loop_length - 1);
        Pack(data, PackLocation{29, 1}, linker.is_locked);
        Pack(data, PackLocation{30, 8}, slew_amount);
        return data;
    }

    void OnDataReceive(uint64_t data) {
        linker.scale = Unpack(data, PackLocation{0, 8});
        linker.root_note = Unpack(data, PackLocation{8, 4});
        linker.chord_index = Unpack(data, PackLocation{12, 3});
        linker.inversion = Unpack(data, PackLocation{15, 2});
        prob_stay = Unpack(data, PackLocation{17, 7});
        linker.loop_length = Unpack(data, PackLocation{24, 5}) + 1;
        linker.is_locked = Unpack(data, PackLocation{29, 1});
        slew_amount = Unpack(data, PackLocation{30, 8});
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
    int cursor;
    int page;
    int prob_stay = 80;
    int slew_amount;
    int current_cv[2];

    // State tracking for efficiency
    int last_chord, last_inv, last_scale, last_root;

    const int16_t sub_semis[17] = {
        0, 0, -1536, -2435, -3072, -3566, -3970, -4312, -4608, -4869, -5102, -5314, -5506, -5684, -5848, -6001, -6144
    };

    void AdvanceSequencer() {
        if (linker.is_locked) {
            linker.seq_step = (linker.seq_step + 1) % linker.loop_length;
            linker.chord_index = linker.seq_history[linker.seq_step];
        } else {
            int rnd = random(100);
            if (rnd < prob_stay) {
                // Stay
            } else if (rnd < prob_stay + (100-prob_stay)*0.6) {
                linker.chord_index = (linker.chord_index + 1) % 7;
            } else {
                linker.chord_index = random(7);
            }
            linker.seq_step = (linker.seq_step + 1) % 32;
            linker.seq_history[linker.seq_step % linker.loop_length] = linker.chord_index;
        }
    }

    void UpdateSubharmonics(int chord_idx, int inv) {
        const OC::Scale &scale = OC::Scales::GetScale(linker.scale);

        int chord_notes[4];
        chord_notes[0] = 0;
        chord_notes[1] = 2;
        chord_notes[2] = 4;
        chord_notes[3] = 6;

        for(int i=0; i<4; ++i) chord_notes[i] = (chord_notes[i] + chord_idx) % scale.num_notes;

        for(int i=0; i<inv; ++i) {
            int first = chord_notes[0];
            chord_notes[0] = chord_notes[1];
            chord_notes[1] = chord_notes[2];
            chord_notes[2] = chord_notes[3];
            chord_notes[3] = first;
        }

        for(int i=0; i<4; ++i) {
            int target_rel_semi = scale.notes[chord_notes[i]];
            linker.divisions[i] = FindClosestSubharmonic(target_rel_semi);
        }
    }

    int FindClosestSubharmonic(int target_rel_semi) {
        int best_n = 1;
        int min_err = 10000;

        for (int n=1; n<=16; ++n) {
            int rel = (sub_semis[n] / 128);
            rel = ((rel % 12) + 12) % 12;

            int err = abs(rel - target_rel_semi);
            if (err > 6) err = 12 - err;

            if (err < min_err) {
                min_err = err;
                best_n = n;
            }
        }
        return best_n;
    }

    void DrawInterface() {
        gfxHeader(page == 0 ? "SubH Harm" : "SubH Rhy");

        if (page == 0) {
            gfxPrint(1, 15, OC::scale_names_short[linker.scale]);
            if (cursor == SCALE) gfxCursor(1, 23, 30);

            gfxPrint(35, 15, OC::Strings::note_names_unpadded[linker.root_note]);
            if (cursor == ROOT) gfxCursor(35, 23, 12);

            gfxPrint(1, 28, "Chord: ");
            const char* chord_names[] = {"I", "ii", "iii", "IV", "V", "vi", "vii"};
            gfxPrint(chord_names[linker.chord_index]);
            if (cursor == CHORD) gfxCursor(37, 36, 18);

            gfxPrint(1, 41, "Inv: ");
            gfxPrint(linker.inversion);
            if (cursor == INVERSION) gfxCursor(25, 49, 10);

            gfxPrint(32, 41, "Slw:");
            gfxPrint(slew_amount);
            if (cursor == SLEW) gfxCursor(32, 49, 28);
        } else {
            gfxPrint(1, 15, "Stay: ");
            gfxPrint(prob_stay);
            gfxPrint("%");
            if (cursor == SEQ_PROB) gfxCursor(30, 23, 30);

            gfxPrint(1, 30, "Loop: ");
            gfxPrint(linker.loop_length);
            if (cursor == LOOP_LEN) gfxCursor(37, 38, 15);

            gfxPrint(1, 45, linker.is_locked ? "LOCKED" : "EVOLVE");
            if (cursor == LOOP_LOCK) gfxCursor(1, 53, 40);
        }

        gfxPrint(1, 55, "/");
        gfxPrint(linker.divisions[0 + hemisphere*2]);
        gfxPrint(32, 55, "/");
        gfxPrint(linker.divisions[1 + hemisphere*2]);

        gfxPrint(50, 55, page + 1);
        gfxPrint("/2");
        if (cursor == PAGE) gfxCursor(50, 63, 12);

        if (linker.IsLinked()) {
            gfxIcon(55, 1, LINK_ICON);
        }
    }
};
