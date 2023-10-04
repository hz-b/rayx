# RAYX: A synchrotron radiation tracer

<img src="https://user-images.githubusercontent.com/13185253/167402648-788eee6a-2ba4-466a-8a0a-62e59663e957.png" width="400" height="400"/>

RAYX is a simulation and design tool for beamlines at electron storage rings. It traces synchrotron radiation through mirrors, gratings, and more. It gives the user helpful insights into how a given beamline performs. 

This software is the successor of [RAY-UI](https://www.helmholtz-berlin.de/forschung/oe/wi/optik-strahlrohre/arbeitsgebiete/ray_en.html) and aims to reengineer a valuable tool for physicists, engineers and others working on beamlines for electron storage rings.

Currently, we only provide a CLI application for the RAYX-CORE library, enabling you to trace beamlines created through RAY-UI efficiently. The main reason for the faster simulations is that we utilize GPUs for the tracing process.


We divide this wiki into three main parts. They are designed for App Users, API Users, and Developer. The areas are not exclusive to these audiences, and an API User might need to look at some pages in the App User Area. A Developer might find interesting info in all three parts.
