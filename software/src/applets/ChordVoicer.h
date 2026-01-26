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

#include "OC_chords.h"

class ChordVoicer : public HemisphereApplet {
public:

    enum ChordVoicerCursor {
        CHORD_TYPE,
        INVERSION,
        VOICING,

        CURSOR_LAST = VOICING
    };

    const char* applet_name() {
        return "ChordVoicer";
    }

    void Start() {
      cursor = 0;
      chord_type = 0;
      inversion = 0;
      voicing = 0;
    }

    void Controller()
    {
        int root_note = In(0);
        root_note = Quantize(0, root_note);

        for (int i = 0; i < 4; ++i) {
            Out(i, root_note + OC::Chords::GetChordNote(root_note, chord_type, i, inversion, voicing) * SEMI);
        }
    }

    void View() {
        DrawInterface();
    }

    void OnEncoderMove(int direction) {
        if (!EditMode()) {
            MoveCursor(cursor, direction, CURSOR_LAST);
            return;
        }

        switch (cursor) {
            case ROOT_NOTE:
                // This is handled by the CV input, so we don't do anything here
                break;
            case CHORD_TYPE:
                chord_type = constrain(chord_type + direction, 0, OC::Chords::CHORDS_QUALITY_LAST - 1);
                break;
            case INVERSION:
                inversion = constrain(inversion + direction, 0, 3);
                break;
            case VOICING:
                voicing = constrain(voicing + direction, 0, OC::Chords::CHORDS_VOICING_LAST - 1);
                break;
        }
    }

    void OnButtonPress() {
        CursorToggle();
    }

    uint64_t OnDataRequest() {
        uint64_t data = 0;
        Pack(data, PackLocation {0, 8}, chord_type);
        Pack(data, PackLocation {8, 8}, inversion);
        Pack(data, PackLocation {16, 8}, voicing);
        return data;
    }

    void OnDataReceive(uint64_t data) {
        chord_type = Unpack(data, PackLocation {0, 8});
        inversion = Unpack(data, PackLocation {8, 8});
        voicing = Unpack(data, PackLocation {16, 8});
    }

protected:
    void SetHelp() {
        //                    "---------------------" <-- Extra text size guide
        help[HELP_DIGITAL1] = "Clock";
        help[HELP_DIGITAL2] = "";
        help[HELP_CV1]      = "Root Note";
        help[HELP_CV2]      = "";
        help[HELP_OUT1]     = "1st Note";
        help[HELP_OUT2]     = "2nd Note";
        help[HELP_EXTRA1]   = "3rd Note";
        help[HELP_EXTRA2]   = "4th Note";
    }

private:
    int cursor;
    int chord_type;
    int inversion;
    int voicing;

    void DrawInterface() {
        gfxPrint(0, 15, "Root: CV1");

        gfxPrint(0, 25, "Type: ");
        gfxPrint(OC::quality_short_names[chord_type]);
        if (cursor == CHORD_TYPE) gfxCursor(0, 33, 60);

        gfxPrint(0, 35, "Inv: ");
        gfxPrint(inversion);
        if (cursor == INVERSION) gfxCursor(0, 43, 60);

        gfxPrint(0, 45, "Voicing: ");
        gfxPrint(OC::voicing_names_short[voicing]);
        if (cursor == VOICING) gfxCursor(0, 53, 60);
    }

};
