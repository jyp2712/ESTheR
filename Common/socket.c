#include "socket.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <poll.h>
#include "log.h"
#include "serial.h"
#include "protocol.h"

#define SOCKET_BACKLOG 5

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

socket_t socket_init(const char *ip, const char *port) {
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
	fdcheck(sockfd);
	fdcheck(ret);

	if(ip == NULL) {
		fdcheck(listen(sockfd, SOCKET_BACKLOG));
		log_inform("Listening on port %s", port);
	}

	return sockfd;
}

socket_t socket_accept(socket_t sv_sock) {
	struct sockaddr rem_addr;
	socklen_t addr_size = sizeof rem_addr;

	socket_t cli_sock = accept(sv_sock, &rem_addr, &addr_size);

	if(cli_sock != -1) {
		struct sockaddr_in *addr_in = (struct sockaddr_in*) &rem_addr;
		char remote_ip[INET_ADDRSTRLEN];

		inet_ntop(AF_INET, &addr_in->sin_addr, remote_ip, INET_ADDRSTRLEN);
		log_inform("Client %s connected on socket %d", remote_ip, cli_sock);
	}

	return cli_sock;
}

socket_t socket_listen(const char *port) {
	socket_t sv_sock = socket_init(NULL, port);
	socket_t cli_sock = socket_accept(sv_sock);

	socket_close(sv_sock);
	return cli_sock;
}

socket_t socket_connect(const char *ip, const char *port) {
	return socket_init(ip, port);
}

static size_t sendall(socket_t sockfd, const unsigned char *buf, size_t len) {
	size_t bytes_sent = 0;

	while(bytes_sent < len) {
		ssize_t n = send(sockfd, buf + bytes_sent, len - bytes_sent, 0);
		fdcheck(n);
		bytes_sent += n;
	}

	return bytes_sent;
}

size_t socket_send_string(const char *message, socket_t sockfd) {
	size_t bytes_sent = sendall(sockfd, (const unsigned char *) message, strlen(message) + 1);
	//log_inform("Sent string: \"%s\"", message);

	return bytes_sent;
}

size_t socket_send_bytes(const unsigned char *message, size_t size, socket_t sockfd) {
	size_t bytes_sent = sendall(sockfd, message, size);
	if(bytes_sent > 0) {
		//log_inform("Sent %ld bytes", bytes_sent);
	}

	return bytes_sent;
}

static ssize_t recvall(socket_t sockfd, unsigned char *buf, size_t len) {
	size_t bytes_received = 0;

	while(bytes_received < len) {
		ssize_t n = recv(sockfd, buf + bytes_received, len - bytes_received, 0);
		if(n == -1) return n;
		if(n == 0) {
			log_report("Connection dropped on socket %d", sockfd);
			return 0;
		}
		bytes_received += n;
		if(len == BUFFER_CAPACITY && buf[bytes_received - 1] == '\0') {
			break;
		}
	}

	return bytes_received;
}

ssize_t socket_receive_string(char *message, socket_t sockfd) {
	ssize_t bytes_received = recvall(sockfd, (unsigned char *) message, BUFFER_CAPACITY);
	if(bytes_received > 0) {
		//log_inform("Received string: \"%s\"", message);
	}

	return bytes_received;
}

ssize_t socket_receive_bytes(unsigned char *message, size_t size, socket_t sockfd) {
	ssize_t bytes_received = recvall(sockfd, message, size);
	if(bytes_received > 0) {
		//log_inform("Received %ld bytes", bytes_received);
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

void socket_close(socket_t sockfd) {
	shutdown(sockfd, SHUT_RDWR);
	int res = close(sockfd);
	if(res != 1) {
		log_inform("Socket %d closed", sockfd);
	}
}
