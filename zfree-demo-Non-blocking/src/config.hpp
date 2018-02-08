#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class Configuration{
private:
  std::string filename;
  std::vector<std::pair<std::string, std::string>> config;
  int lines;
public:
  Configuration(std::string filename);
  std::pair<std::string, std::string> get(int i);
  int size();
};
