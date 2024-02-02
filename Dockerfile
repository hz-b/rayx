FROM ubuntu:23.04

ARG DEBIAN_FRONTEND=noninteractive

ENV NVIDIA_DRIVER_CAPABILITIES compute,graphics,utility

# Install Vulkan
RUN apt-get update \
    && apt-get install -y \
    libxext6 \
    libvulkan1 \
    libvulkan-dev \
    vulkan-tools \ 
    wget \
    gnupg \
    software-properties-common \
    lsb-release \
    cmake \ 
    git \
    build-essential \
    libxrandr-dev \
    libhdf5-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    pkg-config \
    libgtk-3-dev

# Add the LunarG signing key
RUN wget -qO- https://packages.lunarg.com/lunarg-signing-key-pub.asc | tee /etc/apt/trusted.gpg.d/lunarg.asc

# Add the Vulkan SDK repository for Ubuntu 22.04 (Jammy)
RUN wget -qO /etc/apt/sources.list.d/lunarg-vulkan-1.3.275-jammy.list https://packages.lunarg.com/vulkan/1.3.275/lunarg-vulkan-1.3.275-jammy.list

# Update apt package list and install the Vulkan SDK
RUN apt-get update && apt-get install -y vulkan-sdk

# Clone the repository
RUN git clone --recurse-submodules https://github.com/hz-b/rayx.git -b development \
    && cd rayx \
    && mkdir build \
    && cd build \
    && cmake -B . -S .. -DCMAKE_BUILD_TYPE=Release \
    && cmake --build . --config Release

RUN ./rayx/build/bin/release/rayx-core-tst
