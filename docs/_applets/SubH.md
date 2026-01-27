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

### I/O

|        |        Left / Slot 1        |        Right / Slot 2        |
| ------ | :-------------------------: | :--------------------------: |
| TRIG   |            Clock            |            Reset             |
| CV INs |      Chord Modulation       |     Inversion Modulation     |
| OUTs   | SUB 1_1 / 2_1 CV (0-5V) | SUB 1_2 / 2_2 CV (0-5V) |

### UI Parameters

#### Page 1: Harmonic
- **Scale:** Choose the musical scale.
- **Root:** Set the root note of the scale.
- **Chord:** Select the base chord (I, ii, iii, etc.).
- **Inv:** Set the chord inversion (0-3).
- **Slw:** Adjust the slew amount for smooth division changes.

#### Page 2: Rhythmic
- **Stay:** Probability (0-100%) that the sequencer stays on the current chord instead of advancing.
- **Loop:** Length of the sequence history to loop when locked (1-32).
- **EVOLVE / LOCKED:** Toggle between evolving random progressions and looping the sequence history.

### Credits
Authored by The Phazerville Project
