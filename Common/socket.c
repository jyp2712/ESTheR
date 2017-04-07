#include "socket.h"

#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <poll.h>

#define printop(...) if(SOCKET_PRINT_OPERATIONS) {printf(__VA_ARGS__); fflush(stdout);}

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

static int create_socket(struct addrinfo *addr) {
	int sockfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

	if(sockfd != -1) {
		int reuse = 1;
		setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof reuse);
	}

	return sockfd;
}

static int first_valid_socket(const char *ip, const char *port) {
	struct addrinfo *cur, *addr = create_addrinfo(ip, port);
	int sockfd = -1, ret = -1;

	for(cur = addr; cur != NULL; cur = cur->ai_next) {
		printop("Creating socket… ");
		sockfd = create_socket(cur);
		if(sockfd == -1) continue;
		printop("done. (fd = %d)\n", sockfd);

		if(ip == NULL) {
			printop("Binding to port %s… ", port);
			ret = bind(sockfd, cur->ai_addr, cur->ai_addrlen);
		} else {
			printop("Connecting to %s:%s… ", ip, port);
			ret = connect(sockfd, cur->ai_addr, cur->ai_addrlen);
		}

		if(ret != -1) {
			printop("done.\n");
			break;
		}

		socket_close(sockfd);
	}

	freeaddrinfo(addr);
	check(sockfd);
	check(ret);

	if(ip == NULL) {
		printop("Getting ready to listen… ");
		check(listen(sockfd, SOCKET_BACKLOG));
		printop("done.\n");
	}

	return sockfd;
}

static int accept_connection(int sv_sock) {
	struct sockaddr rem_addr;
	socklen_t addr_size = sizeof rem_addr;

	printop("Preparing to accept connections… ");
	int cli_sock = accept(sv_sock, &rem_addr, &addr_size);
	check(cli_sock);

	struct in_addr addr = ((struct sockaddr_in*) &rem_addr)->sin_addr;
	printop("done. (%s connected on socket %d)\n", inet_ntoa(addr), cli_sock);

	return cli_sock;
}

int socket_listen(const char *port) {
	int sv_sock = first_valid_socket(NULL, port);
	int cli_sock = accept_connection(sv_sock);

	socket_close(sv_sock);
	return cli_sock;
}

int socket_connect(const char *ip, const char *port) {
	return first_valid_socket(ip, port);
}

ssize_t socket_send(const char *message, int sockfd) {
	printop("Sending message… ");
	ssize_t bytes_sent = write(sockfd, message, strlen(message) + 1);
	check(bytes_sent);
	printop("done. (%zu bytes sent)\n", bytes_sent);

	return bytes_sent;
}

ssize_t socket_receive(char *message, int sockfd) {
	printop("Receiving message… ");
	ssize_t bytes_received = read(sockfd, message, SOCKET_BUFFER_CAPACITY);
	check(bytes_received);

	if(bytes_received == 0) {
		printop("connection dropped on socket %d.\n", sockfd);
	} else {
		printop("done. (%zu bytes received)\n", bytes_received);
	}

	return bytes_received;
}

void socket_select(const char *port, int portServer1, int portServer2) {
	char buffer[SOCKET_BUFFER_CAPACITY];

	fd_set all_fds, read_fds;
	FD_ZERO(&all_fds);
	FD_ZERO(&read_fds);

	int sv_sock = first_valid_socket(NULL, port);

	FD_SET(sv_sock, &all_fds);
	int fdmax = sv_sock;

	while(true) {
		read_fds = all_fds;
		check(select(fdmax + 1, &read_fds, NULL, NULL, NULL));

		for(int i = 0; i <= fdmax; i++) {
			if(!FD_ISSET(i, &read_fds)) continue;
			if(i == sv_sock) {
				int cli_sock = accept_connection(sv_sock);

				FD_SET(cli_sock, &all_fds);
				if(cli_sock > fdmax) {
					fdmax = cli_sock;
				}
			} else {
				if(socket_receive(buffer, i) == 0) {
					socket_close(i);
					FD_CLR(i, &all_fds);
					continue;
				}
				socket_send (buffer, portServer1);
				socket_send (buffer, portServer2);
				for(int j = 0; j <= fdmax; j++) {
					if(FD_ISSET(j, &all_fds) && j != sv_sock && j != i) {
						socket_send(buffer, j);
					}
				}
			}
		}
	}
}


void socket_poll(const char *port) {
	char buffer[SOCKET_BUFFER_CAPACITY];

	struct pollfd ufds[SOCKET_MAX_POLLED];
	int nfds = 1;

	int sv_sock = first_valid_socket(NULL, port);

	ufds[0].fd = sv_sock;
	ufds[0].events = POLLIN;

	while(true) {
		check(poll(ufds, nfds, -1));

		for(int i = 0; i < nfds; i++) {
			if(!(ufds[i].revents & POLLIN)) continue;
			if(i == 0) {
				ufds[nfds].fd = accept_connection(sv_sock);
				ufds[nfds].events = POLLIN;
				nfds++;
			} else {
				int cli_sock = ufds[i].fd;

				if(socket_receive(buffer, cli_sock) == 0) {
					socket_close(cli_sock);
					continue;
				}

				for(int j = 0; j < nfds; j++) {
					int cur_sock = ufds[j].fd;
					if((ufds[i].revents & POLLIN)
							&& cur_sock != sv_sock && cur_sock != cli_sock) {
						socket_send(buffer, cur_sock);
					}
				}
			}
		}
	}
}

void socket_close(int sockfd) {
	printop("Closing socket %d… ", sockfd);
	check(close(sockfd));
	printop("done.\n");
}
