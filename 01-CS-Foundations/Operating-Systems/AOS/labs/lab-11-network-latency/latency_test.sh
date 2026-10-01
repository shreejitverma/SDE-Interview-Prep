#!/usr/bin/env bash
set -e

# Use network namespaces to avoid polluting the default namespace
# sudo is required for unshare -n

echo "=== Network Latency Budgets ==="

# 1. Baseline: Loopback in default namespace
echo "[+] Testing loopback baseline..."
ping -c 3 127.0.0.1 | tail -n 3
echo ""

# Cleanup old namespaces if they exist
sudo ip netns delete ns1 2>/dev/null || true
sudo ip netns delete ns2 2>/dev/null || true

# 2. Setup Network Namespaces and veth
echo "[+] Setting up network namespaces and veth pair..."
sudo ip netns add ns1
sudo ip netns add ns2

sudo ip link add veth1 type veth peer name veth2
sudo ip link set veth1 netns ns1
sudo ip link set veth2 netns ns2

sudo ip netns exec ns1 ip addr add 10.0.0.1/24 dev veth1
sudo ip netns exec ns1 ip link set veth1 up
sudo ip netns exec ns1 ip link set lo up

sudo ip netns exec ns2 ip addr add 10.0.0.2/24 dev veth2
sudo ip netns exec ns2 ip link set veth2 up
sudo ip netns exec ns2 ip link set lo up

# 3. Test veth latency
echo "[+] Testing veth baseline latency..."
sudo ip netns exec ns1 ping -c 3 10.0.0.2 | tail -n 3
echo ""

echo "[+] Testing veth baseline throughput (iperf3)..."
sudo ip netns exec ns2 iperf3 -s -D
sleep 1
sudo ip netns exec ns1 iperf3 -c 10.0.0.2 -t 2 --format m | grep "sender"
echo ""

# 4. Inject delay and loss with tc netem
echo "[+] Injecting 10ms delay and 5% loss with tc netem..."
sudo ip netns exec ns1 tc qdisc add dev veth1 root netem delay 10ms loss 5%

echo "[+] Testing delayed veth latency..."
sudo ip netns exec ns1 ping -c 10 10.0.0.2 | tail -n 3
echo ""

echo "[+] Testing delayed veth throughput (iperf3)..."
sudo ip netns exec ns1 iperf3 -c 10.0.0.2 -t 2 --format m | grep "sender"
echo ""

# Cleanup
echo "[+] Cleaning up..."
sudo ip netns exec ns2 pkill iperf3 || true
sudo ip netns delete ns1
sudo ip netns delete ns2

echo "Done."
