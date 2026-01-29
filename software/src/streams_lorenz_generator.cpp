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


#include "streams_lorenz_generator.h"

#include "streams_resources.h"

namespace streams {

// using namespace stmlib;

// The Lorenz attractor is a system of three ordinary differential equations
// that exhibits chaotic behavior, famously shaped like a butterfly's wings.
//
// dx/dt = sigma * (y - x)
// dy/dt = x * (rho - z) - y
// dz/dt = x * y - beta * z
//
// How the parameters affect the shape:
//
// sigma: Controls the "viscosity" of the system. Changing it can make the
//        wings of the butterfly attractor wider or narrower.
// rho:   This is the main parameter for chaos. At low values, the output is
//        a simple loop. As rho increases past ~24.74, the output splits into
//        the iconic two "wings," and the path becomes chaotic and unpredictable.
//        Higher values tend to make the attractor larger and more stretched out.
// beta:  Affects the size and symmetry of the wings. Changing beta can make
//        one wing larger or smaller than the other.
void LorenzGenerator::Lorenz(int32_t &x, int32_t &y, int32_t &z, int64_t rho, int64_t sigma, int64_t beta, int64_t dt) {
  int32_t new_x = x + (dt * ((sigma * (y - x)) >> 24) >> 24);
  int32_t new_y = y + (dt * ((x * (rho - z) >> 24) - y) >> 24);
  int32_t new_z = z + (dt * ((x * int64_t(y) >> 24) - (beta * z >> 24)) >> 24);
  x = new_x;
  y = new_y;
  z = new_z;
}

// The Rössler attractor is a simpler system of three differential equations
// that also exhibits chaotic behavior. Its shape is a distinct spiral that
// gets pulled outwards and then folded back on itself.
//
// dx/dt = -y - z
// dy/dt = x + a * y
// dz/dt = b + z * (x - c)
//
// How the parameters affect the shape:
//
// a: Controls the "roundness" of the spiral. Smaller values (like 0.1)
//    create a very circular, uniform spiral. Larger values make it more
//    "peaky" or eccentric.
// b: Controls how much the spiral is "lifted" up along the z-axis on each
//    rotation. Small values keep it relatively flat, while larger values
//    create a taller, more dramatic spiral.
// c: This is the primary chaos control. At low values, the output is a simple
//    circular loop. As 'c' increases, the loop starts to get wider and then
//    begins to fold over on itself, creating the chaotic behavior. Higher
//    values of 'c' lead to more complex folding and a more "noisy" appearance.
void LorenzGenerator::Rossler(int32_t &x, int32_t &y, int32_t &z, int64_t c, int64_t a, int64_t b, int64_t dt) {
  int32_t new_x = x + ((dt * (-y - z)) >> 24);
  int32_t new_y = y + ((dt * (x + ((a * y) >> 24))) >> 24);
  int32_t new_z = z + ((dt * (b + ((z * (x - c)) >> 24))) >> 24);
  x = new_x;
  y = new_y;
  z = new_z;
}

void LorenzGenerator::ScaleLorenz(int32_t x, int32_t y, int32_t z, int32_t &x_scaled, int32_t &y_scaled, int32_t &z_scaled) {
  x_scaled = ((x * 3) >> 16) + 32769;
  y_scaled = ((y * 3) >> 16) + 32769;
  z_scaled = ((z * 3) >> 16);
}

void LorenzGenerator::ScaleRossler(int32_t x, int32_t y, int32_t z, int32_t &x_scaled, int32_t &y_scaled, int32_t &z_scaled) {
  x_scaled = (x >> 14) + 32769;
  y_scaled = (y >> 14) + 32769;
  z_scaled = (z >> 14);
}

void LorenzGenerator::DetermineActiveGenerators(bool &lorenz1, bool &rossler1, bool &lorenz2, bool &rossler2) const {
  lorenz1 = false;
  rossler1 = false;
  lorenz2 = false;
  rossler2 = false;

  uint8_t outputs[4] = {out_a_, out_b_, out_c_, out_d_};
  for (int i = 0; i < 4; ++i) {
    switch (outputs[i]) {
      case LORENZ_OUTPUT_X1:
      case LORENZ_OUTPUT_Y1:
      case LORENZ_OUTPUT_Z1:
      case LORENZ_OUTPUT_LX1_XOR_LY1:
        lorenz1 = true;
        break;

      case LORENZ_OUTPUT_X2:
      case LORENZ_OUTPUT_Y2:
      case LORENZ_OUTPUT_Z2:
        lorenz2 = true;
        break;

      case ROSSLER_OUTPUT_X1:
      case ROSSLER_OUTPUT_Y1:
      case ROSSLER_OUTPUT_Z1:
        rossler1 = true;
        break;

      case ROSSLER_OUTPUT_X2:
      case ROSSLER_OUTPUT_Y2:
      case ROSSLER_OUTPUT_Z2:
        rossler2 = true;
        break;

      case LORENZ_OUTPUT_LX1_PLUS_RX1:
      case LORENZ_OUTPUT_LX1_XOR_RX1:
      case LORENZ_OUTPUT_LX1_PLUS_RZ1:
        lorenz1 = true;
        rossler1 = true;
        break;

      case LORENZ_OUTPUT_LX1_PLUS_LY2:
      case LORENZ_OUTPUT_LX1_PLUS_LZ2:
      case LORENZ_OUTPUT_LX1_XOR_LX2:
        lorenz1 = true;
        lorenz2 = true;
        break;

      case LORENZ_OUTPUT_LX1_PLUS_RX2:
      case LORENZ_OUTPUT_LX1_XOR_RX2:
      case LORENZ_OUTPUT_LX1_PLUS_RZ2:
        lorenz1 = true;
        rossler2 = true;
        break;

      case LORENZ_OUTPUT_LAST:
        break;
    }
  }
}

void LorenzGenerator::Init(uint8_t index) {
  if (index) {
    Lx2_ = 0.1 * (1 << 24);
    Ly2_ = 0;
    Lz2_ = 0;
    Rx2_ = 0.1 * (1 << 24);
    Ry2_ = 0;
    Rz2_ = 0;
    set_sigma2(0);
    set_beta2(0);
    set_a2(0);
    set_b2(0);
    set_c2(0);
    set_rho2(0);
  } else {
    Lx1_ = 0.1 * (1 << 24);
    Ly1_ = 0;
    Lz1_ = 0;
    Rx1_ = 0.1 * (1 << 24);
    Ry1_ = 0;
    Rz1_ = 0;
    set_sigma1(0);
    set_beta1(0);
    set_a1(0);
    set_b1(0);
    set_c1(0);
    set_rho1(0);
  }
}

void LorenzGenerator::Process(
    int32_t freq1,
    int32_t freq2,
    bool reset1,
    bool reset2,
    uint8_t freq_range1,
    uint8_t freq_range2) {
  int32_t rate1 =  (freq1 >> 8);
  if (rate1 < 0) rate1 = 0;
  if (rate1 > 255) rate1 = 255;
  int32_t rate2 = (freq2 >> 8);
  if (rate2 < 0) rate2 = 0;
  if (rate2 > 255) rate2 = 255;

  if (reset1) Init(0) ;
  if (reset2) Init(1) ; 

  // --- State updates ---
  int64_t Ldt1 = static_cast<int64_t>(lut_lorenz_rate[rate1] >> (5 - freq_range1));
  Lorenz(Lx1_, Ly1_, Lz1_, rho1_, sigma1_, beta1_, Ldt1);

  int64_t Rdt1 = static_cast<int64_t>(lut_lorenz_rate[rate1] >> 0);
  Rossler(Rx1_, Ry1_, Rz1_, c1_, a1_, b1_, Rdt1);

  int64_t Ldt2 = static_cast<int64_t>(lut_lorenz_rate[rate2] >> (5 - freq_range2));
  Lorenz(Lx2_, Ly2_, Lz2_, rho2_, sigma2_, beta2_, Ldt2);

  int64_t Rdt2 = static_cast<int64_t>(lut_lorenz_rate[rate2] >> 0);
  Rossler(Rx2_, Ry2_, Rz2_, c2_, a2_, b2_, Rdt2);

  // --- Scaling ---
  // For performance, we only calculate the scaled output values for a given
  // generator if one of its outputs (X, Y, or Z) is currently assigned to one
  // of the four DAC channels. This avoids redundant calculations while ensuring
  // the underlying state of the generator is always continuous.

  bool lorenz1_active, rossler1_active, lorenz2_active, rossler2_active;
  DetermineActiveGenerators(lorenz1_active, rossler1_active, lorenz2_active, rossler2_active);

  int32_t Lz1_scaled = 0;
  int32_t Lx1_scaled = 0;
  int32_t Ly1_scaled = 0;
  if (lorenz1_active)
    ScaleLorenz(Lx1_, Ly1_, Lz1_, Lx1_scaled, Ly1_scaled, Lz1_scaled);

  int32_t Rz1_scaled = 0;
  int32_t Rx1_scaled = 0;
  int32_t Ry1_scaled = 0;
  if (rossler1_active)
    ScaleRossler(Rx1_, Ry1_, Rz1_, Rx1_scaled, Ry1_scaled, Rz1_scaled);

  int32_t Lz2_scaled = 0;
  int32_t Lx2_scaled = 0;
  int32_t Ly2_scaled = 0;
  if (lorenz2_active)
    ScaleLorenz(Lx2_, Ly2_, Lz2_, Lx2_scaled, Ly2_scaled, Lz2_scaled);

  int32_t Rz2_scaled = 0;
  int32_t Rx2_scaled = 0;
  int32_t Ry2_scaled = 0;
  if (rossler2_active)
    ScaleRossler(Rx2_, Ry2_, Rz2_, Rx2_scaled, Ry2_scaled, Rz2_scaled);

  // --- Output mapping ---
  uint8_t out_channel ;
  
  for (uint8_t i = 0; i < 4; ++i) {
    switch(i) {
      case 0:
        out_channel = out_a_ ;
        break ;
      case 1:
        out_channel = out_b_ ;
        break ;
      case 2:
        out_channel = out_c_ ;
        break ;
      case 3:
        out_channel = out_d_ ;
        break ; 
      default:
        // shut up compiler warning: 'out_channel' may be used uninitialized
        out_channel = LORENZ_OUTPUT_LAST; 
        break ;       
    }
 
    switch (out_channel) {
      case LORENZ_OUTPUT_X1:
        dac_code_[i] = Lx1_scaled;
        break;
      case LORENZ_OUTPUT_Y1:
        dac_code_[i] = Ly1_scaled;
        break;
      case LORENZ_OUTPUT_Z1:
        dac_code_[i] = Lz1_scaled;
        break;
      case LORENZ_OUTPUT_X2:
        dac_code_[i] = Lx2_scaled;
        break;
      case LORENZ_OUTPUT_Y2:
        dac_code_[i] = Ly2_scaled;
        break;
      case LORENZ_OUTPUT_Z2:
        dac_code_[i] = Lz2_scaled;
        break;
      case ROSSLER_OUTPUT_X1:
        dac_code_[i] = Rx1_scaled;
        break;
      case ROSSLER_OUTPUT_Y1:
        dac_code_[i] = Ry1_scaled;
        break;
      case ROSSLER_OUTPUT_Z1:
        dac_code_[i] = Rz1_scaled;
        break;
      case ROSSLER_OUTPUT_X2:
        dac_code_[i] = Rx2_scaled;
        break;
      case ROSSLER_OUTPUT_Y2:
        dac_code_[i] = Ry2_scaled;
        break;
      case ROSSLER_OUTPUT_Z2:
        dac_code_[i] = Rz2_scaled;
        break;
      case LORENZ_OUTPUT_LX1_PLUS_RX1:
        dac_code_[i] = (Lx1_scaled + Rx1_scaled) >> 1;
        break;
      case LORENZ_OUTPUT_LX1_PLUS_RZ1:
        dac_code_[i] = (Lx1_scaled + Rz1_scaled) >> 1;
        break;
      case LORENZ_OUTPUT_LX1_PLUS_LY2:
        dac_code_[i] = (Lx1_scaled + Ly2_scaled) >> 1;
        break;
      case LORENZ_OUTPUT_LX1_PLUS_LZ2:
        dac_code_[i] = (Lx1_scaled + Lz2_scaled) >> 1;
        break;
      case LORENZ_OUTPUT_LX1_PLUS_RX2:
        dac_code_[i] = (Lx1_scaled + Rx2_scaled) >> 1;
        break;
      case LORENZ_OUTPUT_LX1_PLUS_RZ2:
        dac_code_[i] = (Lx1_scaled + Rz2_scaled) >> 1;
        break;
      case LORENZ_OUTPUT_LX1_XOR_LY1:
        dac_code_[i] = Lx1_scaled ^ Ly1_scaled ;
        break;
      case LORENZ_OUTPUT_LX1_XOR_LX2:
        dac_code_[i] = Lx1_scaled ^ Lx2_scaled ;
        break;
      case LORENZ_OUTPUT_LX1_XOR_RX1:
        dac_code_[i] = Lx1_scaled ^ Rx1_scaled ;
        break;
      case LORENZ_OUTPUT_LX1_XOR_RX2:
        dac_code_[i] = Lx1_scaled ^ Rx2_scaled ;
        break;
       default:
        break;
    }
  }

}

}  // namespace streams
