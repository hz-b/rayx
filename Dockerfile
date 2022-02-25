FROM ubuntu:latest


# Common apt 
ENV TZ=Europe/Berlin
ENV APT_KEY_DONT_WARN_ON_DANGEROUS_USAGE=DontWarn
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN apt-get update && apt-get install -y  --no-install-recommends \ 
    build-essential \
    ca-certificates \
    git \
    libx11-xcb-dev \
    libxkbcommon-dev \
    libwayland-dev \
    libxrandr-dev \
    libegl1-mesa-dev \
    python3 \
    python3-dev \ 
    wget \ 
    libblas-dev \
    liblapack-dev \ 
    libhdf5-dev \
    gnupg \
    ssh-client


# CMAKE
RUN apt-get install cmake -y 

# Vulkan (Release 1.3.204)
RUN wget -qO - https://packages.lunarg.com/lunarg-signing-key-pub.asc | apt-key add -
RUN wget -qO /etc/apt/sources.list.d/lunarg-vulkan-1.3.204-focal.list https://packages.lunarg.com/vulkan/1.3.204/lunarg-vulkan-1.3.204-focal.list
RUN apt update
RUN apt install vulkan-sdk -y

# RAY-X
RUN git clone --recurse-submodules https://gitlab.helmholtz-berlin.de/RAY/RAY-X.git -b development