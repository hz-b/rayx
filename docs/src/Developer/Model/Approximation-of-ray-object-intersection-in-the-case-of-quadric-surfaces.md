# Approximation of Ray Object Intersection in the Case of Quadric Surfaces

The following illustration intends to aid your intuition concerning the intersection point calculation for rays. Quadrics in Ray-X have the origin \\((0, 0, 0)\\) in the object coordinate system. 

## Geometric Shape of Objects

Beamline objects in Ray-X can have the following shapes:

- Rectangle
- Ellipse

When we define the height and width of an object, we are defining the extent of these shapes - in the figure below, you can see an example of a rectangular shape.

Currently, we are calculating the intersection of a planar surface in the xz-plane and use this as an approximation for our quadric surface. You can see this approximation in the figure below. The points \\(A_1,B_1,C_1,D_1\\) form a plane which isn't curved, like the quadric is in this part.

This could lead to a reported hit (resp. miss) of the object, where there should be a miss (resp. hit).


![image](/docs/src/uploads/809e84f0b69db93770ef0cd57729b892/image.png)