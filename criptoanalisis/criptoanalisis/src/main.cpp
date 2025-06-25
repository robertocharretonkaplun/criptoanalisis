#include "Prerequisites.h"
#include "CryptoGenerator.h"

int main() {
	// 1) Generar una contrase�a de 16 caracteres (may�sculas, min�sculas, d�gitos)
	CryptoGenerator cryptoGen;
	cryptoGen.generatePassword(16); // Generate a password of length 16

	// 2) Generar 16 bytes aleatorios gen�ricos
	auto randomBytes = cryptoGen.generateBytes(16);
	//std::cout << "Random Bytes (hex): " << cryptoGen.toHex(randomBytes) << std::endl;
	return 0;
}

