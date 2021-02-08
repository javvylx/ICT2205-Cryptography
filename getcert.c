/*  ----------------------------------------------------------------------- *
 *   NOTE:     		Using wolfssl 4.6.0						 				*
 *					./configure  --enable-opensslextra --enable-certgen		*
 *   To compile:	gcc -o test getcert.c -lwolfssl -lcrypto				*
 *  ----------------------------------------------------------------------- */

/**** IMPORTS ****/

#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <errno.h>

#include <wolfssl/options.h>
#include <wolfssl/ssl.h>
#include <wolfssl/openssl/pem.h>
#include <wolfssl/openssl/bio.h>
#include <wolfssl/wolfcrypt/rsa.h>


// To create a socket & TCP-connect to server
int create_socket(char[], BIO *);

int main() {

	// Host name to retrieve cert from
	char           		dest_url[] = "https://demo.testfire.net";
	char           		filename[100];
	char           		hostname[256];
	BIO              	*certbio = NULL;
	BIO               	*outbio = NULL;
	WOLFSSL_X509        *servercert = NULL;
	WOLFSSL_METHOD* 	method;
	WOLFSSL_CTX* ctx;
	WOLFSSL* ssl;
	int server = 0;
	int ret, i;
	FILE *fp;
	struct stat st = {0};

	/* ---------------------------------------------------------- *
	 * Create the Input/Output BIO's.                             *
	 * ---------------------------------------------------------- */
	certbio = BIO_new(BIO_s_file());
	outbio  = BIO_new_fp(stdout, BIO_NOCLOSE);

	// initialize wolfssl library
	if(wolfSSL_Init() < 0){
		BIO_printf(outbio, "Could not initialize the OpenSSL library !\n");
	}

	/* ---------------------------------------------------------- *
	 * Set SSL v2, announce SSL v3 and TLS v1					  *
	 * ---------------------------------------------------------- */
	method = wolfSSLv23_client_method();

	/* ---------------------------------------------------------- *
	 * Try to create a new SSL context                            *
	 * ---------------------------------------------------------- */
	if ( (ctx = wolfSSL_CTX_new(method)) == NULL) {
	    BIO_printf(outbio, "Unable to create a new SSL context structure.\n");
	}

	/* ---------------------------------------------------------- *
	* Set CTX options											  *
	* ----------------------------------------------------------- */
	// Disable SSL v2
	wolfSSL_CTX_set_options(ctx, WOLFSSL_OP_NO_SSLv2);
	// Disable verification of server
	wolfSSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, 0); 

	/* ---------------------------------------------------------- *
	 * Create new wolfSSL connection state object                 *
	 * ---------------------------------------------------------- */
	if ( (ssl = wolfSSL_new(ctx)) == NULL) {
	    BIO_printf(outbio, "wolfSSL_new has an error.\n");
	}

	/* ---------------------------------------------------------- *
	 * Make the underlying TCP socket connection                  *
	 * ---------------------------------------------------------- */
	server = create_socket(dest_url, outbio);
	if(server != 0){
		BIO_printf(outbio, "Successfully made the TCP connection to: %s.\n", dest_url);
	}

	/* ---------------------------------------------------------- *
	 * Attach the SSL session to the socket descriptor            *
	 * ---------------------------------------------------------- */
	wolfSSL_set_fd(ssl, server);

	/* ---------------------------------------------------------- *
	 * Try to SSL-connect here, returns 1 for success             *
	 * ---------------------------------------------------------- */
	if ( wolfSSL_connect(ssl) != 1) {
		BIO_printf(outbio, "Error: Could not build a SSL session to: %s.\n", dest_url);
	}
	else {
		BIO_printf(outbio, "Successfully enabled SSL/TLS session to: %s.\n", dest_url);
	}

	/* ---------------------------------------------------------- *
	 * Get the remote certificate into the X509 structure         *
	 * ---------------------------------------------------------- */
	servercert = wolfSSL_get_peer_certificate(ssl);

	if(servercert == NULL) {
		BIO_printf(outbio, "Error: Could not get a certificate from: %s.\n", dest_url);
	}
	else {
		BIO_printf(outbio, "Retrieved the server's certificate from: %s.\n", dest_url);
	}

	/* ---------------------------------------------------------- *
	 * Get certificate values									  *
	 * ---------------------------------------------------------- */

	// Get public key
	EVP_PKEY *pkey = X509_get_pubkey(servercert);

	// IF PK is RSA
	RSA * rsa;
  	rsa = EVP_PKEY_get1_RSA(pkey);
  	// Print RSA
  	RSA_print(outbio, rsa, 0);

  	// Write in PEM format to BIO
	PEM_write_bio_PUBKEY(outbio, pkey);

	// Create directory path
	if (stat("./cert", &st) == -1) {
		// 0777 is permissions of directory
		mkdir("./cert", 0777);
	}

    // Store certificate into PEM file
    strncpy(hostname, strstr(dest_url, "://")+3, sizeof(hostname));
    sprintf(filename, "cert/%s.PEM", hostname);
    fp = fopen (filename, "wb");
	PEM_write_X509(fp, servercert);

	/************ GET M0RE STUFF NEEDED FOR VERIFICATION ************/

	/* ---------------------------------------------------------- *
	 * Free structures			       							  *
	 * ---------------------------------------------------------- */
	wolfSSL_free(ssl);
	close(server);
	wolfSSL_X509_free(servercert);
	wolfSSL_CTX_free(ctx);
	wolfSSL_Cleanup();

	BIO_printf(outbio, "Finished SSL/TLS connection with server: %s.\n", dest_url);
	BIO_printf(outbio, "-------------------------------------------------------------------\n");
	return(0);
}


