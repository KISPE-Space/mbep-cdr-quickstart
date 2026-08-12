#include <arpa/inet.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>


#include <fcntl.h>
#include <errno.h>


#include "udp.h"

int init_udp(UDP_Socket *udp,
             const char *local_ip,
             uint16_t local_port,
             const char *remote_ip,
             uint16_t remote_port)
{
    memset(udp, 0, sizeof(*udp));

    udp->socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (udp->socket_fd < 0)
        return -1;

    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));

    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(local_port);

    if (local_ip == NULL)
        local_addr.sin_addr.s_addr = INADDR_ANY;
    else
        inet_pton(AF_INET, local_ip, &local_addr.sin_addr);

    if (bind(udp->socket_fd,
             (struct sockaddr *)&local_addr,
             sizeof(local_addr)) < 0)
    {
        close(udp->socket_fd);
        return -1;
    }

    if (remote_ip != NULL)
    {
        memset(&udp->addr, 0, sizeof(udp->addr));

        udp->addr.sin_family = AF_INET;
        udp->addr.sin_port = htons(remote_port);
        inet_pton(AF_INET, remote_ip, &udp->addr.sin_addr);

        udp->addr_len = sizeof(udp->addr);
    }

    return 0;
}

int send_udp(UDP_Socket *udp,
             const void *data,
             size_t length)
{
    return sendto(udp->socket_fd,
                  data,
                  length,
                  0,
                  (struct sockaddr *)&udp->addr,
                  udp->addr_len);
}

int receive_udp(UDP_Socket *udp,
                void *buffer,
                size_t buffer_size)
{
    struct sockaddr_in sender;
    socklen_t sender_len = sizeof(sender);

    return recvfrom(udp->socket_fd,
                    buffer,
                    buffer_size,
                    0,
                    (struct sockaddr *)&sender,
                    &sender_len);
}



int set_udp_nonblocking(UDP_Socket *udp)
{
    int flags = fcntl(udp->socket_fd, F_GETFL, 0);

    if (flags < 0)
        return -1;

    return fcntl(udp->socket_fd,
                 F_SETFL,
                 flags | O_NONBLOCK);
}


int receive_udp_nonblocking(UDP_Socket *udp,
                            void *buffer,
                            size_t buffer_size)
{
    int bytes = recvfrom(udp->socket_fd,
                         buffer,
                         buffer_size,
                         0,
                         NULL,
                         NULL);

    if (bytes < 0)
    {
        if (errno == EWOULDBLOCK || errno == EAGAIN)
        {
            // No packet available
            return 0;
        }

        return -1;
    }

    return bytes;
}
















void close_udp(UDP_Socket *udp)
{
    close(udp->socket_fd);
}