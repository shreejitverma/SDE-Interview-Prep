# Image for `# requires: linux` labs when the host is not Linux; built and run by verify.py.
# The base is pinned by multi-arch index digest (ubuntu:24.04 as of 2026-10-24). The packages come from the
# live archive at build time, because Ubuntu's archive snapshots do not cover ports.ubuntu.com, where arm64 lives,
# and an exact version pin fails the build once the archive drops a superseded version. A tool that drifts in a
# rebuild shows up as a `make verify` diff; /etc/toolchain-labs-versions names what was installed.
# The image tag is a hash of this file, so editing it forces a rebuild. Adding a tool may change a recording.
FROM ubuntu:24.04@sha256:fe81560990fae86dc4743202b2cb4d3df98d0bcf949173cecef15ce1127ef852
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update -qq \
  && apt-get install -y -qq --no-install-recommends binutils elfutils g++ gcc gdb libc6-dev make python3 >/dev/null \
  && rm -rf /var/lib/apt/lists/* \
  && dpkg-query -W -f='${Package}-${Version}\n' binutils elfutils g++ gcc gdb libc6-dev make python3 | sort > /etc/toolchain-labs-versions
# Labs run unprivileged so permission and ownership behavior matches a normal user.
RUN useradd --create-home lab
USER lab
WORKDIR /home/lab
