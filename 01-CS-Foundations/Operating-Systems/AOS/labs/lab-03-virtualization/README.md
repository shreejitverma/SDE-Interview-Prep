---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lessons: [L03a, L03b, L03c]
environment: "Lima VM aos (Ubuntu 24.04 arm64)"
tags: [cs6210, cs6210/lab]
---

# lab-03-virtualization: KVM and libvirt by hand: lifecycle, vCPU pinning, ballooning, and KSM page sharing

> [!info] Goal
> Make L03a, L03b, L03c concrete with real commands and measurements.
> Observe KVM and libvirt operations including nested guest lifecycle, vCPU pinning, virtio ballooning, and Kernel Samepage Merging (KSM).

> [!warning] Honor code guard
> This lab deliberately does not implement a course project.
> Observe and operate only; never write a vCPU scheduler or memory coordinator (Project 1).

## Prerequisites

See [setup](../setup/README.md) for the VM.
You need the Lima VM `aos` running with nested KVM enabled.

## Run Commands

Run the lab inside the VM using the setup script.
```sh
labs/setup/run-in-vm.sh labs/lab-03-virtualization test
```

To capture the expected output into a file, use the capture target.
```sh
labs/setup/run-in-vm.sh labs/lab-03-virtualization capture
```

## What you should see

The test script runs two separate workflows: KSM sharing and libvirt operations.
For KSM, the output shows the number of shared pages before and after allocating mergeable memory.
```text
KSM pages_shared before: 0
KSM pages_shared after: 79
```
The libvirt portion defines a tiny nested guest and starts it.
```text
Domain 'aos-lab-03-guest' started
```
It then dynamically modifies vCPU pinning using `virsh vcpupin`.
```text
 VCPU   CPU Affinity
----------------------
 0      1
 1      2
```
It also requests a memory balloon change and dumps the domain statistics.
You will see block device stats and cpu time metrics in the domstats output.
```text
  balloon.current=524288
  balloon.maximum=524288
```

## How it works

The lab uses a C program (`ksm_test.c`) that allocates anonymous memory and fills it with identical bytes.
It calls `madvise(MADV_MERGEABLE)` to tell the Linux kernel that the pages can be safely merged.
We start two instances of this program in the background.
A bash script enables the KSM background daemon by writing to `/sys/kernel/mm/ksm/run`.
The daemon scans for identical pages and merges them, increasing the `pages_shared` count.

For the virtual machine operations, the script defines a libvirt domain XML using `qemu-system-aarch64`.
It points a virtio disk to a downloaded Cirros QCOW2 image.
We use `virsh vcpupin` to tie the guest's virtual CPUs to specific physical CPUs of our nested VM.
We also use `virsh setmem` to request memory changes via the virtio-balloon driver, although minimal guests may ignore it.
Finally, `virsh domstats` retrieves internal hypervisor metrics like CPU user/system time and I/O request counts.

## Experiments to try

1. Change the byte pattern in one of the `ksm_test` processes.
   Prediction: KSM will not find identical pages and the `pages_shared` count will remain at zero.
2. Disable KSM globally by writing 0 to `/sys/kernel/mm/ksm/run`.
   Prediction: The kernel will stop scanning and no pages will be merged, leaving the count unchanged.
3. Pin both vCPUs of the guest to the same physical CPU.
   Prediction: The guest will run slower because its two virtual processors have to timeslice on a single physical core.
4. Try decreasing the balloon memory to an extremely low value like 16M.
   Prediction: The guest kernel might trigger the OOM killer or crash because it lacks sufficient memory to operate.

## Questions

<details>
<summary>Why does the virtio balloon require guest cooperation?</summary>
The hypervisor cannot safely revoke memory that the guest operating system might be using for page tables or kernel data structures.
The virtio balloon driver runs inside the guest OS.
When the hypervisor requests memory, the driver allocates memory within the guest and pins it, then hands those physical page frames back to the hypervisor.
</details>

<details>
<summary>How does KSM handle a write to a shared page?</summary>
KSM marks the shared physical page as read-only in the page tables of all participating processes.
If a process attempts to write to the page, a page fault occurs.
The kernel intercepts the fault, creates a private copy of the page for that process, and updates its page table to point to the new copy with write permissions.
This mechanism is known as Copy-On-Write (COW).
</details>

<details>
<summary>What is the difference between CPU pinning and a vCPU scheduler?</summary>
CPU pinning is a hard constraint that restricts a virtual CPU to run only on a specified set of physical CPUs.
A vCPU scheduler is an active hypervisor component that dynamically decides which virtual CPU runs on which physical CPU over time.
Pinning overrides the scheduler's choices to ensure cache locality or isolation for specific workloads.
</details>
