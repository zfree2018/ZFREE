/*
 * Use SHA256 to hash string type of data
 */


#include "packet_hash.hpp"
#include <cstring>

string hash_TCP_Packet(Tins::PDU::serialization_type data) {

  try{
    data[16] = 0;
    data[17] = 0;
    if(data.size() >= 32){
      for(int i=23; i < 32; i++){
        data[i] = 0;
      }
    }
  }catch (...){
    std::cerr << "Out of Index.\n";
  }

  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, &data[0], data.size());
  SHA256_Final(hash, &sha256);
  stringstream ss;
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    ss << hex << setw(2) << setfill('0') << (int)hash[i];
  }

  return ss.str();
}

