#pragma once
#include "Prerequisites.h"

/**
 * @class CryptoGenerator
 * @brief Genera contraseñas, claves, IVs y salts criptográficamente seguras,
 *        y provee utilidades de codificación (hex/Base64) y limpieza de memoria.
 */
class
	CryptoGenerator {
public:
	/**
	 * @brief Constructor por defecto.
	 *
	 * Inicializa el motor Mersenne Twister con semilla extraída de
	 * std::random_device para garantizar entropía criptográfica.
	 */
	CryptoGenerator() {
		std::random_device rd;  // Dispositivo de generación de números aleatorios con alta entropía.
		m_engine.seed(rd());    // Semilla el motor Mersenne Twister con la entropía del dispositivo.
	}

	~CryptoGenerator() = default;

	/**
		* @brief Genera una contraseña aleatoria.
		*
		* @param length    Longitud de la contraseña a generar.
		* @param useUpper  Incluir letras mayúsculas [A–Z].
		* @param useLower  Incluir letras minúsculas [a–z].
		* @param useDigits Incluir dígitos [0–9].
		* @param useSymbols Incluir símbolos especiales.
		* @return std::string Contraseña generada.
		* @throws std::runtime_error Si no está habilitado ningún tipo de carácter.
		*/
	std::string 
	generatePassword(unsigned int length,
									 bool useUpper = true,
									 bool useLower = true,
									 bool useDigits = true,
									 bool useSymbols = false) {
		const std::string uppers = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		const std::string lowers = "abcdefghijklmnopqrstuvwxyz";
		const std::string digits = "0123456789";
		const std::string symbols = "!@#$%^&*()-_=+[]{}|;:',.<>?/";

		std::string pool;
		if (useUpper) pool += uppers;
		if (useLower) pool += lowers;	
		if (useDigits) pool += digits;
		if (useSymbols) pool += symbols;

		// Check if the pool is empty
		if (pool.empty()) {
			throw std::runtime_error("No character types enabled for password generation.");
		}

		std::uniform_int_distribution<unsigned int> dist(0, pool.size() - 1);
		std::string password;
		password.reserve(length);  // Reservar espacio para evitar reallocaciones.

		for (unsigned int i = 0; i < length; ++i) {
			password += pool[dist(m_engine)];  // Selecciona un carácter aleatorio del pool.
		}
		return password;  // Devuelve la contraseña generada.
	}

	/**
		 * @brief Genera un buffer de bytes aleatorios.
		 *
		 * @param numBytes Cantidad de bytes a generar.
		 * @return std::vector<uint8_t> Vector con valores en [0x00–0xFF].
		 */
	std::vector<uint8_t> 
	generateBytes(unsigned int numBytes) {
		std::vector<uint8_t> bytes(numBytes);
		std::uniform_int_distribution<int> dist(0, 255);  // Distribución uniforme en el rango [0x00–0xFF].
		for (unsigned int i = 0; i < numBytes; ++i) {
			bytes[i] = static_cast<uint8_t>(dist(m_engine));  // Asigna un byte aleatorio al vector.
		}
		return bytes;  // Devuelve el vector de bytes generados.
	}

	// Convierte bytes a cadena hexadecimal
	std::string 
	toHex(const std::vector<uint8_t>& data) {
		std::ostringstream oss;

		for (const auto& byte : data) {
			oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
		}
		return oss.str();  // Convierte el vector de bytes a una cadena hexadecimal.
	}

	// Decodifica una cadena hexadecimal a bytes
	std::vector<uint8_t> 
	fromHex(const std::string& hex) {
		if (hex.size() % 2 != 0)
			throw std::runtime_error("Hex inválido (longitud impar).");
		
		std::vector<uint8_t> data(hex.size() / 2);
		for (size_t i = 0; i < data.size(); ++i) {
			unsigned int byte;
			std::istringstream(hex.substr(2 * i, 2)) >> std::hex >> byte;
			data[i] = static_cast<uint8_t>(byte);
		}
		return data;
	}

	/**
	 * @brief Genera una clave simétrica de tamaño dado en bits.
	 *
	 * @param bits Tamaño de la clave en bits (debe ser múltiplo de 8).
	 * @return std::vector<uint8_t> Clave generada (bits/8 bytes).
	 * @throws std::runtime_error Si bits no es múltiplo de 8.
	 */
	std::vector<uint8_t> 
	generateKey(unsigned int bits) {
		if (bits % 8 != 0) {
			throw std::runtime_error("Bits debe ser múltiplo de 8.");
		}
		return generateBytes(bits / 8);
	}

	/**
		* @brief Genera un vector de inicialización (IV) de tamaño dado en bytes.
		* Un IV es un valor aleatorio que se usa en modos de cifrado simétrico (CBC, CFB, GCM…)
		* para garantizar que la misma clave cifre mensajes idénticos en salidas distintas
		*
		* @param blockSize Tamaño del IV en bytes.
		* @return std::vector<uint8_t> IV generado.
		*/
	std::vector<uint8_t> 
	generateIV(unsigned int blockSize) {
		return generateBytes(blockSize);  // Genera un IV aleatorio del tamaño especificado.
	}

	/**
	 * @brief Genera una salt criptográfica de longitud dada.
	 * Una salt es un valor aleatorio que se combina con la contraseña al derivar una clave 
	 * (por ejemplo, en PBKDF2, scrypt, Argon2) para evitar ataques de tabla arcoíris y asegurar 
	 * que cada derivación sea única. Salt se usa para “espesar” la entropía de contraseñas 
	 * al derivar claves
	 *
	 * @param length Longitud de la salt en bytes.
	 * @return std::vector<uint8_t> Salt generada.
	 */
	std::vector<uint8_t> 
	generateSalt(unsigned int length) {
		return generateBytes(length);
	}

	/**
		 * @brief Convierte un vector de bytes a una cadena Base64.
		 *
		 * @param data Vector de bytes de entrada.
		 * @return std::string Representación Base64 (con relleno ‘=’ si aplica).
		 */
	std::string 
		toBase64(const std::vector<uint8_t>& data) {
		static const char* table =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789+/";
		std::string b64;
		unsigned int i = 0;

		/**
		 * Procesar bloques de 3 bytes.Tomamos 3 bytes y los concatenamos en un entero de 24 bits.
		 * Ese entero lo dividimos en cuatro “seis bits” (24 ÷ 6 = 4).
		 * Cada grupo de 6 bits (valor 0–63) se usa como índice en la table para obtener un carácter.
		 */
		while (i + 2 < data.size()) {
			unsigned int block = (data[i] << 16) | (data[i + 1] << 8) | data[i + 2];
			b64 += table[(block >> 18) & 0x3F];
			b64 += table[(block >> 12) & 0x3F];
			b64 += table[(block >> 6) & 0x3F];
			b64 += table[(block) & 0x3F];
			i += 3;
		}

		// Procesar los últimos 1 o 2 bytes restantes, añadiendo relleno '=' si es necesario.
		if (i < data.size()) {
			uint32_t block = data[i] << 16;
			b64 += table[(block >> 18) & 0x3F];
			if (i + 1 < data.size()) {
				block |= data[i + 1] << 8;
				b64 += table[(block >> 12) & 0x3F];
				b64 += table[(block >> 6) & 0x3F];
				b64 += '=';
			}
			else {
				b64 += table[(block >> 12) & 0x3F];
				b64 += "==";
			}
		}

		return b64;  // Devuelve la cadena Base64 generada.
	}

private:
	std::mt19937 m_engine;  ///< Motor de generación de números aleatorios Mersenne Twister.
};