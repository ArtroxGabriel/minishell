# Dockerfile - build & test container for os-c-template
#
# Usage:
#  Build image:
#    docker build -t os-c-template:dev .
#
#  Run interactive shell:
#    docker run --rm -it -v "$(pwd)":/workspace -w /workspace os-c-template:dev
#
#  Run build:
#    docker run --rm -v "$(pwd)":/workspace -w /workspace os-c-template:dev build
#
#  Run tests (will build first if needed):
#    docker run --rm -v "$(pwd)":/workspace -w /workspace os-c-template:dev test
#
# The container provides a small entrypoint with convenience commands:
#  - build  : configure + build (Ninja) using CMake
#  - test   : run ctest (builds first if necessary)
#  - shell  : open an interactive bash shell
#  - any other args are executed as a command inside the container
#
# Keep this image minimal but useful for CI and local development.

FROM archlinux:latest

ARG USER=builder
ARG UID=1000
ARG GID=1000

# Versions / build options (can be overridden at docker build/run time)
ARG CMAKE_GENERATOR="Ninja"
ARG CMAKE_BUILD_TYPE="Debug"

# Install common build and analysis tooling
RUN pacman -Syu --noconfirm \
    base-devel \
    cmake \
    ninja \
    git \
    clang \
    clang-tools-extra \
    lcov \
    valgrind \
    python-pip \
    python-virtualenv \
    pkgconf \
    cmocka \
    cppcheck \
    ca-certificates \
    tzdata \
    && pip install --no-cache-dir cmake-format || true

# Create non-root user for safer development builds
RUN groupadd -g ${GID} ${USER} || true \
    && useradd -m -u ${UID} -g ${GID} -s /bin/bash ${USER} || true \
    && mkdir -p /workspace /home/${USER}/.local/bin \
    && chown -R ${USER}:${USER} /workspace /home/${USER}/.local

WORKDIR /workspace

# Install an entrypoint that provides small build/test helpers.
# It allows `docker run ... build` and `docker run ... test` for convenience.
COPY --chown=${USER}:${USER} . /workspace

RUN cat > /usr/local/bin/ctemplate-entrypoint.sh <<'EOF'
#!/usr/bin/env bash
set -euo pipefail

# Simple entrypoint for building/testing the CMake-based project.
# Supports commands: build, test, shell. If none provided, opens a shell.

CMD=${1:-shell}
shift || true

# Default cmake settings (can be overriden with env vars)
: ${CMAKE_GENERATOR:="${CMAKE_GENERATOR}"}
: ${CMAKE_BUILD_TYPE:="${CMAKE_BUILD_TYPE}"}
: ${BUILD_DIR:=build}
: ${ENABLE_SANITIZERS:=OFF}
: ${CC:=gcc}
: ${CXX:=g++}

run_cmake_build() {
    mkdir -p "${BUILD_DIR}"
    cmake -S . -B "${BUILD_DIR}" \
        -G "${CMAKE_GENERATOR}" \
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
        -DENABLE_SANITIZERS=${ENABLE_SANITIZERS} \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DDEFAULT_CC=${CC} \
        -DDEFAULT_CXX=${CXX}
    cmake --build "${BUILD_DIR}" -- -j$(nproc)
}

case "${CMD}" in
    build)
        echo "[entrypoint] Building project (generator=${CMAKE_GENERATOR}, build-type=${CMAKE_BUILD_TYPE})"
        run_cmake_build
        ;;
    test)
        echo "[entrypoint] Running tests (will build if necessary)"
        if [ ! -d "${BUILD_DIR}" ] || [ ! -f "${BUILD_DIR}/build.ninja" ] && [ ! -f "${BUILD_DIR}/Makefile" ]; then
            echo "[entrypoint] No build tree found; building first"
            run_cmake_build
        fi
        # Use ctest to run tests; allow forwarding additional args
        ctest --test-dir "${BUILD_DIR}" --output-on-failure "$@"
        ;;
    shell)
        exec /bin/bash -l
        ;;
    *)
        # Execute arbitrary command
        exec "$CMD" "$@"
        ;;
esac
EOF

RUN chmod +x /usr/local/bin/ctemplate-entrypoint.sh

# Ensure the builder user owns the workspace contents and entrypoint
RUN chown -R ${USER}:${USER} /workspace /usr/local/bin/ctemplate-entrypoint.sh

USER ${USER}
ENV PATH=/home/${USER}/.local/bin:${PATH}

ENTRYPOINT ["/usr/local/bin/ctemplate-entrypoint.sh"]
CMD ["shell"]
