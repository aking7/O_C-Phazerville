// Copyright (c) 2024, The Phazerville Project
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
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

struct SubHLinker {
  static SubHLinker* instance;

  uint8_t scale;
  int8_t root_note;
  int8_t chord_index;
  int8_t inversion;
  uint8_t divisions[2];
  int8_t chord_pitches[2];

  uint8_t seq_step;
  uint8_t seq_history[32];
  bool is_locked;
  uint8_t loop_length;
  uint8_t dejavu;
  uint8_t branch_prob;
  uint8_t stay_prob;

  bool registered[2];

  SubHLinker() {
    scale = 0;
    root_note = 0;
    chord_index = 0;
    inversion = 0;
    for (int i = 0; i < 2; ++i) {
      divisions[i] = 1;
      chord_pitches[i] = 0;
    }

    seq_step = 0;
    for (int i = 0; i < 32; ++i) seq_history[i] = 0;
    is_locked = false;
    loop_length = 4;
    dejavu = 0;
    branch_prob = 50;
    stay_prob = 50;

    registered[0] = false;
    registered[1] = false;
  }

  static SubHLinker& get() {
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

  void UpdateSubharmonics(int chord_idx, int inv);
  void AdvanceSequencer();
};

#endif
