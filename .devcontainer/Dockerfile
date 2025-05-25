FROM fedora:42
MAINTAINER Stefan Weiser <stefan.weiser@outlook.de>

# update and install dependencies
RUN dnf -y update
RUN dnf -y install \
    awk \
    clang \
    clang-tools-extra \
    cmake \
    doxygen \
    git \
    gmock-devel \
    gtest-devel \
    ninja-build \
    wget

RUN wget https://github.com/bazelbuild/bazelisk/releases/download/v1.26.0/bazelisk-linux-amd64
RUN chmod +x bazelisk-linux-amd64
RUN mv bazelisk-linux-amd64 /usr/local/bin/bazel

# add user and make it root without password
RUN useradd -G wheel -u 1000 dockeruser
RUN echo '%wheel ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers
USER dockeruser
