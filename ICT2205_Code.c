/*  ----------------------------------------------------------------------- *
 *   NOTE:     		Using wolfssl 4.6.0						 				*
 *					./configure  --enable-opensslextra --enable-certgen		*
 *   To compile:	gcc -o test getcert.c -lwolfssl 						*
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
#include <time.h>
#include "ICT2205_Code.h"

int main() {
    int num_option, code;
	char option[10];
	num_option = 10;
	code = 1;

	while (num_option != 0){
		printf(WEBSITE_MENU);
		fgets(option, sizeof(option), stdin);
		sscanf(option,"%d", &num_option);
		fflush(stdin);
		if (num_option==0){
			exit(0);
		}
		if (num_option < 6) {
			code = program_cert(num_option);
		}
	}
	exit(1);
}



/* ---------------------------------------------------------- *
 * ICT2205 TeamEggFriedRice Crypto Program (Socket) 	      *
 * 0 - Function runs successful                               *
 * -1 - Function hit some error                                *
 * ---------------------------------------------------------- */
int program_cert(int num_option){
	// Host name to retrieve cert from
	char website_url[5][256] = {"https://sec-consult.com", "https://www.zara.com", "https://www.tutorialspoint.com", "https://www.w3schools.com", "https://sg.linkedin.com"};
	char           		dest_url[] = "";
	char           		hostname[256];
	BIO              	*certbio = NULL;
	BIO               	*outbio = NULL;
	WOLFSSL_X509        *servercert = NULL;
	WOLFSSL_METHOD* 	method;
	WOLFSSL_CTX* ctx;
	WOLFSSL* ssl;
	int server = 0;
	int ret, i;
	//Getting the URL user choosing
	strncpy(dest_url, website_url[num_option-1], sizeof(website_url[num_option-1]));
	char* menu_cho = strdup(retr_menu(num_option-1));
	// printf("%s\t\t\t\t%s\n", dest_url, menu_cho); //Commented out by KK
	// printf("%s\n",dest_url); //Commented out by KK
	/* ---------------------------------------------------------- *
	 * Create the Input/Output BIO's.                             *
	 * ---------------------------------------------------------- */
	certbio = BIO_new(BIO_s_file());
	outbio  = BIO_new_fp(stdout, BIO_NOCLOSE);

	// initialize wolfssl library
	if(wolfSSL_Init() < 0){
		BIO_printf(outbio, "Could not initialize the OpenSSL library !\n");
        return -1;
	}

	/* ---------------------------------------------------------- *
	 * Set TLS v1.2                         					  *
	 * ---------------------------------------------------------- */
	method = wolfTLSv1_2_client_method();

	/* ---------------------------------------------------------- *
	 * Try to create a new SSL context                            *
	 * ---------------------------------------------------------- */
	if ( (ctx = wolfSSL_CTX_new(method)) == NULL) {
	    BIO_printf(outbio, "Unable to create a new SSL context structure.\n");
        return -1;
	}

	/* ---------------------------------------------------------- *
	* Set CTX options											  *
	* ----------------------------------------------------------- */
	// Disable SSL v2 and 3
	wolfSSL_CTX_set_options(ctx, WOLFSSL_OP_NO_SSLv2);
	wolfSSL_CTX_set_options(ctx, WOLFSSL_OP_NO_SSLv3);

	// To enable certification validation
	wolfSSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, 0);

	/* ---------------------------------------------------------- *
	* Loading in the various CA Certs							  *
	* ----------------------------------------------------------- */
	if (wolfSSL_CTX_load_verify_locations(ctx, CERT_FILE_CA, NULL)
        != SSL_SUCCESS) {
        fprintf(stderr, "ERROR: failed to load %s, please check the file.\n",
                CERT_FILE_CA);
        return -1;
    }

	/* ---------------------------------------------------------- *
	 * Create new wolfSSL connection state object                 *
	 * ---------------------------------------------------------- */
	if ( (ssl = wolfSSL_new(ctx)) == NULL) {
	    BIO_printf(outbio, "wolfSSL_new has an error.\n");
        return -1;
	}

	/* ---------------------------------------------------------- *
	 * Make the underlying TCP socket connection                  *
	 * ---------------------------------------------------------- */
	server = create_socket(dest_url, outbio);
	if(server != 0){
		BIO_printf(outbio, "Successfully made the TCP connection to: %s.\n", dest_url);
	}
    else{
        BIO_printf(outbio, "create_socket has an error.\n");
        return -1;
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
        return -1;
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
        return -1;
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
	if(rsa == NULL){
		BIO_printf(outbio, "Public key is not using RSA. Exiting...");
		return -1;
	}

	
  	wolfSSL_RSA_print(outbio, rsa, 0);

  	// Write in PEM format to BIO
	//PEM_write_bio_PUBKEY(outbio, pkey);
	/* ---------------------------------------------------------- *
	 * Sending request to the server 							  *
	 * ---------------------------------------------------------- */
	int nu_opt = 10, code = 0;
	char temp[10];

	while(TRUE){
		BIO_printf(outbio, menu_cho);
		fgets(temp, sizeof(temp), stdin);
		sscanf(temp,"%d", &nu_opt);
		fflush(stdin);
		if (nu_opt==0){
			goto cleanup;
		}
		if (nu_opt < 10) {
			code = process_request(nu_opt, dest_url, ssl);
		}
	}

	/* ---------------------------------------------------------- *
	 * Free structures			       							  *
	 * ---------------------------------------------------------- */
	cleanup:
		if (free_structures(ssl,server, servercert, ctx) != 0){
			BIO_printf(outbio,"There is an error in releasing the structures");
			return -1;
		}
		BIO_printf(outbio, "Finished SSL/TLS connection with server: %s.\n", dest_url);
		BIO_printf(outbio, "-------------------------------------------------------------------\n");
		return(0);
}

