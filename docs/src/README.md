# RAY-X: A synchrotron radiation tracer

<img src="https://user-images.githubusercontent.com/13185253/167402648-788eee6a-2ba4-466a-8a0a-62e59663e957.png" width="400" height="400"/>

RAY-X is a simulation and design tool for beamlines in energy storage rings. It traces synchrotron radiation through a series of mirrors, gratings and more and gives the user helpful insights, how a given beamline performs. 

This software is the predecessor of [RAY-UI](https://www.helmholtz-berlin.de/forschung/oe/wi/optik-strahlrohre/arbeitsgebiete/ray_en.html) and aims to reengineer a valuable tool for physicists, engineers and others working on beamlines for electron storage rings.

For the moment we only support a cli application for the RayCore-library, which will enable you to trace beamlines created through RAY-UI efficiently. The main reason for the faster simulations is that we make use of dedicated GPUs.


This wiki is divided into three main parts. They are designed for App Users, API Users, and Developer. The areas are not exclusive to these audiences and an API User might need to look at some pages in the App User Area and a Developer might find interesting info in all three parts.

- [App User Area](./AppUser/AppUser.md)
- [API User Area](./APIUser/APIUser.md)
- [Developer Area](./Developer/Developer.md)