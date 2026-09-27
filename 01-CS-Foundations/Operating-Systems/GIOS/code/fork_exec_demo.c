/*
 * fork_exec_demo.c
 * Demonstrates process creation with fork(), exec(), wait(), and pipe()
 *
 * Compile: gcc -Wall -o fork_exec_demo fork_exec_demo.c
 * Run:     ./fork_exec_demo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

/* ── Demo 1: Basic fork/wait ─────────────────────────────────────── */
void demo_basic_fork(void) {
    printf("\n=== Demo 1: Basic fork/wait ===\n");

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);

    } else if (pid == 0) {
        /* Child process */
        printf("[Child %d] Parent is %d\n", getpid(), getppid());
        printf("[Child %d] I will now exit with status 42\n", getpid());
        exit(42);

    } else {
        /* Parent process */
        int status;
        printf("[Parent %d] Forked child: %d\n", getpid(), pid);

        pid_t done = waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            printf("[Parent %d] Child %d exited normally with code %d\n",
                   getpid(), done, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("[Parent %d] Child %d killed by signal %d\n",
                   getpid(), done, WTERMSIG(status));
        }
    }
}

/* ── Demo 2: fork + exec ──────────────────────────────────────────── */
void demo_fork_exec(void) {
    printf("\n=== Demo 2: fork + exec ===\n");

    pid_t pid = fork();
    if (pid == 0) {
        /* Child: replace process image with 'ls -la /' */
        char *argv[] = { "ls", "-la", "/tmp", NULL };
        execvp("ls", argv);

        /* execvp only returns on failure */
        perror("execvp failed");
        exit(127);
    } else {
        int status;
        waitpid(pid, &status, 0);
        printf("[Parent] ls exited with %d\n", WEXITSTATUS(status));
    }
}

/* ── Demo 3: Pipe between parent and child ─────────────────────────── */
void demo_pipe(void) {
    printf("\n=== Demo 3: Pipe ===\n");

    int pipefd[2];
    if (pipe(pipefd) == -1) { perror("pipe"); exit(1); }
    /* pipefd[0] = read end, pipefd[1] = write end */

    pid_t pid = fork();
    if (pid == 0) {
        /* Child: write to pipe */
        close(pipefd[0]);           /* Close unused read end */

        const char *msg = "Hello from child process!";
        ssize_t written = write(pipefd[1], msg, strlen(msg));
        printf("[Child] Wrote %zd bytes to pipe\n", written);
        close(pipefd[1]);
        exit(0);
    } else {
        /* Parent: read from pipe */
        close(pipefd[1]);           /* Close unused write end */

        char buf[256] = {0};
        ssize_t n = read(pipefd[0], buf, sizeof(buf) - 1);
        printf("[Parent] Read from pipe: '%s' (%zd bytes)\n", buf, n);
        close(pipefd[0]);

        wait(NULL);
    }
}

/* ── Demo 4: Shell-like: pipe ls | grep ─────────────────────────────── */
void demo_pipeline(void) {
    printf("\n=== Demo 4: Pipeline (ls | grep .c) ===\n");

    int pipefd[2];
    pipe(pipefd);

    pid_t pid1 = fork();
    if (pid1 == 0) {
        /* First child: ls -la */
        dup2(pipefd[1], STDOUT_FILENO);  /* redirect stdout to pipe write end */
        close(pipefd[0]);
        close(pipefd[1]);
        execlp("ls", "ls", "-la", ".", NULL);
        perror("ls"); exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        /* Second child: grep */
        dup2(pipefd[0], STDIN_FILENO);   /* redirect stdin to pipe read end */
        close(pipefd[1]);
        close(pipefd[0]);
        execlp("grep", "grep", "\\.c", NULL);
        perror("grep"); exit(1);
    }

    /* Parent: close both pipe ends, wait for both children */
    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

/* ── Demo 5: vfork() - copy-on-write optimization ─────────────────── */
void demo_vfork(void) {
    printf("\n=== Demo 5: vfork + execvp ===\n");
    /*
     * vfork() does NOT copy parent's address space.
     * Child runs in parent's memory until exec() or _exit().
     * Use only if immediately exec'ing - more efficient than fork().
     */
    pid_t pid = vfork();
    if (pid == 0) {
        /* Child shares parent's memory - MUST exec or _exit immediately! */
        char *argv[] = { "echo", "vfork child running!", NULL };
        execvp("echo", argv);
        _exit(1);  /* _exit, not exit (to not flush parent's stdio buffers!) */
    } else {
        int status;
        waitpid(pid, &status, 0);
        printf("[Parent] vfork child done, status=%d\n", WEXITSTATUS(status));
    }
}

/* ── Demo 6: Zombie and orphan processes ─────────────────────────────── */
void demo_zombie_and_orphan(void) {
    printf("\n=== Demo 6: Process States ===\n");

    /* Create a zombie: child exits but parent doesn't wait */
    pid_t zombie_child = fork();
    if (zombie_child == 0) {
        printf("[Child %d] Exiting - becoming zombie until parent waits\n", getpid());
        exit(0);
    }

    /* Sleep to let child become zombie */
    sleep(1);
    printf("[Parent] Zombie child %d exists. Check: ps -o pid,ppid,stat\n", zombie_child);

    /* Check zombie with ps */
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "ps -o pid,ppid,stat -p %d", zombie_child);
    system(cmd);

    /* Reap the zombie */
    int status;
    waitpid(zombie_child, &status, 0);
    printf("[Parent] Reaped zombie child.\n");

    /* Create an orphan: parent exits before child */
    pid_t orphan_child = fork();
    if (orphan_child == 0) {
        sleep(2);  /* Parent will exit while child is sleeping */
        /* By now parent is gone; init/systemd (PID 1) is new parent */
        printf("[Orphan %d] My new parent is PID %d (was not original parent)\n",
               getpid(), getppid());
        exit(0);
    } else {
        printf("[Parent] Exiting, leaving orphan %d\n", orphan_child);
        /* Don't wait - child becomes orphan, adopted by init */
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    printf("Fork/Exec Demo - PID: %d\n", getpid());

    if (argc > 1) {
        int demo = atoi(argv[1]);
        switch(demo) {
            case 1: demo_basic_fork(); break;
            case 2: demo_fork_exec(); break;
            case 3: demo_pipe(); break;
            case 4: demo_pipeline(); break;
            case 5: demo_vfork(); break;
            case 6: demo_zombie_and_orphan(); break;
            default:
                fprintf(stderr, "Usage: %s [1-6]\n", argv[0]);
                return 1;
        }
    } else {
        /* Run all demos */
        demo_basic_fork();
        demo_fork_exec();
        demo_pipe();
        demo_pipeline();
        demo_vfork();
        /* demo_zombie_and_orphan exits parent, so run it last */
        /* demo_zombie_and_orphan(); */
    }

    return 0;
}
