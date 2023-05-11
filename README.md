# Rayx
Rayx is a simulation tool for beamlines at electron storage rings. At this stage, it still is intended to be used alongside [RAY-UI](https://www.helmholtz-berlin.de/forschung/oe/wi/optik-strahlrohre/arbeitsgebiete/ray_en.html). Our primary concern for rayx is high-performance tracing, which we achieve by utilizing GPUs via the Vulkan API. 

## New Features for Rayx
- Global (not sequential) tracing of beamlines
- GPU utilization for faster tracing
- Multiple beamlines can easily be traced in succession
- Groups of objects can be added for easy group transforms (e.g., Reflection Zone Plates)

You can find our [Wiki here](https://hz-b.github.io/RAY-X/)