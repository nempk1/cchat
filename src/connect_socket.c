#include "connect_socket.h"

static struct timeval defaut_timeout = {.tv_sec = 0, .tv_usec = 250000};

int T_SSL_init(struct T_SSL **tssl)
{
	if(((*tssl) = malloc(sizeof(struct T_SSL))) == NULL) {
		errno = ENOMEM;
		return -1;
	}
	
	if(pthread_mutex_init(&(*tssl)->mutex, NULL))
		return -1;
	if(pthread_cond_init(&(*tssl)->cond, NULL))
		return -1;

	return 0;
}

/* Return connect socket fd */
int connect_socket(char *ip, char *port)
{
	/* Specify type of socket */
	struct addrinfo hints;

	/* zefo fill the structure */	
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM; /* TCP */
	
	// Struct to where to store de info retrevied
	struct addrinfo *peer_address;

	if (getaddrinfo(ip, port, &hints, &peer_address)) {
		fprintf(stderr, "getaddrinfo() failed.\n");
		return -1;
	}
	
	// Creating socket
	int socket_fd;
	socket_fd = socket(peer_address->ai_family, peer_address->ai_socktype,
			peer_address->ai_protocol);
	
	if (socket_fd == -1) {
		fprintf(stderr, "socket() failed.\n");
		return -1;
	}
	
	// Connecting socket
	if (connect(socket_fd, peer_address->ai_addr,
			peer_address->ai_addrlen)) {
		fprintf(stderr, "connect() failed.\n");
		return -1;
	}

	freeaddrinfo(peer_address);

	return socket_fd;
}

struct T_SSL*
T_SSL_connect(char *ip, char *port)
{
	struct T_SSL *tssl = NULL;
	if(T_SSL_init(&tssl))
		return NULL;
	
	// Specifie the type of service
	struct addrinfo hints;

	// Zero fill the structure
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;// TCP
	
	// Struct to where to store de info retrevied
	struct addrinfo *peer_address;

	if (getaddrinfo(ip, port, &hints, &peer_address)) {
		fprintf(stderr, "getaddrinfo() failed.\n");
		return NULL;
	}
	
	// Creating socket
	int socket_fd;
	socket_fd = socket(peer_address->ai_family, peer_address->ai_socktype,
			peer_address->ai_protocol);
	
	if (socket_fd == -1) {
		fprintf(stderr, "socket() failed.\n");
		return NULL; 
	}

	// Set timeout for socket
	if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &defaut_timeout, sizeof(defaut_timeout))) {
		fprintf(stderr, "setsockopt() failed\n");
		return NULL;
	}

	// Connecting socket
	if (connect(socket_fd, peer_address->ai_addr,
			peer_address->ai_addrlen)) {
		fprintf(stderr, "connect() failed.\n");
		return NULL;
	}

	freeaddrinfo(peer_address);
	
	SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
	if(!ctx) {
		fprintf(stderr, "SSL_CTX_new() failed.\n");
		return NULL;	
	}

	tssl->ssl = SSL_new(ctx);
	if(!tssl) {
		fprintf(stderr, "SSL_new() failed.\n");
		return NULL;
	}
	
	SSL_set_fd(tssl->ssl, socket_fd);
	if(SSL_connect(tssl->ssl) == -1) {
		fprintf(stderr, "SSL_connect() failed.\n");
		return NULL;
	}

	return tssl;
}

int T_SSL_write(struct T_SSL *fd, const void *buf, int len)
{
	int result = 0;
	fd->want_to_write = true;

	pthread_mutex_lock(&fd->mutex);
	
	result = SSL_write(fd->ssl, buf, len);

	if(fd->want_to_write) {
		fd->want_to_write = false;
		pthread_cond_signal(&fd->cond);
	}

	pthread_mutex_unlock(&fd->mutex);
	return result;
}

int T_SSL_read(struct T_SSL *fd, void *buf, int len)
{
	int result = 0;
        pthread_mutex_lock(&fd->mutex);

	while(fd->want_to_write) {
		pthread_cond_wait(&fd->cond, &fd->mutex);
	}	

	fd->want_to_write = false;

	result = SSL_read(fd->ssl, buf, len);
	pthread_mutex_unlock(&fd->mutex);
	return result;
}
ssize_t CUSTOM_send(int fd, const void *buf, size_t len, int flags)
{
	static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_lock(&mutex);
	ssize_t result = send(fd, buf, len, flags);
	pthread_mutex_unlock(&mutex);

	return result;
}	
