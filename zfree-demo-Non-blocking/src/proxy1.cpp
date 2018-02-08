#include "sniffer.hpp"
#include "pistache/http.h"
#include "pistache/router.h"
#include "pistache/endpoint.h"
#include <arpa/inet.h>
#include <mutex>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <thread>
#include <unistd.h>

#define MAXBUF 1024

#ifdef DEBUG_MODE
#define DEBUG_STDERR(x) (std::cerr << x)
#define DEBUG_STDOUT(x) (std::cout << x)
#else
#define DEBUG_STDERR(x)
#define DEBUG_STDOUT(x)
#endif

using namespace std;
using namespace Net;

/// This class aims for providing consistent hash value storage and matching
/// Currently it uses map as in memory storage, later may be changed.
/// depend on performance.
class PacketStorage {
private:
  map<string, ParsedPacket> hash_map;
  mutex map_mutex;

public:
  void insert(string key, ParsedPacket &p) {
    lock_guard<mutex> lock(this->map_mutex);
    hash_map[key] = p;
  };
  size_t remove(string key){
    lock_guard<mutex> lock(this->map_mutex);
    return hash_map.erase(key);
  };
  ParsedPacket find(string key) {
    lock_guard<mutex> lock(this->map_mutex);
    map<string, ParsedPacket>::iterator it = this->hash_map.find(key);
    if (it != hash_map.end()) {
      return it->second;
    } else {
      ParsedPacket p;
      return p;
    }
  };
};

class HashFromSA{
private:
  // map second parameter is possible policy
  map<string, string> hash_map;
  mutex map_mutex;

public:
  void insert(string key, string policy){
    lock_guard<mutex> lock(this->map_mutex);
    hash_map[key] = policy;
  };

  size_t remove(string key){
    lock_guard<mutex> lock(this->map_mutex);
    return hash_map.erase(key);
  };

  string find(string key) {
    lock_guard<mutex> lock(this->map_mutex);
    map<string, string>::iterator it = this->hash_map.find(key);
    if(it != this->hash_map.end()){
      return it->second;
    }else{
      return "";
    }
  }
};

class CounterForDiffClient{
private:
  map<string, int> hash_map;
  mutex map_mutex;

public:

  int getCounter(string ip_addr){
    lock_guard<mutex> lock(this->map_mutex);
    map<string, int>::iterator it = this->hash_map.find(ip_addr);
    if(it == this->hash_map.end()){
      return 0;
    }
    else{
      return it->second;
    }
  }

  void increaseCount(string ip_addr){
    lock_guard<mutex> lock(this->map_mutex);
    map<string, int>::iterator it = this->hash_map.find(ip_addr);
    if(it == this->hash_map.end()){
      this->hash_map[ip_addr] = 1;
    }
    else{
      hash_map[ip_addr] = hash_map[ip_addr] + 1;
    }
  }
};

class TrafficAmountForDiffIP{
private:
  map<string, long> hash_map;
  mutex map_mutex;

public:

  long getAmount(string ip_addr){
    lock_guard<mutex> lock(this->map_mutex);
    map<string, long>::iterator it = this->hash_map.find(ip_addr);
    if(it == this->hash_map.end()){
      return 0;
    }
    else{
      return it->second;
    }
  }

  void increaseAmount(string ip_addr, ParsedPacket p){
    lock_guard<mutex> lock(this->map_mutex);
    map<string, long>::iterator it = this->hash_map.find(ip_addr);
    if(it == this->hash_map.end()){
      // eth-header + ip header
      this->hash_map[ip_addr] = p.tcpPacket.size() + 18+20;
    }
    else{
      hash_map[ip_addr] = hash_map[ip_addr] + p.tcpPacket.size() + 18+20;
    }
  }
};

unsigned long totalIncomeBytes = 0;
void accTotalIncomingHTTPTraffic(ParsedPacket p){
  if (p.srcPort.compare("80") == 0 || p.srcPort.compare("443") == 0){
    totalIncomeBytes = totalIncomeBytes + 18 + 20 + p.tcpPacket.size();
  }
}

// global variable for multiple threads access.
PacketStorage packetStorage;
HashFromSA hashValsFromSA;
CounterForDiffClient countersForDiffIPs;
TrafficAmountForDiffIP trafficAmount;

//++++++++++++++++++get matched packets+++++++++++++++++++++
int getMatchedPackets(){
  // currently only for one ip
  return countersForDiffIPs.getCounter("10.0.0.11");
};
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//----------------------------Rest Server ---------------------------------------
class StatsEndpoint {
public:
  StatsEndpoint(Net::Address addr)
      : httpEndpoint(std::make_shared<Net::Http::Endpoint>(addr)) {}

