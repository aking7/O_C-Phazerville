---
layout: default
---
# SubH

**SubH** is an applet designed to pair with the **Moog Subharmonicon**. It translates musical notes into the closest subharmonic divisions and features a probability-based sequencer.

### Key Features:
- **VCO + Undertone per side:** Each instance drives one Subharmonicon VCO — a chord tone as the fundamental pitch, plus the divisor that voices a second chord tone as an undertone below it. Divisors are recomputed against the note each VCO actually plays, so the subs track their VCOs through every chord change.
- **Bass from below:** Undertones are the only way to play beneath a VCO, so the chord's bass is always voiced by a suboscillator — just like the hardware works. SubH auditions every voice assignment and picks the one with the least undertone pitch error (most diatonic 7th chords land on *pure* /3 undertones).
- **Linked 4-Note Mode:** When two instances are loaded in a Hemisphere pair, they link to share scale, chord, and sequencer state, driving both VCOs for a full 4-note chord. A solo instance drives VCO 1 (the pair that owns the bass).
- **Auto Voice-Leading:** With Inv set to Auto (the default), each chord change picks the inversion whose voices move the least from the previous chord — common tones hold, the rest slide. Smooth, connected progressions instead of leaps.
- **Tonic Gravity (Home):** A functional-harmony map (ii→V, V→I, vii→I...) blended into the random walk. Dominants resolve to I, so progressions evolve freely yet keep articulating the key.
- **Phrasing:** Len doubles as a phrase length even when unlocked: scaled by Home, the progression leans onto V going into the last step of each phrase and resolves to I on the downbeat.
- **Secondary Dominants (SecD):** When enabled, new chords are sometimes approached through their own dominant (V/V→V, V/ii→ii) — controlled chromatic tension that always resolves. The header shows e.g. "V/ii" while one is sounding.
- **Key Drift:** On a cadence to I, a Drift % chance to modulate to a related key along the circle of fifths. Slow, long-form harmonic evolution; the Root setting (and the display underline) follows the drift.
- **Performance CVs:** When linked, the right side's CV inputs become live controls: CV1 transposes the whole chord (1V/oct, semitone steps), CV2 modulates Home.
- **Probability Sequencer:** A rhythmic engine that evolves chord progressions based on a "Stay" probability.
- **Loop Lock:** The "Lock" feature allows you to freeze the current progression and loop it indefinitely.
- **Slew/Morphing:** Adjustable slew for smooth transitions between chords.
- **Chord Display:** The bottom row shows the four sounding notes, low to high; the key's root is underlined wherever it appears in the chord.

### Patching to the Subharmonicon

The Subharmonicon has only **two SUB CV inputs** — one per VCO — and each sets the divisor for *both* of that VCO's suboscillators together. It cannot independently CV-control the second suboscillator. So instead of driving four divisors, SubH drives **one VCO per hemisphere** as a pitch + divisor pair:

- **Output A → VCO input** (1V/oct): the chord tone played as that VCO's fundamental.
- **Output B → VCO SUB input**: the divisor that voices a second chord tone as an undertone below the fundamental.

So one SubH instance drives one VCO (a 2-note voicing); a **linked pair** drives both VCOs for a full 4-note chord.

**Setup on the synth:** center the **SUB FREQ knobs** (both SUB 1 and SUB 2 on each VCO), and bypass the internal sequencer (VCO pitch inputs are summed with the SEQ step knobs). The SUB CV input reads -5V to +5V as divisor 16 down to 1 in 16 equal zones of 0.625V; SubH outputs the center of the zone (divisor 1 = +4.69V, 8 = +0.31V, 16 = -4.69V).

*Note:* on hardware whose DAC floor is -3V (classic o_C), divisors above 13 are unreachable, so SubH only ever chooses divisors 1-13 — no clamping, no silent pitch errors. VCO pitch outputs are unaffected.

### I/O

|        |        Left / Slot 1        |        Right / Slot 2 (linked)  |
| ------ | :-------------------------: | :-----------------------------: |
| TRIG   |            Clock            |   Reset (cadences to I)         |
| CV 1   |      Chord Modulation       |   Transpose (1V/oct, semitones) |
| CV 2   |    Inversion Modulation     |   Home Modulation               |
| OUT A  |     VCO 1 pitch (1V/oct)    |     VCO 2 pitch (1V/oct)        |
| OUT B  |      VCO 1 SUB divisor      |      VCO 2 SUB divisor          |

A solo (unlinked) instance keeps Chord/Inversion on its CV inputs and drives VCO 1.

### UI Parameters

Settings live in one scrolling list (four rows visible; a bar on the right edge shows the scroll position). The header always shows the current chord, and the bottom row always shows the four sounding notes.

- **Scale:** Choose the musical scale.
- **Root:** Set the root note of the scale — this is the key that **Home** gravitates toward.
- **Chrd:** Select the base chord (I, ii, iii, etc.).
- **7th:** Press the button to toggle between 7th chords (On) and triads (Off). Triads double the root instead of playing the 7th — they avoid the tritone, so every voice lands on a pure (just-intonation) undertone.
- **Inv:** Chord inversion 0-3, or **Auto** (default): each chord change auditions all four inversions and keeps the one whose voices move least from the previous chord.
- **Home:** Tonic gravity (0-100%). When the sequencer advances, this is the probability it follows a functional-harmony map instead of wandering: ii goes to V, V resolves to I, vii resolves to I, vi sets up ii/IV. It also scales the phrase cadencing (V into the last step of a Len-length phrase, I on the downbeat). At 0 the progression is a free random walk; at 100 it cadences constantly.
- **SecD:** Press the button to toggle secondary dominants. When On, a move to a new chord may route through that chord's own dominant first (probability scales with Home).
- **Drft:** Key drift (0-100%). On a cadence to I, this is the chance the key modulates a fifth or fourth along the circle of fifths. Root follows the drift.
- **Oct:** Transpose the whole chord by -2 to +2 octaves. Both VCO pitches shift and the subs follow automatically, since each divisor is an interval below its own VCO.
- **Slew:** Adjust the slew amount for smooth pitch/division changes.
- **Stay:** Probability (0-100%) that the sequencer stays on the current chord instead of advancing.
- **DjVu:** Probability (0-100%) of recalling a chord from the sequence history instead of generating a new one.
- **Brch:** When advancing, probability (0-100%) of jumping to a random chord instead of the next one.
- **Len:** Length of the sequence history to loop when locked (1-32).
- **EVOL / LOCK:** Toggle between evolving random progressions and looping the sequence history.

### Credits
Authored by The Phazerville Project
