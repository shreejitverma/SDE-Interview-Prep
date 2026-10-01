#!/bin/bash
set -e

echo "=== Lab 03 Virtualization ==="

# 1. KSM Test
echo "--- KSM Page Sharing ---"
# enable KSM if not enabled (requires sudo)
sudo sh -c 'echo 1 > /sys/kernel/mm/ksm/run' || true
sudo sh -c 'echo 10 > /sys/kernel/mm/ksm/sleep_millisecs' || true
sudo sh -c 'echo 50000 > /sys/kernel/mm/ksm/pages_to_scan' || true

pages_shared_before=$(cat /sys/kernel/mm/ksm/pages_shared)
echo "KSM pages_shared before: $pages_shared_before"

./ksm_test &
PID1=$!
./ksm_test &
PID2=$!

# Wait for KSM to scan and merge
sleep 15

pages_shared_after=$(cat /sys/kernel/mm/ksm/pages_shared)
echo "KSM pages_shared after: $pages_shared_after"

kill $PID1 $PID2 >/dev/null 2>&1 || true
wait $PID1 $PID2 >/dev/null 2>&1 || true

# 2. VM operations
echo "--- KVM and libvirt ---"
virsh destroy aos-lab-03-guest >/dev/null 2>&1 || true
virsh undefine aos-lab-03-guest --nvram >/dev/null 2>&1 || true
virsh undefine aos-lab-03-guest >/dev/null 2>&1 || true

# Define the VM
cat << XML > domain.xml
<domain type='kvm'>
  <name>aos-lab-03-guest</name>
  <memory unit='MiB'>512</memory>
  <currentMemory unit='MiB'>512</currentMemory>
  <vcpu placement='static'>2</vcpu>
  <os>
    <type arch='aarch64' machine='virt'>hvm</type>
    <loader readonly='yes' type='pflash'>/usr/share/AAVMF/AAVMF_CODE.fd</loader>
    <nvram template='/usr/share/AAVMF/AAVMF_VARS.fd'>/var/lib/libvirt/qemu/nvram/aos-lab-03-guest_VARS.fd</nvram>
  </os>
  <features>
    <acpi/>
    <apic/>
  </features>
  <cpu mode='host-passthrough'/>
  <clock offset='utc'/>
  <on_poweroff>destroy</on_poweroff>
  <on_reboot>restart</on_reboot>
  <on_crash>destroy</on_crash>
  <devices>
    <emulator>/usr/bin/qemu-system-aarch64</emulator>
    <disk type='file' device='disk'>
      <driver name='qemu' type='qcow2'/>
      <source file='$(pwd)/cirros.img'/>
      <target dev='vda' bus='virtio'/>
    </disk>
    <serial type='pty'>
      <target port='0'/>
    </serial>
    <console type='pty'>
      <target type='serial' port='0'/>
    </console>
    <memballoon model='virtio'/>
  </devices>
</domain>
XML

virsh define domain.xml
virsh start aos-lab-03-guest

# Wait for boot (cirros is small, but needs a few seconds)
sleep 20

# vCPU pinning
echo "- vCPU Pinning"
virsh vcpupin aos-lab-03-guest
virsh vcpupin aos-lab-03-guest 0 1
virsh vcpupin aos-lab-03-guest 1 2
virsh vcpupin aos-lab-03-guest

# Memory ballooning
echo "- Memory Ballooning"
virsh dominfo aos-lab-03-guest | grep memory
virsh setmem aos-lab-03-guest 256M --live || true
sleep 5
virsh dominfo aos-lab-03-guest | grep memory

# domstats
echo "- Domstats"
virsh domstats aos-lab-03-guest

# Cleanup
echo "--- Cleanup ---"
virsh destroy aos-lab-03-guest || true
virsh undefine aos-lab-03-guest --nvram || true
