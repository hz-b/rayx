wiki for efficiency calculations

### Snell's law
A fraction of the light is reflected and another transmitted:

![refraction_fresnel](https://upload.wikimedia.org/wikipedia/commons/8/89/Fresnel1.svg)<br>
&theta;<sub>i</sub> = (normal) incidence angle <br>
&theta;<sub>r</sub> = (normal) reflection angle (same as &theta;<sub>i</sub>)<br>
&theta;<sub>t</sub> = (normal) transmittance angle <br>
N<sub>1</sub> = refraction index of material from which the ray is coming  (left in image)<br>
N<sub>2</sub> = refraction index of material into which the ray is going (right in image)<br>
all parameters are potentially complex numbers. The refractive indices are retrieved from files (Palik, Henke, Cromer..)

Snell's law: <br>
<img src="https://latex.codecogs.com/svg.latex?\small&space; 
\begin{align*}
N_1 \sin \theta_i = N_2 \sin \theta_t \rightarrow \sin \theta_t = \frac{N_1}{N_2} \sin \theta_i
\end{align*}
"/>

&theta;<sub>i</sub>, N<sub>1</sub>, N<sub>2</sub> are known, we are looking for &theta;<sub>t</sub>.  <br>
We do not calculate the angle specifically but only the cosinus, which is sufficient for further calculations and more efficient/precise than calculating the angle itself because we do not need to use more trigonometric functions.
We can calculate the incidence angle &theta;<sub>i</sub> of each ray from its direction and the surface normal. Then we calculate cos(&theta;<sub>i</sub>) and from that we can derive cos(&theta;<sub>t</sub>) with snell's law:

<img src="https://latex.codecogs.com/svg.latex?\small&space; 
\begin{align*}
(\sin \theta_i)^2 &= 1 - (\cos \theta_i)^2 \\
(\sin \theta_t)^2 &= (\frac{N_1}{N_2})^2 (\sin \theta_i)^2 \\
\cos \theta_t &= \sqrt{1 - (\sin \theta_t)^2} = \sqrt{1 - \Big(\frac{N_1}{N_2} \sin \theta_i\Big)^2}
\end{align*}
"/>

The cosine of both angles is then used in the Fresnel equations to calculate the s- and p-polarization

### Fresnel equation
Any polarization state can be described by two components: one vertical and one horizontal. Or - relative to the plane of incidence - s- and p-polarization. 
p-polarization (parallel, left image) lies parallel in the plane of incidence and s-polarization (senkrecht, right image) is orthogonal to the plane of incidence.

<img src="https://upload.wikimedia.org/wikipedia/commons/4/4d/Polarisation_p.png" alt="ppol" width="200"/>
<img src="https://upload.wikimedia.org/wikipedia/commons/3/3c/Polarisation_s.png" alt="spol" width="200"/>

the reflectance of both polarizations is calculated with the fresnel equations:

<img src="https://latex.codecogs.com/svg.latex?\small&space; 
\begin{align*}
r_s = \frac{N_1 \cdot \cos \theta_i - N_2 \cdot \cos \theta_t}{N_1 \cdot \cos \theta_i + N_2 \cdot \cos \theta_t} \\
r_p = \frac{N_2 \cdot \cos \theta_i - N_1 \cdot \cos \theta_t}{N_2 \cdot \cos \theta_i + N_1 \cdot \cos \theta_t}
\end{align*}
"/>

(The transmitted power is then "the rest": t<sub>s</sub> = 1 - r<sub>s</sub> and t<sub>p</sub> = 1 - r<sub>p</sub>)