  void init(size_t thr = 2) {
    auto opts = Net::Http::Endpoint::options()
        .threads(thr)
        .flags(Net::Tcp::Options::InstallSignalHandler);
    httpEndpoint->init(opts);
    setupRoutes();
    Http::Header::Registry::registerHeader<CustomHeader>();
  }

  void start() {
    httpEndpoint->setHandler(router.handler());
    httpEndpoint->serve();
  }

  void shutdown() {
    httpEndpoint->shutdown();
  }

private:
  void setupRoutes() {
    using namespace Net::Rest;
    //bind handler here
    Routes::Get(router, "/packets", Routes::bind(&StatsEndpoint::doGetPackets, this));
    Routes::Get(router, "/total", Routes::bind(&StatsEndpoint::doGetTotalTraffic, this));
  }

  //handler
  void doGetPackets(const Rest::Request &request, Net::Http::ResponseWriter response) {
//    int packetsCount = 3000;
//    int packetsCount = getMatchedPackets();
    long trafficCount = trafficAmount.getAmount("10.0.0.10");
    cout << "------------------traffic amount " << trafficCount << endl;
    response.headers().add<CustomHeader>();
    response.send(Http::Code::Ok, std::to_string(trafficCount));
  }

  //handler
  void doGetTotalTraffic(const Rest::Request &request, Net::Http::ResponseWriter response) {
    response.headers().add<CustomHeader>();
    response.send(Http::Code::Ok, std::to_string(totalIncomeBytes));
  }

  class CustomHeader : public Http::Header::Header {
  public:

    NAME("Access-Control-Allow-Origin")

    CustomHeader() :
        value('*') {}

    void parse(const std::string &data) {
    }

    void write(std::ostream &os) const {
      os << value;
    }
  private:
    char value;
  };

  std::shared_ptr<Net::Http::Endpoint> httpEndpoint;
  Rest::Router router;
};

//-------------------------------Rest END------------------------------------

/// get certificate from one ssl connection
/// then verify certificate
/// \param ssl point to one ssl connection
/// \returns true if certificate can be verified.
bool verifyCert(SSL *ssl) {
  X509 *cert;
  cert = SSL_get_peer_certificate(ssl);
  // TODO complete verification.
  return true;
}

/// encapsulation for TLS connection
/// packaged corresponding socket file descriptor, SSL object, it's context
/// and TLS server socket address.
class SSLConnection {
public:
  int sock_fd;
  struct sockaddr_in srvr = {};
  SSL *ssl;
  SSL_CTX *ssl_ctx;
};

bool matchWhenGetPacket(ParsedPacket p){
  string packetHash = p.packetHash;
  string findRst = hashValsFromSA.find(packetHash);
  if(!findRst.empty()){
    // for specific user IP increase it's counter
    countersForDiffIPs.increaseCount(p.srcIP);
    trafficAmount.increaseAmount(p.srcIP, p);
    cout << "tcp size: " << p.tcpPacket.size() << "\n";
    cout << "-----------------" << trafficAmount.getAmount(p.srcIP) << endl;
    DEBUG_STDOUT("Counter for " << p.srcIP + " " << std::to_string(countersForDiffIPs.getCounter(p.srcIP)));
    // remove entity
    hashValsFromSA.remove(packetHash);
  }
  else{
    // save packets for later matching
    packetStorage.insert(packetHash, p);
  }
}

bool matchWhenGetHashFromSA(string hash){
  ParsedPacket p = packetStorage.find(hash);
  if(p.packetHash.empty()){
    // save this hash value into
    hashValsFromSA.insert(hash, "zero_rated");
  }
  else{
    countersForDiffIPs.increaseCount(p.srcIP);
    trafficAmount.increaseAmount(p.srcIP, p);
    cout << "-----------------" << trafficAmount.getAmount(p.srcIP) << endl;
    cout << "tcp size: " << p.tcpPacket.size() << "\n";
    DEBUG_STDOUT("Counter for " << p.srcIP << " " << std::to_string(countersForDiffIPs.getCounter(p.srcIP)) << "\n");
    // remove entity
    packetStorage.remove(hash);
  }
}


/// executed by one thread.
/// keep sniffer packet from the network interface specified by user.
/// \param iface name of network interface
void packetSniffer(string iface, string filter) {
//  string protocol = "tcp"; // here may changed
  PayloadSniffer payloadSniffer(iface, filter);
  while (1) {
    ParsedPacket p = payloadSniffer.next();
    matchWhenGetPacket(p);
    accTotalIncomingHTTPTraffic(p);
  }
}

