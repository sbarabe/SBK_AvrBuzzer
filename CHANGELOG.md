# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project follows [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

### Changed

### Fixed

## [1.1.1] - 2026-07-16

### Added

- Added a dedicated `TunePlayer` example sketch demonstrating melody playback.

### Changed

- Improved README documentation for the `TunePlayer` API and tune definition workflow.
- Expanded API documentation with melody playback information and usage examples.
- Improved example organization and library documentation.
- Updated `TunePlayer` to include `SBK_Tune.h` directly, simplifying its use.

## [1.1.0] - 2026-07-16

### Added

- Added non-blocking `TunePlayer` for asynchronous melody playback.
- Added `Tune` data structure and `SBK_DEFINE_TUNE()` helper macro for defining melodies stored in program memory (`PROGMEM`).
- Added musical note duration constants, including common dotted durations.
- Added original example melodies and a complete `TunePlayer` Arduino example sketch.

### Changed

- Refactored melody playback into a reusable `TunePlayer` class associated with a `Buzzer` instance.
- Removed global-state dependencies from the melody playback engine, allowing multiple independent `TunePlayer` instances.
- Simplified melody creation by integrating musical note definitions, duration constants, and tune declaration helpers into `SBK_Tune.h`.
- Updated examples and documentation to reflect the new melody playback API.

## [1.0.1] - 2026-07-15

### Changed

- Updated single-ended two-pin operation so that, when pin2 is provided, it is held LOW and acts as the passive piezo buzzer return path.
- Updated API documentation to clarify the behavior of pin2 in single-ended and differential output modes.

### Fixed

- Fixed initialization of the second GPIO output so its direct port register pointer and bit mask are available in both single-ended and differential modes.
- Ensured both buzzer outputs are driven LOW when playback stops, the buzzer is muted, or the output mode changes.
- Prevented invalid access to the second GPIO port data when pin2 is provided in single-ended mode.

## [1.0.0] - 2026-07-15

### Added

- Initial public release of SBK_AvrBuzzer.
- Non-blocking tone playback using AVR Timer1 interrupts.
- Fixed-frequency tone generation.
- Linear frequency chirp generation.
- Single-ended (1-pin) output mode.
- Differential (2-pin) output mode for increased sound output.
- Runtime output mode switching.
- Mute and unmute functionality.
- Compile-time checks for AVR architecture, Timer1 availability, and `F_CPU`.
- Automatic fallback to single-ended mode when no second output pin is provided.
- Support for AVR-based Arduino boards using the Arduino framework.
- Arduino Library Manager metadata.
- PlatformIO Registry metadata.
- Example documentation and API reference.

[Unreleased]: https://github.com/sbarabe/SBK_AvrBuzzer/compare/v1.1.1...HEAD
[1.1.1]: https://github.com/sbarabe/SBK_AvrBuzzer/compare/v1.1.0...v1.1.1
[1.1.0]: https://github.com/sbarabe/SBK_AvrBuzzer/compare/v1.0.1...v1.1.0
[1.0.1]: https://github.com/sbarabe/SBK_AvrBuzzer/compare/v1.0.0...v1.0.1
[1.0.0]: https://github.com/sbarabe/SBK_AvrBuzzer/releases/tag/v1.0.0
