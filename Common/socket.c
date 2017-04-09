#include "socket.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <poll.h>
#include "log.h"

static void check(long ret) {
	guard(ret != -1, strerror(errno));
}

static struct addrinfo *create_addrinfo(const char *ip, const char *port) {
	if(port != NULL) {
		int n = atoi(port);
		guard(n >= 1024 && n <= 65535,
				"port number must be between 1024 and 65535");
	}

	struct addrinfo hints, *addr;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if(ip == NULL) hints.ai_flags = AI_PASSIVE;

	int status = getaddrinfo(ip, port, &hints, &addr);
	guard(status == 0, gai_strerror(status));

	return addr;
}

static socket_t create_socket(struct addrinfo *addr) {
	socket_t sockfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

	if(sockfd != -1) {
		int reuse = 1;
		setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof reuse);
	}

	return sockfd;
}

static socket_t first_valid_socket(const char *ip, const char *port) {
	struct addrinfo *cur, *addr = create_addrinfo(ip, port);
	socket_t sockfd = -1;
	int ret = -1;

	for(cur = addr; cur != NULL; cur = cur->ai_next) {
		sockfd = create_socket(cur);
		if(sockfd == -1) continue;
		log_inform("Socket %d created", sockfd);

		if(ip == NULL) {
			ret = bind(sockfd, cur->ai_addr, cur->ai_addrlen);
		} else {
			ret = connect(sockfd, cur->ai_addr, cur->ai_addrlen);
		}

		if(ret == -1) {
			socket_close(sockfd);
			continue;
		}

		if(ip == NULL) {
			log_inform("Binded to port %s", port);
		} else {
			log_inform("Connected to %s:%s", ip, port);
		}
		break;
	}

	freeaddrinfo(addr);
	check(sockfd);
	check(ret);

	if(ip == NULL) {
		check(listen(sockfd, SOCKET_BACKLOG));
		log_inform("Listening on port %s", port);
	}

	return sockfd;
}

static socket_t accept_connection(socket_t sv_sock) {
	struct sockaddr rem_addr;
	socklen_t addr_size = sizeof rem_addr;

	socket_t cli_sock = accept(sv_sock, &rem_addr, &addr_size);
	check(cli_sock);

	struct sockaddr_in *addr_in = (struct sockaddr_in*) &rem_addr;
	char remote_ip[INET_ADDRSTRLEN];

	inet_ntop(AF_INET, &addr_in->sin_addr, remote_ip, INET_ADDRSTRLEN);
	log_inform("Client %s connected on socket %d", remote_ip, cli_sock);

	return cli_sock;
}

socket_t socket_listen(const char *port) {
	socket_t sv_sock = first_valid_socket(NULL, port);
	socket_t cli_sock = accept_connection(sv_sock);

	socket_close(sv_sock);
	return cli_sock;
}

socket_t socket_connect(const char *ip, const char *port) {
	return first_valid_socket(ip, port);
}

static size_t sendall(socket_t sockfd, const char *buf, size_t len) {
	size_t bytes_sent = 0;

	while(bytes_sent < len) {
		ssize_t n = write(sockfd, buf + bytes_sent, len - bytes_sent);
		check(n);
		bytes_sent += n;
	}

	return bytes_sent;
}

size_t socket_send(const char *message, socket_t sockfd) {
	size_t bytes_sent = sendall(sockfd, message, strlen(message) + 1);
	log_inform("Sent %ld bytes: \"%s\"", bytes_sent, message);

	return bytes_sent;
}

size_t socket_receive(char *message, socket_t sockfd) {
	ssize_t bytes_received = read(sockfd, message, SOCKET_BUFFER_CAPACITY);
	check(bytes_received);

	if(bytes_received == 0) {
		log_inform("Connection dropped on socket %d", sockfd);
	} else {
		log_inform("Received %ld bytes: \"%s\"", bytes_received, message);
	}

	return bytes_received;
}

fdset_t socket_set_create(void) {
	fdset_t fds;
	fds.max = -1;
	FD_ZERO(&fds.set);
	return fds;
}

void socket_set_add(socket_t fd, fdset_t *fds) {
	FD_SET(fd, &fds->set);
	if(fd > fds->max) {
		fds->max = fd;
	}
}

void socket_select(const char *port, const fdset_t *sockfds) {
	char buffer[SOCKET_BUFFER_CAPACITY];

	fdset_t all_fds = sockfds != NULL ? *sockfds : socket_set_create();
	fdset_t read_fds = socket_set_create();

	socket_t sv_sock = first_valid_socket(NULL, port);
	socket_set_add(sv_sock, &all_fds);

	while(true) {
		read_fds = all_fds;
		check(select(read_fds.max + 1, &read_fds.set, NULL, NULL, NULL));

		for(socket_t i = 0; i <= all_fds.max; i++) {
			if(!FD_ISSET(i, &read_fds.set)) continue;
			if(i == sv_sock) {
				socket_t cli_sock = accept_connection(sv_sock);
				socket_set_add(cli_sock, &all_fds);
			} else {
				if(socket_receive(buffer, i) == 0) {
					socket_close(i);
					FD_CLR(i, &all_fds.set);
					continue;
				}
				printf("Message received: \"%s\"\n", buffer);
				for(socket_t j = 0; j <= all_fds.max; j++) {
					if(FD_ISSET(j, &all_fds.set) && j != sv_sock && j != i) {
						socket_send(buffer, j);
					}
				}
			}
		}
	}
}

void socket_close(socket_t sockfd) {
	check(close(sockfd));
	log_inform("Socket %d closed", sockfd);
}
