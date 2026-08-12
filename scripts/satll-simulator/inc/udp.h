#ifndef _UDP_H
#define _UDP_H
#include <stdint.h>
#include <stddef.h>
#include <netinet/in.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TX_PORT 10011
#define RX_PORT 10022

typedef struct
{
    int socket_fd;
    struct sockaddr_in addr;
    socklen_t addr_len;
} UDP_Socket;
extern UDP_Socket udp;

/*
 * Creates a UDP socket.
 *
 * local_ip:
 *      NULL      -> bind to all interfaces
 *      "0.0.0.0" -> bind to all interfaces
 *      otherwise -> bind to specified interface
 *
 * remote_ip:
 *      NULL if only receiving.
 *      Otherwise destination IP for send_udp().
 */
int init_udp(UDP_Socket *udp,
             const char *local_ip,
             uint16_t local_port,
             const char *remote_ip,
             uint16_t remote_port);

int send_udp(UDP_Socket *udp,
             const void *data,
             size_t length);

int receive_udp(UDP_Socket *udp,
                void *buffer,
                size_t buffer_size);

int set_udp_nonblocking(UDP_Socket *udp);



int receive_udp_nonblocking(UDP_Socket *udp,
                            void *buffer,
                            size_t buffer_size);




void close_udp(UDP_Socket *udp);


#ifdef __cplusplus
}
#endif


#endif