int process_request(int n, char* target, WOLFSSL* ssl){
	struct stat st = {0};
	FILE *fp;
	time_t seconds = time(NULL);
	char* hn = "";
	char fn[1000];
	char reponse[16384];

	printf("---Crafting request---\n");
	char* buff = retr_request(n,target);
	printf("---Sending request---\n");
	int ret = wolfSSL_write(ssl, buff, (int)strlen(buff));


	if (ret != strlen(buff)) {
		fprintf(stderr, "ERROR: failed to write entire message\n");
		fprintf(stderr, "%d bytes of %d bytes were sent. \n%s\n", ret, (int)strlen(buff), buff);
		return -1;
	}

	// Create directory path
	if (stat("./request", &st) == -1) {
		// 0777 is permissions of directory
		mkdir("./request", 0777);
	}

	printf("%s","---Receving request (Writing to file)---\n");
    // Store certificate into PEM file
    hn = strdup(strstr(target, "://")+3);
    sprintf(fn, "request/%s-%ld.html", hn,seconds);
    fp = fopen (fn, "a");
	while(TRUE){
		memset(reponse, 0, sizeof(reponse));
		if (wolfSSL_read(ssl, reponse, sizeof(reponse)-1) == -1) {
			break;
		}
		fputs(reponse, fp);
		if(strstr(reponse,"</html>") != NULL){
			break;
		}
	}
	fclose(fp);
	return 0;
}

char* custom_request(){
	char buff[16834];
	memset(buff,0,sizeof(buff));
	printf("Message for server: ");
	fflush(stdin);
    if (fgets(buff, sizeof(buff), stdin) == NULL) {
        fprintf(stderr, "ERROR: failed to get message for server\n");
        return "Error";
    }

	char* buff2 = strdup(replace_newline(buff));
	return buff2;
}

