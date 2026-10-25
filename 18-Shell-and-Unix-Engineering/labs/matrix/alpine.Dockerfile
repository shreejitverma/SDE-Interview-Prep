# Shell matrix image 1 of 2: BusyBox userland, five bash releases, and the small POSIX shells.
# Each bash comes from the official `bash` image for that release, copied next to Alpine's own shells.
# Built and run by labs/run-matrix.sh --containers; tags are pinned so a run is reproducible.
FROM bash:3.2 AS bash32
FROM bash:4.4 AS bash44
FROM bash:5.1 AS bash51
FROM bash:5.2 AS bash52
FROM bash:5.3 AS bash53

FROM alpine:3.22
RUN apk add --no-cache bash bats dash mksh ncurses-libs readline yash zsh \
  && apk list --installed bash bats busybox dash mksh yash zsh | cut -d' ' -f1 | sort > /etc/shell-matrix-versions
COPY --from=bash32 /usr/local/bin/bash /opt/shells/bash-3.2
COPY --from=bash44 /usr/local/bin/bash /opt/shells/bash-4.4
COPY --from=bash51 /usr/local/bin/bash /opt/shells/bash-5.1
COPY --from=bash52 /usr/local/bin/bash /opt/shells/bash-5.2
COPY --from=bash53 /usr/local/bin/bash /opt/shells/bash-5.3
# Labs run unprivileged so permission and ownership behavior matches a normal user.
RUN adduser -D lab
USER lab
WORKDIR /home/lab
