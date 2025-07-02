#include "Prerequisites.h"
#include "CryptoGenerator.h"

int main() {
	// 1) Generar una contraseña de 16 caracteres (mayúsculas, minúsculas, dígitos)
	CryptoGenerator cryptoGen;
	cryptoGen.generatePassword(16); // Generate a password of length 16

	// 2) Generar 16 bytes aleatorios genéricos
	auto randomBytes = cryptoGen.generateBytes(16);
	std::cout << "Random Bytes (hex): " << cryptoGen.toHex(randomBytes) << std::endl;

	// 3) Clave AES de 128 bits
	auto key128 = cryptoGen.generateKey(128);
	std::cout << "Key 128-bit (hex): " << cryptoGen.toHex(key128) << std::endl;

	// 4) IV de 128 bits (16 bytes)
	auto iv = cryptoGen.generateIV(16);
	std::cout << "IV 128-bit (hex): " << cryptoGen.toHex(iv) << "\n";

	// 5) Salt de 16 bytes
	auto salt = cryptoGen.generateSalt(16);
	std::cout << "Salt (Base64): " << cryptoGen.toBase64(salt) << "\n";

	// 6) Safe release 
	//cryptoGen.secureWipe(iv);
	//cryptoGen.secureWipe(salt);

	// 7) from Base64
	std::string base64String = cryptoGen.toBase64(salt); // Example Base64 string
	std::cout << "Base64: " << base64String << "\n";

	auto fromBase64 = cryptoGen.fromBase64(base64String);
	std::cout << "From Base64: " << cryptoGen.toHex(fromBase64) << "\n";


	return 0;
}

