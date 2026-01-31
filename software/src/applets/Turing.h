#include "util/util_turing.h"

// Copyright (c) 2018, Chysn
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

class Turing : public HemisphereApplet {
public:

    enum TuringCursor {
        PROBABILITY,
        LENGTH,

        CURSOR_LAST = LENGTH
    };

    const char* applet_name() {
        return "Turing";
    }

    void Start() {
        turing.Init();
        cursor = 0;
    }

    void Controller()
    {
        ForEachChannel(ch)
        {
            if (Clock(ch))
            {
                turing.Clock();
                bool current_bit = turing.get_LSB();
                Out(ch, current_bit ? HEMISPHERE_MAX_CV : 0);
                GateOut(ch, current_bit);
            }
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
            case PROBABILITY:
                turing.set_probability(constrain(turing.probability() + direction, 0, 255));
                break;
            case LENGTH:
                turing.set_length(constrain(turing.length() + direction, 2, 32));
                break;
        }
    }

    uint64_t OnDataRequest() {
        uint64_t data = 0;
        Pack(data, PackLocation {0, 32}, turing.get_shift_register());
        Pack(data, PackLocation {32, 8}, turing.probability());
        Pack(data, PackLocation {40, 5}, turing.length());
        return data;
    }

    void OnDataReceive(uint64_t data) {
        turing.set_shift_register(Unpack(data, PackLocation {0, 32}));
        turing.set_probability(Unpack(data, PackLocation {32, 8}));
        turing.set_length(Unpack(data, PackLocation {40, 5}));
    }

protected:
    void SetHelp() {
        //                    "------------------" <-- Size Guide
        help[HEMISPHERE_HELP_DIGITALS] = "Clock";
        help[HEMISPHERE_HELP_CVS]      = "Prob";
        help[HEMISPHERE_HELP_OUTS]     = "CV & Gate";
        help[HEMISPHERE_HELP_ENCODER]  = "Select/Edit";
        //                    "------------------"
    }

private:
    int cursor;
    util::TuringShiftRegister turing;

    void DrawInterface() {
        // Probability
        gfxPrint(1, 15, "P:");
        gfxPrint(15, 15, turing.probability());
        if (cursor == PROBABILITY) gfxCursor(15, 23, 18);

        // Length
        gfxPrint(33, 15, "L:");
        gfxPrint(47, 15, turing.length());
        if (cursor == LENGTH) gfxCursor(47, 23, 12);

        // Shift Register
        menu::DrawMask<true, 32, 8, 1>(62, 40, turing.get_shift_register(), turing.length());
    }
};
