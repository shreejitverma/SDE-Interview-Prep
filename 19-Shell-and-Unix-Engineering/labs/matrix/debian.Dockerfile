# Shell matrix image 2 of 2: GNU userland, dash as /bin/sh, ksh93u+m, posh, mksh, zsh, and BusyBox,
# plus three awks (mawk as the default awk, gawk, and original-awk, the one true awk) for awk portability labs.
# Built and run by labs/run-matrix.sh --containers; the base tag is pinned so a run is reproducible.
FROM debian:trixie-slim
RUN apt-get update -qq \
  && apt-get install -y -qq --no-install-recommends bash bats busybox dash gawk ksh93u+m mawk mksh original-awk posh zsh >/dev/null \
  && rm -rf /var/lib/apt/lists/* \
  && update-alternatives --quiet --set awk /usr/bin/mawk \
  && dpkg-query -W -f='${Package}-${Version}\n' bash bats busybox coreutils dash gawk ksh93u+m mawk mksh original-awk posh zsh | sort > /etc/shell-matrix-versions
# Labs run unprivileged so permission and ownership behavior matches a normal user.
RUN useradd --create-home lab
USER lab
WORKDIR /home/lab
