#pragma once
#include "Prerequisites.h"

class 
Vigenere {
public:
	Vigenere() = default;

	Vigenere(const std::string& key) :key(normalizeKey(key)) {
		if (key.empty()) {
			throw std::invalid_argument("La clave no puede estar vacía o sin letras.");
		}
	}

	static std::string 
	normalizeKey(const std::string& rawKey) {
		std::string k;
		for (char c : rawKey) {
			if(std::isalpha(static_cast<unsigned char>(c))) {
				k += std::toupper(static_cast<unsigned char>(c)); // Convert to uppercase and append
			}
		}
		return k;
	}

	std::string encode(const std::string& text) {
		std::string result;
		result.reserve(text.size()); // Reserve space for efficiency
		unsigned int i = 0; // Index for the key

		for (char c : text) {
			if (std::isalpha(static_cast<unsigned char>(c))) {
				bool isLower = std::islower(static_cast<unsigned char>(c));
				char base = isLower ? 'a' : 'A'; // Determine base based on case

				// Desplazamiento de la key
				int shift = key[i % key.size()] - 'A'; // Calculate shift based on key character mod26
				// Encode
				char encodedChar = static_cast<char>((c - base + shift) % 26 + base);
				result += encodedChar; // Append encoded character to result
				i++; // Increment key index
			}
			else {
				result += c; // Non-alphabetic characters are added unchanged
			}
		}

		return result; // Return the encoded string
	}

	std::string decode(const std::string& text) {
		std::string result;
		result.reserve(text.size()); // Reserve space for efficiency
		unsigned int i = 0; // Index for the key

		for (char c : text) {
			if (std::isalpha(static_cast<unsigned char>(c))) {
				bool isLower = std::islower(static_cast<unsigned char>(c));
				char base = isLower ? 'a' : 'A'; // Determine base based on case

				// Desplazamiento de la key
				int shift = key[i % key.size()] - 'A'; // Calculate shift based on key character mod26
				// decode
				char encodedChar = static_cast<char>(((c - base) - shift + 26) % 26 + base);
				result += encodedChar; // Append encoded character to result
				i++; // Increment key index
			}
			else {
				result += c; // Non-alphabetic characters are added unchanged
			}
		}

		return result; // Return the encoded string
	}

private:
	std::string key; // The key for the Vigenere cipher
};