## Applet Analysis

This document provides a detailed analysis of the CV and audio applets available in the firmware.

### CV Applets

*   **ADEG.h**: Attack-Decay Envelope Generator.
*   **ADSREG.h**: Attack-Decay-Sustain-Release Envelope Generator.
*   **ASR.h**: Analog Shift Register. ASR is a sample and hold module that stores up to 64 values and can be linked with another ASR.
*   **AttenuateOffset.h**: Attenuverter and Offset generator. It can attenuate, invert, and offset incoming CV signals.
*   **Binary.h**: Binary counter. It takes four inputs (2 digital, 2 CV) and outputs a binary-weighted sum and a count of the high bits.
*   **BootsNCat.h**: A Euclidean rhythm generator based on the Risset drum pattern. It can generate complex rhythmic patterns from a few simple parameters.
*   **Brancher.h**: A Bernoulli gate, which is a probabilistic trigger router. It routes an incoming trigger to one of two outputs based on a probability setting.
*   **BugCrack.h**: A chaotic signal generator based on the Lorenz attractor. It produces three chaotic CV signals.
*   **Burst.h**: A burst generator that produces a user-defined number of triggers at a specified rate.
*   **Button.h**: A dual momentary button emulator. It can be configured to output triggers or gates.
*   **CVRecV2.h**: A CV recorder that can record and playback up to 64 seconds of CV.
*   **Calculate.h**: A simple calculator for CV signals. It can perform basic arithmetic operations on two CV inputs.
*   **Calibr8.h**: A calibration tool for the module's CV inputs and outputs.
*   **Carpeggio.h**: A Cartesian arpeggiator. It generates arpeggios based on a 4x4 grid of notes.
*   **Chordinator.h**: A chord generator that can create chords from a single CV input.
*   **ClkToGate.h**: A clock to gate converter. It can convert a clock signal into a gate of a specified length.
*   **ClockDivider.h**: A clock divider that can divide an incoming clock by a user-defined ratio.
*   **ClockSetup.h**: A global clock configuration tool.
*   **ClockSetupT4.h**: A global clock configuration tool for the Teensy 4.1.
*   **ClockSkip.h**: A probabilistic clock skipper. It will randomly skip incoming clock pulses based on a probability setting.
*   **Combin8.h**: A logic gate module that can perform various logic operations on two inputs.
*   **Compare.h**: A CV comparator. It compares two CV inputs and outputs a gate signal when one is greater than the other.
*   **Cumulus.h**: A granular CV processor based on the Mutable Instruments Clouds module.
*   **DivSeq.h**: A sequential clock divider. It divides an incoming clock by a sequence of user-defined ratios.
*   **DivSeq10.h**: A 10-step sequential clock divider.
*   **DrLoFi.h**: A sample rate and bit depth reducer. It can be used to create lo-fi CV effects.
*   **DrumMap.h**: A drum map that can map incoming triggers to different drum sounds.
*   **DualQuant.h**: A dual quantizer that can quantize two CV inputs to a user-defined scale.
*   **DuoTET.h**: A dual quantizer that can quantize two CV inputs to a user-defined tuning system.
*   **EbbAndLfo.h**: A dual LFO with a variety of waveforms and modulation options.
*   **EnigmaJr.h**: A simplified version of the Enigma machine, a famous cipher machine.
*   **EnsOscKey.h**: An ensemble oscillator that can generate up to 8 voices.
*   **EnvFollow.h**: An envelope follower that can extract the amplitude envelope of an audio signal.
*   **EuclidX.h**: A Euclidean rhythm generator with extended features.
*   **GameOfLife.h**: A cellular automaton based on Conway's Game of Life.
*   **GateDelay.h**: A gate delay that can delay an incoming gate signal by a user-defined amount.
*   **GatedVCA.h**: A voltage-controlled amplifier that is controlled by a gate signal.
*   **Logic.h**: A logic gate module that can perform various logic operations on two inputs.
*   **LowerRenz.h**: A chaotic signal generator based on the Lorenz attractor.
*   **Metronome.h**: A metronome that can be used to generate a clock signal.
*   **MidiLoop.h**: A MIDI looper that can record and playback MIDI notes.
*   **MiniSeq.h**: A simple 8-step sequencer.
*   **MultiScale.h**: A multi-scale quantizer that can quantize a CV input to multiple scales simultaneously.
*   **Palimpsest.h**: A CV recorder that can record and playback up to 64 seconds of CV.
*   **Pigeons.h**: A dual random voltage generator based on the Pigeon chaotic system.
*   **PolyDiv.h**: A polyrhythmic clock divider.
*   **ProbabilityDivider.h**: A probabilistic clock divider.
*   **ProbabilityMelody.h**: A probabilistic melody generator.
*   **ResetClock.h**: A clock that can be reset by an external trigger.
*   **RndWalk.h**: A random walk generator.
*   **RunglBook.h**: A chaotic signal generator based on the Rungler algorithm.
*   **ScaleDuet.h**: A dual quantizer that can quantize two CV inputs to a user-defined scale.
*   **Schmitt.h**: A Schmitt trigger, which is a comparator with hysteresis.
*   **Scope.h**: An oscilloscope that can be used to visualize CV signals.
*   **Seq32.h**: A 32-step sequencer.
*   **SeqPlay7.h**: A 7-track sequencer.
*   **SequenceX.h**: A sequencer with extended features.
*   **ShiftGate.h**: A gate shifter that can shift the timing of a gate signal.
*   **ShiftReg.h**: A shift register that can be used to create complex patterns of CV signals.
*   **Shredder.h**: A Cartesian sequencer that can be used to create complex patterns of CV signals.
*   **Shuffle.h**: A clock shuffler that can add swing to a clock signal.
*   **Slew.h**: A slew limiter that can be used to smooth out CV signals.
*   **Squanch.h**: A sample and hold module with a variety of features.
*   **Stairs.h**: A staircase waveform generator.
*   **Strum.h**: A strumming arpeggiator.
*   **Switch.h**: A CV-controlled switch.
*   **SwitchSeq.h**: A sequential switch.
*   **TB3PO.h**: A sequencer based on the Roland TB-303.
*   **TLNeuron.h**: A neural network-based CV processor.
*   **Trending.h**: A trend detector that can detect rising, falling, and steady CV signals.
*   **TrigSeq.h**: A trigger sequencer.
*   **TrigSeq16.h**: A 16-step trigger sequencer.
*   **Tuner.h**: A tuner that can be used to tune oscillators.
*   **TwoRings.h**: A dual ring modulator.
*   **VectorEG.h**: A vector envelope generator.
*   **VectorLFO.h**: A vector LFO.
*   **VectorMod.h**: A vector modulator.
*   **VectorMorph.h**: A vector morphing oscillator.
*   **Voltage.h**: A voltage source that can be used to generate a constant CV signal.
*   **WTVCO.h**: A wavetable voltage-controlled oscillator.
*   **Xfader.h**: A crossfader that can be used to mix two CV signals.
*   **hMIDIIn.h**: A MIDI input module.
*   **hMIDIOut.h**: A MIDI output module.

### Audio Applets

*   **CrosspanApplet.h**: A stereo panner.
*   **DelayApplet.h**: A delay effect.
*   **DynamicsApplet.h**: A dynamics processor that can be used as a compressor, expander, or gate.
*   **FilterFolderApplet.h**: A wavefolder and filter effect.
*   **FreeverbApplet.h**: A reverb effect.
*   **HandSawApplet.h**: A sawtooth wave oscillator.
*   **InputApplet.h**: An input module that can be used to route audio signals into the module.
*   **LadderApplet.h**: A ladder filter.
*   **MidSideApplet.h**: A mid-side processor.
*   **OscApplet.h**: An oscillator with a variety of waveforms.
*   **PassthruApplet.h**: A passthrough module that can be used to route audio signals through the module without processing.
*   **SamverbApplet.h**: A reverb effect.
*   **UpsampledApplet.h**: An upsampling module that can be used to increase the sample rate of an audio signal.
*   **VCAApplet.h**: A voltage-controlled amplifier.
*   **WAVPlayerApplet.h**: A WAV file player.
