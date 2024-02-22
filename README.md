# RAYX

RAYX is a simulation tool designed for beamlines at electron storage rings. Currently, it is intended to be used in conjunction with [RAY-UI](https://www.helmholtz-berlin.de/forschung/oe/wi/optik-strahlrohre/arbeitsgebiete/ray_en.html). Our primary focus with RAYX is on high-performance tracing, achieved through the utilization of GPUs via the Vulkan API.

<div style="text-align:center">
    <img src="https://user-images.githubusercontent.com/13185253/167402648-788eee6a-2ba4-466a-8a0a-62e59663e957.png" width="300" height="300"/>
</div>

## RAYX vs RAY-UI

RAYX offers several advanced features, including:
- Global (not sequential) tracing of beamlines for enhanced efficiency.
- GPU utilization for accelerated tracing performance.
- A dedicated mode for sequentially tracing multiple beamlines with ease.
- Grouping functionality, allowing objects to be grouped together for simplified group transformations.

In contrast, RAY-UI provides a graphical user interface (GUI) for editing beamlines, a feature currently lacking in RAYX. This means that .rml files for beamlines must be edited manually or generated using RAY-UI. Integrating a beamline editor into rayx-ui is among our top priorities for future development.

## Installing or Building RAYX

For additional information, please visit our [Wiki](https://hz-b.github.io/rayx/). We are committed to delivering stable releases, which can be found [here](https://github.com/hz-b/rayx/releases). Please note that the `master` branch and other branches might be unstable, and building RAYX from the source could lead to unstable software. If you experience any issues with our distributed binaries, do not hesitate to [open an issue](https://github.com/hz-b/rayx/issues/new/choose). We are keen on providing assistance and offering alternative solutions as the need arises.
