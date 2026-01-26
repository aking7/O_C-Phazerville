## Applet Analysis

This document provides a detailed analysis of the CV and audio applets available in the firmware, grouped by their features.

### CV Applets

#### Sequencers
*   **Carpeggio.h**: A Cartesian arpeggiator that generates arpeggios based on a 4x4 grid of notes.
*   **DivSeq.h**: A sequential clock divider that divides an incoming clock by a sequence of user-defined ratios.
*   **DivSeq10.h**: A 10-step sequential clock divider.
*   **MiniSeq.h**: A simple 8-step sequencer.
*   **ProbabilityMelody.h**: A probabilistic melody generator.
*   **Seq32.h**: A 32-step sequencer.
*   **SeqPlay7.h**: A 7-track sequencer.
*   **SequenceX.h**: A sequencer with extended features.
*   **Shredder.h**: A Cartesian sequencer that can be used to create complex patterns of CV signals.
*   **Strum.h**: A strumming arpeggiator.
*   **TB3PO.h**: A sequencer based on the Roland TB-303.

#### Clock & Trigger Modifiers
*   **BootsNCat.h**: A Euclidean rhythm generator based on the Risset drum pattern.
*   **Brancher.h**: A Bernoulli gate, which is a probabilistic trigger router.
*   **Burst.h**: A burst generator that produces a user-defined number of triggers at a specified rate.
*   **ClkToGate.h**: A clock to gate converter.
*   **ClockDivider.h**: A clock divider.
*   **ClockSkip.h**: A probabilistic clock skipper.
*   **EuclidX.h**: A Euclidean rhythm generator with extended features.
*   **GateDelay.h**: A gate delay.
*   **PolyDiv.h**: A polyrhythmic clock divider.
*   **ProbabilityDivider.h**: A probabilistic clock divider.
*   **ShiftGate.h**: A gate shifter.
*   **Shuffle.h**: A clock shuffler that can add swing to a clock signal.
*   **TrigSeq.h**: A trigger sequencer.
*   **TrigSeq16.h**: A 16-step trigger sequencer.

#### CV Generators & Modulators
*   **ADEG.h**: Attack-Decay Envelope Generator.
*   **ADSREG.h**: Attack-Decay-Sustain-Release Envelope Generator.
*   **BugCrack.h**: A chaotic signal generator based on the Lorenz attractor.
*   **EbbAndLfo.h**: A dual LFO with a variety of waveforms and modulation options.
*   **GameOfLife.h**: A cellular automaton based on Conway's Game of Life.
*   **LowerRenz.h**: A chaotic signal generator based on the Lorenz attractor.
*   **Pigeons.h**: A dual random voltage generator based on the Pigeon chaotic system.
*   **RndWalk.h**: A random walk generator.
*   **RunglBook.h**: A chaotic signal generator based on the Rungler algorithm.
*   **Stairs.h**: A staircase waveform generator.
*   **VectorEG.h**: A vector envelope generator.
*   **VectorLFO.h**: A vector LFO.

#### CV Processors & Utilities
*   **ASR.h**: Analog Shift Register.
*   **AttenuateOffset.h**: Attenuverter and Offset generator.
*   **Binary.h**: Binary counter.
*   **CVRecV2.h**: A CV recorder.
*   **Calculate.h**: A simple calculator for CV signals.
*   **Chordinator.h**: A chord generator.
*   **Combin8.h**: A logic gate module.
*   **Compare.h**: A CV comparator.
*   **DrLoFi.h**: A sample rate and bit depth reducer.
*   **DualQuant.h**: A dual quantizer.
*   **DuoTET.h**: A dual quantizer for custom tuning systems.
*   **EnvFollow.h**: An envelope follower.
*   **GatedVCA.h**: A voltage-controlled amplifier controlled by a gate.
*   **Logic.h**: A logic gate module.
*   **MultiScale.h**: A multi-scale quantizer.
*   **Palimpsest.h**: A CV recorder.
*   **ScaleDuet.h**: A dual quantizer.
*   **Schmitt.h**: A Schmitt trigger.
*   **ShiftReg.h**: A shift register.
*   **Slew.h**: A slew limiter.
*   **Squanch.h**: A sample and hold module.
*   **Switch.h**: A CV-controlled switch.
*   **SwitchSeq.h**: A sequential switch.
*   **TLNeuron.h**: A neural network-based CV processor.
*   **Trending.h**: A trend detector.
*   **Xfader.h**: A crossfader.

#### Oscillators
*   **EnsOscKey.h**: An ensemble oscillator.
*   **TwoRings.h**: A dual ring modulator.
*   **VectorMod.h**: A vector modulator.
*   **VectorMorph.h**: A vector morphing oscillator.
*   **WTVCO.h**: A wavetable voltage-controlled oscillator.

#### System & MIDI
*   **Button.h**: A dual momentary button emulator.
*   **Calibr8.h**: A calibration tool.
*   **ClockSetup.h**: A global clock configuration tool.
*   **ClockSetupT4.h**: A global clock configuration tool for the Teensy 4.1.
*   **Metronome.h**: A metronome.
*   **MidiLoop.h**: A MIDI looper.
*   **ResetClock.h**: A clock that can be reset.
*   **Scope.h**: An oscilloscope.
*   **Tuner.h**: A tuner.
*   **Voltage.h**: A voltage source.
*   **hMIDIIn.h**: A MIDI input module.
*   **hMIDIOut.h**: A MIDI output module.

#### Misc/Unique
*   **Cumulus.h**: A granular CV processor based on the Mutable Instruments Clouds module.
*   **DrumMap.h**: A drum map.
*   **EnigmaJr.h**: A simplified version of the Enigma machine.

---

### Audio Applets

#### Effects
*   **CrosspanApplet.h**: A stereo panner.
*   **DelayApplet.h**: A delay effect.
*   **DynamicsApplet.h**: A dynamics processor (compressor, expander, or gate).
*   **FilterFolderApplet.h**: A wavefolder and filter effect.
*   **FreeverbApplet.h**: A reverb effect.
*   **LadderApplet.h**: A ladder filter.
*   **MidSideApplet.h**: A mid-side processor.
*   **SamverbApplet.h**: A reverb effect.

#### Sound Sources
*   **HandSawApplet.h**: A sawtooth wave oscillator.
*   **OscApplet.h**: An oscillator with a variety of waveforms.
*   **WAVPlayerApplet.h**: A WAV file player.

#### Utilities
*   **InputApplet.h**: An input module for routing audio signals.
*   **PassthruApplet.h**: A passthrough module.
*   **UpsampledApplet.h**: An upsampling module.
*   **VCAApplet.h**: A voltage-controlled amplifier.
