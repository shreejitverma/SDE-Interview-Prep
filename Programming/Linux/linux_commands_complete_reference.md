# Complete Linux Commands Reference

This is an exhaustive list of all essential Linux commands organized by category.

## Table of Contents
1. [File & Directory Management](#file--directory-management)
2. [File Content & Viewing](#file-content--viewing)
3. [File Permissions & Ownership](#file-permissions--ownership)
4. [User & Group Management](#user--group-management)
5. [Process Management](#process-management)
6. [System Information](#system-information)
7. [Disk & Storage](#disk--storage)
8. [Networking](#networking)
9. [Compression & Archiving](#compression--archiving)
10. [Text Editors](#text-editors)
11. [Package Management](#package-management)
12. [System Services](#system-services)
13. [Logging & Monitoring](#logging--monitoring)
14. [Scheduling & Automation](#scheduling--automation)
15. [Search & Replace](#search--replace)
16. [Environment & Variables](#environment--variables)
17. [Other Utilities](#other-utilities)

---

## File & Directory Management

### Navigation

| Command | Description | Example |
|---------|-------------|---------|
| pwd | Print working directory - shows current path | `pwd` |
| cd | Change directory - navigate between directories | `cd /home/user/Documents` |
| ls | List directory contents - view files and folders | `ls -la /home` |
| tree | Display directory tree structure recursively | `tree -L 2 /home` |
| find | Search for files in directory hierarchy | `find . -name '*.txt' -type f` |
| locate | Find files using database (faster than find) | `locate python3` |

### File Operations

| Command | Description | Example |
|---------|-------------|---------|
| touch | Create empty file or update timestamp | `touch newfile.txt` |
| cp | Copy files or directories | `cp file1.txt file2.txt` |
| mv | Move or rename files and directories | `mv old_name.txt new_name.txt` |
| rm | Remove files or directories | `rm file.txt` |
| rmdir | Remove empty directories | `rmdir empty_dir` |
| mkdir | Create new directories | `mkdir -p /path/to/new/dir` |
| file | Determine file type | `file document.pdf` |
| stat | Display file/directory information | `stat file.txt` |

### Directory Operations

| Command | Description | Example |
|---------|-------------|---------|
| pushd | Push directory onto stack and change | `pushd /var/log` |
| popd | Pop directory from stack | `popd` |
| dirs | List directory stack | `dirs -v` |

---

## File Content & Viewing

### Text Viewing

| Command | Description | Example |
|---------|-------------|---------|
| cat | Display file contents | `cat file.txt` |
| less | View file contents page by page | `less large_file.txt` |
| more | View file contents with paging | `more file.txt` |
| head | Display first lines of file | `head -n 20 file.txt` |
| tail | Display last lines of file | `tail -f log.txt` |
| nl | Number lines in output | `nl file.txt` |
| od | Display file in octal/hex/ASCII format | `od -c file.txt` |

### Text Processing

| Command | Description | Example |
|---------|-------------|---------|
| grep | Search text patterns in files | `grep -r 'pattern' /path` |
| egrep | Extended grep with regex | `egrep '[0-9]+' file.txt` |
| fgrep | Fixed string grep (faster) | `fgrep 'exact_string' file.txt` |
| sed | Stream editor for text transformation | `sed 's/old/new/g' file.txt` |
| awk | Pattern scanning and processing language | `awk '{print $1, $3}' file.txt` |
| cut | Extract columns from text | `cut -d',' -f1,3 file.csv` |
| tr | Translate or delete characters | `tr 'a-z' 'A-Z' < file.txt` |
| sort | Sort lines of text | `sort -n numbers.txt` |
| uniq | Remove consecutive duplicate lines | `uniq file.txt` |
| wc | Count lines, words, bytes | `wc -l file.txt` |
| diff | Compare files line by line | `diff file1.txt file2.txt` |
| cmp | Compare files byte by byte | `cmp file1.txt file2.txt` |
| comm | Compare sorted files | `comm file1.txt file2.txt` |
| patch | Apply patch files | `patch < changes.patch` |

---

## File Permissions & Ownership

### Permissions

| Command | Description | Example |
|---------|-------------|---------|
| chmod | Change file permissions | `chmod 755 script.sh` |
| chmod +x | Make file executable | `chmod +x script.sh` |
| chmod -R | Recursively change permissions | `chmod -R 644 /path/to/dir` |
| umask | Set default file creation permissions | `umask 022` |

### Ownership

| Command | Description | Example |
|---------|-------------|---------|
| chown | Change file owner and group | `chown user:group file.txt` |
| chown -R | Recursively change ownership | `chown -R www-data:www-data /var/www` |
| chgrp | Change file group | `chgrp groupname file.txt` |

---

## User & Group Management

### User Management

| Command | Description | Example |
|---------|-------------|---------|
| useradd | Create new user account | `useradd -m -s /bin/bash username` |
| usermod | Modify user account properties | `usermod -aG sudo username` |
| userdel | Delete user account | `userdel -r username` |
| passwd | Set or change user password | `passwd username` |
| whoami | Display current user | `whoami` |
| id | Display user ID and group ID | `id username` |
| groups | Show groups user belongs to | `groups username` |
| su | Switch user (substitute user) | `su - username` |
| sudo | Execute command with root privileges | `sudo apt-get install package` |
| finger | Display user information | `finger username` |

### Group Management

| Command | Description | Example |
|---------|-------------|---------|
| groupadd | Create new group | `groupadd groupname` |
| groupmod | Modify group properties | `groupmod -n newname oldname` |
| groupdel | Delete group | `groupdel groupname` |

---

## Process Management

### Process Information

| Command | Description | Example |
|---------|-------------|---------|
| ps | Display process status | `ps aux` |
| ps aux | List all running processes | `ps aux | grep python` |
| top | Display real-time system processes | `top` |
| htop | Interactive process viewer (better than top) | `htop` |
| pstree | Display process tree | `pstree -p` |
| jobs | List current shell jobs | `jobs -l` |
| fg | Bring job to foreground | `fg %1` |
| bg | Run job in background | `bg %1` |
| wait | Wait for process to complete | `wait PID` |

### Process Control

| Command | Description | Example |
|---------|-------------|---------|
| kill | Send signal to process | `kill -15 PID` |
| killall | Kill all processes by name | `killall firefox` |
| pkill | Kill process by pattern | `pkill -f 'python script'` |
| nice | Start process with different priority | `nice -n 10 command` |
| renice | Change process priority | `renice -n 5 -p PID` |
| nohup | Run command immune to hangups | `nohup python script.py &` |
| disown | Remove job from shell control | `disown %1` |
| exec | Replace current shell with command | `exec python script.py` |

---

## System Information

### System Status

| Command | Description | Example |
|---------|-------------|---------|
| uname | Display system information | `uname -a` |
| hostnamectl | View/set system hostname | `hostnamectl set-hostname newname` |
| uptime | Show system uptime and load | `uptime` |
| date | Display/set system date and time | `date` |
| cal | Display calendar | `cal 2025` |
| df | Display disk space usage | `df -h` |
| du | Display directory size | `du -sh /home` |
| free | Show memory usage | `free -h` |
| vmstat | Virtual memory statistics | `vmstat 1 5` |
| lsof | List open files | `lsof -i :8080` |
| dmesg | Display kernel ring buffer | `dmesg | tail -n 20` |

### Hardware Info

| Command | Description | Example |
|---------|-------------|---------|
| lsblk | List block devices | `lsblk -o NAME,SIZE,TYPE` |
| lscpu | Display CPU information | `lscpu` |
| lspci | List PCI devices | `lspci` |
| lsusb | List USB devices | `lsusb` |
| hwinfo | Display hardware information | `hwinfo --short` |

---

## Disk & Storage

| Command | Description | Example |
|---------|-------------|---------|
| fdisk | Partition management tool | `sudo fdisk -l` |
| parted | GNU parted - partition editor | `sudo parted /dev/sda` |
| mount | Mount filesystem | `sudo mount /dev/sda1 /mnt` |
| umount | Unmount filesystem | `sudo umount /mnt` |
| fsck | Filesystem check and repair | `sudo fsck /dev/sda1` |
| mkfs | Create filesystem | `sudo mkfs.ext4 /dev/sda1` |
| dd | Copy/convert data at low level | `sudo dd if=/dev/sda of=backup.img` |

---

## Networking

### Network Info

| Command | Description | Example |
|---------|-------------|---------|
| ifconfig | Display network interface config | `ifconfig eth0` |
| ip | Show/manipulate routing, devices, policy | `ip addr show` |
| hostname | Display or set hostname | `hostname -I` |
| netstat | Display network statistics | `netstat -an` |
| ss | Socket statistics (replacement for netstat) | `ss -tuln` |
| arp | Display ARP cache | `arp -a` |
| route | Display/modify routing table | `route -n` |
| iw | Wireless device configuration | `iw dev` |

### Network Operations

| Command | Description | Example |
|---------|-------------|---------|
| ping | Test connectivity to host | `ping -c 4 google.com` |
| traceroute | Trace network path to host | `traceroute google.com` |
| tracepath | Trace path to network host | `tracepath -m 20 google.com` |
| dig | Domain information groper (DNS lookup) | `dig google.com` |
| nslookup | Query DNS | `nslookup google.com` |
| whois | Query WHOIS database | `whois google.com` |
| curl | Transfer data using URL | `curl -O https://example.com/file` |
| wget | Download files from web | `wget -r https://example.com` |
| nc | Netcat - network utility | `nc -l -p 5000` |

### SSH & Remote

| Command | Description | Example |
|---------|-------------|---------|
| ssh | Secure shell remote login | `ssh user@host.com` |
| ssh-keygen | Generate SSH key pair | `ssh-keygen -t rsa -b 4096` |
| scp | Secure copy files over SSH | `scp file.txt user@host:/path` |
| sftp | Secure FTP | `sftp user@host.com` |
| ssh-copy-id | Copy SSH public key to server | `ssh-copy-id user@host.com` |
| ssh-agent | SSH private key agent | `ssh-agent bash` |
| telnet | Remote login (unencrypted) | `telnet example.com 23` |
| ftp | File Transfer Protocol | `ftp ftp.example.com` |

---

## Compression & Archiving

### Compression

| Command | Description | Example |
|---------|-------------|---------|
| gzip | Compress files with gzip | `gzip file.txt` |
| gunzip | Decompress gzip files | `gunzip file.txt.gz` |
| bzip2 | Compress with bzip2 | `bzip2 file.txt` |
| bunzip2 | Decompress bzip2 files | `bunzip2 file.txt.bz2` |
| xz | Compress with xz (best compression) | `xz file.txt` |
| unxz | Decompress xz files | `unxz file.txt.xz` |
| zip | Compress to zip format | `zip archive.zip file1 file2` |
| unzip | Decompress zip files | `unzip archive.zip` |

### Archiving

| Command | Description | Example |
|---------|-------------|---------|
| tar | Archive files/directories | `tar -cvf archive.tar /path` |
| tar -xvf | Extract tar archive | `tar -xvf archive.tar` |
| tar -tvf | List tar archive contents | `tar -tvf archive.tar` |
| tar -czf | Create gzip-compressed tar | `tar -czf archive.tar.gz /path` |
| tar -xzf | Extract gzip-compressed tar | `tar -xzf archive.tar.gz` |
| tar -cjf | Create bzip2-compressed tar | `tar -cjf archive.tar.bz2 /path` |

---

## Text Editors

| Command | Description | Example |
|---------|-------------|---------|
| nano | Simple text editor | `nano file.txt` |
| vi | VI text editor | `vi file.txt` |
| vim | VI Improved text editor | `vim file.txt` |
| emacs | Emacs text editor | `emacs file.txt` |
| gedit | GNOME text editor | `gedit file.txt` |
| sed | Stream editor (non-interactive) | `sed -i 's/old/new/g' file.txt` |

---

## Package Management

### APT (Debian/Ubuntu)

| Command | Description | Example |
|---------|-------------|---------|
| apt-get update | Update package list | `sudo apt-get update` |
| apt-get upgrade | Upgrade installed packages | `sudo apt-get upgrade` |
| apt-get install | Install package | `sudo apt-get install package_name` |
| apt-get remove | Remove package | `sudo apt-get remove package_name` |
| apt-get autoremove | Remove unused dependencies | `sudo apt-get autoremove` |
| apt search | Search for packages | `apt search python3` |
| apt show | Show package information | `apt show package_name` |

### YUM/DNF (RedHat/CentOS)

| Command | Description | Example |
|---------|-------------|---------|
| yum install | Install package (older YUM) | `sudo yum install package_name` |
| yum update | Update packages (YUM) | `sudo yum update` |
| dnf install | Install package (newer DNF) | `sudo dnf install package_name` |
| dnf update | Update packages (DNF) | `sudo dnf update` |
| dnf search | Search packages | `dnf search package_name` |

---

## System Services

### Service Management

| Command | Description | Example |
|---------|-------------|---------|
| systemctl start | Start a service | `sudo systemctl start apache2` |
| systemctl stop | Stop a service | `sudo systemctl stop apache2` |
| systemctl restart | Restart a service | `sudo systemctl restart apache2` |
| systemctl reload | Reload service config | `sudo systemctl reload apache2` |
| systemctl enable | Enable service on boot | `sudo systemctl enable apache2` |
| systemctl disable | Disable service on boot | `sudo systemctl disable apache2` |
| systemctl status | Check service status | `systemctl status apache2` |
| systemctl list-units | List all services | `systemctl list-units --type=service` |

### System Control

| Command | Description | Example |
|---------|-------------|---------|
| shutdown | Shutdown the system | `sudo shutdown -h now` |
| reboot | Reboot the system | `sudo reboot` |
| poweroff | Power off the system | `sudo poweroff` |
| systemctl suspend | Suspend system | `systemctl suspend` |
| systemctl hibernate | Hibernate system | `systemctl hibernate` |

---

## Logging & Monitoring

### Log Viewing

| Command | Description | Example |
|---------|-------------|---------|
| journalctl | View systemd journal logs | `journalctl -xe` |
| journalctl -f | Follow journal output | `journalctl -f -u apache2` |
| tail -f | Follow file updates (for logs) | `tail -f /var/log/syslog` |
| less /var/log | View specific log file | `less /var/log/auth.log` |

### Monitoring

| Command | Description | Example |
|---------|-------------|---------|
| watch | Execute command periodically | `watch -n 1 'df -h'` |
| iotop | Monitor I/O usage by process | `sudo iotop` |
| iftop | Monitor network bandwidth | `sudo iftop` |
| nethogs | Monitor per-process network usage | `sudo nethogs` |
| atop | Advanced system monitoring | `atop` |

---

## Scheduling & Automation

| Command | Description | Example |
|---------|-------------|---------|
| crontab -e | Edit cron jobs | `crontab -e` |
| crontab -l | List cron jobs | `crontab -l` |
| crontab -r | Remove cron jobs | `crontab -r` |
| at | Schedule one-time command | `at 14:30 < script.sh` |
| atq | List at scheduled jobs | `atq` |
| atrm | Remove at scheduled job | `atrm 1` |

---

## Search & Replace

| Command | Description | Example |
|---------|-------------|---------|
| grep -r | Recursively search in files | `grep -r 'pattern' /path` |
| grep -i | Case-insensitive search | `grep -i 'pattern' file.txt` |
| grep -v | Invert match (show non-matches) | `grep -v 'pattern' file.txt` |
| grep -E | Use extended regex | `grep -E '^[0-9]+' file.txt` |
| grep -c | Count matching lines | `grep -c 'pattern' file.txt` |
| grep -n | Show line numbers | `grep -n 'pattern' file.txt` |
| grep -o | Show only matching part | `grep -o 'pattern' file.txt` |

---

## Environment & Variables

| Command | Description | Example |
|---------|-------------|---------|
| env | Display environment variables | `env` |
| printenv | Print environment variables | `printenv` |
| echo $VAR | Display specific variable | `echo $HOME` |
| export | Set environment variable | `export MY_VAR='value'` |
| set | Display shell variables | `set` |
| unset | Unset variable | `unset MY_VAR` |
| source | Execute script in current shell | `source ~/.bashrc` |

---

## Other Utilities

| Command | Description | Example |
|---------|-------------|---------|
| man | Display command manual | `man ls` |
| info | Display command info (alternative to man) | `info ls` |
| whatis | Brief description of command | `whatis ls` |
| whereis | Locate command, source, and manual | `whereis ls` |
| which | Show full path to command | `which python3` |
| type | Show how command would be interpreted | `type ls` |
| alias | Create command alias | `alias ll='ls -l'` |
| unalias | Remove command alias | `unalias ll` |
| history | Display command history | `history` |
| clear | Clear terminal screen | `clear` |
| tee | Send output to file and stdout | `cat file.txt | tee output.txt` |
| xargs | Build and execute command lines | `find . -name '*.txt' | xargs rm` |
| parallel | Execute parallel jobs | `parallel echo ::: 1 2 3` |

---

## Summary

This comprehensive reference contains **210 total Linux commands** organized into 17 categories:

- File & Directory Management: 17 commands
- File Content & Viewing: 21 commands
- File Permissions & Ownership: 7 commands
- User & Group Management: 13 commands
- Process Management: 17 commands
- System Information: 16 commands
- Disk & Storage: 7 commands
- Networking: 26 commands
- Compression & Archiving: 14 commands
- Text Editors: 6 commands
- Package Management: 12 commands
- System Services: 13 commands
- Logging & Monitoring: 9 commands
- Scheduling & Automation: 6 commands
- Search & Replace: 7 commands
- Environment & Variables: 7 commands
- Other Utilities: 13 commands

All commands include descriptions and real-world examples.
