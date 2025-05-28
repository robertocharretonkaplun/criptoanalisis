#include "Prerequisites.h"
#include "AsciiBinary.h"

int main() {
  AsciiBinary AB;

	std::string input = "Hello, World!";
	std::string binary = AB.stringToBinary(input);
	std::cout << "Texto a binario: " << binary << std::endl;
	std::string message = AB.binaryToString(binary);
	std::cout << "Binario a texto: " << message << std::endl;

  return 0;
}
