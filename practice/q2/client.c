#include <fcntl.h>
#include <sys/socket.h>

int main()
{
    int sfd = socket(AF_INET, SOCK_STREAM, 0);

    int flags = fcntl(sfd, F_GETFL, 0);

    flags &= ~O_NONBLOCK;

    fcntl(sfd, F_SETFL, flags);
}