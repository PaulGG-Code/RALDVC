# Pin to linux/amd64 so the lab works identically on Linux, Intel Mac, Apple Silicon,
# and Windows. The vulnerability exercises depend on x86-64 stack/heap layout.
FROM --platform=linux/amd64 ubuntu:22.04

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
    git \
    python3-dev \
    libc6-dbg \
    cppcheck \
    && rm -rf /var/lib/apt/lists/*

# Install pwntools and semgrep and flowfinder
RUN pip3 install pwntools semgrep flowfinder

# Install pwndbg for enhanced GDB heap/stack visualization
# Adds: heap, vis_heap_chunks, telescope, context commands in GDB
RUN git clone --depth=1 https://github.com/pwndbg/pwndbg /opt/pwndbg \
    && cd /opt/pwndbg && ./setup.sh --quiet 2>&1 | tail -5

# Install syft (SBOM generator)
RUN curl -sSfL https://raw.githubusercontent.com/anchore/syft/main/install.sh \
    | sh -s -- -b /usr/local/bin

# Install grype (vulnerability scanner for SBOMs and images)
RUN curl -sSfL https://raw.githubusercontent.com/anchore/grype/main/install.sh \
    | sh -s -- -b /usr/local/bin

# GDB config: Intel syntax, no pagination, pretty printing
RUN echo 'set disassembly-flavor intel' >> /root/.gdbinit && \
    echo 'set pagination off'           >> /root/.gdbinit && \
    echo 'set print pretty on'          >> /root/.gdbinit

# Create lab directory
WORKDIR /lab

# Day 1 – vulnerability demos and exercises
COPY day1/vulnerabilities/ /lab/day1/vulnerabilities/
COPY day1/exercises/       /lab/day1/exercises/

# Day 2 – real-world target (wuftpd) for static analysis
COPY day2/wuftpd/          /lab/day2/wuftpd/

# Disable ASLR (also set via sysctl in docker-compose, but belt-and-suspenders)
RUN echo 0 > /proc/sys/kernel/randomize_va_space 2>/dev/null || true

# Build all day1 vulnerability modules at image build time
RUN for dir in /lab/day1/vulnerabilities/*/; do \
        echo "Building $dir ..."; \
        make -C "$dir" all 2>&1 || true; \
    done

# Build all day1 exercises at image build time
RUN make -C /lab/day1/exercises all 2>&1 || true

# Add a helpful banner on shell start
RUN echo 'cat /lab/day1/vulnerabilities/WELCOME.txt 2>/dev/null || true' >> /root/.bashrc

# Entrypoint: tries to disable ASLR at container start, continues gracefully if
# the host kernel denies the write (Mac/Windows Docker Desktop).
COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
CMD ["/bin/bash"]