/* ---------------------------------------------------------- *
 * Creates the socket & TCP-connect to server 				  *
 * ---------------------------------------------------------- */
int create_socket(char url_str[], BIO *out) {
  int sockfd;
  char hostname[256] = "";
  char    portnum[6] = "443";
  char      proto[6] = "";
  char      *tmp_ptr = NULL;
  int           port;
  struct hostent *host;
  struct sockaddr_in dest_addr;

  // Remove / from url_string
  if(url_str[strlen(url_str)] == '/')
    url_str[strlen(url_str)] = '\0';

  // Get protocol (i.e. http)
  strncpy(proto, url_str, (strchr(url_str, ':')-url_str));

  // Get hostname after ://
  strncpy(hostname, strstr(url_str, "://")+3, sizeof(hostname));

  // If hostname has colon
  if(strchr(hostname, ':')) {
    tmp_ptr = strchr(hostname, ':');
    // Last : is the start of port number
    strncpy(portnum, tmp_ptr+1,  sizeof(portnum));
    *tmp_ptr = '\0';
  }

  port = atoi(portnum);

  // Perform lookup on hostname
  if ( (host = gethostbyname(hostname)) == NULL ) {
    BIO_printf(out, "Error: Cannot resolve hostname %s.\n",  hostname);
    abort();
  }

  /* ---------------------------------------------------------- *
   * Create TCP socket 			                                *
   * ---------------------------------------------------------- */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  dest_addr.sin_family=AF_INET;
  dest_addr.sin_port=htons(port);
  dest_addr.sin_addr.s_addr = *(long*)(host->h_addr);

  // Zero the rest of the struct
  memset(&(dest_addr.sin_zero), '\0', 8);

  tmp_ptr = inet_ntoa(dest_addr.sin_addr);

  // Connect to host
  if ( connect(sockfd, (struct sockaddr *) &dest_addr,
                              sizeof(struct sockaddr)) == -1 ) {
    BIO_printf(out, "Error: Cannot connect to host %s [%s] on port %d.\n",
             hostname, tmp_ptr, port);
  }

  return sockfd;
}


