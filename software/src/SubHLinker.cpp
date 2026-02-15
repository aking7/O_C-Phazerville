#include "SubHLinker.h"
#include <Arduino.h>

SubHLinker* SubHLinker::instance = 0;

// Maps integer division N (1-16) to semitone drop (12 * log2(N))
const int8_t SubHLinker::subharmonic_semitones[17] = {
  0, // N=0 (unused)
  0, // N=1 (Unison)
  12, // N=2 (Octave)
  19, // N=3 (Octave + Fifth)
  24, // N=4 (2 Octaves)
  28, // N=5 (2 Oct + Major 3rd)
  31, // N=6 (2 Oct + Fifth)
  34, // N=7 (2 Oct + Minor 7thish)
  36, // N=8 (3 Octaves)
  38, // N=9 (3 Oct + Major 2nd)
  40, // N=10 (3 Oct + Major 3rd)
  42, // N=11 (3 Oct + Fourthish)
  43, // N=12 (3 Oct + Fifth)
  44, // N=13 (3 Oct + Neutral 6th)
  46, // N=14 (3 Oct + Minor 7th)
  47, // N=15 (3 Oct + Major 7th)
  48 // N=16 (4 Octaves)
};

void SubHLinker::UpdateSubharmonics(int chord_idx, int inv) {
  const OC::Scale& scale = OC::Scales::GetScale(this->scale);

  int chord_notes[4];
  chord_notes[0] = 0; // Root
  chord_notes[1] = 2; // 3rd
  chord_notes[2] = 4; // 5th
  chord_notes[3] = 6; // 7th

  // Apply Chord Rotation (Inversion)
  // We only need the first 2 chord tones after inversion for our 2 voices
  for (int i = 0; i < 4; ++i)
    chord_notes[i] = (chord_notes[i] + chord_idx) % scale.num_notes;

  for (int i = 0; i < inv; ++i) {
    int first = chord_notes[0];
    chord_notes[0] = chord_notes[1];
    chord_notes[1] = chord_notes[2];
    chord_notes[2] = chord_notes[3];
    chord_notes[3] = first;
  }

  // Voice 0 (Left Hemisphere): Logic for Output A/B
  // Uses the first note of the inverted chord
  this->chord_pitches[0] = this->root_note + scale.notes[chord_notes[0]];
  // Division[0] is already set by UI

  // Voice 1 (Right Hemisphere): Logic for Output C/D
  // Uses the second note of the inverted chord (e.g. the "3rd" if inv=0)
  // Or maybe the 3rd note (5th) to give it more spread?
  // Let's stick to the next note in the stack for now.
  this->chord_pitches[1] = this->root_note + scale.notes[chord_notes[1]];
  // Division[1] is already set by UI
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
