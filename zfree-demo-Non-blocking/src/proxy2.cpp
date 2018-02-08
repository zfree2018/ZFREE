#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <queue>
#include <sys/wait.h>
#include <unistd.h>
#include <thread>
#include <sys/socket.h>
#include <sys/types.h>
#include <chrono>
#include "sniffer.hpp"
#include "ConcurrentQueue.hpp"

using namespace Tins;
using namespace std;

#define MAXBUF 1024

ConcurrentQueue sharedQueue;

SSL *globalSSL;
bool globalIsConnected;

void ethSniffer(const string &iface, string filter) {
  //string iface = "ens33";
  cout <<"sniffer network interface " << iface << endl;
//  string protocol = "tcp";

  PayloadSniffer payloadSniffer(iface, filter);
  while (1) {
    ParsedPacket p = payloadSniffer.next();
    string packetHash = p.packetHash;
//    std::cout << "hash: " << packetHash;
//    std::cout << " receive time: " << std::chrono::system_clock::now().time_since_epoch().count() << "\n";

    if (!packetHash.empty()) {
      if (globalIsConnected){
        char buf[MAXBUF + 1];
        strcpy(buf, packetHash.c_str());

        if (SSL_write(globalSSL, buf, strlen(buf)) <= 0)
        {
          printf("Sending message '%s'Failed, Error code: %d,Error message: '%s'\n",
                 buf, errno, strerror(errno));
          sharedQueue.push(packetHash);
          globalIsConnected = false;
          break;
        }
        else {
          printf("Send Immediately '%s' succeed, total length: %d bytes\n", buf, static_cast<int>(strlen(buf)));
          std::cout << " send time: " << std::chrono::system_clock::now().time_since_epoch().count() << "\n";
        }

      }
      else {sharedQueue.push(packetHash);}
    }
  }

}

//SSL context initialization
SSL_CTX *SSLInit() 
{
  SSL_library_init();
  OpenSSL_add_all_algorithms();
  /* load all error message strings */
  SSL_load_error_strings();
  SSL_CTX *ctx;

  string cert_loc = "./cacert.pem";
  string private_key = "./privkey.pem";
  /* create a TLS context based upon TLS v2 and v3  */
  ctx = SSL_CTX_new(SSLv23_server_method());

  if (ctx == NULL) {
    ERR_print_errors_fp(stdout);
    exit(1);
  }

  /* load certificate */
  if (SSL_CTX_use_certificate_file(ctx, cert_loc.c_str(), SSL_FILETYPE_PEM) <=
      0) {
    ERR_print_errors_fp(stdout);
    exit(1);
  }
  /* load private key*/
  if (SSL_CTX_use_PrivateKey_file(ctx, private_key.c_str(), SSL_FILETYPE_PEM) <=
      0) {
    ERR_print_errors_fp(stdout);
    exit(1);
  }
  /* check private key with certificate*/
  if (!SSL_CTX_check_private_key(ctx)) {
    ERR_print_errors_fp(stdout);
    exit(1);
  }
  return ctx;

}

//create a tcp listening socket
int create_socket(const unsigned &port)
{
  int s;
  struct sockaddr_in addr;

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) {
    perror("Unable to create socket");
    exit(EXIT_FAILURE);
  }
  int ret = ::bind(s, (struct sockaddr*)&addr, sizeof(addr));
  if ( ret < 0) {
    perror("Unable to bind");
    exit(EXIT_FAILURE);
  }

  if (listen(s, 10) < 0) {
    perror("Unable to listen");
    exit(EXIT_FAILURE);
  }

  return s;
}

// client thread
void clientProcess(int clientSocket)
{
  printf("%s\n", "start client thread");
  SSL_CTX *ctx = SSLInit(); 
  SSL *ssl;
  ssl = SSL_new(ctx);
  SSL_set_fd(ssl, clientSocket);
  char buf[MAXBUF + 1];
  if (SSL_accept(ssl) <= 0) 
  {
    ERR_print_errors_fp(stderr);
  }
  else 
  {
//    bool isConnected = true;
    globalSSL = ssl;
    globalIsConnected = true;
    while (globalIsConnected) {
      
      while (!sharedQueue.empty()) 
      {
        string hashValue = sharedQueue.pop();
        strcpy(buf, hashValue.c_str());
        if (SSL_write(ssl, buf, strlen(buf)) <= 0) 
        {
          printf("Sending message'%s'Failed, Error code: %d,Error message: '%s'\n", 
                  buf, errno, strerror(errno));
          sharedQueue.push(hashValue);
          globalIsConnected = false;
          break;
        } 
        else {
          printf("Send '%s' succeed, total length: %d bytes\n", buf, static_cast<int>(strlen(buf)));
          std::cout << " send time: " << std::chrono::system_clock::now().time_since_epoch().count() << "\n";
        }
      }
      if (!globalIsConnected) //release resource when socket is closed by client
      {
          SSL_shutdown(ssl);
          SSL_free(ssl);
          SSL_CTX_free(ctx);
          close(clientSocket);
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(2000));
      
    }
    
  }
  
}

void msgServer(const unsigned &port) {

  int sockfd = create_socket(port);

  // loop for client connections
  while (1) {
    cout << "Waiting for client connections" << endl;

    sockaddr_in client_addr;
    uint len = sizeof(client_addr);
    
    /* wait for client connections*/
    int clientSocket = accept(sockfd, (struct sockaddr*)&client_addr, &len);
    if (clientSocket < 0) {
      perror("Unable to accept");
      exit(EXIT_FAILURE);
    }
    else
    {
       std::thread clientThread(clientProcess, clientSocket);
       // detach client thread so server will receive new socket connection
       clientThread.detach(); 
    }
        
  }
  /* close listening */
  close(sockfd);
}

int main(int argc, char **argv) {
  
  if (argc != 4){
    printf("Usage <sniffer port> <packet-filter> <server port> \n");
    return 0;
  }
  
  string iface = argv[1];
  string filter = argv[2];
  unsigned port = atoi(argv[3]);
  signal(SIGPIPE, SIG_IGN);
  std::thread snifferThread(ethSniffer, iface, filter);
  std::thread msgServerThread(msgServer, port);
  snifferThread.join();
  msgServerThread.join();

  return 0;
}