/// Create SSL connection with TLS server
/// \param conn
/// \returns true if TLS connection established. false otherwise.
bool createSSL(SSLConnection &conn, string ip, string port) {
  SSL_library_init();                                 // initialize library
  OpenSSL_add_all_algorithms();                       // load all algorithms
  SSL_load_error_strings();                           // load error
  conn.ssl_ctx = SSL_CTX_new(SSLv23_client_method()); // create context

  if (conn.ssl_ctx == NULL) {
    ERR_print_errors_fp(stdout);
    return false;
  }
  // create socket for TCP communication
  if ((conn.sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Error occur while creating socket");
    return false;
  }
  conn.srvr.sin_family = AF_INET;
  // set server side port
  conn.srvr.sin_port = htons(atoi(port.c_str()));
  // set server ip
  if (inet_aton(ip.c_str(), (struct in_addr *)&conn.srvr.sin_addr.s_addr) ==
      0) {
    string err = "Cannot set ip address: " + ip;
    perror(err.c_str());
    return false;
  }
  // connect to server
  if (connect(conn.sock_fd, (struct sockaddr *)&conn.srvr, sizeof(conn.srvr)) !=
      0) {
    string err = "Connect to " + ip + ":" + port + " failed\n";
    perror(err.c_str());
    return false;
  }
  // printf("server connected\n");
  // create ssl
  conn.ssl = SSL_new(conn.ssl_ctx);
  // bind socket file descriptor and ssl;
  SSL_set_fd(conn.ssl, conn.sock_fd);
  // create ssl connection
  if (SSL_connect(conn.ssl) == -1) {
    ERR_print_errors_fp(stderr);
    perror("SSL connection failed");
    return false;
  } else {
    printf("Connected with %s encryption\n", SSL_get_cipher(conn.ssl));
  }
  return true;
}

/// close one TLS connection and it's corresponding socket.
void closeSSL(SSLConnection &conn) {
  if (conn.ssl != NULL) {
    SSL_shutdown(conn.ssl);
    SSL_free(conn.ssl);
  }
  close(conn.sock_fd);
  SSL_CTX_free(conn.ssl_ctx);
}

/// connect to different proxy2
/// invoked by std:thread
/// \param server_ip specifies the TLS server ip
/// \param server_port TLS server port for TLS connection
void sslClient(string server_ip, string server_port) {
//  unsigned int totalHashReceived = 0;
//  unsigned int matchedHash = 0;
  while (1) {
    SSLConnection conn;
    cout << "start SSL connection\n";
    try {
      if (createSSL(conn, server_ip, server_port)) {
        while (1) {
          char buffer[MAXBUF + 1];
          int len;
          fill(buffer, buffer + MAXBUF + 1, 0);
          len = SSL_read(conn.ssl, buffer, MAXBUF);

          // TODO check whether SSL connection will break.
          if (len > 0) {
            DEBUG_STDOUT("receive message: " << buffer << " total length "
                                             << len << "\n");
            string hashValue(buffer);
            DEBUG_STDOUT("hash: " << hashValue << "\n");
            matchWhenGetHashFromSA(hashValue);
          } else if (len == 0) {
            printf("Read end\n");
            break;
          } else {
            printf("Failed! Error code: %d, error message: %s \n", errno,
                   strerror(errno));
            break;
          }
        }
        printf("Received from %s:%s \n", server_ip.c_str(), server_port.c_str());

      } else {
      }
    }catch (const std::exception& e){
      std::cout << e.what() << "\n";
    }catch (...){
      std::cerr << "Unknown exception \n";
    }

    try {
      closeSSL(conn);
    }catch (...){

    }


    // sleep 5 seconds
    for (int i = 0; i < 3; i++) {
      usleep(1000 * 1000);
      printf("idle %d seconds\n", i);
    }
  }
}

int main(int argc, char **argv) {
  // fake server address and ports
  vector<pair<string, string>> config;
  config.push_back(make_pair("10.0.1.10", "8889"));
//  config.push_back(make_pair("127.0.0.1", "8889"));
#ifdef DEBUG_MODE
  cout << "debug mode\n";
#endif

  if (argc != 3) {
    cout << "parameters <network-interface> <filter-string>\n";
    cout << "e.g. ./proxy1 ens33 \"ip dst 10.0.0.10\" \n";
    return -1;
  }
  string iface = argv[1];
  string filter = argv[2];

  thread snifferThread(packetSniffer, iface, filter);

  thread *clients[config.size()];
  for (int i = 0; i < config.size(); ++i) {
    pair<string, string> c = config[i];
    string IP = c.first;
    string port = c.second;
    thread *t = new thread(sslClient, IP, port);
    clients[i] = t;
    //(*t).join();
  }
  // start REST server
  Net::Port port(9080);
  int thr = 2;
  Net::Address addr(Net::Ipv4::any(), port);
  StatsEndpoint stats(addr);
  stats.init(thr);
  stats.start();
  // end start

  for (int i = 0; i < config.size(); i++) {
    clients[i]->join();
  }
  snifferThread.join();
  std::cout << "deleting threads"; // TODO figure out why change to printf cause
                                   // problem
  for (int i = 0; i < config.size(); ++i) {
    delete clients[i];
  }
  stats.shutdown();
  return 0;
}
