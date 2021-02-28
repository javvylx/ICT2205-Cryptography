#include <wolfssl/options.h>
#include <wolfssl/ssl.h>
#include <wolfssl/openssl/pem.h>
#include <wolfssl/openssl/bio.h>
#include <wolfssl/wolfcrypt/rsa.h>

#define CERT_FILE_CA "./cert/USERTrustRSACertificationAuthority.pem" //Which website CA cert is this
#define CERT_FILE_CA2 "./cert/DigiCertGlobalRootCA.pem" //Which website CA cert is  this
#define CERT_FILE_CA3 "./cert/AmazonCA.pem" //Which website CA cert is  this
#define WEBSITE_MENU "Please choose a number according to the options to retrieve certificates\n1) sec-consult.com\n2) www.zara.com\n3) www.tutorialspoint.com\n4) www.w3schools.com\n5) sg.linkedin.com\n0) Exit\nOption: " //Starting menu for user
#define SC_MENU "Please choose a number accordingly. Option 1 & 2 are sample requests, to prove the script is working.\n1) GET - Homepage\n2) POST - Search function\n3) Custom request (Manually enter request)\n0) Return to main menu\nOption: "
#define ZARA_MENU "Please choose a number accordingly. Option 1 & 2 are sample requests, to prove the script is working.\n1) GET - Homepage\n2) POST - Add Cart function\n3) Custom request (Manually enter request)\n0) Return to main menu\nOption: "
#define TP_MENU "Please choose a number accordingly. Option 1 & 2 are sample requests, to prove the script is working.\n1) GET - Homepage\n2) POST - Login function\n3) Custom request (Manually enter request)\n0) Return to main menu\nOption: "
#define W3S_MENU "Please choose a number accordingly. Option 1 & 2 are sample requests, to prove the script is working.\n1) GET - Homepage\n2) POST - Quiz function\n3) Custom request (Manually enter request)\n0) Return to main menu\nOption: "
#define LI_MENU "Please choose a number accordingly. Option 1 & 2 are sample requests, to prove the script is working.\n1) GET - Homepage\n2) POST - Forgot Password function\n3) Custom request (Manually enter request)\n0) Return to main menu\nOption: "

// To create a socket & TCP-connect to server
int create_socket(char[], BIO *);
int program_cert(int num_option);
int free_structures(WOLFSSL* ssl, int server, WOLFSSL_X509* servercert, WOLFSSL_CTX* ctx);
void catchar(char* main, char c);
char* replace_newline(char* buff);
char* retr_request(int n, char* target);
char* custom_request();
int process_request(int n, char* target, WOLFSSL* ssl);
int program_cert(int num_option);
char* retr_menu(int n);
