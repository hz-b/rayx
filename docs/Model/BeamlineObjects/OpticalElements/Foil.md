# Foil 

The Foil is an optical element that simulates the interaction of light with thin material layers (e.g., gold filters). It calculates polarization-dependent transmission/reflection using Fresnel equations.

## Tracing Parameter

Mandatory:
- Reflectivity Type (see more in chapter Optical Elements)
- Slope Error
- World Position
- Misalignment

Foil:
- Substrate Thicness
- Substrate Roughness 


## Transmission Coefficient Calculation

### Angle Theta
\begin{aligned}
\sin\theta_1 &= \frac{N_1}{N_2}\sin\theta_0 \\
\theta_2 &= \theta_0 \quad \text{(Exit angle equals incidence angle)}
\end{aligned}

### Fresnel Coefficients
These are calculated for the entrance and exit surfaces. 
\begin{aligned}
r_s &= \frac{N_1\cos\theta_0 - N_2\cos\theta_1}{N_1\cos\theta_0 + N_2\cos\theta_1}, \ 
t_s &= \frac{2N_1\cos\theta_0}{N_1\cos\theta_0 + N_2\cos\theta_1} \
r_p &= \frac{N_2\cos\theta_0 - N_1\cos\theta_1}{N_2\cos\theta_0 + N_1\cos\theta_1}, \
t_p &= \frac{2N_1\cos\theta_0}{N_2\cos\theta_0 + N_1\cos\theta_1}
\end{aligned}
### Phaseshift

\\begin{aligned}
\delta &= \frac{2\pi}{\lambda}N_2d\cos\theta_1, \
\phi &= e^{i\delta}
\end{aligned}

### Total Transmission

\begin{aligned}
t_{\text{total}} &= \frac{t_{01}t_{12}e^{i\delta}}{1 + r_{01}r_{12}e^{2i\delta}}
\end{aligned}

## RML Configuration

```XML
  <object name="Foil" type="Foil">
   <param id="geometricalShape" comment="rectangle" enabled="T">0</param>
   <param id="totalWidth" enabled="T">40</param>
   <param id="totalHeight" enabled="T">60</param>
   <param id="normalIncidenceAngle" enabled="T">0</param>
   <param id="distancePreceding" enabled="T">10000</param>
   <param id="azimuthalAngle" enabled="T">0</param>
   <param id="transmissionType" comment="Derived by Material" enabled="T">1</param>
   <param id="materialSubstrate" enabled="T">Au</param>
   <param id="thicknessSubstrate" enabled="T">50</param>
   <param id="roughnessSubstrate" enabled="T">0</param>
   <param id="densitySubstrate" auto="T" enabled="T">19.3</param>
   <param id="surfaceCoating" comment="Substrate only" enabled="T">0</param>
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
   <param id="lateralThicknessGradientCoating" comment="No" enabled="F">0</param>
   <param id="gradientC1B1" enabled="F">0</param>
   <param id="gradientC1B2" enabled="F">0</param>
   <param id="gradientC1B3" enabled="F">0</param>
   <param id="gradientC1B4" enabled="F">0</param>
   <param id="gradientC1B5" enabled="F">0</param>
   <param id="gradientC1B6" enabled="F">0</param>
   <param id="gradientC1B7" enabled="F">0</param>
   <param id="gradientC1B8" enabled="F">0</param>
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