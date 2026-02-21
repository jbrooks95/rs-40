# RS-40 Plugin - User Manual

**Analog Preamp Emulation**
Version 1.0.0 | VST3, AU

Inspired by the vintage Ashly SC-40 Instrument Preamplifier.

---

## Overview

The RS-40 is a preamp and channel strip plugin with a 3-band semi-parametric EQ and a diode clipper distortion circuit, inspired by the Ashly SC-40 instrument preamp.

The signal path mimics the Low Level (instrument) input on the original unit, with a high-impedance input stage and 0-40 dB of gain.

### Signal Flow

```
INPUT --> GAIN --> [DIODE CLIPPER] --> LO EQ --> MID EQ --> HI EQ --> OUTPUT
                   (if DIST is on)
```

The diode clipper sits *before* the EQ section, matching the hardware's effects send/return loop placement. This means the EQ shapes the clipped signal.

---

## Controls

### Preamp Section

| Control | Range | What it does |
|---------|-------|--------------|
| **GAIN** | 0 - 100% | Input gain from unity to ~40 dB. |
| **LO +/-** | -15 to +15 dB | Low shelf boost/cut. |
| **LO FREQ** | 30 - 300 Hz | Sets the corner frequency of the low shelf. |
| **MID +/-** | -15 to +15 dB | Mid band peaking boost/cut. |
| **MID FREQ** | 160 Hz - 8 kHz | Sweeps the center frequency of the mid band. |
| **HI +/-** | -15 to +15 dB | High shelf boost/cut. |
| **HI FREQ** | 700 Hz - 8 kHz | Sets the corner frequency of the high shelf. |
| **OUTPUT** | 0 - 100% | Master output level. |

### Effects Loop - Diode Clipper

| Control | Range | What it does |
|---------|-------|--------------|
| **DIST** | On / Off | Enables the diode clipper. When off, the signal passes through clean. |
| **DRIVE** | 0.1x - 10x | Controls how hard the signal is pushed into the diodes. Low drive gives subtle saturation. High drive gives heavy, symmetrical soft clipping. |
| **BLEND** | 0 - 100% | Dry/wet mix. At 0% the signal is fully clean (even with DIST on). At 100% the signal is fully clipped. Intermediate values give parallel distortion. |

To match the original hardware mod, set DRIVE to 1.0x and BLEND to 100%. The original hardware had no drive or blend controls — the diodes were passive, wired into a phone plug that inserted directly into the effects send jack. DRIVE at 1.0x mimics this behavior.

### Character

| Control | Range | What it does |
|---------|-------|--------------|
| **ANALOG** | 0 - 100% | Controls how much op-amp saturation is modeled across the signal path. At 0% the signal path is perfectly clean. At higher values you get soft compression on peaks, subtle harmonics, and gentle high-frequency rolloff. |

### Indicators

| Indicator | Meaning |
|-----------|---------|
| **Peak LED** | Lights red when the signal at any stage in the chain approaches clipping (~-1 dB). Monitors post-input, post-effects-loop, and post-EQ levels simultaneously. When lit, it means the signal is being saturated or distorted somewhere in the chain — this is not necessarily a problem, but indicates you are driving the circuit hard. |
| **Diode Schematic** | Displays a circuit diagram of the 1N914 anti-parallel diode clipper. Wires and diodes change color when DIST is engaged, with an animated left-to-right sweep simulating signal flow. |
| **Waveform Scope** | Real-time oscilloscope in the CHARACTER section showing the output waveform. The trace color shifts from amber to crimson at higher amplitudes. |

---

## Tips

**Pair with a cabinet sim or IR loader.** The RS-40 is a preamp which generally sounds best when followed by a cabinet emulation or impulse response loader, just like the original hardware often needed a power amp and speaker cabinet to complete the signal chain.

**BLEND for parallel distortion.** Instead of running a separate parallel bus, use the BLEND knob to mix clean and clipped signal. A low BLEND (10-30%) with high DRIVE gives grit while preserving dynamics.

**Watch the peak LED.** The peak LED monitors multiple points in the signal chain, just like the original hardware. If it's lighting up, one or more stages are being driven hard. It does not mean the signal is digitally clipping, but you're in the zone where saturation is happening.

**Double-click any knob to reset it.** Double-clicking a knob returns it to its default value. This works on all rotary controls.

---

## About

The RS-40 is inspired by the Ashly SC-40, originally manufactured by Ashly Audio starting in 1979. It was a rack-mount instrument preamp designed to provide clean, low-noise first-stage amplification for a wide variety of musical instruments, with a tunable 3-band EQ, an effects loop for line-level devices, and direct outputs for connection to PA console microphone inputs.

Formats: VST3, AU
