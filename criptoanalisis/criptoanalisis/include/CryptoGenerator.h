#pragma once
#include "Prerequisites.h"

/**
 * @class CryptoGenerator
 * @brief Genera contrase�as, claves, IVs y salts criptogr�ficamente seguras,
 *        y provee utilidades de codificaci�n (hex/Base64) y limpieza de memoria.
 */
class
	CryptoGenerator {
public:
	/**
	 * @brief Constructor por defecto.
	 *
	 * Inicializa el motor Mersenne Twister con semilla extra�da de
	 * std::random_device para garantizar entrop�a criptogr�fica.
	 */
	CryptoGenerator() {
		std::random_device rd;  // Dispositivo de generaci�n de n�meros aleatorios con alta entrop�a.
		m_engine.seed(rd());    // Semilla el motor Mersenne Twister con la entrop�a del dispositivo.
	}

	~CryptoGenerator() = default;

	/**
		* @brief Genera una contrase�a aleatoria.
		*
		* @param length    Longitud de la contrase�a a generar.
		* @param useUpper  Incluir letras may�sculas [A�Z].
		* @param useLower  Incluir letras min�sculas [a�z].
		* @param useDigits Incluir d�gitos [0�9].
		* @param useSymbols Incluir s�mbolos especiales.
		* @return std::string Contrase�a generada.
		* @throws std::runtime_error Si no est� habilitado ning�n tipo de car�cter.
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
			password += pool[dist(m_engine)];  // Selecciona un car�cter aleatorio del pool.
		}
		return password;  // Devuelve la contrase�a generada.
	}

	/**
		 * @brief Genera un buffer de bytes aleatorios.
		 *
		 * @param numBytes Cantidad de bytes a generar.
		 * @return std::vector<uint8_t> Vector con valores en [0x00�0xFF].
		 */
	std::vector<uint8_t> 
	generateBytes(unsigned int numBytes) {
		std::vector<uint8_t> bytes(numBytes);
		std::uniform_int_distribution<int> dist(0, 255);  // Distribuci�n uniforme en el rango [0x00�0xFF].
		for (unsigned int i = 0; i < numBytes; ++i) {
			bytes[i] = static_cast<uint8_t>(dist(m_engine));  // Asigna un byte aleatorio al vector.
		}
		return bytes;  // Devuelve el vector de bytes generados.
	}


private:
	std::mt19937 m_engine;  ///< Motor de generaci�n de n�meros aleatorios Mersenne Twister.
};