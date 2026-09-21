# Build container: compiles + runs tests during `docker build`.
FROM debian:bookworm-slim AS builder

RUN apt-get update \
  && apt-get install -y --no-install-recommends g++ cmake make \
  && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY CMakeLists.txt ./
COPY src/ src/
COPY tests/ tests/

RUN cmake -B build -DCMAKE_BUILD_TYPE=Release \
  && cmake --build build -j"$(nproc)" \
  && ctest --test-dir build --output-on-failure

# Slim runtime: game binary only.
FROM debian:bookworm-slim
COPY --from=builder /src/build/snake /usr/local/bin/snake
ENTRYPOINT ["snake"]
