FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    gcc \
    gdb \
    gdb-multiarch \
    python3 \
    python3-pip \
    make \
    ltrace \
    strace \
    valgrind \
    checksec \
    binutils \
    wget \
    curl \
    file \
    vim \
    nano \
    && rm -rf /var/lib/apt/lists/*

# Install pwntools
RUN pip3 install pwntools

# Create lab directory
WORKDIR /lab

# Copy vulnerabilities into the image
COPY vulnerabilities/ /lab/vulnerabilities/

# Disable ASLR (also set via sysctl in docker-compose, but belt-and-suspenders)
RUN echo 0 > /proc/sys/kernel/randomize_va_space 2>/dev/null || true

# Build all modules at image build time
RUN for dir in /lab/vulnerabilities/*/; do \
        echo "Building $dir ..."; \
        make -C "$dir" all 2>&1 || true; \
    done

# Add a helpful banner on shell start
RUN echo 'cat /lab/vulnerabilities/WELCOME.txt 2>/dev/null || true' >> /root/.bashrc

CMD ["/bin/bash"]
