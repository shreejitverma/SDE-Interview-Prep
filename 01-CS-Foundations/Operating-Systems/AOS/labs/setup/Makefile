# CS 6210 AOS lab environment. Run from the host unless noted.
VM ?= aos

.PHONY: vm install check-env shell
vm:            ## create and start the Lima VM
	limactl create --name=$(VM) aos-lab.yaml && limactl start $(VM)
install:       ## install lab packages inside the VM
	limactl shell $(VM) sudo bash "$(CURDIR)/install-packages.sh"
check-env:     ## report lab capabilities inside the VM
	limactl shell $(VM) bash "$(CURDIR)/check-env.sh"
shell:         ## open a shell in the VM at this folder
	limactl shell --workdir "$(CURDIR)/.." $(VM)
