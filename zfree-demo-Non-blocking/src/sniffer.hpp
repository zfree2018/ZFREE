#undef CERT
#include <tins/tins.h>

using namespace Tins;
using namespace std;

class ParsedPacket{
public:
  string packetHash;
  string srcIP;
  string dstIP;
  string srcPort;
  string dstPort;
  Tins::PDU::serialization_type tcpPacket;
};

class PayloadSniffer {
private:
  Sniffer *tinsSniffer;

public:
  ParsedPacket next();
  PayloadSniffer(string iface, string protocol);
  ~PayloadSniffer();
};
