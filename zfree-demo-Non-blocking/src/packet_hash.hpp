
#include <iomanip>
#include <sstream>
#include <string>
#include <openssl/sha.h>
#include <vector>
#include <tins/tins.h>

using namespace std;


string hash_TCP_Packet(Tins::PDU::serialization_type data);

vector<uint8_t> ts2bytes(pair<uint32_t, uint32_t> timestamp);