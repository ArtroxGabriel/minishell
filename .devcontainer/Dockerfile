# Base image: lightweight C/C++ development image maintained for VS Code devcontainers.
# Pick an Arch Linux image that includes common build tools. This image already
# creates a non-root `vscode` user which the devcontainer configuration expects.
FROM archlinux:latest

# Arguments for user configuration (kept for clarity; base image typically provides these)
ARG USER=vscode
ARG USER_UID=1000
ARG USER_GID=1000

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC

# Install build tooling, formatters, linters and useful utilities.
# Keep the list minimal but practical for systems programming in C.
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

# Create a workspace directory and give the vscode user ownership.
RUN mkdir -p /workspace \
  && chown -R ${USER_UID}:${USER_GID} /workspace

# Switch to non-root user used by the devcontainer
USER ${USER}

# Ensure pip user installs are on PATH for the vscode user
ENV PATH=/home/${USER}/.local/bin:${PATH}

# Helpful default: create a build directory (owner is the non-root user)
RUN mkdir -p /workspace/build

# Default workdir for VS Code
WORKDIR /workspace

# Notes for maintainers:
# - Keep package installs small to reduce image size.
# - If you need newer CMake/Clang on a CI machine, prefer installing from upstream apt repos
#   or using prebuilt binaries rather than the distribution packages.
# - The devcontainer.json orchestrates editor extensions and VS Code-specific settings;
#   this Dockerfile focuses on OS-level dependencies.
