# RAYX Output


This chapter describes the contents of the output file.
Regardless of whether the output is provided in CSV or HDF5 format, the file follows a consistent structure.





## Columns

- **path_id**:  Identifier for a singular ray
- **path_event_id**:  Counter for events for the ray
- **position_x**:  Position in Element Coordinates in mm
- **position_y**:  Position in Element Coordinates in mm
- **position_z**:  Position in Element Coordinates in mm
- **direction_x**:  Direction in Element Coordinates in mm
- **direction_y**:  Direction in Element Coordinates in mm
- **direction_z**:  Direction in Element Coordinates in mm
- **electric_field_x_real**:  cumulated value of X-real
- **electric_field_x_imag**:  cumulated value of X-imag
- **electric_field_y_real**:  cumulated value of Y-real
- **electric_field_y_imag**:  cumulated value of Y-imag
- **electric_field_z_real**:  cumulated value of Z-real
- **electric_field_z_imag**:  cumulated value of Z-imag
- **optical_path_length**:  accumulated distance in mm
- **energy**: Photon energy in eV
- **order**: Order of Diffraction
- **object_id**: Identifier of the Beamline Element (including Sources)
- **source_id**: Identifier Light Source
- **event_type**:  Type of recorded [Event](../Model/07_Events.md)
- **rand_counter**:  Seed for the PNRG
