<<<<<<< HEAD
# ict2205-crypto

 *   ./configure  --enable-opensslextra --enable-certgen		*
 *   To compile:  gcc -o test getcert.c -lwolfssl			*
=======
# ICT2205 Assignment 1 Part II

A C program which aims to:
* Validate public key certificates of websites
* Identify the public key values {e, n} in the website certificates
* Interact with websites with GET and POST messages over HTTPS

## Getting Started

These instructions will guide you to ensure code is working.

### Prequisites

Cryptographic library: 
[WolfSSL](https://github.com/wolfSSL/wolfssl/releases) - Version 4.6.0 and above

Linux Apt Install:
```
sudo apt update
sudo apt install autoconf
sudo apt install automake
sudo apt install libtool
```

### Installing
Unzip wolfssl-x.x.x-stable.tar.gz into /opt/wolfssl
```
mkdir /opt/wolfssl
unzip wolfssl-4.6.0-stable.tar.gz -d /opt/wolfssl
```
Installing WolfCrypt library
```
cd wolfssl-4.6.0-stable
./autogen.sh
./configure  --enable-opensslextra --enable-certgen
make
sudo make install
ldconfig
```
Compiling the program
* Please ensure that ```ICT2205_Code.c``` and ```ICT2205_Code.h```are in the same directory
* '-lcrypto' is not needed if you are running wolfssl 4.6.0 and above
```
gcc ICT2205_Code.c -o testssl -lwolfssl -lcrypto
```

## Running the program
To run the program, type this in the cmd
```
./testssl
```
Follow the instructions as prompted in the cmd to start
```
Please choose a number according to the options to retrieve certificates
1) sec-consult.com
2) www.zara.com
3) www.tutorialspoint.com
4) www.w3schools.com
5) sg.linkedin.com
0) Exit
Option: 4
```
After selecting the website, the program will attempt to open a socket and connect to retrieve the public key values of the website certificate
```
Successfully made the TCP connection to: https://www.w3schools.com.
Successfully enabled SSL/TLS session to: https://www.w3schools.com.
Retrieved the server's certificate from: https://www.w3schools.com.

RSA Private-Key: (2048 bit)
Modulus:
    00:bb:e4:97:40:f2:f9:60:5c:3e:c4:7b:eb:4a:5f:
    7c:ce:db:a3:5c:4b:ff:ed:3a:1f:ab:98:0d:15:07:
    fa:9d:35:fb:d9:ae:f8:4d:57:52:76:a9:28:f2:91:
    03:ab:df:0f:de:c6:aa:12:cd:3e:68:4c:fd:d4:75:
    e6:d0:a6:77:72:2b:d3:85:6a:dd:76:2f:b7:2e:77:
    9f:84:07:88:ff:67:bb:7d:c9:76:c4:fd:fa:ed:21:
    a7:b1:a5:4f:19:9a:b2:4c:aa:7e:76:aa:2d:7e:e2:
    2d:b8:6e:c1:79:79:58:83:3d:1d:1a:dc:17:61:05:
    1d:16:f9:77:af:e6:22:02:71:80:cd:25:ea:8a:d3:
    05:ec:a9:6d:8d:3a:7f:a3:1c:9e:f4:8a:87:df:65:
    81:4f:a1:e1:68:f8:17:4e:27:88:76:b9:d5:9f:ef:
    e4:03:14:17:05:df:eb:23:f2:79:20:5a:5d:7f:d4:
    0d:31:04:b2:68:96:03:45:9c:71:5b:fe:e9:80:35:
    5c:c5:ae:5c:a0:79:f6:70:a1:50:bf:ab:25:6a:1d:
    c3:b3:ba:41:34:b9:31:ad:29:94:f6:fd:ad:fa:88:
    8d:0a:76:72:73:4d:e8:96:e2:90:bd:c3:98:06:84:
    d9:5a:93:d3:b2:d5:ab:f3:04:f0:30:19:77:6d:dc:
    36:9b
Exponent: 65537 (0x10001)
```
You may then proceed to interact with the website with 3 options and the response will be saved as a HTML file under /request folder
1. Pre-crafted GET request
   ```
   Please choose a number accordingly. Option 1 & 2 are sample requests, to prove the script is working.
   1) GET - Homepage
   2) POST - Quiz function
   3) Custom request (Manually enter request)
   0) Return to main menu
   Option: 1
   ---Crafting request---
   ---Sending request---
   ---Receving request (Writing to file)---
   ```
2. Pre-crafted POST request
   ```
   Please choose a number accordingly. Option 1 & 2 are sample requests, to prove the script is working.
   1) GET - Homepage
   2) POST - Quiz function
   3) Custom request (Manually enter request)
   0) Return to main menu
   Option: 2
   ---Crafting request---
   ---Sending request---
   ---Receving request (Writing to file)---
   ```
3. Custom request
   ```
   Please choose a number accordingly. Option 1 & 2 are sample requests, to prove the script is working.
   1) GET - Homepage
   2) POST - Quiz function
   3) Custom request (Manually enter request)
   0) Return to main menu
   Option: 3
   ---Crafting request---
   Message for server: GET / HTTP/1.1\r\nHost: www.w3schools.com\r\n\r\n
   ---Sending request---
   ---Receving request (Writing to file)---
   ```
When the SSL/TLS socket is established, enter 0 to end the socket and return to the main menu
In the main menu, enter 0 to exit the program

## Team Members (TeamEggFriedRice)

* 1902125 EUGENE TAN WEI JIE
* 1902129 JAVIER LIM ZHENG HAO
* 1902130 TAN FU WEI
* 1902164 CHIN BING HONG
* 1902198 TAY KAI KENG
* 1902199 CLAUDIA CHAN

## Acknowledgments
* WolfSSL
>>>>>>> 593ecc95b5bce414601f9a3524410749507af68c
