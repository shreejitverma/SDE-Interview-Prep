#!/usr/bin/env bash
set -e

echo "--- Capabilities ---"
echo "Current capabilities of this process:"
capsh --print | grep Current || true

echo ""
echo "--- Namespaces (User and Mount) ---"
echo "Inside current namespace:"
id

echo ""
echo "Entering new user and mount namespace (sudo unshare -U -m -r)..."
sudo unshare -U -m -r sh -c '
    echo "Inside namespace:"
    id
    echo "Mounting tmpfs to /mnt..."
    mount -t tmpfs none /mnt
    echo "I am a secret" > /mnt/secret.txt
    ls -l /mnt/secret.txt
'

echo ""
echo "Back in original namespace:"
echo "Checking for /mnt/secret.txt (should not exist):"
ls -l /mnt/secret.txt 2>/dev/null || echo "File not found. Mount isolation successful."
