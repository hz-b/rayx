# RAYX

<table>
  <tr>
    <td>
      <img src="https://github.com/user-attachments/assets/d12229b0-7820-475f-8f02-6b2f253c5081" alt="RAYX Logo" width="600">
    </td>
    <td>
      <strong>RAYX</strong> is a powerful, multi-component simulation platform designed to streamline the design and optimization of beamlines in synchrotron light source facilities. At the core of the platform is <i>rayx-core</i>, a high-performance library that delivers precise light tracing capabilities on both CPUs and GPUs. This core library ensures that users can achieve detailed and accurate simulations at high speeds, making it an ideal solution for complex beamline designs.
    </td>
  </tr>
</table>

To simplify the usage of _rayx-core_, the platform includes rayx, a command-line interface (CLI) tool designed for fast, one-shot tracing of beamlines. It provides comprehensive data on every ray-element intersection, making it especially valuable for generating large datasets efficiently. With its focus on ease of use, _rayx_ empowers users to quickly run simulations and retrieve detailed ray-tracing results.

For users who prefer a more visual approach, _rayx-ui_ offers a graphical user interface (GUI) that includes a 3D viewport of the beamline, enabling interactive design and exploration. This GUI provides an intuitive interface to construct and modify beamlines, allowing users to visualize their designs in real-time. _rayx-ui_ not only enhances the design process but also allows users to iteratively optimize configurations based on immediate visual feedback.

## RAYX vs RAY-UI

RAYX offers several advanced features, including:
- Global (not sequential) tracing of beamlines
- GPU utilization for accelerated tracing performance
- A dedicated mode for tracing multiple beamlines with ease
- Objects in RAYX can be grouped for simplified group transformations
- A GUI for intuitive beamline design

## Installing or Building RAYX

[![testUbuntu](https://github.com/hz-b/rayx/actions/workflows/testUbuntu.yml/badge.svg?branch=master)](https://github.com/hz-b/rayx/actions/workflows/testUbuntu.yml) [![testWindows](https://github.com/hz-b/rayx/actions/workflows/testWindows.yml/badge.svg?branch=master)](https://github.com/hz-b/rayx/actions/workflows/testWindows.yml) [![testUbuntuClang](https://github.com/hz-b/rayx/actions/workflows/testUbuntuClang.yml/badge.svg?branch=master)](https://github.com/hz-b/rayx/actions/workflows/testUbuntuClang.yml) [![MDBookDeploy](https://github.com/hz-b/rayx/actions/workflows/mdBookDeploy.yml/badge.svg)](https://github.com/hz-b/rayx/actions/workflows/mdBookDeploy.yml)

For additional information, please visit our [Wiki](https://hz-b.github.io/rayx/) and read our latest [paper](https://pubs.aip.org/aip/rsi/article/96/6/061302/3348292/RAYX-An-optics-simulation-software-for-synchrotron), that introduces RAYX to the scientific community. We are committed to delivering stable releases, which can be found [here](https://github.com/hz-b/rayx/releases). Please note that the `master` branch and other branches might be unstable, and building RAYX from the source could lead to unstable software. We recommend this only for developers and experienced users. If you experience issues with our distributed binaries or API, do not hesitate to [open an issue](https://github.com/hz-b/rayx/issues/new/choose). We are keen to provide assistance and develop features as the need arises.
