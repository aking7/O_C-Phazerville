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

#include "../bjorklund.h"
#include "../tideslite.h"

class EuclideanLFO : public HemisphereApplet {
public:

    enum EuclideanLFOCursor {
        LENGTH,
        BEATS,
        OFFSET,
        RATE,
        WAVEFORM,

        CURSOR_LAST = WAVEFORM
    };

    const char* applet_name() {
        return "EuclideanLFO";
    }

    void Start() {
      // Initialize default values
      length = 16;
      beats = 8;
      offset = 0;
      pitch = -3 * 12 * 128; // C1
      slope = 0;
      shape = 48; // Triangle
      fold = 0;
    }

    void Controller()
    {
        pattern = EuclideanPattern(length, beats, offset);
        if (Clock(0)) {
            euclidean_step = (euclidean_step + 1) % length;
            if ((pattern >> euclidean_step) & 0x01) {
                phase = 0;
            }
        }

        uint32_t phase_increment = ComputePhaseIncrement(pitch);
        phase += phase_increment;

        TidesLiteSample sample;
        ProcessSample(slope, shape, fold, phase, sample);
        Out(0, sample.unipolar);
        Out(1, sample.bipolar);
    }

    void View() {
        DrawInterface();
    }

    /* The default encoder press action is to toggle editing.
     * You can override this for more complex behavior. */
    // void OnButtonPress() { }

    /* Pressing the select button after highlighting a parameter for editing
     * can invoke a secondary action here. By default, it just cancels editing. */
    // void AuxButton() { }

    uint64_t OnDataRequest() {
        uint64_t data = 0;
        Pack(data, PackLocation {0,8}, length);
        Pack(data, PackLocation {8,8}, beats);
        Pack(data, PackLocation {16,8}, offset);
        Pack(data, PackLocation {24, 16}, pitch);
        Pack(data, PackLocation {40, 8}, shape);
        return data;
    }

    void OnDataReceive(uint64_t data) {
        length = Unpack(data, PackLocation {0,8});
        beats = Unpack(data, PackLocation {8,8});
        offset = Unpack(data, PackLocation {16,8});
        pitch = Unpack(data, PackLocation {24, 16});
        shape = Unpack(data, PackLocation {40, 8});
    }

protected:
    void SetHelp() {
        //                    "------------------" <-- Size Guide
        help[HEMISPHERE_HELP_DIGITAL1] = "Clock";
        help[HEMISPHERE_HELP_DIGITAL2] = "Reset";
        help[HEMISPHERE_HELP_CV1] = "Rate";
        help[HEMISPHERE_HELP_CV2] = "Wave";
        help[HEMISPHERE_HELP_OUT1] = "Unipolar";
        help[HEMISPHERE_HELP_OUT2] = "Bipolar";
    }

private:
    int cursor;
    uint32_t pattern;
    uint32_t phase;
    int euclidean_step;

    uint8_t length;
    uint8_t beats;
    uint8_t offset;
    int16_t pitch;
    int slope;
    int shape;
    int fold;

    void OnEncoderMove(int direction) {
        if (!EditMode()) {
            MoveCursor(cursor, direction, CURSOR_LAST);
            return;
        }

        switch (cursor) {
        case LENGTH:
            length = constrain(length + direction, 2, 32);
            if (beats > length) beats = length;
            if (offset >= length) offset = length - 1;
            break;
        case BEATS:
            beats = constrain(beats + direction, 0, length);
            break;
        case OFFSET:
            offset = constrain(offset + direction, 0, length - 1);
            break;
        case RATE:
            pitch = constrain(pitch + direction * 128, -12 * 12 * 128, 12 * 12 * 128);
            break;
        case WAVEFORM:
            shape = constrain(shape + direction, 0, 127);
            break;
        }
    }

    void DrawInterface() {
        gfxPrint(1, 2, "Len: ");
        gfxPrint(length);
        gfxPrint(1, 12, "Beats: ");
        gfxPrint(beats);
        gfxPrint(1, 22, "Rot: ");
        gfxPrint(offset);

        switch (cursor) {
        case LENGTH:
            gfxCursor(1, 10, 12, 7);
            break;
        case BEATS:
            gfxCursor(1, 20, 12, 7);
            break;
        case OFFSET:
            gfxCursor(1, 30, 12, 7);
            break;
        case RATE:
            gfxCursor(32, 40, 30, 7);
            break;
        case WAVEFORM:
            gfxCursor(32, 50, 30, 7);
            break;
        }

        // Draw the Euclidean pattern
        for (int i = 0; i < length; i++) {
            if ((pattern >> i) & 0x01) {
                gfxRect(1 + i * 4, 32, 3, 3);
            } else {
                gfxPixel(2 + i * 4, 33);
            }
        }

        // Draw the LFO waveform
        TidesLiteSample sample;
        int last_y = 0;
        for (int i = 0; i < 64; i++) {
            ProcessSample(slope, shape, fold, i * (0xffffffff / 64), sample);
            int y = 63 - (sample.unipolar >> 10);
            if (i > 0) {
                gfxLine(i - 1, last_y, i, y);
            }
            last_y = y;
        }

        gfxPrint(32, 42, "Rate: ");
        gfxPrintFreqFromPitch(pitch);
        gfxPrint(32, 52, "Wave: ");
        gfxPrint(shape);
    }

};
