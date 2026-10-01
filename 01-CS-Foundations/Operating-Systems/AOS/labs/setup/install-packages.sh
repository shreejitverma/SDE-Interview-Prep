#!/usr/bin/env bash
# Install every package the CS 6210 AOS labs use. Run inside the Lima guest:
#   limactl shell aos sudo bash labs/setup/install-packages.sh
set -euo pipefail
export DEBIAN_FRONTEND=noninteractive
apt-get update -q
apt-get install -y -q --no-install-recommends \
  build-essential clang gdb make cmake pkg-config git curl ca-certificates \
  linux-tools-common "linux-tools-$(uname -r)" bpftrace bpfcc-tools \
  numactl hwloc libnuma-dev strace ltrace valgrind \
  qemu-system-arm qemu-utils qemu-efi-aarch64 ipxe-qemu libvirt-daemon-system libvirt-clients virtinst \
  python3 python3-venv python3-pip python3-libvirt cloud-image-utils \
  openmpi-bin libopenmpi-dev libomp-dev \
  protobuf-compiler libprotobuf-dev protobuf-compiler-grpc libgrpc++-dev \
  fio iperf3 iproute2 rt-tests stress-ng libfuse3-dev fuse3 \
  libcap2-bin libseccomp-dev util-linux sysstat default-jdk-headless
LAB_USER="${SUDO_USER:-$USER}"
usermod -aG kvm,libvirt "$LAB_USER" || true
# perf: allow unprivileged software events, tracepoints, and kprobes for the lab user.
# Apple Virtualization.framework guests have no hardware PMU, so cycles and cache events stay unsupported.
echo 'kernel.perf_event_paranoid = 1' > /etc/sysctl.d/90-aos-perf.conf
echo 'kernel.kptr_restrict = 0' >> /etc/sysctl.d/90-aos-perf.conf
sysctl -q --system
# Real-time scheduling (chrt -f, SCHED_DEADLINE needs root) and locked memory for the real-time labs.
cat > /etc/security/limits.d/90-aos-rt.conf <<LIMITS
$LAB_USER - rtprio 99
$LAB_USER - memlock unlimited
LIMITS
# Python packages for the simulators and gRPC labs, in a venv the labs share.
python3 -m venv /opt/aos-venv
/opt/aos-venv/bin/pip install -q --upgrade pip
/opt/aos-venv/bin/pip install -q grpcio grpcio-tools pytest matplotlib cryptography fusepy
echo "install-packages: done"
