---
layout: default
---
# SubH

**SubH** is an applet designed to pair with the **Moog Subharmonicon**. It translates musical notes into the closest subharmonic divisions and features a probability-based sequencer.

### Key Features:
- **Subharmonic Snapping:** Automatically finds the closest integer division (1-16) for a given note in the selected scale.
- **Linked 4-Output Mode:** When two instances of SubH are loaded in a Hemisphere pair, they link together to share scale, chord, and sequencer state, providing 4 synchronized outputs.
- **Probability Sequencer:** A rhythmic engine that evolves chord progressions based on a "Stay" probability.
- **Loop Lock:** The "Lock" feature allows you to freeze the current progression and loop it indefinitely.
- **Slew/Morphing:** Adjustable slew for smooth transitions between subharmonic divisions.

### Patching to the Subharmonicon

Patch the outputs to the **VCO 1 SUB** / **VCO 2 SUB** inputs and set the **SUB FREQ knobs to their center position** (as the Subharmonicon manual specifies for bipolar CV control). The SUB CV inputs read -5V to +5V as divisor 16 down to 1 in 16 equal zones of 0.625V; SubH outputs the center of the zone for each divisor (divisor 1 = +4.69V, 8 = +0.31V, 16 = -4.69V).

*Note:* on hardware whose DAC floor is -3V (classic o_C), divisors above 13 clamp to ~13.

### I/O

|        |        Left / Slot 1        |        Right / Slot 2        |
| ------ | :-------------------------: | :--------------------------: |
| TRIG   |            Clock            |            Reset             |
| CV INs |      Chord Modulation       |     Inversion Modulation     |
| OUTs   | SUB 1_1 / 2_1 CV (±4.7V) | SUB 1_2 / 2_2 CV (±4.7V) |

### UI Parameters

The page switches automatically as the cursor moves between the Harmonic and Rhythmic settings.

#### Page 1: Harmonic
- **Scale:** Choose the musical scale.
- **Root:** Set the root note of the scale.
- **Chrd:** Select the base chord (I, ii, iii, etc.).
- **Inv:** Set the chord inversion (0-3).
- **Slew:** Adjust the slew amount for smooth division changes.

#### Page 2: Rhythmic
- **Stay:** Probability (0-100%) that the sequencer stays on the current chord instead of advancing.
- **DjVu:** Probability (0-100%) of recalling a chord from the sequence history instead of generating a new one.
- **Brch:** When advancing, probability (0-100%) of jumping to a random chord instead of the next one.
- **Len:** Length of the sequence history to loop when locked (1-32).
- **EVOL / LOCK:** Toggle between evolving random progressions and looping the sequence history.

### Credits
Authored by The Phazerville Project
