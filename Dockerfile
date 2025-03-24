FROM ubuntu:24.04
RUN apt-get update && apt-get install -y build-essential qemu-system-x86 gcc-multilib
WORKDIR /xv6