#pragma once
#include "Prerequisites.h"

class
	CesarEncryption {
public:
	CesarEncryption() = default;
	~CesarEncryption() = default;

	std::string
	encode(const std::string& texto, int desplazamiento) {
		std::string result = "";

		for (char c : texto) {
			if (c >= 'A' && c <= 'Z') {
				result += (char)(((c - 'A' + desplazamiento) % 26) + 'A');
			}
			else if (c >= 'a' && c <= 'z') {
				result += (char)(((c - 'a' + desplazamiento) % 26) + 'a');
			}
			else if (c >= '0' && c <= '9') {
				result += (char)(((c - '0' + desplazamiento) % 10) + '0');
			}
			else {
				result += c;
			}
		}

		return result;
	}

	std::string
	decode(const std::string& texto, int desplazamiento) {
		return encode(texto, 26 - (desplazamiento % 26));
	}

	void
	bruteForceAttack(const std::string& texto) {
		std::cout << "\nIntentos de descifrado por fuerza bruta:\n";
		for (int clave = 0; clave < 26; clave++) {
			std::string intento = encode(texto, 26 - clave);
			std::cout << "Clave " << clave << ": " << intento << std::endl;
		}
	}

	int
	evaluatePossibleKey(const std::string& texto) {
		int frecuencias[26] = { 0 };

		// Contar frecuencias de letras
		for (char c : texto) {
			if (c >= 'a' && c <= 'z') {
				frecuencias[c - 'a']++;
			}
			else if (c >= 'A' && c <= 'Z') {
				frecuencias[c - 'A']++;
			}
		}

		// Letras comunes en español (orden de frecuencia)
		const char letrasEsp[] = { 'e', 'a', 'o', 's', 'r', 'n', 'i', 'd', 'l', 'c' };

		// Buscar la letra más frecuente en el mensaje cifrado
		int indiceMax = 0;
		for (int i = 1; i < 26; ++i) {
			if (frecuencias[i] > frecuencias[indiceMax]) {
				indiceMax = i;
			}
		}

		// Probar varias suposiciones de mapeo
		int mejorClave = 0;
		int mejorPuntaje = -1;

		for (char letraRef : letrasEsp) {
			int clave = (indiceMax - (letraRef - 'a') + 26) % 26;
			int puntaje = 0;

			// Descifrar con esa clave
			std::string descifrado = encode(texto, 26 - clave);

			// Palabras comunes simples
			std::string comunes[] = { "el", "de", "la", "que", "en", "y", "los", "se" };

			for (std::string palabra : comunes) {
				if (descifrado.find(palabra) != std::string::npos) {
					puntaje++;
				}
			}

			if (puntaje > mejorPuntaje) {
				mejorPuntaje = puntaje;
				mejorClave = clave;
			}
		}

		return mejorClave;
	}

	void 
	encryptFile(const std::string& inputPath,
							const std::string& outputPath,
							int desplazamiento)	{
		std::string contenido = readFile(inputPath);
		std::string cifrado = encode(contenido, desplazamiento);
		writeFile(outputPath, cifrado);
	}

	void 
	decryptFile(const std::string& inputPath,
							const std::string& outputPath,
							int desplazamiento) {
		std::string contenido = readFile(inputPath);
		std::string claro = decode(contenido, desplazamiento);
		writeFile(outputPath, claro);
	}

	/// Genera N=26 archivos con nombre base + clave, por ejemplo: "salida_0.txt", "salida_1.txt", ...
	void 
	bruteForceFile(const std::string& inputPath,
		const std::string& outputDir,
		const std::string& baseName) {
		std::string contenido = readFile(inputPath);
		for (int clave = 0; clave < 26; clave++) {
			std::string descifrado = encode(contenido, 26 - clave);
			std::ostringstream filename;
			filename << outputDir << "/" << baseName << "_" << clave << ".txt";
			writeFile(filename.str(), descifrado);
		}
	}

	private:
		std::string 
		readFile(const std::string& path) {
			std::ifstream in(path, std::ios::in | std::ios::binary);
			if (!in) throw std::runtime_error("No se pudo abrir para lectura: " + path);
			std::ostringstream ss;
			ss << in.rdbuf();
			return ss.str();
		}

		void 
		writeFile(const std::string& path, const std::string& data) {
			std::ofstream out(path, std::ios::out | std::ios::binary);
			if (!out) throw std::runtime_error("No se pudo abrir para escritura: " + path);
			out << data;
		}
private:

};