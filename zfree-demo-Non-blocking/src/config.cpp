#include "config.hpp"

// read file name and put all its line in config by pair
Configuration::Configuration(std::string file_name) {

  // open file by read only
  std::ifstream infile(file_name, std::ios::in);
  std::string cur_line;
  if (infile) // file exits
  {
    while (getline(infile, cur_line)) {
      auto index = cur_line.find(',');
      std::pair<std::string, std::string> keyVal;
      if (index != std::string::npos) {

        // Split around ',' character
        keyVal =
            make_pair(cur_line.substr(0, index), cur_line.substr(index + 1));

        // Trim any leading ' ' in the value part
        // (you may wish to add further conditions, such as '\t')
        while (!keyVal.second.empty() && keyVal.second.front() == ' ')
          keyVal.second.erase(0, 1);

        config.push_back(keyVal);
      }
    }
  } else
    std::cerr << "No such file!\n";

  lines = (int)config.size();

  infile.close();
};

// get: get ith line in configuration file and return by pair
std::pair<std::string, std::string> Configuration::get(int i) {
  std::pair<std::string, std::string> cur_pair = this->config.at(i);
  return cur_pair;
};

// size: return size of file(number of lines)
int Configuration::size(void) { return this->lines; };
