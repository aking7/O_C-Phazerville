#include <Arduino.h>
#include "SubHLinker.h"

SubHLinker *SubHLinker::instance = 0;

// ========================== Undertone tables ===============================
//
// A suboscillator can only sound *below* its VCO, at f/n for n = 1-16.
// Octave-folded, each divisor lands on a fixed pitch class relative to the
// fundamental: n=3 -> +498c, n=5 -> +814c, n=7 -> +231c, n=9 -> +996c,
// n=11 -> +649c, n=13 -> +359c (n=2/4/8 are octaves). Classic o_C hardware
// can't output below -3V, which makes divisors above 13 unreachable, so the
// tables only ever choose n <= 13.

// For each desired interval (semitones above the fundamental, mod 12), the
// divisor whose undertone octave-folds closest to it:
static const uint8_t best_n_for_semi[12] = {2, 2, 7, 13, 13, 3, 11, 11, 5, 5, 9, 2};
// ...that choice's pitch error in cents (used to score voice assignments):
static const uint8_t n_err_cents[12]     = {0, 100, 31, 59, 41, 2, 49, 51, 14, 86, 4, 100};
// ...and how far below the fundamental each divisor n sounds, in semitones:
static const uint8_t n_depth_semi[14] = {0, 0, 12, 19, 24, 28, 31, 34, 36, 38, 40, 42, 43, 44};

// ========================= Chord construction ==============================

// Chord spec -> 4 pitch classes, bass first (pc[0] is the chord's bass tone).
void SubHLinker::BuildChordPCs(int degree, bool is_sec_dom, int pc[4]) const {
    const OC::Scale &s = OC::Scales::GetScale(this->scale);
    // scale.notes are semitone * 128, relative to the root
    #define DEG_PC(d) ((this->root_note + s.notes[(d) % s.num_notes] / 128) % 12)

    if (is_sec_dom) {
        // The dominant 7th *of* the target degree, built chromatically:
        // a major triad (+m7) rooted a perfect 5th above the target's root.
        int r = (DEG_PC(degree) + 7) % 12;
        pc[0] = r;
        pc[1] = (r + 4) % 12;
        pc[2] = (r + 7) % 12;
        pc[3] = this->seventh ? (r + 10) % 12 : r;
    } else {
        // Stacked thirds on the scale degree. Triads double the root; the
        // doubled tone voices as a pure octave undertone and avoids the
        // tritone, so triads land on pure undertones everywhere.
        pc[0] = DEG_PC(degree);
        pc[1] = DEG_PC(degree + 2);
        pc[2] = DEG_PC(degree + 4);
        pc[3] = this->seventh ? DEG_PC(degree + 6) : pc[0];
    }
    #undef DEG_PC
}

// ============================== Voicing ====================================

// Map 4 pitch classes (bass first) onto the hardware: the bass must come
// from a suboscillator (undertones are the only way to play below a VCO),
// so VCO 1's sub takes it. Audition every way to give the three upper tones
// to {VCO 1, VCO 2, VCO 2's sub} and keep the split with the least total
// undertone pitch error. Because divisors are intervals below the
// fundamental each VCO actually plays, the subs track their VCOs correctly
// through every chord change.
void SubHLinker::VoiceChord(const int pc[4], Voicing &v) const {
    static const uint8_t perms[6][3] = {
        {1,2,3}, {1,3,2}, {2,1,3}, {2,3,1}, {3,1,2}, {3,2,1}
    };
    int best = 9999, fa = 1, fb = 2, sb = 3;
    for(int i=0; i<6; ++i) {
        int relA = (pc[0]           - pc[perms[i][0]] + 12) % 12;
        int relB = (pc[perms[i][2]] - pc[perms[i][1]] + 12) % 12;
        int score = n_err_cents[relA] + n_err_cents[relB];
        if (score < best) {
            best = score;
            fa = perms[i][0]; fb = perms[i][1]; sb = perms[i][2];
        }
    }

    v.div[0] = best_n_for_semi[(pc[0]  - pc[fa] + 12) % 12];
    v.div[1] = best_n_for_semi[(pc[sb] - pc[fb] + 12) % 12];

    // Octave placement: aim the bass about an octave below center (0V), and
    // keep VCO 2's undertone above it so the bass stays the lowest voice.
    int dA = n_depth_semi[v.div[0]];
    int fundA = pc[fa], bass = fundA - dA;
    for(int k=0, e=999; k<=5; ++k) {
        int f = pc[fa] + 12*k - 12;
        int s = f - dA;
        if (abs(s + 12) < e) { e = abs(s + 12); fundA = f; bass = s; }
    }

    int dB = n_depth_semi[v.div[1]];
    int fundB = pc[fb] + 48, subB = fundB - dB;
    for(int k=0, e=999; k<=5; ++k) {
        int f = pc[fb] + 12*k - 12;
        int s = f - dB;
        if (s <= bass) continue; // the bass stays lowest
        if (abs(s - (bass + 7)) < e) { e = abs(s - (bass + 7)); fundB = f; subB = s; }
    }

    v.pitch[0] = fundA * 128;
    v.pitch[1] = fundB * 128;

    // Sounding notes, low to high
    int s4[4] = {bass, subB, fundA, fundB};
    for(int i=0; i<3; ++i)
        for(int j=i+1; j<4; ++j)
            if (s4[j] < s4[i]) { int t = s4[i]; s4[i] = s4[j]; s4[j] = t; }
    for(int i=0; i<4; ++i) v.notes[i] = s4[i];
}

