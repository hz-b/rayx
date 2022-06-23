# Ray Generation

Rays are described in tree-dimensional space with an origin and a direction.
The origin are described with a three dimensional vector. However, the direction can be interpreted as the cosines of two angles $\phi$ and $\psi$. Since the z-axis is the direction of the center ray, the direction of all other rays can be described as the angle between ray direction vector and z-y-plane (horizontal divergence $\phi$) and between vector and z-x-plane (vertical divergence $\psi$).
See also [documentation](https://it-ed-git.basisit.de/RAY/RAY/-/wikis/uploads/bdcf4515e03b2fccf462c5f0d76052c3/Paper_Schaefers_RAY_Springer_2007.pdf) p.18,19.

$ray = \begin{bmatrix}
    x_s \\ y_s \\ z_s
    \end{bmatrix} + t 
    \begin{bmatrix}
    l_S \\ m_S \\ n_S
    \end{bmatrix}
    = \begin{bmatrix}
    x_s \\ y_s \\ z_s
    \end{bmatrix} + t 
    \begin{bmatrix}
    sin \phi_S \cdot cos \psi_S \\ cos \psi_S \\ cos \psi_S \cdot cos \phi_S
    \end{bmatrix}$

When the ray is created in the light source, its origin and direction angles are chosen randomly.

## Light sources: Point source
The user sets the following parameters:
* number of rays
* source dimensions width(x-dir), height(y-dir) and depth(z-dir) 
* horizontal and vertical divergence

The intensity distribution in the lightsource is understood as the probability distribution of the parameters position and angle.
The parameters $x$, $y$ and $z$ of position and the angles $\phi$ and $\psi$ are chosen randomly but according to a probability distribution. Uniformly distributed random numbers within the range $[0,1]$ are obtained by FORTRAN's [random_number](https://gcc.gnu.org/onlinedocs/gcc-4.8.0/gfortran/RANDOM_005fNUMBER.html) function.
There are two options for the probability distribution in the light source:
soft edge (Gaussian) and hard edge (uniformly).
Afterwards, some predefined offset can be added to each component.

### Hard edge
The hard edge option is the simpler one since the random numbers are already uniformly distributed. Thus, the ray parameters $x$, $y$, $z$, $\phi$, $\psi$ are chosen uniformly within the given extent (width, height, divergence..) of the point source by calculating a random number $u$, subtracting 0.5 such that the number ranges in $[-0.5,+0.5]$ and multiplying with the given width/height/depth or horizontal/vertical extent of the point source.

$x_S = (u_x - 0.5) \cdot width$ <br>
$y_S = (u_y - 0.5) \cdot height$ <br>
$z_S = (u_z - 0.5) \cdot depth$ <br>
$\phi_S = (u_\phi - 0.5) \cdot hor. div$ <br>
$\psi_S = (u_\psi - 0.5) \cdot vert. div$ <br>


Thus, the x-coordinate of the origin, for example, lies within $[-\frac{width}{2}, +\frac{width}{2}]$ and is picked uniformly.

### Soft edge
(See also [documentation](https://it-ed-git.basisit.de/RAY/RAY/-/wikis/uploads/bdcf4515e03b2fccf462c5f0d76052c3/Paper_Schaefers_RAY_Springer_2007.pdf) p.14)

For the soft edge we have to transform the uniform distribution to a Gaussian distribution with $\mu = 0$ and $\sigma = extent$. The resulting values for a specific ray are not bound by the input parameters as for the hard edge. Instead, the more they deviate from $\mu$ the more rare they are. 

#### In FORTRAN code
The old implementation uses the following apporach:

1. create two random numbers $u_1,u_2$ from a uniform distribution in $[0,1]$
2. $x =(u_1 - 0.5 ) \cdot \sigma \cdot 9$
3. $wx =  \exp(\frac{-x^2}{2})$
4. $dif = wx-u_2$
5. $wx$ is the value of the densitiy function $f(x)$ of the distribution with $\mu=0$ and $\sigma$. If $wx < u_2$, then repeat from step 1 (keep $x$ with probability $f(x)$)
6. else keep $x$


Here, 9 is used as a factor for $\sigma$, since the Gaussian distribution is defined from $-\infty$ to $+\infty$ but a confidence interval $[-4.5 \sigma, +4.5 \sigma]$ contains already more than 99.99%.

$w(x_s)$ lies in $[0,1]$. It is $1$ for $x_s = 0$ and becomes smaller the more $x_s$ deviates from $0$ according to the gaussian bell curve.
Since $w(x_s)$ is the probability that $x_s$ occurs in our gaussian distribution, we want to use $x_s$ for the origin of the ray with a probability of $w(x_s)$. Thus, we get a second random number $u_x'$ from FORTRAN's random_number function and if $u_x' \leq w(x_s)$ holds we use $x_S$ for the ray. Otherwise, $x_S$ is discarded and re-calculated with a new random number $u_x$. Thereby, we use $x_S$ with a probability that corresponds to the Gaussian distribution.

However, this is not very efficient as a lot of rays are thrown away. The following method uses a different approach which - although some values are discarded as well - is more efficient and creates two values in one iteration instead of one ([more here](https://www.desy.de/~sschmitt/blobel/eBuch.pdf) Section 5.5.2):

#### More efficiently:
1. create two random numbers $u_1, u_2$ from a uniform distribution in $[0,1]$
2. calculate $x_1 = 2u_1-1$ and $x_2 = 2u_2 -1$
3. calculate  $r^2 = x_1^2 + x_2^2r^2$
4. if $r^2 > 1$ repeat from step 1
5. else calculate 
$z_1 = x_1 \cdot \sqrt{-2\frac{\ln{r^2}}{r^2}} \cdot \sigma$ and 
$z_2 = x_2 \cdot \sqrt{-2\frac{\ln{r^2}}{r^2}} \cdot \sigma$

Now $z_1$ and $z_2$ are distributed according to the Gaussian distribution with $\mu = 0$ and $\sigma = extent$ as defined by the user.

This calculation is faster since we get 2 values instead of 1. 
(In python the first method took twice as long as this method to create the same amount of values)