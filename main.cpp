#include "./hash_map.h"

int main() {
  HashMap<int, int> h;
  h.insert({5, 10});
  std::cout << h.at(5) << std::endl;
  std::cout << h.at(6) << std::endl;
  return 0;
}
