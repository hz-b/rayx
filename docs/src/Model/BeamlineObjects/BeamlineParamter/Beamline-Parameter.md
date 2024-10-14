# Baamline Parameter

This table explains all the parameters that can be used in a RAYX beamline.
They are ordered for Elements and Sources and roughly in their order of importance.

## Light Source Parameter

| Name                           | Description                                                   |
|--------------------------------|---------------------------------------------------------------|
| Number of Rays                 | Number of rays emitted from the source                        |
| Source Width                   | Width of the light source in millimeters                      |
| Source Height                  | Height of the light source in millimeters                     |
| Source Depth                   | Depth of the light source in millimeters                      |
| Horizontal Divergence          | Horizontal angular spread of the rays                         |
| Vertical Divergence            | Vertical angular spread of the rays                           |
| Source Width Distribution      | Distribution type for source width                            |
| Source Height Distribution     | Distribution type for source height                           |
| Horizontal Divergence Distribution| Distribution type for horizontal divergence                 |
| Vertical Divergence Distribution | Distribution type for vertical divergence                    |
| Linear Pol 0                   | Degree of linear polarization at 0 degrees                    |
| Linear Pol 45                  | Degree of linear polarization at 45 degrees                   |
| Circular Pol                   | Degree of circular polarization                               |
| Energy Distribution Type       | How the nergy distribution should be defined (e.g. Values, File) |
| Energy Spread Type             | Distribution pattern for the Energy (e.g Gaussian)            |
| Energy Spread                  | Spread of energy around the central photon energy             |
| Photon Energy Distribution File| Filepath for .DAT- File with Energy distribution information  |
| Photon Energy                  | Central photon energy                                         |
| Separate Energies              | Number of seperate energy spikes                              |
| Electron Energy Orientation    | Movement in the Synchrotron (clockwise, counter-clockwise)    |
| Source Pulse Type              | (unused)                                                      |
| Vertical E-beam Divergence     | Vertical divergence of the electron beam                      |
| Bending Radius                 | Bending radius of the electron bunch in a bending magnet |
| Electron Energy                | Energy of the electrons in the source                         |
| Alignment Error                | Possible misalignment of the source                           |
| Photon Flux                    | --                                                            |
| Photon Energy                  | Central photon energy of the source                           |
| Energy Spread                  | Energy spread around the central energy                       |
| Energy Spread Unit             | Unit for energy spread (eV)                                   |
| Number of Equidistant Circles  | --                                                            |
| Maximum Opening Angle          | Maximum divergence angle for the rays                         |
| Minimum Opening Angle          | Minimum divergence angle for the rays                         |
| Delta Opening Angle            | --                                                            |
| Parameter P                    | --                                                            |
| Parameter P Type               | --                                                            |
| Sigma Type                     | --                                                            |
| Undulator Length               | --                                                            |
| Electron Sigma X               | Horizontal electron beam size                                 |
| Electron Sigma Xs              | Horizontal electron beam divergence                           |
| Electron Sigma Y               | Vertical electron beam size                                   |
| Electron Sigma Ys              | Vertical electron beam divergence                             |


## Optical Element Parameter

| Name                           | Description                                                   |
|--------------------------------|---------------------------------------------------------------|
| World Position                 | 3D coordinates of the optical element's position              |
| Total Width                    | Width of the optical element                                  |
| Total Length                   | Length of the optical element                                 |
| Total Height                   | Height of the optical element                                 |
| Grazing Incidence Angle        | Angle of incidence for grazing rays                           |
| Arm Length                     | Length of the arm of the optical element                      |
| Entrance Arm Length            | Length of the entrance arm                                    |
| Exit Arm Length                | Length of the exit arm                                        |
| Entrance Arm Length (Mer)       | Meridional entrance arm length                                |
| Exit Arm Length (Mer)          | Meridional exit arm length                                    |
| Entrance Arm Length (Sag)      | Sagittal entrance arm length                                  |
| Exit Arm Length (Sag)          | Sagittal exit arm length                                      |
| Central Beamstop               | Stop placed in the center of the beam                         |
| Total Width Stop               | Width of the beamstop                                         |
| Total Height Stop              | Height of the beamstop                                        |
| Opening Width                  | Width of the optical element opening                          |
| Opening Height                 | Height of the optical element opening                         |
| Opening Shape                  | Shape of the optical element opening (e.g., rectangle, circle)|
| Grating Mount                  | --                                                            |
| Radius                         | Radius of curvature for curved surfaces                       |
| Design Energy                  | Target design energy for the optical element                  |
| Line Density                   | --                                                            |
| Order of Diffraction           | Diffraction order for grating elements                        |
| Design Energy Mounting         | --                                                            |
| Design Order of Diffraction    | --                                                            |
| Design Alpha Angle             | --                                                            |
| Design Beta Angle              | --                                                            |
| Short Radius                   | Short radius for elliptical elements                          |
| Long Radius                    | Long radius for elliptical elements                           |
| Fresnel Z Offset               | --                                                            |
| Bending Radius Direction       | --                                                            |
| Parameter A11                  | --                                                            |
| Design Grazing Inc Angle       | Grazing incidence angle in design                             |
| Long Half Axis A               | Long half-axis for elliptical elements                        |
| Short Half Axis B              | Short half-axis for elliptical elements                       |
| Figure Rotation                | --                                                            |
| Distance Preceding             | Distance from preceding element                               |
| Misalignment Coordinate System | Coordinate system for misalignment                            |
| Reflectivity Type              | Type of reflectivity used                                     |
| Material Substrate             | Material of the substrate                                     |
| Geometrical Shape              | Shape of the optical element                                  |
| Image Type                     | Type of image plane                                           |
| Azimuthal Angle                | Azimuthal angle of rotation                                   |
| Additional Order               | --                                                            |
| Slope Error                    | Slope error for reflecting surfaces                           |
| Slope Error Sag                | Sagittal slope error                                          |
| Slope Error Mer                | Meridional slope error                                        |
| Thermal Distortion Amp         | Amplitude of thermal distortion                               |
| Thermal Distortion Sigma X     | Sigma in X for thermal distortion                             |
| Thermal Distortion Sigma Z     | Sigma in Z for thermal distortion                             |
| Cylindrical Bowing Amp         | Amplitude of cylindrical bowing                               |
| Cylindrical Bowing Radius      | Radius for cylindrical bowing                                 |
| VLS Parameter                  | --                                                            |
| Reflectivity Type              | Type of reflectivity coating                                  |
| Material Substrate             | Substrate material for reflecting surfaces                    |
| Roughness Substrate            | Surface roughness of the substrate                            |
| Density Substrate              | Density of the substrate material                             |
| Surface Coating                | Coating material for the optical surface                      |
| Coating File                   | External file for custom coating                              |
| Number Layer                   | Number of layers in multilayer coatings                       |
| Material Coating 1             | Material of the first coating layer                           |
| Thickness Coating 1            | Thickness of the first coating layer                          |
| Roughness Coating 1            | Roughness of the first coating layer                          |
| Density Coating 1              | Density of the first coating layer                            |
| Material Coating 2             | Material of the second coating layer                          |
| Thickness Coating 2            | Thickness of the second coating layer                         |
| Roughness Coating 2            | Roughness of the second coating layer                         |
| Density Coating 2              | Density of the second coating layer                           |
| Material Top Layer             | Material of the top layer coating                             |
| Thickness Top Layer            | Thickness of the top layer coating                            |
| Roughness Top Layer            | Roughness of the top layer coating                            |
| Density Top Layer              | Density of the top layer coating                              |
