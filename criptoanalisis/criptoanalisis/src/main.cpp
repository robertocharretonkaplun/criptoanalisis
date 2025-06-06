#include "Prerequisites.h"
#include "DES.h"

int main() {
	//std::bitset<64> plaintext("0001001000110100010101100111100010011010101111001101111011110001");
	std::bitset<64> key("0001001100110100010101110111100110011011101111001101111111110001");

	std::string phrase = "$Hola DES!";
	//std::string userKey = "$el_pepe$";

	// Asegurar que la frase tenga longitud múltiplo de 8
	while (phrase.size() % 8 != 0) {
		phrase += '\0'; // null space
	}

	std::vector<std::bitset<64>> cipherBlocks;
	std::cout << "Texto original: " << phrase << std::endl;
	std::cout << "Cifrado en hexadecimal:\n";

	// Cifrado con DES
	DES des(key);
	for (size_t i = 0; i < phrase.size(); i += 8) {
		std::string block = phrase.substr(i, 8);
		auto blockBits = des.stringToBitset64(block);
		auto encrypted = des.encode(blockBits);
		cipherBlocks.push_back(encrypted);
		std::cout << std::hex << std::uppercase << std::setw(16) << std::setfill('0') << encrypted.to_ullong() << " ";
	}

	std::cout << std::endl;

	// Descifrado con DES
	std::string output;
	for (const auto& block : cipherBlocks) {
		auto decrypted = des.decode(block);
		output += des.bitset64ToString(decrypted);
	}

	std::cout << "Texto descifrado: " << phrase << std::endl;
	//auto ciphertext = des.encode(plaintext);
	//std::cout << "Cifrado: " << ciphertext << std::endl;

	return 0;
}
