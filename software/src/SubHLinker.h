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

#pragma once

#ifndef SUBH_LINKER_H_
#define SUBH_LINKER_H_

#include "OC_scales.h"

// Shared chord engine for the SubH applet pair. One singleton serves both
// hemispheres: the left applet outputs VCO 1 as {pitch, SUB divisor}, the
// right VCO 2. The engine is a three-stage pipeline, each stage isolated so
// features can be added without touching the others:
//
//   AdvanceSequencer()    progression brain: stay/home/branch random walk,
//                         phrase cadencing, secondary dominants, key drift
//   BuildChordPCs()       chord spec (degree + flags) -> 4 pitch classes
//   VoiceChord()          pitch classes -> hardware voicing: 2 VCO pitches,
//                         2 undertone divisors, 4 sounding notes
//
// UpdateSubharmonics() runs the latter two, including auto voice-leading
// (auditioning all inversions for minimal motion from the previous chord).
struct SubHLinker {
    static SubHLinker *instance;

    static const int INV_AUTO = 4;  // inversion chosen by voice-leading

    // --- Harmony settings
    uint8_t scale;
    int8_t root_note;     // the key center; Home and Drift orbit this
    int8_t chord_index;   // current scale degree, 0-6
    bool sec_dom;         // currently playing V7 *of* chord_index
    int8_t inversion;     // 0-3, or INV_AUTO
    bool seventh;         // 7th chords (true) or triads with doubled root
    int8_t octave;        // whole-chord transpose, -2..+2 octaves
    uint8_t home;         // tonic gravity % (functional moves + phrasing)
    bool use_sec_dom;     // approach new chords via their own dominant
    uint8_t drift;        // % chance to slip to a related key on a cadence

    // --- Sequencer settings/state
    uint8_t seq_step;
    uint8_t seq_history[32];  // packed specs: degree | (sec_dom << 3)
    bool is_locked;
    uint8_t loop_length;      // loop AND phrase length
    uint8_t dejavu;
    uint8_t branch_prob;
    uint8_t stay_prob;

    // --- Performance CV, written each frame by the linked right side
    int16_t cv_transpose;  // o_C units, added to both VCO pitch outputs
    int8_t cv_home;        // signed offset added to home

    // --- Voicing results (consumed by both applet instances)
    uint8_t divisions[2];  // SUB divisor per VCO (1-13)
    int16_t vco_pitch[2];  // fundamental pitch per VCO (o_C units)
    int8_t sounding[4];    // sounding chord tones (semitones), low to high

    bool registered[2];

    SubHLinker() {
        scale = 0;
        root_note = 0;
        chord_index = 0;
        sec_dom = false;
        inversion = INV_AUTO;
        seventh = true;
        octave = 0;
        home = 50;
        use_sec_dom = false;
        drift = 0;

        seq_step = 0;
        for(int i=0; i<32; ++i) seq_history[i] = 0;
        is_locked = false;
        loop_length = 4;
        dejavu = 0;
        branch_prob = 50;
        stay_prob = 50;

        cv_transpose = 0;
        cv_home = 0;

        for(int i=0; i<2; ++i) { divisions[i] = 1; vco_pitch[i] = 0; }
        for(int i=0; i<4; ++i) sounding[i] = 0;

        registered[0] = false;
        registered[1] = false;
    }

    static SubHLinker &get() {
        if (!instance) instance = new SubHLinker;
        return *instance;
    }

    void Register(int hemisphere) {
        if (hemisphere >= 0 && hemisphere < 2) registered[hemisphere] = true;
    }

    void Unload(int hemisphere) {
        if (hemisphere >= 0 && hemisphere < 2) registered[hemisphere] = false;
    }

    bool IsLinked() {
        return (registered[0] && registered[1]);
    }

    void UpdateSubharmonics(int chord_idx, bool is_sec_dom, int inv);
    void AdvanceSequencer();

private:
    // A complete hardware voicing for one chord
    struct Voicing {
        uint8_t div[2];
        int16_t pitch[2];
        int8_t notes[4];  // sounding semitones, sorted low to high
    };

    void BuildChordPCs(int degree, bool is_sec_dom, int pc[4]) const;
    void VoiceChord(const int pc[4], Voicing &v) const;
};

#endif
