# Stage 1: Build DPP
FROM ubuntu:22.04 as builder

ENV http_proxy=http://krmp-proxy.9rum.cc:3128
ENV https_proxy=http://krmp-proxy.9rum.cc:3128

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    clang \
    g++ \
    cmake \
    curl \
    libcurl4-openssl-dev \
    git \
    libssl-dev \
    zlib1g-dev

# Clone and build DPP
RUN git clone --recursive https://github.com/brainboxdotcc/DPP.git /tmp/DPP && \
    mkdir /tmp/DPP/build && \
    cd /tmp/DPP/build && \
    cmake .. && \
    make -j$(nproc) && \
    make install

# Stage 2: Final image
FROM ubuntu:22.04

ENV http_proxy=http://krmp-proxy.9rum.cc:3128
ENV http_proxys=http://krmp-proxy.9rum.cc:3128

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libcurl4-openssl-dev \
    libssl-dev \
    zlib1g

# Copy DPP from builder
COPY --from=builder /usr/local/lib /usr/local/lib
COPY --from=builder /usr/local/include /usr/local/include

# Set up project directory
WORKDIR /app
COPY . .

# Build project
RUN apt-get install -y g++ cmake && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make

CMD ["./build/mybot"]