// Voice the current chord. With a fixed inversion, the chord tones rotate
// so a different tone takes the bass. With INV_AUTO, all four inversions
// are auditioned and the one whose voices move least from the previous
// chord wins -- classic nearest-voicing voice-leading.
void SubHLinker::UpdateSubharmonics(int chord_idx, bool is_sec_dom, int inv) {
    int base[4];
    BuildChordPCs(chord_idx, is_sec_dom, base);

    int inv_lo = inv, inv_hi = inv;
    if (inv == INV_AUTO) { inv_lo = 0; inv_hi = 3; }

    Voicing best = {};
    int best_cost = 0x7FFF;
    for(int i = inv_lo; i <= inv_hi; ++i) {
        int pc[4];
        for(int j=0; j<4; ++j) pc[j] = base[(j + i) % 4];

        Voicing v;
        VoiceChord(pc, v);

        int cost = 0;
        for(int j=0; j<4; ++j) cost += abs(v.notes[j] - this->sounding[j]);
        if (cost < best_cost) { best_cost = cost; best = v; }
    }

    for(int i=0; i<2; ++i) {
        this->divisions[i] = best.div[i];
        this->vco_pitch[i] = best.pitch[i];
    }
    for(int i=0; i<4; ++i) this->sounding[i] = best.notes[i];
}

// ========================= Progression engine ==============================

// Common-practice chord flow: each degree's likely destinations, weighted by
// repetition. Dominant function (V, vii) pulls back to I, so even a random
// walk keeps articulating the key center.
static const uint8_t func_next[7][4] = {
    {1, 3, 4, 5},  // I   -> ii, IV, V, vi
    {4, 4, 6, 4},  // ii  -> V (mostly), vii
    {5, 3, 5, 3},  // iii -> vi, IV
    {4, 0, 1, 4},  // IV  -> V (mostly), I, ii
    {0, 0, 5, 0},  // V   -> I (mostly), vi (deceptive)
    {1, 3, 1, 3},  // vi  -> ii, IV
    {0, 0, 2, 0},  // vii -> I (mostly), iii
};

void SubHLinker::AdvanceSequencer() {
    int eff_home = constrain(this->home + this->cv_home, 0, 100);

    int dv_rnd = (int)random(100);
    bool use_history = this->is_locked || (dv_rnd < this->dejavu);

    if (use_history) {
        this->seq_step = (this->seq_step + 1) % this->loop_length;
        uint8_t h = this->seq_history[this->seq_step];
        this->chord_index = h & 0x07;
        this->sec_dom = (h >> 3) & 1;
        return;
    }

    // Phrase position of the step we're choosing (Len doubles as the phrase
    // length even when unlocked)
    int pos_next = (this->seq_step + 1) % this->loop_length;

    int next_deg;
    bool next_sec = false;

    if (this->sec_dom) {
        // A dominant must resolve: V/X always lands on X
        next_deg = this->chord_index;
    } else if (this->loop_length > 1 && pos_next == this->loop_length - 1
               && (int)random(100) < eff_home) {
        // Set up the cadence going into the last step of the phrase
        next_deg = 4; // V
    } else if (this->loop_length > 1 && pos_next == 0
               && (int)random(100) < eff_home) {
        // Resolve home on the phrase downbeat
        next_deg = 0; // I
        // ...and sometimes slip to a closely-related key while we're here
        if ((int)random(100) < this->drift) {
            this->root_note = (this->root_note + (random(2) ? 7 : 5)) % 12;
        }
    } else if ((int)random(100) < this->stay_prob) {
        next_deg = this->chord_index;
    } else if ((int)random(100) < eff_home) {
        // Tonic gravity: move along the functional-harmony map
        next_deg = func_next[this->chord_index % 7][(int)random(4)];
    } else if ((int)random(100) < this->branch_prob) {
        next_deg = (int)random(7);
    } else {
        next_deg = (this->chord_index + 1) % 7;
    }

    // Approach a *new* chord through its own dominant (V/X), resolving to X
    // on the following step
    if (this->use_sec_dom && !this->sec_dom && next_deg != this->chord_index
        && (int)random(100) < eff_home / 2) {
        next_sec = true;
    }

    this->seq_step = (this->seq_step + 1) % 32;
    this->seq_history[this->seq_step % this->loop_length] =
        (uint8_t)(next_deg | (next_sec ? 0x08 : 0));

    this->chord_index = next_deg;
    this->sec_dom = next_sec;
}
