# Tracing Output

The calculations trough the tracing process gernerate a lot of information about the rays. For a lot of different events like colliosions with elements we store the data about the ray in files for you to analyze. 
RAY-X generates two different types of output, namely a CSV file and a H5 file. In this chapter you find an in depth description on how to use them.
When tracing a Beamline the default setting is to generate a H5 file with information for each ray and every collisions.

## Reading the CSV File

If you chose to trace the Beamline shown in chapter 2 you have one Lightsource and one Optical Element. 


### Position
The Position of the Ray can be given in differen coordinate systems. 
The World-Coordinats, the element-coordinates, the ray-coordinates.


### Event type
There are seven different event types a Ray can posess. They are coded in


0: Fly off
The future Path of this Ray does not intersect any elements anymore.
This Ray now flies off to infinity.

1: Just hit element
Ray is in world coordinates.
This Ray has just hit `m_lastElement`.
And will continue tracing afterwards.
Ray is in element coordinates of the hit element.

2: Not engough Bounces
The ray has found another collision, but the maximum number of allowed bounces was reached.

3: Absorbed
This Ray was absorbed by `m_lastElement`.
Ray is in element coordinates, relative to `m_lastElement`.

4: Uninitialized
This is a yet uninitialized ray from outputData.
This is the initial weight within outputData, and if less events than `maxEvents` are taken,
The remaining weights in outputData will stay ETYPE_UNINIT even when returned to the CPU.

5: Beyond Horizon Error
// This is an error code.
// Functions like refrac2D can error due to "ray beyond horizon", see utils.comp.
// In that case this is returned as final event.

6: Fatal Error
// This is a general error code that means some assertion failed in the shader.
// This error code is typically generated using `throw`.

