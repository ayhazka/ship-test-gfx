FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y ca-certificates

RUN echo "deb [trusted=yes] https://crashoveride95.github.io/apt/ ./" \
        > /etc/apt/sources.list.d/n64sdk.list && \
    apt-get update && apt-get install -y \
        build-essential \
        make \
        binutils-mips-n64 \
        gcc-mips-n64 \
        newlib-mips-n64 \
        n64sdk \
        makemask \
        libmus

RUN groupadd -g 1000 user && useradd -u 1000 -g 1000 -m -s /bin/bash user
ENV LANG en_US.UTF-8
ENV N64_LIBGCCDIR="/opt/crashsdk/lib/gcc/mips64-elf/12.2.0"
ENV PATH="${PATH}:/opt/crashsdk/bin"

USER user
WORKDIR /build
