#include "Prerequisites.h"
#include "DES.h"

int main() {
  std::bitset<64> plaintext("0001001000110100010101100111100010011010101111001101111011110001");
  std::bitset<64> key("0001001100110100010101110111100110011011101111001101111111110001");

  std::string userKey = "$el_pepe$";

  DES des(key);
  auto ciphertext = des.encode(plaintext);
  std::cout << "Cifrado: " << ciphertext << std::endl;

  return 0;
}
