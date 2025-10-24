## Changelog

### RAYX-CORE

* New optical element: **Multi Layer Mirror**
    * support for up to 1000 coatings
* New source: **Ray List Source**
    * user can provide a list of rays to be taced
* Generate rays on selected device
    before, rays were generated from sources on the cpu, singlethreaded
    now, ray generation will take place on the selected device, gpu or cpu, multithreaded
    * record generated rays, so they can be analyzed and plotted by the user
* Changes in Ray attributes
    * refactor `path_length` to `optical_path_length`
        before, represented the geometrical path length
        now, represents the optical path length
    * refactor `element_id` to `object_id`
        before, an event refered to an element
        now an event may relate to generated rays from a source or element
* Drop support for misalignment
    * misalignment was an artifact from RAY-UI, which was incomplete in rayx, incapable of applying translational and rotational adjustments correctly. now it is removed, making space for a new design of this concept
* Rework reading and writing rays to csv file
    * improve readability of csv files
        * allow scientific notation
        * use precise number of digits for floating point numbers
        * add ability to write integers and strings
    * use ray attribute mask, to determine what attributes are written
* Extend beamline tree (`class BeamlineNode`, `class Group`)
    * add node to tree, release node from tree, reparent node, find node
    * add bijection between sources/elements and object_id
* Extend `struct Rays` (structure of arrays)
* Make more types be used in type-safe manner, in preparation to make them available from rayx-python (https://github.com/hz-b/rayx/pull/415)
* Several performance optimizations
    * use rays in SoA fashion, including gpu kernels, allows for masking recorded attributes as early as possible
* Fix energy distribuition type: list of weighted values for photon energy (dat file)
* Fix single precision calculation in conversion from global to local electric field and calculation of degree of polarization. use double precision

### RAYX (cli)

* Add cli option to override the number of rays in the sources of the traced beamline
`-n,--number-of-rays INT     Override the number of rays for all sources`
* Add cli option to sort output events by object_id. This can speed-up analysis when plotting per object
`-O,--sort-by-object-id      Sort rays by object_id before writing to output file`

* Enable usage of option `-o` to specify output directory of trace results for multiple rml inputs

* rework cli parsing
    * add capability to positional arguments
        by default arguments will be treated as RML input
        e.g. `./rayx my_beamline.rml my_other_beamline.rml`
        this allows for double clicking rml files in the file manager to run rayx. just set rayx as default application for file extension `.rml`
    * add capability to array-like arguments
        e.g. `rayx my_beamline.rml -A position_x position_z`
    * use `--` to terminate the list of values to an array-like argument)
        e.g. `./rayx -A position_x position_z -- my_beamline.rml`

* Rename some of the cli arguments
* Validate output events

### Other Changes

* Fallback to single-threaded tracing on CPU when OpenMP is not available during compilation

## Other

**Full Changelog**: https://github.com/hz-b/rayx/compare/v0.1.0...v1.0.0
