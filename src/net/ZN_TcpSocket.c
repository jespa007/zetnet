#include "zetnet.h"

SOCKET ZN_TcpSocket_NewSocketServer(const char *_host, int _portno)
{
	SOCKET socket_server = INVALID_SOCKET;
	struct addrinfo hints;
	struct addrinfo *result = NULL;
	struct addrinfo *ptr = NULL;
	char port_str[16];

	struct sockaddr_storage bound_addr;
	socklen_t bound_addr_len = 0;
	bool have_bound_addr = false;

	int i_result;
	int error = 0;

	memset(&hints, 0, sizeof(hints));
	memset(&bound_addr, 0, sizeof(bound_addr));

	snprintf(port_str, sizeof(port_str), "%d", _portno);

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	i_result = getaddrinfo(_host, port_str, &hints, &result);
	if (i_result != 0) {
		fprintf(stderr, "\ngetaddrinfo failed with error: %d\n", i_result);
		return INVALID_SOCKET;
	}

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		socket_server = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (socket_server == INVALID_SOCKET) {
#ifdef _WIN32
			error = WSAGetLastError();
#else
			error = errno;
#endif
			fprintf(stderr, "\nsocket failed with error: %i\n", error);
			continue;
		}

		{
			int opt = 1;
			if (setsockopt(socket_server, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) < 0) {
#ifdef _WIN32
				error = WSAGetLastError();
#else
				error = errno;
#endif
				fprintf(stderr, "\nsetsockopt(SO_REUSEADDR) failed with error: %i\n", error);
				/* Not fatal. Continue trying to bind. */
			}
		}

		if (bind(socket_server, ptr->ai_addr, (int)ptr->ai_addrlen) < 0) {
#ifdef _WIN32
			error = WSAGetLastError();
#else
			error = errno;
#endif

#ifdef _WIN32
			if (error == WSAEADDRINUSE) {
				fprintf(stderr, "\nport %i already in use\n", _portno);
			} else
#endif
			{
				fprintf(stderr, "\nbind failed with error: %i\n", error);
			}

			ZN_TcpSocket_CloseSocket(&socket_server);
			continue;
		}

		if (ptr->ai_addrlen <= sizeof(bound_addr)) {
			memcpy(&bound_addr, ptr->ai_addr, ptr->ai_addrlen);
			bound_addr_len = (socklen_t)ptr->ai_addrlen;
			have_bound_addr = true;
		}

		break;
	}

	freeaddrinfo(result);

	if (socket_server == INVALID_SOCKET) {
		return INVALID_SOCKET;
	}

	i_result = listen(socket_server, ZN_TCP_SERVER_MAX_CLIENTS);
	if (i_result == SOCKET_ERROR) {
#ifdef _WIN32
		error = WSAGetLastError();
#else
		error = errno;
#endif
		fprintf(stderr, "\nlisten failed with error: %i\n", error);
		ZN_TcpSocket_CloseSocket(&socket_server);
		return INVALID_SOCKET;
	}

	if (have_bound_addr && bound_addr.ss_family == AF_INET) {
		struct sockaddr_in *addr = (struct sockaddr_in *)&bound_addr;

		printf("Setup server  (%s:%i)\n",
			   inet_ntoa(addr->sin_addr),
			   ntohs(addr->sin_port));
	} else {
		printf("Setup server  (%s:%i)\n",
			   _host ? _host : "0.0.0.0",
			   _portno);
	}

	return socket_server;
}

SOCKET ZN_TcpSocket_NewSocketClient(const char *_host, int _portno)
{
	SOCKET socket_client = INVALID_SOCKET;
	struct addrinfo hints;
	struct addrinfo *ptr = NULL;
	struct addrinfo *result = NULL;
	char port_str[16];

	int i_result;
	int error = 0;

	if (!_host) {
		fprintf(stderr, "\nZN_TcpSocket_NewSocketClient : host is NULL\n");
		return INVALID_SOCKET;
	}

	memset(&hints, 0, sizeof(hints));
	snprintf(port_str, sizeof(port_str), "%d", _portno);

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	i_result = getaddrinfo(_host, port_str, &hints, &result);
	if (i_result != 0) {
		fprintf(stderr,
				"\nZN_TcpSocket_NewSocketClient : getaddrinfo for '%s:%i' failed with error: %d\n",
				_host,
				_portno,
				i_result);
		return INVALID_SOCKET;
	}

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		socket_client = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (socket_client == INVALID_SOCKET) {
#ifdef _WIN32
			error = WSAGetLastError();
#else
			error = errno;
#endif
			fprintf(stderr, "\nsocket failed with error: %i\n", error);
			continue;
		}

#if defined(__DEBUG__)
		printf("Connecting to family: %d\n", ptr->ai_family);
#endif

		i_result = connect(socket_client, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (i_result == SOCKET_ERROR) {
#ifdef _WIN32
			error = WSAGetLastError();
#else
			error = errno;
#endif
			fprintf(stderr, "\nconnect failed with error: %i\n", error);

			ZN_TcpSocket_CloseSocket(&socket_client);
			socket_client = INVALID_SOCKET;
			continue;
		}

		break;
	}

	freeaddrinfo(result);

	if (socket_client == INVALID_SOCKET) {
		fprintf(stderr, "\nUnable to connect to server!\n");
	}

	return socket_client;
}

ssize_t ZN_TcpSocket_ReceiveBytes(SOCKET sock, uint8_t *_buf, size_t _buf_len)
{
	ssize_t result;

	if (sock == INVALID_SOCKET || !_buf || _buf_len == 0) {
		return ZN_ERROR;
	}

	result = recv(sock, (char *)_buf, (int)_buf_len, 0);

	return result;
}

ssize_t ZN_TcpSocket_SendBytes(SOCKET _socket, const uint8_t *_buffer, size_t _buffer_len)
{
	size_t total_sent = 0;

	if (_socket == INVALID_SOCKET || !_buffer || _buffer_len == 0) {
		return ZN_ERROR;
	}

	while (total_sent < _buffer_len) {
		size_t remaining = _buffer_len - total_sent;
		int chunk_len = remaining > 0x7fffffffU ? 0x7fffffff : (int)remaining;

		ssize_t sent = send(_socket, (const char *)_buffer + total_sent, chunk_len, 0);

		if (sent < 0) {
#ifdef _WIN32
			int err = WSAGetLastError();
			if (err == WSAEINTR) {
				continue;
			}
#else
			if (errno == EINTR) {
				continue;
			}
#endif
			return ZN_ERROR;
		}

		if (sent == 0) {
			return ZN_ERROR;
		}

		total_sent += (size_t)sent;
	}

	return (ssize_t)total_sent;
}

void ZN_TcpSocket_CloseChannel(SOCKET _socket, int _channel)
{
	if (_socket == INVALID_SOCKET) {
		return;
	}

	shutdown(_socket, _channel);
}

void ZN_TcpSocket_CloseSocket(SOCKET *_socket)
{
	if (!_socket || *_socket == INVALID_SOCKET) {
		return;
	}

#ifdef _WIN32
	closesocket(*_socket);
#else
	close(*_socket);
#endif

	*_socket = INVALID_SOCKET;
}
