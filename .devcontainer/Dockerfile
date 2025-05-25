FROM fedora:42
MAINTAINER Stefan Weiser <stefan.weiser@outlook.de>

RUN dnf -y update
RUN dnf -y install clang clang-tools-extra awk git cmake ninja-build gtest-devel gmock-devel

RUN useradd -G wheel -u 1000 dockeruser
RUN echo '%wheel ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers
USER dockeruser
