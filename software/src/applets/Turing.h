// Copyright (c) 2024, Phazerville
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
        // Initialize default values
        probability = 50;
        length = 16;
        shift_register = 0;
        cursor = 0;
    }

    void Controller()
    {
        ForEachChannel(ch)
        {
            if (Clock(ch))
            {
                // Get the current bit (MSB)
                bool current_bit = (shift_register >> (length - 1)) & 1;

                // Output the CV for the current bit
                Out(ch, current_bit ? HEMISPHERE_MAX_CV : 0);
                GateOut(ch, current_bit);

                // Decide whether to flip the bit based on probability
                if (random(1, 100) <= probability) {
                    current_bit = !current_bit;
                }

                // Shift the register
                shift_register = (shift_register << 1) | current_bit;

                // Create a mask to keep the register within the desired length
                uint16_t mask = (1 << length) - 1;
                shift_register &= mask;
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
                probability = constrain(probability + direction, 0, 100);
                break;
            case LENGTH:
                length = constrain(length + direction, 1, 16);
                break;
        }
    }

    uint64_t OnDataRequest() {
        uint64_t data = 0;
        Pack(data, PackLocation {0, 7}, probability);
        Pack(data, PackLocation {7, 5}, length);
        return data;
    }

    void OnDataReceive(uint64_t data) {
        probability = Unpack(data, PackLocation {0, 7});
        length = Unpack(data, PackLocation {7, 5});
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
    uint8_t probability;
    uint8_t length;
    uint16_t shift_register;

    void DrawInterface() {
        // Probability
        gfxPrint(1, 15, "P:");
        gfxPrint(15, 15, probability);
        gfxPrint("%");
        gfxCursor(15, 23, 18);

        // Length
        gfxPrint(33, 15, "L:");
        gfxPrint(47, 15, length);
        gfxCursor(47, 23, 12);

        // Shift Register
        for (int i = 0; i < length; i++) {
            if ((shift_register >> i) & 1) {
                gfxRect(62 - (i * 4), 40, 3, 3);
            } else {
                gfxFrame(62 - (i * 4), 40, 3, 3);
            }
        }
    }
};
