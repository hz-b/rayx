# Quad(ric) Function

Function for calculating the intersection of a ray with the surface of an optical element in 3-dimensional space.

### Input: 

* 16 parameters a<sub>11</sub> to a<sub>44</sub> to define the surface in 3D space with the general equation for second order surfaces. Parameter values for specific surfaces can be found in the [documentation](https://it-ed-git.basisit.de/RAY/RAY/-/wikis/uploads/bdcf4515e03b2fccf462c5f0d76052c3/Paper_Schaefers_RAY_Springer_2007.pdf).
* <img src="https://latex.codecogs.com/svg.latex?\small&space;ray=\begin{bmatrix} x_{S'} \\ y_{S'} \\z_{S'} \\ \end{bmatrix} + t \begin{bmatrix} l_{S'} \\ m_{S'}\\ n_{S'} \\ \end{bmatrix}"/>
* weight/III (set to 0(python)/-4(Fortran) if ray missed surface, unchanged otherwise)
* icurv: determines the sign in the formula for calculating t, if negative first intersection point, if positive second intersection point.

### Output:

* modified ray
* normal at intersection point
* weight/III

### Calculation of intersection point
general equation for second order surfaces:<br>
F(x,y,z) = a<sub>11</sub>x<sup>2</sup> + a<sub>22</sub>y<sup>2</sup> + a<sub>33</sub>z<sup>2</sup> + 2a<sub>12</sub>xy + 2a<sub>13</sub>xz + 2a<sub>23</sub>yz + 2a<sub>14</sub>x + 2a<sub>24</sub>y + 2a<sub>34</sub>z + a<sub>44</sub> 

The intersection is determined by inserting the x,y and z-coordinates of the ray in F(x,y,z) and set to zero: <br>
F(x<sub>S'</sub>+t\*l<sub>S'</sub>, y<sub>S'</sub>+t\*m<sub>S'</sub>, z<sub>S'</sub>+t\*n<sub>S'</sub>) = 0

We obtain a quadratic equation of the form <img src="https://latex.codecogs.com/svg.latex?\small&space;0=a\cdot t^2+b\cdot t+c" /> with variable t and the following coefficients:

<img src="https://latex.codecogs.com/svg.latex?\small&space;
\begin{align*}
    a &= a_{11}l_{S'}^2 + a_{22}m_{S'}^2 + a_{33}n_{S'}^2 + 2a_{13}l_{S'}n_{S'} + 2a_{12}l_{S'}m_{S'} + 2a_{23}m_{S'}n_{S'} \\
    b &= 2a_{11}x_{S'}l_{S'} + 2a_{22}y_{S'}m_{S'} + 2a_{33}z_{S'}n_{S'} \\
    &+ 2a_{12}y_{S'}l_{S'} + 2a_{12}x_{S'}m_{S'} 
    + 2a_{13}z_{S'}l_{S'} + 2a_{13}x_{S'}n_{S'} \\
    &+ 2a_{23}z_{S'}m_{S'} + 2a_{23}y_{S'}n_{S'} 
    + 2a_{14}l_{S'} + 2a_{24}m_{S'} + 2a_{34}n_{S'} \\
    c &= a_{11}x_{S'}^2 + a_{22}y_{S'}^2 + a_{33}z_{S'}^2 + 2a_{12}x_{S'}y_{S'} + 2a_{13}x_{S'}z_{S'} + 2a_{23}y_{S'}z_{S'} \\ 
    &+ 2a_{14}x_{S'} + 2a_{24}y_{S'} + 2a_{34}z_{S'} + a_{44}
\end{align*}" />

Since a,b and c can simply be calculated, we could solve the equation directly with <img src="https://latex.codecogs.com/svg.latex?\small&space;t=\frac{-b+ICURV\cdot \sqrt{b^2-4ac}}{2a}" /> and use t to find the intersection point. If ICURV is negative, we get the first intersection point with the object (smaller t). If ICURV is positive we get the second intersection point from when the ray exits the element (larger t). Some optimizations are applied to this formula in the code. Depending on the largest component in the direction of the ray, it is normalized in x, y or z direction to simplify the ray equation. Thus, there are three cases.

### Optimization

Assume l<sub>S'</sub> >= m<sub>S'</sub> and l<sub>S'</sub> >= n<sub>S'</sub> (first case). Then, we can divide the direction by l<sub>S'</sub>, such that it is normalized in x and the y- and z-coordinates are within [-1,1] (I). Moreover, we can translate the origin of the ray along the direction vector towards the origin of the coordinate system until the y-z-plane is hit (x=0, II). Then, we end up with a normalized ray (III), where x=t.

<img src="https://latex.codecogs.com/svg.latex?\small&space;
\begin{align*}
    ray 
    &\overset{\text{I}}{=} \begin{bmatrix} x_{S'} \\ y_{S'} \\z_{S'} \\ \end{bmatrix} + t \begin{bmatrix} 1 \\ m_{S'}/l_{S'}\\ n_{S'}/l_{S'} \\ \end{bmatrix} \\
    &\overset{\text{II}}{=} \begin{bmatrix} x_{S'}-1\cdot x_{S'} \\ y_{S'} - (m_{S'}/l_{S'}) \cdot y_{S'} \\z_{S'} - (n_{S'}/l_{S'}) \cdot z_{S'} \\ \end{bmatrix} + t \begin{bmatrix} 1 \\ m_{S'}/l_{S'}\\ n_{S'}/l_{S'} \\ \end{bmatrix} \\
    &\overset{\text{III}}{=} \begin{bmatrix} 0 \\ y \\ z \end{bmatrix} + t \begin{bmatrix} 1 \\ a_{ml} \\ a_{nl} \\ \end{bmatrix} 
\end{align*}" />

When we plug this parameterization of the ray into F(x,y,z)=0, some terms in a, b and c are removed (see code).

Since every term in b contains the factor 2, the equation for calculating t can be simplified:
<img src="https://latex.codecogs.com/svg.latex?\small&space;
\begin{align*}
    t = \frac{-2\frac{b}{2} +ICURV \cdot \sqrt{(2\frac{b}{2})^2 - 4ac}}{2a} = \frac{-2\frac{b}{2} +ICURV \cdot 2\sqrt{(\frac{b}{2})^2 - ac}}{2a} = \frac{-\frac{b}{2} +ICURV \cdot \sqrt{(\frac{b}{2})^2 - ac}}{a}
\end{align*}" />

In the code the factor 2 is left out of the equation for b from the beginning. Thus, what is called b in the code is actually b/2.

If the term in the root is negative there is no intersection and weight is set to 0 (or III to -4 in fortran). 
Otherwise it is checked whether the factor a is much smaller than c. Then, the divisor is very small or zero which can cause problems with the division. In that case, a\*t<sup>2</sup> is removed from the quadratic equation a\*t<sup>2</sup> + b\*t + c which thus yields t=x=-c/b or t=x=-c/2/b in the code since the factor 2 is excluded from b ([see also](https://www.cs.uaf.edu/2012/spring/cs481/section/0/lecture/01_26_ray_intersections.html) (Ray Quadric Intersection)). 
If a is not much smaller than c, t is calculated with the simplified equation above. Subsequently, t (=x) is plugged into the modified ray equation and the intersection point is calculated.

The other two cases for when y or z are the largest component of the direction of the ray are similar.

The partial derivatives of F(x,y,z) form the normal vector f<sub>x,y,z</sub> of the surface. Inserting the calculated intersection point into the normal vector yields the normal of the surface at this specific point. The intersection point is set to be the new origin of the ray. The ray direction (l<sub>S'</sub>, m<sub>S'</sub>, n<sub>S'</sub>) remains unchanged.

### General equation for second order surfaces
The equation can describe the surface of several elements in a similar way as a circle with radius 1 can be described in 2D as x<sup>2</sup> + y<sup>2</sup> - 1 = 0. Elements that can be described by this general equation include sphere, ellipsoid, plane, cone, cylinder, paraboloid. These are so called quadric surfaces. Surfaces that cannot be described by any quadratic function include for example the torus.
The equation refers to a right-handed coordinate system with the center of the optical element in the origin. The element's surface is the x-z-plane and the y-axis is the normal vector. 

<img src="https://latex.codecogs.com/svg.latex?\small&space;
\begin{align*}
    F(x,y,z) &= \vec{x}^TA\vec{x} \\
    \vec{x} = \begin{bmatrix} x \\ y \\ z \\ 1 \end{bmatrix}
    A &= \begin{bmatrix} a_{11} & a_{12} & a_{13} & a_{14} \\
    a_{21} & a_{22} & a_{23} & a_{24} \\
    a_{31} & a_{32} & a_{33} & a_{34} \\
    a_{41} & a_{42} & a_{43} & a_{44} 
    \end{bmatrix} \\
    a_{ij} = a_{ji} &\Rightarrow A^T = A
\end{align*}" />

Literature: <br>
[https://en.wikipedia.org/wiki/Quadric](https://en.wikipedia.org/wiki/Quadric) <br>
[https://www.win.tue.nl/~sterk/Bouwkunde/2db60-chap3.pdf](https://www.win.tue.nl/~sterk/Bouwkunde/2db60-chap3.pdf)
