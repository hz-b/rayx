# Optical Elements

Optical elements are a central part of the tracing process. They define how light is guided through the beamline - where it is focused, redirected, or scattered.
In the context of RAYX a beamline consists of two types of elements:
- LightSources, to create the rays, and
- OpticalElements, to be hit by and manipulate these rays

In this section, you can read up on the design choices we made when implementing OpticalElements in RAYX.

## Behavior, Cutout, Surface

Next to its position and orientation, an OpticalElement is classified by three parts: The Behaviour, Surface and Cutout.
- The Behavior defines how the element interacts with a ray (eg. reflecting, absorbing, redirecting)
- The Surface expresses the curvature of an OpticalElement
- The cutout defines the boundaries of the OpticalElement. In other words it "cuts" a finite shape out of the large Surface.

### Behavior

Behavior determines what happens to a ray once it hits the OpticalElement.
Such a "hit" might result in absorbtion, reflection or the ray might pass through the OpticalElement.
Typical examples of Behaviours are Mirror, Grating, Slit and [RZP](./RZP.md).

Behaviours are defined in the Shared/Behaviour.h file.
Each Behaviour has a "behave" function (eg. behaveMirror, behaveSlit, ...) that translates the incoming ray to the outgoing ray.
These functions are gathered in the behave.comp file.

### Surface

Surfaces in RAYX are defined as either a plane, a quadric, or a toroid.
We use mathematical formulas to represent them internally, which means they are not necessarily bounded in size.
Optical elements are often subtly curved; to the human eye, they might appear indistinguishable from planar elements.

There are two ways to describe the reflectivity of the Surface. The User can choose between reflectivity Type '100%' and 'derived by material'.
If you want the second option you need to specify the following parameter:
- Material Substrate
- Roughness Substrate
- Density Substrate
- Surface Coating
- Coating File
- Number Layer
- Material Coating 1
- Thickness Coating 1
- Roughness Coating 1
- Density Coating 1
- Material Coating 2
- Thickness Coating 2
- Roughness Coating 2
- Density Coating 2
- Material Top Layer
- Thickness Top Layer
- Roughness Top Layer
- Density Top Layer

An example of the RML input looks like this:

```XML
    <param id="reflectivityType" comment="Derived by Material" enabled="T">1</param>
    <param id="materialSubstrate" enabled="T">Au</param>
    <param id="roughnessSubstrate" enabled="T">0</param>
    <param id="densitySubstrate" auto="T" enabled="T">19.300000000000001</param>
    <param id="surfaceCoating" comment="Substrate only" enabled="T">0</param>
    <param id="coatingFile" absolute="" enabled="F"></param>
    <param id="numberLayer" enabled="F">2</param>
    <param id="materialCoating1" enabled="F"></param>
    <param id="thicknessCoating1" enabled="F">0</param>
    <param id="roughnessCoating1" enabled="F">0</param>
    <param id="densityCoating1" auto="T" enabled="F">0</param>
    <param id="materialCoating2" enabled="F"></param>
    <param id="thicknessCoating2" enabled="F">0</param>
    <param id="roughnessCoating2" enabled="F">0</param>
    <param id="densityCoating2" auto="T" enabled="F">0</param>
    <param id="materialTopLayer" enabled="F"></param>
    <param id="thicknessTopLayer" enabled="F">0</param>
    <param id="roughnessTopLayer" enabled="F">0</param>
    <param id="densityTopLayer" auto="T" enabled="F">0</param>
```
### Cutout

The cutout defines the boundaries of the OpticalElement, by cutting a shape out of the Surface.
As the surfaces of OpticalElements often only slightly differ from the XZ plane, we implement Cutouts by a simple 2D shape applied to the coordinates X and Z.

Cutouts come in different shapes:
- Rectangle
- Ellipse
- Trapezoid
- Unlimited

The central function is the `bool inCutout(Cutout cutout, double x, double z);`.
A given 3D point `p` is _within the cutout c_, if `inCutout(c, p.x, p.z)` returns `true`.

Not all OpticalElements use exactly one Cutout.
The Slit for example uses three Cutouts, one for the ray-absorbing shape around the "opening", then one for the "opening" itself, and another one for the ray-absorbing beamstop within the opening.

### Ray-OpticalElement collision

When checking whether a ray collides with an OpticalElement, we first convert the Ray to the element coordinate system of the ray.
This makes (0, 0, 0) the center of the element, which generally lies in the XZ plane.
Rays then come from negative or positive y.

We then ask the Surface of our OpticalElement for a hitpoint using the `findCollisionWith` function.
And finally, if this hitpoint is in the cutout, we have found a collision.

### How They Are Combined

In the following image, you can see a visualization of how the surface and cutout interact.
The surface is a quadric that defines a sphere. The cutout is a rectangle, defined by points \\(A\\), \\(B\\), \\(C\\), and \\(D\\).
<!-- TODO what role does icurv really play? -->
Coupled with the `icurv` parameter, the cutout is mapped to the correct side of the sphere, visualized by points \\(A_1\\), \\(B_1\\), \\(C_1\\), and \\(D_1\\).
If a ray intersects the element's surface within the bounds of the cutout, it will be counted as a hit.
This is, where the behaviour comes into play to calculate the continuing path of the ray.

The cutout itself does not have a position; it is always at the origin of the element's coordinate system.

For some quadrics, this rule might not adequately define the position.
Therefore, we use the `icurv` parameter to determine whether the quadric is concave or convex.
This suffices since we calculate all intersection points with elements.
When two intersections occur, the `icurv` parameter informs us which intersection point to select.

![image](../../res/wastebox.png)
