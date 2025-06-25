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


private:
	std::mt19937 m_engine;  ///< Motor de generación de números aleatorios Mersenne Twister.
};