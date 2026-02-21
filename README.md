# RS-40 — Analog Preamp Emulation Plugin

A preamp emulation plugin inspired by the Ashly SC-40.

## Features

- **Input Stage** — Variable gain (0–40dB) with built-in subsonic filter at ~34Hz
- **3-Band Semi-Parametric EQ**
  - Lo: ±15dB, sweepable 30–300 Hz
  - Mid: ±15dB, sweepable 160 Hz–8 kHz
  - Hi: ±15dB, sweepable 700 Hz–8 kHz
- **Diode Clipper** — 1N914 anti-parallel clipping circuit with drive and blend controls
- **Analog Character Control** — Dial in RC4558 op-amp saturation across the signal path
- **Peak LED** — Multi-point peak buss monitoring
- **Real-Time Visualizations** — Animated diode schematic and waveform oscilloscope

Formats: **VST3** (macOS, Windows) | **AU** (macOS)

## Project Structure

```
RS-40/
├── CMakeLists.txt              # Build configuration
├── LICENSE                     # GPL-3.0
├── README.md                   # This file
├── MANUAL.md                   # User manual
├── Resources/
│   ├── IBMPlexMono-Bold.ttf    # Embedded UI font
│   └── LICENSE-IBMPlexMono.txt # SIL Open Font License
├── JUCE/                       # ← Clone JUCE here (not included)
├── .github/workflows/          # CI/CD (auto build + release)
└── Source/
    ├── PluginProcessor.h/cpp   # Main audio processor
    ├── PluginEditor.h/cpp      # GUI
    ├── RS40LookAndFeel.h       # Custom knob/button styling
    └── DSP/
        ├── RS40Channel.h/cpp   # Complete channel signal chain
        ├── InputStage.h/cpp    # Input gain + subsonic HPF
        ├── EQSection.h/cpp     # 3-band semi-parametric EQ
        ├── DiodeClipper.h/cpp  # 1N914 anti-parallel diode model
        ├── OutputStage.h/cpp   # Output buffering + pad
        ├── RC4558Model.h/cpp   # Op-amp saturation model
        ├── BiquadFilter.h      # Biquad filter (EQ workhorse)
        └── Oversampling.h      # Anti-aliased oversampling
```

## Prerequisites

1. **C++ compiler** with C++17 support
   - Windows: Visual Studio 2022 (Community is free)
   - macOS: Xcode 15+ (Command Line Tools)

2. **CMake** 3.22 or newer

3. **JUCE Framework** v8.x — clone into the project directory:
   ```bash
   git clone --depth 1 --branch 8.0.12 https://github.com/juce-framework/JUCE.git
   ```

## Building

### macOS

```bash
# Configure
SDK=$(xcrun --sdk macosx --show-sdk-path)
CPLUS_INCLUDE_PATH="${SDK}/usr/include/c++/v1" cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Build
CPLUS_INCLUDE_PATH="${SDK}/usr/include/c++/v1" cmake --build build --config Release
```

Artifacts land in `build/RS40_artefacts/Release/`:
- `VST3/RS-40.vst3`
- `AU/RS-40.component`
- `Standalone/RS-40.app`

AU and VST3 are automatically copied to `~/Library/Audio/Plug-Ins/`.

### Windows

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The VST3 will be in `build/RS40_artefacts/Release/VST3/RS-40.vst3`.

## Testing

The **Standalone** target lets you test without a DAW:

```bash
# macOS
open "build/RS40_artefacts/Release/Standalone/RS-40.app"

# Windows
build\RS40_artefacts\Release\Standalone\RS-40.exe
```

For DAW testing, copy the VST3 to your plugin folder:
- Windows: `C:\Program Files\Common Files\VST3\`
- macOS: `~/Library/Audio/Plug-Ins/VST3/` (done automatically by the build)

Then rescan plugins in your DAW and insert "RS-40" on a track.

## License

This project is licensed under the [AGPLv3](LICENSE).

The IBM Plex Mono font is licensed under the [SIL Open Font License 1.1](Resources/LICENSE-IBMPlexMono.txt).

The original Ashly SC-40 is a product of Ashly Audio Inc. This software is an independent emulation and is not affiliated with or endorsed by Ashly Audio.