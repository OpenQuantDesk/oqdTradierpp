# Multi-stage Dockerfile for oqdTradierpp library

# Build stage
FROM ubuntu:22.04 AS builder

# Avoid prompts from apt
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    git \
    wget \
    libboost-all-dev \
    libsimdjson-dev \
    libssl-dev \
    libgtest-dev \
    libgmock-dev \
    libwebsocketpp-dev \
    valgrind \
    lcov \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /src

# Copy source code
COPY . .

# Configure and build
RUN cmake -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wpedantic" \
    && cmake --build build --config Release -j$(nproc)

# Run tests to ensure build is working
RUN cd build && \
    ./tests/oqdTradierpp_unit_tests && \
    ./tests/performance/oqdTradierpp_performance_tests

# Install the library
RUN cd build && make install

# Runtime stage
FROM ubuntu:22.04 AS runtime

ENV DEBIAN_FRONTEND=noninteractive

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libboost-system1.74.0 \
    libboost-thread1.74.0 \
    libboost-url1.74.0 \
    libsimdjson11 \
    libssl3 \
    && rm -rf /var/lib/apt/lists/*

# Copy installed library from builder
COPY --from=builder /usr/local/lib/liboqdTradierpp.so* /usr/local/lib/
COPY --from=builder /usr/local/include/oqdTradierpp /usr/local/include/oqdTradierpp/

# Update library cache
RUN ldconfig

# Set working directory for user applications
WORKDIR /app

# Add a non-root user
RUN useradd -m -s /bin/bash trader && chown trader:trader /app
USER trader

# Default command
CMD ["/bin/bash"]

# Development stage with full toolchain
FROM builder AS development

# Install additional development tools
RUN apt-get update && apt-get install -y \
    gdb \
    vim \
    nano \
    htop \
    strace \
    clang-tidy \
    cppcheck \
    doxygen \
    graphviz \
    && rm -rf /var/lib/apt/lists/*

# Create development user
RUN useradd -m -s /bin/bash -G sudo developer && \
    echo "developer ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

# Set up development environment
USER developer
WORKDIR /workspace

# Copy examples and tests for development
COPY --chown=developer:developer examples/ /workspace/examples/
COPY --chown=developer:developer tests/ /workspace/tests/

# Set development environment variables
ENV CMAKE_BUILD_TYPE=Debug
ENV CTEST_OUTPUT_ON_FAILURE=1

CMD ["/bin/bash"]