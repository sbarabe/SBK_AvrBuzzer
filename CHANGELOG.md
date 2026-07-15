# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project follows [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

### Changed

### Fixed

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

[Unreleased]: https://github.com/sbarabe/SBK_AvrBuzzer/compare/v1.0.0...HEAD
[1.0.0]: https://github.com/sbarabe/SBK_AvrBuzzer/releases/tag/v1.0.0
