#include "sniffer.hpp"
#include "packet_hash.hpp"

void printTcp(TCP tcp){
  auto bytes = tcp.serialize();
  std::cout << "tcp packet hex str: ";
  for (int i = 0; i < bytes.size(); ++i){
    cout << std::hex << (int)bytes[i];
    cout << " ";
  }
  std::cout << "\n";
}

void printIP(IP ip){
  auto bytes = ip.serialize();
  std::cout << "IP packet hex str: ";
  for (int i = 0; i < bytes.size(); ++i){
    cout << std::hex << (int)bytes[i];
    cout << " ";
  }
  std::cout << "\n";
}

void printEthPacket(PDU* packet){
  auto bytes = packet->serialize();
  std::cout << "Eth packet hex str: ";
  for (int i = 0; i < bytes.size(); ++i){ 
    cout << std::hex << (int)bytes[i];
    cout << " ";
  }
  std::cout << "\n";
}

ParsedPacket PayloadSniffer::next() {
  ParsedPacket p;
  while(1){
    PDU *packet = this->tinsSniffer->next_packet();
    try {
      // get IP and port
      IP &ip = packet->rfind_pdu<IP>();
      TCP &tcp = packet->rfind_pdu<TCP>();
      p.srcIP = ip.src_addr().to_string();
      p.srcPort = std::to_string(tcp.sport());
      p.dstIP = ip.dst_addr().to_string();
      p.dstPort = std::to_string(tcp.dport());
      p.tcpPacket = tcp.serialize();
      if(p.srcPort.compare("22") == 0 || p.dstPort.compare("22") == 0){
        continue;
      }else{
        break;
      }
//      break;

    }catch (const pdu_not_found){
      std::cerr << "PDU not found; try next packet\n";
      continue;
    } catch (...){
      std::cerr << "other problem; try next packet\n";
    }
  }

  string hashStr = hash_TCP_Packet(p.tcpPacket);
  std::cout << "packet src " << p.srcIP << ":" << p.srcPort << " packet dst " << p.dstIP << ":" << p.dstPort << " hash: " << hashStr << "\n";
  p.packetHash = hashStr;

//  printTcp(tcp);
//  printIP(ip);
//  printEthPacket(packet);

//  try {
//    p.timestamp = tcp.timestamp();
//  }catch (const option_not_found) {
//    cout << "option_not_found";
//    pair<uint32_t, uint32_t> emptyTS;
//    p.timestamp = emptyTS;
//  }
//  try{
//
//    const RawPDU data = packet->rfind_pdu<RawPDU>();
//    vector<uint8_t> payload = data.payload();
//
//    string hashValue = hash_TCP_Packet(payload, p.srcIP, p.dstIP, p.srcPort, p.dstPort, p.timestamp);
//    if (hashValue.empty()){
//      hashValue = "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
//    }
//    p.packetHash = hashValue;
//  }
//  catch(const pdu_not_found){
//    vector<uint8_t> emptyPayload;
//    string hashValue = hash_TCP_Packet(emptyPayload, p.srcIP, p.dstIP, p.srcPort, p.dstPort, p.timestamp);
//    p.packetHash = hashValue;
//  }
//  catch(...){
//    cout <<"Other exception\n";
//  }

  return p; // FIXME this may have problem.
}



PayloadSniffer::PayloadSniffer(string iface, string filter) {
  SnifferConfiguration config;
  config.set_promisc_mode(true);
  config.set_immediate_mode(true);
  config.set_filter(filter);
  Sniffer *sniffer = new Sniffer(iface, config);
  tinsSniffer = sniffer;
}

PayloadSniffer::~PayloadSniffer(){
  delete tinsSniffer;
}