char* retr_request(int n, char* target){
	char* buff;
  buff = malloc(sizeof(char) * 16384); // Comment out this if you have segmentation fault
	strcpy(buff, "");
	if(n == 3){
		buff = custom_request();
		if(buff == "Error"){
			return "Error";
		}
		return buff;
	}
	if(strcmp(target,"https://sec-consult.com")==0){
		char request[2][256] = {"GET / HTTP/1.1\r\nHost: sec-consult.com\r\n\r\n", "POST /search/ HTTP/1.1\r\nHost: sec-consult.com\r\nContent-Length: 56\r\n\r\ntx_indexedsearch_pi2%5Bsearch%5D%5Bsword%5D=Cryptography"};
		strncpy(buff, request[n-1], sizeof(request[n-1]));
	}
	else if(strcmp(target,"https://www.zara.com")==0){
		char request[2][256] = {"GET /sg/ HTTP/1.1\r\nHost: www.zara.com\r\n\r\n", "POST /sg/en/shop/cart/add?ajax=true HTTP/1.1\r\nHost: www.zara.com\r\nContent-Length: 19\r\n\r\n{'products':[{}]}"};
		strncpy(buff, request[n-1], sizeof(request[n-1]));
	}
	else if(strcmp(target,"https://www.tutorialspoint.com")==0){
		char request[2][256] = {"GET /index.htm HTTP/1.1\r\nHost: www.tutorialspoint.com\r\n\r\n", "POST /videotutorials/ajax/ajaxUserLogin.php HTTP/1.1\r\nHost: www.tutorialspoint.com\r\nContent-Length: 35\r\n\r\nemail=user%40gmail.com&pwd=password"};
		strncpy(buff, request[n-1], sizeof(request[n-1]));
	}
	else if(strcmp(target,"https://www.w3schools.com")==0){
		char request[2][256] = {"GET / HTTP/1.1\r\nHost: www.w3schools.com\r\n\r\n", "POST /quiztest/quiztest.asp?qtest=HTML HTTP/1.1\r\nHost: www.w3schools.com\r\nContent-Length: 7\r\n\r\nq=covid"};
		strncpy(buff, request[n-1], sizeof(request[n-1]));
	}
	else if(strcmp(target,"https://sg.linkedin.com")==0){
		char request[2][256] = {"GET / HTTP/1.1\r\nHost: sg.linkedin.com\r\n\r\n", "POST /homepage-guest/api/ingraphs/ HTTP/1.1\r\nHost: sg.linkedin.com\r\nContent-Length: 19\r\n\r\n{'pageKey':'guest'}"};
		strncpy(buff, request[n-1], sizeof(request[n-1]));
	}
	return buff;
}

/* ---------------------------------------------------------- *
 * Retrieve Menu to display to user							  *
 * ---------------------------------------------------------- */
char* retr_menu(int n){
	switch (n)
	{
	case 0: return SC_MENU;
	case 1: return ZARA_MENU;
	case 2: return TP_MENU;
	case 3: return W3S_MENU;
	case 4: return LI_MENU;
	default: return "Not found";
	}
}


/* ---------------------------------------------------------- *
 * Concat char pointer to char at the end					  *
 * ---------------------------------------------------------- */
void catchar(char* main, char c){
        int len = strlen(main);
        main[len] = c;
        main[len+1] = '\0';
}


/* ---------------------------------------------------------- *
 * Converting \\r\\n to char \r\n							  *
 * ---------------------------------------------------------- */
char* replace_newline(char* buff){
	char* a;
	a = strdup("");
	while(*buff){
		char ch = *buff++;
		putchar(ch);
		if(ch=='\\'){
			switch (*buff++)
			{
				case 'r' : ch = '\r'; break;
				case 'n' : ch = '\n'; break;
				case '\\' : ch = '\\'; break;
				default: break;
			}
		}
		catchar(a, ch);
	}
	return a;
}


/* ---------------------------------------------------------- *
 * Free structures 							 				  *
 * ---------------------------------------------------------- */
int free_structures(WOLFSSL* ssl, int server, WOLFSSL_X509* servercert, WOLFSSL_CTX* ctx){
	wolfSSL_free(ssl);
	close(server);
	wolfSSL_X509_free(servercert);
	wolfSSL_CTX_free(ctx);
	wolfSSL_Cleanup();
    return 0;
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