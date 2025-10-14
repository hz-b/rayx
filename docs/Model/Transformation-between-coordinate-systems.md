# Transformation between coordinate systems

## How to calculate World Coordinates

This explains how to get world coordinates (global position and orientation of an optical element) from the sequential setup of a beamline that is often used, where the position and orientation of an element is described with distances and rotations relative to its predecesor.

First a small introduction to rotation matrices which is not super relevant for the transformations but might help to visualize the rotations and understand why some angles are positive and others negative.
 
<details><summary>Rotation Matrices</summary>

### Rotation matrices

A rotation through an angle \\(\theta\\) can either be active or passive.
An active rotation around for example the z-axis through the angle \\(\theta\\) rotates the point within the coordinate system. Thereby, the coordinates of the point are changed whereas the coordinate system is left unchanged. When looking along the rotation axis in a right-handed coordinate system towards the origin, the rotation direction is counter-clockwise.

\\[
R_a({\theta}) = 
\begin{bmatrix}
    \cos(\theta) & -\sin(\theta) & 0 \\\\
    \sin(\theta) & \cos(\theta) & \\\\
    0           & 0         & 1 
\end{bmatrix}
\\]

A passive rotation leaves the position of the vector unchanged and rotates the axes of the coordinate system relative to the vector i.e. rotates the basis vectors (change of basis). When looking along the rotation-axis towards the origin in a right-handed coordinate system, the rotation of the rotating axes is clockwise. Thus, it is defined as an active rotation (applied to the basis vectors) in the other direction i.e. through the negative angle (\\(\cos(-\theta) = \cos(\theta)\\) and \\(-\sin(\theta) = \sin(-\theta))\\):

\\[   
    R_p({\theta}) = \begin{bmatrix}
    \cos(\theta) & \sin(\theta) & 0 \\\\
    -\sin(\theta) & \cos(\theta) &  \\\\
    0           & 0         & 1 
    \end{bmatrix} = \begin{bmatrix}
    \cos(-\theta) & -\sin(-\theta) & 0 \\\\
    \sin(-\theta) & \cos(-\theta) &  \\\\
    0           & 0         & 1 
    \end{bmatrix}
\\]

The relation between axes and the position of the point are the same after each of the rotations: After the passive rotation the basis vectors are different and the vector coordinates stay the same whereas after the active rotation the vector coordinates are different but the basis vectors are the same.

Example for active (left) and passive (right) rotation through \\(\alpha=25^\circ\\): 
![active_passive](/docs/src/uploads/7eb17510a7f200d4ce89ed337e0a4eda/rotation_active_vs_passive.PNG)
 The relative position of the vector to the axes is the same after each rotation. 
