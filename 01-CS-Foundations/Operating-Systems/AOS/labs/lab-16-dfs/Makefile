.PHONY: all run test clean

all: fuse_cache
	chmod +x raid5.py lfs.py

fuse_cache: fuse_cache.c
	gcc -Wall -Wextra -O2 -pthread fuse_cache.c -o fuse_cache $$(pkg-config fuse3 --cflags --libs)

run: all
	@echo "=== RAID-5 Parity ==="
	@echo "Hello RAID-5 World" > raid_input.txt
	./raid5.py write raid_input.txt disk
	@ls -l disk_*.dat
	@rm disk_1.dat
	@echo "Removed disk 1. Reconstructing..."
	./raid5.py reconstruct 1 disk
	./raid5.py read raid_output.txt disk
	@cat raid_output.txt
	@echo
	@echo "=== Log-Structured Store ==="
	@rm -f store.log store.log.tmp
	./lfs.py store.log write 1 "First version of object 1"
	./lfs.py store.log write 2 "Object 2 data"
	./lfs.py store.log write 1 "Second version of object 1"
	./lfs.py store.log index
	./lfs.py store.log read 1
	./lfs.py store.log clean
	./lfs.py store.log index
	@echo "=== FUSE Write-Back Cache ==="
	@mkdir -p mnt
	@rm -f backing_test.txt
	@./fuse_cache backing_test.txt mnt > fuse.log 2>&1 & \
	 FUSE_PID=$$!; \
	 sleep 1; \
	 echo "Writing to cached mount..."; \
	 python3 -c "import time; f=open('mnt/test.txt', 'w'); f.write('A'*10); f.flush(); time.sleep(2); f.close()" & \
	 WRITER_PID=$$!; \
	 sleep 1; \
	 echo "Wrote 10 bytes (file still open)."; \
	 echo "Checking backing store directly (should be empty or not exist):"; \
	 cat backing_test.txt 2>/dev/null || echo "Not flushed yet"; \
	 wait $$WRITER_PID; \
	 echo "File closed. Checking backing store again:"; \
	 cat backing_test.txt 2>/dev/null || echo "Still missing!"; \
	 echo; \
	 echo "Unmounting..."; \
	 fusermount3 -u mnt 2>/dev/null || umount mnt 2>/dev/null || true; \
	 sleep 1; \
	 kill $$FUSE_PID 2>/dev/null || true; \
	 echo "Checking backing store again:"; \
	 cat backing_test.txt 2>/dev/null || echo "Still missing!"; \
	 echo; \
	 cat fuse.log

test: all
	@set -e; for n in 0 1 15 16 17 1000; do \
	  head -c $$n /dev/urandom > t_in.bin; \
	  ./raid5.py write t_in.bin t >/dev/null; \
	  ./raid5.py read t_out.bin t >/dev/null; cmp -s t_in.bin t_out.bin; \
	  for d in 0 1 2 3 4; do \
	    cp t_$$d.dat t_keep.dat; rm t_$$d.dat; \
	    ./raid5.py reconstruct $$d t >/dev/null; cmp -s t_$$d.dat t_keep.dat; \
	    ./raid5.py read t_out.bin t >/dev/null; cmp -s t_in.bin t_out.bin; \
	  done; \
	done; rm -f t_in.bin t_out.bin t_keep.dat t_*.dat; echo "PASS: RAID-5 round trip and single-disk rebuild (sizes 0-1000, every disk)"
	@$(MAKE) -s run >/dev/null && echo "PASS: lab-16-dfs demo runs"

clean:
	rm -f raid_input.txt raid_output.txt disk_*.dat store.log store.log.tmp fuse.log fuse_cache backing_test.txt
	fusermount3 -u mnt 2>/dev/null || true
	rm -rf mnt
