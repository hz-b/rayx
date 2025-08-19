## Changelog

### Breaking Changes

* For h5 files, the internal data layout has changed. rayx/rayx-ui wont be able to write/read the old format

### RAYX-CORE
* New optical element: **Crystal** (https://github.com/hz-b/rayx/pull/380)
    * For more information visit our [wiki](https://hz-b.github.io): https://hz-b.github.io/rayx/Model/BeamlineObjects/OpticalElements/Crystal.html
* New optical element: **Single layer foil** (https://github.com/hz-b/rayx/pull/391)
    * For more information visit our [wiki](https://hz-b.github.io): https://hz-b.github.io/rayx/Model/BeamlineObjects/OpticalElements/Foil.html
* Beamlines are now represented as a tree graph (improved support for grouping of elements) (https://github.com/hz-b/rayx/pull/350)
* For h5 files, the internal data layout has changed (https://github.com/hz-b/rayx/pull/374)
    * Each attribute is stored separately
    * Each attribute may or may not be stored, depending on format options set (`--format ...` flag)
* May record events from a single element only (https://github.com/hz-b/rayx/pull/365)
* May record specific attributes of events only (https://github.com/hz-b/rayx/pull/374)
* Advance electric field using optical path length (https://github.com/hz-b/rayx/pull/394)
* Rework verbose mode console output (`--verbose` flag)
* Fix efficiency calculation
* Updated dependency `alpaka` to version `2.0.0` (https://github.com/alpaka-group/alpaka/releases/tag/2.0.0) (https://github.com/hz-b/rayx/pull/399)
    * Inherently, the build dependency `boost` was dropped in rayx
* By default, rayx-core now compiles for all major cuda architectures (https://github.com/hz-b/rayx/pull/399)

### RAYX (cli)

* Rename cli format options
`-F,--format TEXT            Write specific Ray attributes to output H5 files. Provide a space-separated list of attributes. default value: "path_id position_x position_y position_z event_type direction_x direction_y direction_z energy electric_field_x electric_field_y electric_field_z path_length order element_id source_id "`
* Add cli option to dump meta data of h5 and rml files
`-D,--dump TEXT              Dump the meta data of a file (h5 or rml)`
* Add cli option to record events from a single element only
`-R,--record-element INT     Record events only for a specifc element`
* Add cli option to specify output filepath of csv or h5 files (https://github.com/hz-b/rayx/pull/366)
`-o,--output TEXT            Output path or filename`

### RAYX-UI

* Improve Wayland support on Linux (https://github.com/hz-b/rayx/pull/353)
* Add flag to run in verbose mode (`--verbose` flag) (https://github.com/hz-b/rayx/pull/351)

### Other Changes
* Improved ease of compilation using `compile.sh` script (https://github.com/hz-b/rayx/pull/385)
* Add CMake option to enable/disable OpenMP backend (https://github.com/hz-b/rayx/pull/357)
* Add CMake option to build `rayx-core` as static lib (https://github.com/hz-b/rayx/pull/343)
* Enable use of rayx as a submodule in another project (https://github.com/hz-b/rayx/pull/359)
* Add shell.nix file, for starting a nix-shell environment, containing all dependencies required to build (https://github.com/hz-b/rayx/pull/389)

## New Contributors
* @JonasTrenkler made their first contribution (https://github.com/hz-b/rayx/pull/357)

## Other

**Full Changelog**: https://github.com/hz-b/rayx/compare/v0.21.2...v1.0.0

<!-- ### Runtime Performance Comparison -->
<!-- Runtime performance compared to the previous release (https://github.com/hz-b/rayx/tree/v0.21.2) -->

## Probably coming soon

* Mac support including multithreaded tracing on CPU using OpenMP (arm, x86)
* Nix flake for reproducable dependency handling to make developing and building rayx more accessible
* Multi layer foil
