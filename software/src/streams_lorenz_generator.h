// Copyright 2014 Émilie Gillet.
// Copyright 2016 Tim Churches
//
// Original Author: Émilie Gillet (ol.gillet@gmail.com)
// Modifications for use of this code in firmare for the Ornament and Crime module:
// Tim Churches (tim.churches@gmail.com)
//
// Idea for using Rössler generator attributable to Hotlblack Desiato
// (see http://forbinthesynthesizer.blogspot.com.au/2015/11/rossler-barrow.html)
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
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
// Lorenz and Rössler systems.

#ifndef STREAMS_LORENZ_GENERATOR_H_
#define STREAMS_LORENZ_GENERATOR_H_

#include "util/util_macros.h"
// #include "stmlib/stmlib.h"
// #include "streams/meta_parameters.h"

namespace streams {

const size_t kNumChannels = 4;

enum ELorenzOutputMap {
  LORENZ_OUTPUT_X1,
  LORENZ_OUTPUT_Y1,
  LORENZ_OUTPUT_Z1,
  LORENZ_OUTPUT_X2,
  LORENZ_OUTPUT_Y2,
  LORENZ_OUTPUT_Z2,
  ROSSLER_OUTPUT_X1,
  ROSSLER_OUTPUT_Y1,
  ROSSLER_OUTPUT_Z1,
  ROSSLER_OUTPUT_X2,
  ROSSLER_OUTPUT_Y2,
  ROSSLER_OUTPUT_Z2,
  LORENZ_OUTPUT_LX1_PLUS_RX1,
  LORENZ_OUTPUT_LX1_PLUS_RZ1,
  LORENZ_OUTPUT_LX1_PLUS_LY2,
  LORENZ_OUTPUT_LX1_PLUS_LZ2,
  LORENZ_OUTPUT_LX1_PLUS_RX2,
  LORENZ_OUTPUT_LX1_PLUS_RZ2,
  LORENZ_OUTPUT_LX1_XOR_LY1,
  LORENZ_OUTPUT_LX1_XOR_LX2,
  LORENZ_OUTPUT_LX1_XOR_RX1,
  LORENZ_OUTPUT_LX1_XOR_RX2,
  LORENZ_OUTPUT_LAST,
};

class LorenzGenerator {
 public:
  LorenzGenerator() { }
  ~LorenzGenerator() { }
  
  void Init(uint8_t index);
  
  void Process(int32_t freq1, int32_t freq2, bool reset1, bool reset2, uint8_t freq_range1, uint8_t freq_range2);
 
  void set_index(uint8_t index) {
    index_ = index;
  }

 
  inline void set_rho1(int16_t rho) { rho1_ = (rho * (1 << 13)) + (24.0 * (1 << 24)); }
  inline void set_rho2(int16_t rho) { rho2_ = (rho * (1 << 13)) + (24.0 * (1 << 24)); }

  inline void set_sigma1(int16_t sigma) { sigma1_ = (sigma * (1 << 13)) + (10.0 * (1 << 24)); }
  inline void set_sigma2(int16_t sigma) { sigma2_ = (sigma * (1 << 13)) + (10.0 * (1 << 24)); }

  inline void set_beta1(int16_t beta) { beta1_ = (beta * (1 << 13)) + (8.0 / 3.0 * (1 << 24)); }
  inline void set_beta2(int16_t beta) { beta2_ = (beta * (1 << 13)) + (8.0 / 3.0 * (1 << 24)); }

  inline void set_a1(int16_t a) { a1_ = (a * (1 << 13)) + (0.1 * (1 << 24)); }
  inline void set_a2(int16_t a) { a2_ = (a * (1 << 13)) + (0.1 * (1 << 24)); }

  inline void set_b1(int16_t b) { b1_ = (b * (1 << 13)) + (0.1 * (1 << 24)); }
  inline void set_b2(int16_t b) { b2_ = (b * (1 << 13)) + (0.1 * (1 << 24)); }

  inline void set_c1(int16_t c) { c1_ = (c * (1 << 13)) + (5.75 * (1 << 24)); }
  inline void set_c2(int16_t c) { c2_ = (c * (1 << 13)) + (5.75 * (1 << 24)); }

  inline void set_out_a(uint8_t out_a) {
    out_a_ = out_a;
  }

  inline void set_out_b(uint8_t out_b) {
    out_b_ = out_b;
  }

  inline void set_out_c(uint8_t out_c) {
    out_c_ = out_c;
  }

  inline void set_out_d(uint8_t out_d) {
    out_d_ = out_d;
  }
 
 inline const uint16_t dac_code(uint8_t index) const {
    return dac_code_[index];
  }

 private:
  int32_t Lx1_, Ly1_, Lz1_;
  int32_t Rx1_, Ry1_, Rz1_;
  int32_t Lx2_, Ly2_, Lz2_;
  int32_t Rx2_, Ry2_, Rz2_;

  uint8_t out_a_, out_b_, out_c_, out_d_ ;

  int64_t sigma1_, sigma2_, rho1_, rho2_, beta1_, beta2_, c1_, c2_, a1_, a2_, b1_, b2_;
  
  // O+C
  uint16_t dac_code_[kNumChannels];
 
  uint8_t index_;
  
  static void Lorenz(int32_t &x, int32_t &y, int32_t &z, int64_t rho, int64_t sigma, int64_t beta, int64_t dt);
  static void Rossler(int32_t &x, int32_t &y, int32_t &z, int64_t c, int64_t a, int64_t b, int64_t dt);
  static void ScaleLorenz(int32_t x, int32_t y, int32_t z, int32_t &x_scaled, int32_t &y_scaled, int32_t &z_scaled);
  static void ScaleRossler(int32_t x, int32_t y, int32_t z, int32_t &x_scaled, int32_t &y_scaled, int32_t &z_scaled);
  void DetermineActiveGenerators(bool &lorenz1, bool &rossler1, bool &lorenz2, bool &rossler2) const;

  DISALLOW_COPY_AND_ASSIGN(LorenzGenerator);
};

}  // namespace streams

#endif  // STREAMS_LORENZ_GENERATOR_H_
