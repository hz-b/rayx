# Slit

TBA

## Tracing Parameter

Mandatory:
- Reflectivity Type (see more in chapter Optical Elements)
- Slope Error
- World Position
- Misalignment

Cutout:
- Opening Shape
- Opening Width
- Opening Height
- Central Beamstop
- Total Width Stop
- Total Height Stop



## RML Object

To track a Slit using an RML File, you'll require an XML Object to encompass all the mirror details.

```XML
  <object name="Slit" type="Slit">
   <param id="geometricalShape" comment="rectangle" enabled="T">0</param>
   <param id="totalWidth" enabled="T">20</param>
   <param id="totalHeight" enabled="T">2</param>
   <param id="centralBeamstop" comment="none" enabled="T">0</param>
   <param id="totalWidthStop" enabled="F">20</param>
   <param id="totalHeightStop" enabled="F">1</param>
   <param id="distancePreceding" enabled="T">10000</param>
   <param id="azimuthalAngle" enabled="T">0</param>
   <param id="alignmentError" comment="No" enabled="T">1</param>
   <param id="translationXerror" enabled="F">0</param>
   <param id="translationYerror" enabled="F">0</param>
   <param id="translationZerror" enabled="F">0</param>
   <param id="rotationXerror" enabled="F">0</param>
   <param id="rotationYerror" enabled="F">0</param>
   <param id="rotationZerror" enabled="F">0</param>
   <param id="worldPosition" enabled="F">
    <x>0.0000000000000000</x>
    <y>0.0000000000000000</y>
    <z>10000.0000000000000000</z>
   </param>
   <param id="worldXdirection" enabled="F">
    <x>1.0000000000000000</x>
    <y>0.0000000000000000</y>
    <z>0.0000000000000000</z>
   </param>
   <param id="worldYdirection" enabled="F">
    <x>0.0000000000000000</x>
    <y>1.0000000000000000</y>
    <z>0.0000000000000000</z>
   </param>
   <param id="worldZdirection" enabled="F">
    <x>0.0000000000000000</x>
    <y>0.0000000000000000</y>
    <z>1.0000000000000000</z>
   </param>
  </object>
```