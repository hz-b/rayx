# Image Plane

The **Image Plane** is a fundamental optical element characterized by its flat, absorbing surface. This element is designed to capture all incoming rays, regardless of their direction or divergence. The Image Plane can be used in various optical simulations and is typically placed at a specified distance from the light source.

## Light Properties

In an Image Plane, rays are absorbed upon contact, allowing for the collection of all light interacting with the surface. This element does not modify the rays' properties but serves as a detector or absorber.

## Standard Image Plane

### RML Configuration

The standard configuration for an Image Plane does not impose any restrictions on size, meaning it effectively has an unlimited area for rays to hit.


```XML
  <object name="ImagePlane" type="ImagePlane">
   <param id="distanceImagePlane" enabled="T">1000</param>
   <param id="worldPosition" enabled="F">
    <x>0.0000000000000000</x>
    <y>0.0000000000000000</y>
    <z>1000.0000000000000000</z>
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


## Parameters
- distanceImagePlane: The distance from the light source to the image plane.
- worldPosition: The (x, y, z) coordinates of the image plane in world space.
- worldXdirection: Direction vector along the X-axis.
- worldYdirection: Direction vector along the Y-axis.
- worldZdirection: Direction vector along the Z-axis.



## Image Plane with Cutout

In scenarios where only a specific area of the image plane is needed, a cutout can be specified. This will restrict the absorption to the defined geometrical shape.

```XML
  <object name="ImagePlane" type="ImagePlane">
   <param id="geometricalShape" comment="rectangle" enabled="T">0</param>
   <param id="totalWidth" enabled="T">50</param>
   <param id="totalHeight" enabled="T">200</param>
   <param id="distanceImagePlane" enabled="T">1000</param>
   <param id="worldPosition" enabled="F">
    <x>0.0000000000000000</x>
    <y>0.0000000000000000</y>
    <z>1000.0000000000000000</z>
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

## Parameters with Cutout
- geometricalShape: The shape of the cutout (e.g., rectangle).
- totalWidth: Width of the image plane (or cutout).
- totalHeight: Height of the image plane (or cutout).
- distanceImagePlane: The distance from the light source to the image plane.
- worldPosition: The (x, y, z) coordinates of the image plane in world space.
- worldXdirection: Direction vector along the X-axis.
- worldYdirection: Direction vector along the Y-axis.
- worldZdirection: Direction vector along the Z-axis.