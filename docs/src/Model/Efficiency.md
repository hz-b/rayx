
# Efficiency

This wiki documents the usage of efficiency and polarization techniques utilized in RAYX.

## Snell's law
Snell's law [...] is a formula used to describe the relationship between the angles of incidence and refraction, when referring to light or other waves passing through a boundary between two different isotropic media, such as water, glass, or air [[1]](#1).

![refraction_fresnel](https://upload.wikimedia.org/wikipedia/commons/8/89/Fresnel1.svg)<br>
$\theta_i =$ (normal) incidence angle
$\theta_r =$ (normal) reflection angle (same as $\theta_i$)
$\theta_t =$ (normal) transmittance angle
$N_1 =$ refraction index of material from which the ray is coming  (left in image)
$N_2 =$ refraction index of material into which the ray is going (right in image)

All parameters are potentially complex numbers.
The refractive indices $N_1$ and $N_2$ are retrieved from databases (Palik, Henke, Cromer..).

Snell's law:
$$
N_1 \sin \theta_i = N_2 \sin \theta_t \rightarrow \sin \theta_t = \frac{N_1}{N_2} \sin \theta_i
$$

With $\theta_i$, $\theta_r$, $N_1$, $N_2$ being known, we are looking for $\theta_t$.
We can calculate the incidence angle $\theta_i$ of each ray from its direction and the surface normal. Then we calculate $\cos(\theta_i)$ and from that we can derive $\cos(\theta_t)$ with snell's law:

$$
(\sin \theta_i)^2 = 1 - (\cos \theta_i)^2 \\\\
(\sin \theta_t)^2 = (\frac{N_1}{N_2})^2 (\sin \theta_i)^2 \\\\
\cos \theta_t = \sqrt{1 - (\sin \theta_t)^2} = \sqrt{1 - \Big(\frac{N_1}{N_2} \sin \theta_i\Big)^2}
$$

The cosine of both angles is then used in the Fresnel equations to calculate the s- and p-polarization

## Fresnel equation [[2]](#2)
Any polarization state can be described by two components: one vertical and one horizontal. Or - relative to the plane of incidence - s- and p-polarization. 
p-polarization (parallel, left image) lies parallel in the plane of incidence and s-polarization (senkrecht, right image) is orthogonal to the plane of incidence.

<img src="https://upload.wikimedia.org/wikipedia/commons/4/4d/Polarisation_p.png" alt="ppol" width="200"/>
<img src="https://upload.wikimedia.org/wikipedia/commons/3/3c/Polarisation_s.png" alt="spol" width="200"/>

the amplitude coefficient of a reflection for both polarizations is calculated with the fresnel equations:

$$r_s = \frac{N_1 \cdot \cos \theta_i - N_2 \cdot \cos \theta_t}{N_1 \cdot \cos \theta_i + N_2 \cdot \cos \theta_t}$$
$$r_p = \frac{N_2 \cdot \cos \theta_i - N_1 \cdot \cos \theta_t}{N_2 \cdot \cos \theta_i + N_1 \cdot \cos \theta_t}$$

the amplitude coefficient of a refraction for both polarizations is calculated with the fresnel equations:

$$t_s = \frac{2.0 \cdot N_1 \cdot \cos \theta_i}{N_1 \cdot \cos \theta_i + N_2 \cdot \cos \theta_t}$$
$$t_p = \frac{2.0 \cdot N_1 \cdot \cos \theta_i}{N_2 \cdot \cos \theta_i + N_1 \cdot \cos \theta_t}$$

## Mirror reflection [[2]](#2)

Reflecting a ray on a mirror involves polarization and phase changes of the incident electric field of the ray. The update of the electric field can be described by 3 steps:

1. Rotate incident electric field from global coordinates into local basis of incoming propagation vector $\vec{k_{q-1}}$ using matrix $Q_{q,in}$.
1. Multiply fresnel amplitude coefficients with components of the electric field ($r_{s,q}$ and $r_{p,q}$ for reflection) using jones matrix $J_{q}$.
1. Rotate resulting electric field back into global coordinates by rotating into basis of outgoing propagation vector $\vec{k_{q}}$ using matrix $Q_{q,out}$.

In order to obtain rotation matrices $Q_{q,in}$ and $Q_{q,out}$, the basis vectors need to be found.

$$
\begin{aligned}
    \vec{s}_{q} &= \frac{\vec{k}_{q-1} \times \vec{\eta}_{q}}{|\vec{k}_{q-1} \times \vec{\eta}_{q}|} \\
    \quad \vec{p}_{q} &= \vec{k}_{q-1} \times \vec{s}_{q} \\
    \vec{s}'_{q} &= \vec{s}_{q}, \quad \vec{p}'_{q} = \vec{k}_{q} \times \vec{s}_{q}
\end{aligned}
$$

Both incident vector $\vec{k}_{q-1}$ and reflected vector $\vec{k}_{q}$ as well as normal vector $\vec{\eta}_{q}$ lie within the plane of incidence while the normal vector is perpendicular to the plane itself and defines it's front face.
We define vectors $\vec{s}$ and $\vec{p}$ perpendicular to each other and to propagation vector $\vec{k_{q-1}}$, while $\vec{s}$ is perpendicular to the plane of incidence and $\vec{k}$ lies within the plane.

$$
\begin{aligned}
    O_{q,in} &= \begin{pmatrix}
    s'_{x,q} & p'_{x,q} & k_{x,q} \\
    s'_{y,q} & p'_{y,q} & k_{y,q} \\
    s'_{z,q} & p'_{z,q} & k_{z,q}
    \end{pmatrix} \\
    J_{q} &= \begin{pmatrix}
    r_{s,q} & 0 & 0 \\
    0 & r_{p,q} & 0 \\
    0 & 0 & 1
    \end{pmatrix} \\
    O_{q,out} &= \begin{pmatrix}
    s_{x,q} & s_{y,q} & s_{z,q} \\
    p_{x,q} & p_{y,q} & p_{z,q} \\
    k_{x,q-1} & k_{y,q-1} & k_{z,q-1}
    \end{pmatrix} \\
    P_{q} &= O_{q,in} \cdot J_{q} \cdot O_{q,out}
\end{aligned}
$$

All operations are composed into the Polarization Ray Tracing Matrix $P_{q}$, encorparating fresnel amplitude coefficients into $J_{q}$.

$$
E_{q} = P_q E_{q-1}
$$

The incident electric field $E_{q-1}$ can now be multiplied by Polarization Ray Tracing matrix $P_{q}$, resulting in the ray's a new electric field after the intercept with the mirror.

## References
<a id="1">[1]</a> https://en.wikipedia.org/wiki/Snell%27s_law
<a id="2">[2]</a> Russel A. Chipman, Wai-Sze Tiffany Lam, Garam Young "Polarized Light and Optical Systems" (2019)