See also [active vs passive transformation](https://en.wikipedia.org/wiki/Active_and_passive_transformation)

However, active and passive are in our case only an *interpretation* of the rotations that makes sense when looking at the beamline from a global point of view. Globally seen, the local coordinate system of each optical element is rotated and translated differently with respect to a global coordinate system whereas the vectors (the rays) only change by e.g. reflection when interacting with an element.
Thus, we have coordinate systems for optical elements, that are identical for each element in the sense that the y-axis is the normal and the x-z-plane is the tangent plane of the surface at the origin, and for rays where the center ray is the z-axis and a global system. The transformation between the systems is implemented by rotating and translating the vectors within the same coordinate system.
\\(\rightarrow\\) In the implementation only the vectors are transformed by active transformations and the axes of the coordinate system stay the same, although in the "real world", the beams remain unchanged and only the coordinate system is rotated around them.
</details>

The following sections describe how to calculate the transformation matrices from beam coordinates to element coordinates and again to (new) beam coordinates based on the given distance and angles in the sequential setup.
This is relevant for RAYX if you need to calculate the world coordinates from the user parameters that describe the sequential setup yourself and cannot directly use the world coordinates from the rml file: 


<details><summary>Positioning of elements in sequential setup</summary>

In the RAY-UI rays are represented in a beam coordinate system. In that system the main ray always points from the origin towards the z-axis wheras the individual rays have slight deviations in their direction and origin.

Elements are represented in an element coordinate system.
The elements are mostly located in the x-z-plane of their coordinate system. The y-axis is the normal in the center of the element (Visualized in the [documentation](https://it-ed-git.basisit.de/RAY/RAY/-/wikis/uploads/bdcf4515e03b2fccf462c5f0d76052c3/Paper_Schaefers_RAY_Springer_2007.pdf)).


In order to calculate the intersection point with the "quad" function, we first need to transfer the incoming rays from beam coordinates to the object coordinates. The relation between these system is defined by two angles \\(\alpha\\) and \\(\chi\\) and a translation by \\(z_0\\). The transformation affects the position and direction of the ray.

1. the offset \\(z_0\\) describes the distance between the previous element or the source and the current optical element. 

2. the main ray should have a specific incidence angle \\(\alpha\\) (angle between main ray and x-z-plane of the optical element). This rotation is counter-clockwise around the x-axis:

\\[
    R_x({\alpha}) = \begin{bmatrix}
    1 & 0 & 0 \\\\
    0 & \cos(\alpha) & -\sin(\alpha) \\\\
    0 & \sin(\alpha) & \cos(\alpha)
    \end{bmatrix}
\\]

<details><summary>Side note</summary>
Side note for visualization: An example for this rotation interpreted as an [active](/docs/src/uploads/33a69b81f3f7c491842bcdeda4ca97b7/incidence_angle_active.PNG) and as a [passive](/docs/src/uploads/5ab2555382dc2b2ef10a9864aaee0224/incidence_angle_passive.PNG) rotation of the main ray (z-axis) and a ray \\(v\\) through the grazing incidence angle \\(\alpha=25°\\). The first coordinate system shows the incoming ray in the ray-coordinate system. In the second one the rays/the axes are rotated through \\(\alpha\\) such that the rays lie in the element-coordinate system. In the third image the reflection is calculated. Finally, in the last image, the reflected ray/the axes are rotated to the new ray-coordinate system.
Since we are using a right-handed coordinate system, the x-axis points into the image and the rotations that appear to be clockwise are actually counter-clockwise around the x-axis.
</details>

3. The second rotation through angle \\(\chi\\) around the z-axis tilts the optical element such that the ray is not reflected upwards (\\(\chi=0°\\)) but to the right (\\(\chi=90°\\)), downwards (\\(\chi=180°\\)) or to the left (\\(\chi=270°\\)). This is a clockwise rotation. Thus, we rotate through \\(-\chi\\) when \\(\chi\\) is given.

After tracing we need to transform the ray back to the beam coordinate system. Therefore we rotate back around \\(\chi\\) and then rotate around the exit angle \\(\beta\\) All these parameters are given as "user" parameters. The following section describes how to calculate beam-element and element-beam transformation matrices.
</details>


<details><summary>Calculating transformation matrices between elements in sequential setup</summary>

##### Beam to Element
1. Translation by \\(z_0\\) in z direction = distance to preceeding element

2. Rotation by azimuthal angle \\(\chi\\) around z-axis. 

\\[
    R_z(-\chi) = \begin{bmatrix} \cos(-\chi) & -\sin(-\chi) & 0 \\\\
    \sin(-\chi) & \cos(-\chi) & 0 \\\\
    0 & 0 & 1 \end{bmatrix} =
    \begin{bmatrix} \cos(\chi) & \sin(\chi) & 0 \\\\
    -\sin(\chi) & \cos(\chi) & 0 \\\\
    0 & 0 & 1 \end{bmatrix}
\\]

3. Rotation through grazing incidence angle \\(\alpha\\) around x-axis. Sometimes, the normal incidence angle with \\(90°-\alpha\\) is given. Then, it has to be converted to the grazing incidence angle \\(\alpha\\).<br>

\\[
    R_x(\alpha) = \begin{bmatrix} 1 & 0 & 0 \\\\
    0 & \cos(\alpha) & -\sin(\alpha) \\\\
    0 & \sin(\alpha) & \cos(\alpha) \end{bmatrix}
\\]

Putting it all together this is an affine transformation and can be written in homogeneous coordinates as one single matrix:

\\[
\begin{align*}
    M_{b2e} &= R_{x}(\alpha) R_z(-\chi) T_z(z_0) \\\\
    M_{b2e} &= \begin{bmatrix} 1 & 0 & 0 & 0\\\\
    0 & \cos(\alpha) & -\sin(\alpha) & 0 \\\\ 0 & \sin(\alpha) & \cos(\alpha) & 0 \\\\ 0 & 0 & 0 & 1 \end{bmatrix} \cdot \begin{bmatrix} \cos(\chi) & \sin(\chi) & 0 & 0\\\\ -\sin(\chi) & \cos(\chi) & 0 & 0\\\\ 0 & 0 & 1 & 0 \\\\ 0 & 0 & 0 & 1 \end{bmatrix} \cdot \begin{bmatrix} 1 & 0 & 0 & 0 \\\\ 0 & 1 & 0 & 0 \\\\ 0 & 0 & 1 & -z_0 \\\\ 0 & 0 & 0 & 1 \end{bmatrix} \\\\
&= \begin{bmatrix} \cos(\chi) & \sin(\chi) & 0 & 0 \\\\
-\sin(\chi)\cos(\alpha) & \cos(\chi)\cos(\alpha) & -\sin(\alpha) & z_0 \sin(\alpha) \\\\
-\sin(\chi) \sin(\alpha) & \sin(\alpha)\cos(\chi) & \cos(\alpha) & -z_0 \cos(\alpha) \\\\ 0 & 0 & 0 & 1 \end{bmatrix}
\end{align*}
\\]

##### Element to Beam
After the interaction with the element, the reflected ray \\(x_R\\) is transformed back to a beam coordinate system. The rotations around the axes are applied in reverse order.

1. Rotation through gracing exit angle \\(\beta\\) around x-axis. We do not need to rotate back through \\(\alpha\\) but keep rotating in the same direction since the new z-axis should point in the direction of the reflected and not of the incoming main ray. E.g. \\(\beta\\) is the same as \\(\alpha\\) for mirrors.<br>

\\[
    R_x(\beta) = \begin{bmatrix} 1 & 0 & 0 \\\\ 
    0 & \cos(\beta) & -\sin(\beta) \\\\ 0 & \sin(\beta) & \cos(\beta) \end{bmatrix}
\\]

2. Rotation back through \\(\chi\\).<br>

\\[
R_z(\chi) = R_z^{-1}(-\chi) = \begin{bmatrix} \cos(\chi) & -\sin(\chi) & 0 \\\\
    \sin(\chi) & \cos(\chi) & 0 \\\\
    0 & 0 & 1 \end{bmatrix}
\\]

In homogeneous coordinates:

\\[
\begin{align*}
    M_{e2b} &= R_z(\chi)R_{x}(\beta) \\\\
    M_{e2b} &= \begin{bmatrix} \cos(\chi) & -\sin(\chi) & 0 & 0\\\\
    \sin(\chi) & \cos(\chi) & 0 & 0\\\\
    0 & 0 & 1 & 0 \\\\ 0 & 0 & 0 & 1 \end{bmatrix} \cdot  \begin{bmatrix} 1 & 0 & 0 & 0\\\\ 
    0 & \cos(\beta) & -\sin(\beta) & 0 \\\\ 0 & \sin(\beta) & \cos(\beta) & 0 \\\\ 0 & 0 & 0 & 1\end{bmatrix}\\\\
&= \begin{bmatrix} \cos(\chi) & -\sin(\chi) \cos(\beta) & \sin(\chi)\sin(\beta) & 0 \\\\
\sin(\chi) & \cos(\chi)\cos(\beta) & -\cos(\chi)\sin(\beta) & 0 \\\\
0 & \sin(\beta) & \cos(\beta) & 0 \\\\ 0 & 0 & 0 & 1 \end{bmatrix}
\end{align*}
\\]

(Since there is no translation a 3x3 matrix would suffice)

### Misalignment
Misalignment is used when the optical element does not lie exactly where it should after applying the beam to element matrix. Therefore some rotation or translation might be necessary before the intersection point can be calculated.

The misalignment transformation matrix \\(M_{mis}\\) is simply derived from the user parameters \\(d_x\\), \\(d_y\\), \\(d_z\\), \\(d_{\phi}\\), \\(d_{\psi}\\), \\(d_{\chi}\\). It can be calculated by spliting into a transformation matrix (from \\(d_x\\), \\(d_y\\), \\(d_z\\)) and multiplying with a rotation matrix (from \\(d_{\phi}\\), \\(-d_{\psi}\\), \\(d_{\chi}\\)):

\\[
    \begin{align*}
        M_{mis} &= T_{x,y,z} R_{\phi, -\psi, \chi} \\\\
        &= \begin{bmatrix} 1 & 0 & 0 & -d_x\\\\
        0 & 1 & 0 & -d_y\\\\
        0 & 0 & 1 & -d_z \\\\ 0 & 0 & 0 & 1 \end{bmatrix} \cdot R^x_{-\psi} \cdot R^y_{\phi} \cdot R^z_{\chi}
    \end{align*} 
\\]

where e.g. \\(R^x_{\psi}\\) is the 4x4 homogeneous rotation matrix through \\(\psi\\) around the x-axis.

The inverse misalignment matrix is then calculated as follows:

\\[
    \begin{align*}
        M_{mis}^{-1} &= R_{-\psi, \phi, \chi}^{-1} \cdot T_{x,y,z}^{-1}\\\\
        &= (R^x_{-\psi} \cdot R^y_{\phi} \cdot R^z_{\chi}) ^{T} \cdot 
    \begin{bmatrix}
        1 & 0 & 0 & d_x \\\\
        0 & 1 & 0 & d_y \\\\
        0 & 0 & 1 & d_z \\\\
        0 & 0 & 0 & 1 
    \end{bmatrix} 
    \end{align*}
\\]

Since rotation matrices are orthogonal, the inverse of \\((R^x_{-\psi} R^y_{\phi} R^z_{\chi})\\) is the same as the transpose. The inverse of the translation matrix is the same but with negative offsets.

\\(M_{mis}\\), \\(M_{mis}^{-1}\\) are multiplied with \\(M_{b/g2e}\\) and \\(M_{e2g/b}\\), respectively, to form the final transformation matrices which could be given to the shader if we would still use the sequential approach in RAYX. However, we use a global coordinate system instead of the beam coordinate system but don't worry you didn't just read all of that for nothing, it will be important in the derivation of the transformation from global to element coordinates and back.
</details>

The next section describes how to replace the beam coordinate system that is used in the sequential approach with the global coordinate system and how to calculate the position and orientation.

<details><summary>Sequential to world coordinates</summary>

As explained in the previous sections, there is no global coordinate system in the sequential implementation but instead rays are transformed from beam coordinate system to element coordinate system and back to a different beam coordinate system such that the z-axis of the beam coordinate system always follows the main ray, which means that the main ray with \\(pos=(0,0,0)\\), \\(dir=(0,0,1)\\) in beam coordinates is always the same after each interaction with an optical element.

In a global coordinate system this is different. The origin of the system is the (first) source. When the main ray hits the first element, it is transformed into the element's coordinate system, traced (e.g. reflected) and transformed back into the global coordinate system. Then it does no longer have the values \\(pos=(0,0,0)\\) and \\(dir=(0,0,1)\\). To achieve this for the first element (i=1) in the beamline, we can still use \\(M_{b2e}\\) that we defined previously \\((M_{g2e}^{-1} = M_{b2e}^{-1})\\) since for the first element the global coordinate system is the same as the beam coordinate system of the incoming rays (bc the source is in \\((0,0,0)\\) which is the origin of both the global coord system and the initial beam coord. system). However, we need a different element to global coordinate system transformation for this elemet \\(M_{g2e}^{-1} \neq M_{b2e}^1\\) and of course also for all following elements. Moreover, for all following elements we also need a different \\(M_{g2e}^i \neq M_{b2e}^i\\) for \\(i>1\\).

### Transformation matrices from position and orientation
global coordinates are sometimes given by the user directly via e.g. an rml file, which stores the global orientation as a 3x3 matrix and the position as a 3 element vector. Expanding both to homogeneous 4x4 rotation/translation matrices makes it possible to calculate \\(M_{g2e}\\) and \\(M_{e2g}\\) by multiplying them.
For the case that the beamline was still build sequentially, it was decided to first build the global position and orientation from \\(\alpha\\), \\(\beta\\), \\(\chi\\), the distance \\(z_0\\) and the misalignment and then derive the matrices \\(M_{g2e}\\) and \\(M_{e2g}\\)  in the same way.

The following calculations can be used for all optical elements. For the ellipsoid, however, the misalignment can be defined in the coordinate system of the mirror or of the curvation. The usual misalignment is in the coordinate system of the mirror. The coordinate system of the curvation differs by a rotation through the tangent angle \\(\theta\\) around the x-axis. This angle depends on the shape of the ellipsoid. Depending on the coordinate system, we add the rotation \\(T_x(\theta)\\) (in red), for all other elements this is irrelevant so \\(\theta = 0\\).

As mentioned before, in the case that the element is the first in the beamline, it is simply placed at a certain distance on the z-axis. Therefore, the position (pos) is, in homogeneous coordinates:

\\[
    pos^0 = \begin{bmatrix} 0 \\\\ 0 \\\\ z_0^0 \\\\ 1 \end{bmatrix} + or^0 \cdot \color{red}{R_x^0(\theta)} \cdot \color{black}{\begin{bmatrix} d_x^0 \\\\ d_y^0 \\\\ d_z^0 \\\\ 1 \end{bmatrix}}
\\]

where or is the orientation of the element and \\(d_x\\), \\(d_y\\), \\(d_z\\) are the positional misalignment.
The orientation of the first element is calculated as follow:

\\[
    or^0 = R_x^0(\alpha) R_z^0(-\chi) \cdot \color{red}{R_x^0(\theta)} \cdot \color{black}{R_{\phi, -\psi, \chi}^0} \cdot \color{red}{R_x^0(\theta)^T} 
\\]

where \\(R_{\phi -\psi \chi}\\) contains the orientational misalignment and \\(R_x(\alpha) R_z(-\chi)\\) is the rotational part of \\(M_{b2e}\\) (without the translation by \\(z_0\\) since the distance is not part of the orientation but of the position)

When the element is not the first in the beamline, we need in addition to the ususal parameters of this element (\\(\alpha\\), \\(\beta\\), \\(\chi\\), the distance \\(z_0\\) and the misalignment) also the global position and orientation and the \\(M_{e2b}\\) matrix of the previous element. Unfortunately, we also have to remove the misalignment from the global position of the previous element (equation 1), then we can add the distance from the previous to new element to the position of the previous element following the direction of the outgoing ray (2). Finally, we can add the positional misalignment of element i to the position (3).

\\[
    \begin{align}
        pos^{(i-1)} &= pos^{i-1} - or^{i-1} \cdot \color{red}{R_x^{i-1}(\theta)} \cdot \color{black}{\begin{bmatrix} d_x^{i-1} \\\\ d_y^{i-1} \\\\ d_z^{i-1} \\\\ 1 \end{bmatrix}} \\\\
        pos^i &= pos^{i-1} - or^{i-1} \cdot R_x^{i-1}(\theta) \cdot \begin{bmatrix} 0 \\\\ 0 \\\\ z_0^i \\\\ 1 \end{bmatrix} \\\\
        pos^i &= pos^i + or^{i} \cdot \color{red}{R_x^{i}(\theta)} \cdot \color{black}{\begin{bmatrix} d_x^i \\\\ d_y^i \\\\ d_z^i \\\\ 1 \end{bmatrix}}
    \end{align}
\\]


The calculation of the orientation of the ith element is a bit simpler. The global orientation of element i is the global orientation of the previous element \\((or^{i-1})\\) without the rotational misalignment (1) multiplied with the rotation of the new element coordinate system with respect to the previous element coordinate system \\((M_{e2b}, \text{eq 2})\\) multiplied with the orientation of the new element in its own element coordinate system (which is calculated in the same way as for the first element: local orientation \\(\cdot\\) misalignment, eq. 3):

\\[
    \begin{align}
        or^{(i-1)} &= or^{i-1} \cdot \color{red}{R_x^{i-1}(\theta)^T} \cdot \color{black}{(R_{\phi, -\psi, \chi}^{i-1})^{T}} \cdot \color{red}{R_x^{i-1}(\theta)} \\\\
        or^i &= or^{(i-1)} \cdot M_{e2b}^{i-1} \\\\
        or^i &= or^i \cdot (R_x^i(\alpha) R_z^i(-\chi) \cdot \color{red}{R_x^i(\theta)} \cdot \color{black}{R_{\phi, -\psi, \chi}^i} \color{red}{R_x^i(\theta)^T})
    \end{align}
\\]

These calculations are done in WorldUserParams.cpp. They have been tested but still there might be some mistake in there, so feel free to question the calculations if something is not working.

</details>

Once you have the global position and orientation, you can derive the Transformation matrices by expanding the position p to a homogeneous translation matrix and the orientation to a homogeneous rotation matrix:

\\[
M_{g2e} = 
\begin{bmatrix} 
1 & 0 & 0 & -p_x \\\\
0 & 1 & 0 & -p_y \\\\
0 & 0 & 1 & -p_z \\\\
0 & 0 & 0 & 1
\end{bmatrix} \cdot or
\\]
\\[
M_{e2g} = M_{g2e}^{-1} = or^T \cdot \begin{bmatrix} 
1 & 0 & 0 & p_x \\\\
0 & 1 & 0 & p_y \\\\
0 & 0 & 1 & p_z \\\\
0 & 0 & 0 & 1
\end{bmatrix}
\\]