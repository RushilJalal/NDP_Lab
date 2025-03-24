#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>

void create_daemon()
{
    pid_t pid;

    // Step 1: Fork the parent process
    pid = fork();
    if (pid < 0)
    {
        exit(EXIT_FAILURE); // Fork failed
    }
    if (pid > 0)
    {
        exit(EXIT_SUCCESS); // Exit parent process
    }

    // Step 2: Create a new session
    if (setsid() < 0)
    {
        exit(EXIT_FAILURE);
    }

    // Step 3: Change working directory to root
    if (chdir("/") < 0)
    {
        exit(EXIT_FAILURE);
    }

    // Step 4: Close standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Step 5: Reset file creation mask
    umask(0);

    // Open syslog for logging
    openlog("DaemonExample", LOG_PID, LOG_DAEMON);

    while (1)
    {
        syslog(LOG_NOTICE, "Daemon is running...");
        sleep(10); // Perform periodic tasks here
    }

    closelog();
}

int main()
{
    create_daemon();
    return 0;
}
