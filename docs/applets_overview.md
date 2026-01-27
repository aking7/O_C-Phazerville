# Applets, Apps, and Applet Analysis

This document provides a detailed analysis of the CV and audio applets available in the firmware, grouped by their features. It also includes a list of the available apps.

## CV Applets

#### Sequencers
*   **[Carpeggio](_applets/Carpeggio.md)**: A Cartesian arpeggiator that generates arpeggios based on a 4x4 grid of notes.
*   **[DivSeq](_applets/DivSeq.md)**: A sequential clock divider that divides an incoming clock by a sequence of user-defined ratios.
*   **DivSeq10.h**: A 10-step sequential clock divider.
*   **[MiniSeq](_applets/Sequins.md)**: A simple 8-step sequencer.
*   **[ProbabilityMelody](_applets/ProbMeloD.md)**: A probabilistic melody generator.
*   **[Seq32](_applets/Seq32.md)**: A 32-step sequencer.
*   **[SeqPlay7](_applets/SeqPlay7.md)**: A 7-track sequencer.
*   **[SequenceX](_applets/SequenceX.md)**: A sequencer with extended features.
*   **[Shredder](_applets/Shredder.md)**: A Cartesian sequencer that can be used to create complex patterns of CV signals.
*   **[Strum](_applets/Strum.md)**: A strumming arpeggiator.
*   **[TB3PO](_applets/TB-3PO.md)**: A sequencer based on the Roland TB-303.

#### Clock & Trigger Modifiers
*   **[BootsNCat](_applets/BootsNCat.md)**: A Euclidean rhythm generator based on the Risset drum pattern.
*   **[Brancher](_applets/Brancher.md)**: A Bernoulli gate, which is a probabilistic trigger router.
*   **[Burst](_applets/Burst.md)**: A burst generator that produces a user-defined number of triggers at a specified rate.
*   **[ClkToGate](_applets/Clk2Gate.md)**: A clock to gate converter.
*   **[ClockDivider](_applets/ClockDivider.md)**: A clock divider.
*   **[ClockSkip](_applets/Clock-Skipper.md)**: A probabilistic clock skipper.
*   **[EuclidX](_applets/EuclidX.md)**: A Euclidean rhythm generator with extended features.
*   **[GateDelay](_applets/Gate-Delay.md)**: A gate delay.
*   **[PolyDiv](_applets/PolyDiv.md)**: A polyrhythmic clock divider.
*   **[ProbabilityDivider](_applets/ProbDiv.md)**: A probabilistic clock divider.
*   **[ShiftGate](_applets/ShiftGate.md)**: A gate shifter.
*   **[Shuffle](_applets/Shuffle.md)**: A clock shuffler that can add swing to a clock signal.
*   **[TrigSeq](_applets/TrigSeq.md)**: A trigger sequencer.
*   **[TrigSeq16](_applets/TrigSeq16.md)**: A 16-step trigger sequencer.

#### CV Generators & Modulators
*   **[ADEG](_applets/AD-EG.md)**: Attack-Decay Envelope Generator.
*   **[ADSREG](_applets/ADSR-EG.md)**: Attack-Decay-Sustain-Release Envelope Generator.
*   **[BugCrack](_applets/BugCrack.md)**: A chaotic signal generator based on the Lorenz attractor.
*   **[EbbAndLfo](_applets/Ebb-&-LFO.md)**: A dual LFO with a variety of waveforms and modulation options.
*   **[GameOfLife](_applets/GameOfLife.md)**: A cellular automaton based on Conway's Game of Life.
*   **[LowerRenz](_applets/LowerRenz.md)**: A chaotic signal generator based on the Lorenz attractor.
*   **[Pigeons](_applets/Pigeons.md)**: A dual random voltage generator based on the Pigeon chaotic system.
*   **[RndWalk](_applets/Random-Walk.md)**: A random walk generator.
*   **[RunglBook](_applets/RunglBook.md)**: A chaotic signal generator based on the Rungler algorithm.
*   **[Stairs](_applets/Stairs.md)**: A staircase waveform generator.
*   **[VectorEG](_applets/VectorEG.md)**: A vector envelope generator.
*   **[VectorLFO](_applets/VectorLFO.md)**: A vector LFO.

#### CV Processors & Utilities
*   **[ASR](_applets/ASR.md)**: Analog Shift Register.
*   **[AttenuateOffset](_applets/AttenOff.md)**: Attenuverter and Offset generator.
*   **[Binary](_applets/Binary-Counter.md)**: Binary counter.
*   **[CVRecV2](_applets/CV-Recorder.md)**: A CV recorder.
*   **[Calculate](_applets/Calculate.md)**: A simple calculator for CV signals.
*   **[Chordinator](_applets/Chordinate.md)**: A chord generator.
*   **Combin8.h**: A logic gate module.
*   **[Compare](_applets/Compare.md)**: A CV comparator.
*   **[DrLoFi](_applets/Dr.-LoFi.md)**: A sample rate and bit depth reducer.
*   **[DualQuant](_applets/Dual-Quantizer.md)**: A dual quantizer.
*   **[DuoTET](_applets/DuoTET.md)**: A dual quantizer for custom tuning systems.
*   **[EnvFollow](_applets/Envelope-Follower.md)**: An envelope follower.
*   **[GatedVCA](_applets/Gated-VCA.md)**: A voltage-controlled amplifier controlled by a gate.
*   **[Logic](_applets/Logic.md)**: A logic gate module.
*   **[MultiScale](_applets/MultiScale.md)**: A multi-scale quantizer.
*   **[Palimpsest](_applets/Palimpsest.md)**: A CV recorder.
*   **[ScaleDuet](_applets/ScaleDuet.md)**: A dual quantizer.
*   **[Schmitt](_applets/Schmitt-Trigger.md)**: A Schmitt trigger.
*   **[ShiftReg](_applets/DualTM.md)**: A shift register.
*   **[Slew](_applets/Slew.md)**: A slew limiter.
*   **[Squanch](_applets/Squanch.md)**: A sample and hold module.
*   **[Switch](_applets/Switch.md)**: A CV-controlled switch.
*   **[SwitchSeq](_applets/Switch-Seq.md)**: A sequential switch.
*   **[TLNeuron](_applets/Threshold-Logic-Neuron.md)**: A neural network-based CV processor.
*   **[Trending](_applets/Trending.md)**: A trend detector.
*   **[Xfader](_applets/Xfader.md)**: A crossfader.

