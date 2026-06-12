#include <Arduino.h>
#include "SubHLinker.h"

SubHLinker *SubHLinker::instance = 0;

// For each pitch class (semitones above the VCO), the divisor n (1-16) whose
// subharmonic f/n lands closest to that pitch class, octave-folded:
// n=15 -> +112c, n=7 -> +231c, n=13 -> +359c, n=3 -> +498c, n=11 -> +649c,
// n=5 -> +814c, n=9 -> +996c. Divisors above 13 need CV below -3V, so they
// clamp to 13 on hardware without a 20Vpp output range.
static const uint8_t best_n_for_semi[12] = {1, 15, 7, 7, 13, 3, 11, 11, 5, 5, 9, 1};

void SubHLinker::UpdateSubharmonics(int chord_idx, int inv) {
    const OC::Scale &scale = OC::Scales::GetScale(this->scale);

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
        // Scale notes are stored as semitone * 128; extract 0-11 pitch class
        int rel = (target_rel_semi / 128) % 12;
        this->divisions[i] = best_n_for_semi[rel];
    }
}

void SubHLinker::AdvanceSequencer() {
    int next_idx;

    int dv_rnd = (int)random(100);
    bool use_history = this->is_locked || (dv_rnd < this->dejavu);

    if (use_history) {
        this->seq_step = (this->seq_step + 1) % this->loop_length;
        next_idx = this->seq_history[this->seq_step];
    } else {
        if ((int)random(100) < this->stay_prob) {
            next_idx = this->chord_index;
        } else {
            int br_rnd = (int)random(100);
            if (br_rnd < this->branch_prob) {
                next_idx = (int)random(7);
            } else {
                next_idx = (this->chord_index + 1) % 7;
            }
        }

        this->seq_step = (this->seq_step + 1) % 32;
        this->seq_history[this->seq_step % this->loop_length] = next_idx;
    }

    this->chord_index = next_idx;
}
