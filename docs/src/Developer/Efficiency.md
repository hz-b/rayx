# Efficiency 

wiki for efficiency calculations

### Snell's law
A fraction of the light is reflected and another transmitted:

![refraction_fresnel](https://upload.wikimedia.org/wikipedia/commons/8/89/Fresnel1.svg)<br>
\\(\theta_i =\\)  (normal) incidence angle <br>
\\(\theta_r =\\) (normal) reflection angle (same as \\(\theta_i\\))<br>
\\(\theta_t =\\) (normal) transmittance angle <br>
\\(N_1 =\\) refraction index of material from which the ray is coming  (left in image)<br>
\\(N_2 =\\) refraction index of material into which the ray is going (right in image)<br>

all parameters are potentially complex numbers. The refractive indices are retrieved from files (Palik, Henke, Cromer..)

Snell's law:
\\[
N_1 \sin \theta_i = N_2 \sin \theta_t \rightarrow \sin \theta_t = \frac{N_1}{N_2} \sin \theta_i
\\]

\\(\theta_i\\), \\(N_1\\), \\(N_2\\) are known, we are looking for \\(\theta_t\\).  <br>
We do not calculate the angle specifically but only the cosinus, which is sufficient for further calculations and more efficient/precise than calculating the angle itself because we do not need to use more trigonometric functions.
We can calculate the incidence angle \\(\theta_i\\) of each ray from its direction and the surface normal. Then we calculate \\(\cos(\theta_i)\\) and from that we can derive \\(\cos(\theta_t)\\) with snell's law:

\\[
(\sin \theta_i)^2 = 1 - (\cos \theta_i)^2 \\\\
(\sin \theta_t)^2 = (\frac{N_1}{N_2})^2 (\sin \theta_i)^2 \\\\
\cos \theta_t = \sqrt{1 - (\sin \theta_t)^2} = \sqrt{1 - \Big(\frac{N_1}{N_2} \sin \theta_i\Big)^2}
\\]

The cosine of both angles is then used in the Fresnel equations to calculate the s- and p-polarization

### Fresnel equation
Any polarization state can be described by two components: one vertical and one horizontal. Or - relative to the plane of incidence - s- and p-polarization. 
p-polarization (parallel, left image) lies parallel in the plane of incidence and s-polarization (senkrecht, right image) is orthogonal to the plane of incidence.

<img src="https://upload.wikimedia.org/wikipedia/commons/4/4d/Polarisation_p.png" alt="ppol" width="200"/>
<img src="https://upload.wikimedia.org/wikipedia/commons/3/3c/Polarisation_s.png" alt="spol" width="200"/>

the reflectance of both polarizations is calculated with the fresnel equations:

\\[r_s = \frac{N_1 \cdot \cos \theta_i - N_2 \cdot \cos \theta_t}{N_1 \cdot \cos \theta_i + N_2 \cdot \cos \theta_t}\\]
\\[r_p = \frac{N_2 \cdot \cos \theta_i - N_1 \cdot \cos \theta_t}{N_2 \cdot \cos \theta_i + N_1 \cdot \cos \theta_t}\\]

(The transmitted power is then "the rest": \\(t_s = 1 - r_s\\) and \\(t_p = 1 -r_p\\))