#### Oscillators
*   **[EnsOscKey](_applets/EnsOscKey.md)**: An ensemble oscillator.
*   **[TwoRings](_applets/TwoRings.md)**: A dual ring modulator.
*   **[VectorMod](_applets/VectorMod.md)**: A vector modulator.
*   **[VectorMorph](_applets/VectorMorph.md)**: A vector morphing oscillator.
*   **[WTVCO](_applets/WTVCO.md)**: A wavetable voltage-controlled oscillator.

#### System & MIDI
*   **[Button](_applets/Button2.md)**: A dual momentary button emulator.
*   **[Calibr8](_applets/Calibr8.md)**: A calibration tool.
*   **[ClockSetup](Clock-Setup.md)**: A global clock configuration tool.
*   **ClockSetupT4.h**: A global clock configuration tool for the Teensy 4.1.
*   **[Metronome](_applets/Metronome.md)**: A metronome.
*   **MidiLoop.h**: A MIDI looper.
*   **[ResetClock](_applets/Reset-Clock.md)**: A clock that can be reset.
*   **[Scope](_applets/Scope.md)**: An oscilloscope.
*   **[Tuner](_applets/Tuner.md)**: A tuner.
*   **[Voltage](_applets/Voltage.md)**: A voltage source.
*   **[hMIDIIn](_applets/MIDI-Input.md)**: A MIDI input module.
*   **[hMIDIOut](_applets/MIDI-Out.md)**: A MIDI output module.

#### Misc/Unique
*   **[Cumulus](_applets/Cumulus.md)**: A granular CV processor based on the Mutable Instruments Clouds module.
*   **[DrumMap](_applets/DrumMap.md)**: A drum map.
*   **[EnigmaJr](_applets/Enigma-Jr.md)**: A simplified version of the Enigma machine.

---

## Audio Applets

#### Effects
*   **[Crosspan](_audio_applets/Crosspan.md)**: A stereo panner.
*   **[Delay](_audio_applets/Delay.md)**: A delay effect.
*   **DynamicsApplet.h**: A dynamics processor (compressor, expander, or gate).
*   **[FilterFolder](_audio_applets/FoldMMF.md)**: A wavefolder and filter effect.
*   **FreeverbApplet.h**: A reverb effect.
*   **[Ladder](_audio_applets/LadderApplet.md)**: A ladder filter.
*   **[MidSide](_audio_applets/MidSide.md)**: A mid-side processor.
*   **SamverbApplet.h**: A reverb effect.

#### Sound Sources
*   **HandSawApplet.h**: A sawtooth wave oscillator.
*   **[Osc](_audio_applets/Osc.md)**: An oscillator with a variety of waveforms.
*   **WAVPlayerApplet.h**: A WAV file player.

#### Utilities
*   **[Input](_audio_applets/Input.md)**: An input module for routing audio signals.
*   **PassthruApplet.h**: A passthrough module.
*   **[Upsampled](_audio_applets/Upsampled.md)**: An upsampling module.
*   **[VCA](_audio_applets/VCA.md)**: A voltage-controlled amplifier.

---

## Apps
*   **[Acid Curds](_apps/Acid-Curds.md)**
*   **[Automatonnetz](_apps/Automatonnetz.md)**
*   **[Backup and Restore](_apps/Backup-and-Restore.md)**
*   **[Calibr8or](_apps/Calibr8or.md)**
*   **[Captain MIDI](_apps/Captain-MIDI.md)**
*   **[CopierMaschine](_apps/CopierMaschine.md)**
*   **[Dialectic Ping-Pong](_apps/Dialectic-Ping-Pong.md)**
*   **[Enigma](_apps/Enigma.md)**
*   **[Harrington 1200](_apps/Harrington-1200.md)**
*   **[Low-rents](_apps/Low-rents.md)**
*   **[Meta-Q](_apps/Meta-Q.md)**
*   **[Neural Net](_apps/Neural-Net.md)**
*   **[Passencore](_apps/Passencore.md)**
*   **[Piqued](_apps/Piqued.md)**
*   **[Pong](_apps/Pong.md)**
*   **[Quadrants](_apps/Quadrants.md)**
*   **[Quadraturia](_apps/Quadraturia.md)**
*   **[Quantermain](_apps/Quantermain.md)**
*   **[References](_apps/References.md)**
*   **[Scale Editor](_apps/Scale-Editor.md)**
*   **[Scenery](_apps/Scenery.md)**
*   **[Scenes](_apps/Scenes.md)**
*   **[Setup & About](_apps/Setup-About.md)**
*   **[The Darkest Timeline](_apps/The-Darkest-Timeline.md)**
*   **[Viznutcracker, sweet!](_apps/Viznutcracker-sweet.md)**
*   **[Waveform Editor](_apps/Waveform-Editor.md)**
