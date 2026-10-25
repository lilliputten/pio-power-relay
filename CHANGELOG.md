<!--
 @since 2026.10.25
 @changed 2026.10.25, 21:20
-->

# CHANGELOG

## [Issue #1: Relay/SD/WiFi/web server, power plans, info screens & hardware button control](https://github.com/lilliputten/pio-power-relay/issues/1) #[v.0.0.1](https://github.com/lilliputten/pio-power-relay/releases/tag/v.0.0.1) - 2026.10.25

Already implemented the core features: It takes the project from a bare relay module to a feature-complete controller with persistent data storage, WiFi connectivity, a web API, on-device TFT UI with info screens, scheduled power plans, temperature sensing, and hardware button interaction.

Relay & Hardware:

- Bare relay module driver
- Relay demo code
- `HwButton` module: single press to freeze/release, double press to switch to the next info screen

Storage & Data:

- SD card read/write demo
- Data files support (SD or LittleFS) including data/strings file routines
- Data objects migrated to `std::vector` types
- Data file format command updated to find **all** source files via a Node script

Scheduling & Power Management:

- `PowerPlan` object: manages a single power plan slot list and drives the corresponding power relay
- `TimeUtils` utility module (`getSecPeriodSteps`) with tests
- Tests infrastructure added

WiFi & Web:

- WiFi configuration in both **AP** and **STA** modes
- Simple web server
- REST API methods to get/update slots data
- JSON status endpoints for power plan and power station
- Regex-based URLs conditionally disabled to free ~260K of flash

Display & UI:

- Setup information displayed on TFT screen
- TFT scrolling support (initialization info rendered via `tftUtils->writeLine`)
- `InfoScreens` module with core loop and display routines
- Info screens for: `TEMP_SENSOR`, `WIFI_NETWORK`, `WIFI_ACCESS_POINT`, `POWER_CHANNEL`

Sensors & Station:

- `PowerStation` and `TempSensor` modules
- Power station JSON status method

Known Issues / Follow-ups:

- FS initialization was broken during TFT scrolling work (commit `af2623c`) — needs verification/fix
- `PowerPlan` relay management was in progress at introduction — confirm completeness

Testing:

- Added tests for `TimeUtils`, including `getSecPeriodSteps` and subsequent test updates

Commit Range:

`42bd6cc` → `b7d69e1`

See also:

- [PR](https://github.com/lilliputten/pio-power-relay/pull/2)
- [Compare versions](https://github.com/lilliputten/pio-power-relay/compare/v.0.0.1...v.0.0.2)

## [v.0.0.1](https://github.com/lilliputten/pio-power-relay/releases/tag/v.0.0.1)

- Basic features.

## [v.0.0.0](https://github.com/lilliputten/pio-power-relay/releases/tag/v.0.0.0)

- Created initial project.
