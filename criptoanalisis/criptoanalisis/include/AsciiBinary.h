#pragma once
#include "Prerequisites.h"

class AsciiBinary {
public:
  AsciiBinary() = default;
  ~AsciiBinary() = default;

  // Convierte un carácter a una cadena de 8 bits
  std::string bitset(char c) const {
    std::string result(8, '0');
    unsigned char value = static_cast<unsigned char>(c);
    for (int i = 7; i >= 0; --i) {
      result[i] = (value & 1) ? '1' : '0';
      value >>= 1;
    }
    return result;
  }

  // Convierte una cadena de texto a su representación binaria ASCII (bits separados por espacios)
  std::string stringToBinary(const std::string& input) const {
    std::ostringstream oss;
    for (unsigned char c : input) {
      oss << bitset(c) << ' ';
    }
    std::string output = oss.str();
    if (!output.empty()) {
      output.pop_back(); // Eliminar espacio final
    }
    return output;
  }

  // Convierte un token binario de 8 bits a un carácter
  char binaryToChar(const std::string& binary) const {
    if (binary.size() != 8)
      throw std::invalid_argument("Cada token debe tener 8 bits.");
    unsigned char value = 0;
    for (char bit : binary) {
      value = (value << 1) | (bit - '0');
    }
    return static_cast<char>(value);
  }

  // Convierte una cadena de bits (tokens separados por espacios) a texto
  std::string binaryToString(const std::string& binaryInput) const {
    std::istringstream iss(binaryInput);
    std::string token;
    std::string result;
    while (iss >> token) {
      result += binaryToChar(token);
    }
    return result;
  }

  // --- Métodos de archivo ---

  // Lee un archivo binario completo y lo convierte a texto de bits ASCII
  void fileToBinary(const std::string& inputPath,
    const std::string& outputPath) const
  {
    std::string data = readBinaryFile(inputPath);
    std::string asBits = stringToBinary(data);
    writeTextFile(outputPath, asBits);
  }

  // Lee un archivo de texto con bits ASCII y reconstruye el archivo binario original
  void binaryToFile(const std::string& inputPath,
    const std::string& outputPath) const
  {
    std::string bitsText = readTextFile(inputPath);
    std::string original = binaryToString(bitsText);
    writeBinaryFile(outputPath, original);
  }

private:
  // Lectura/escritura de archivos binarios y de texto
  static std::string readBinaryFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) throw std::runtime_error("No se pudo abrir para lectura: " + path);
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
  }

  static void writeTextFile(const std::string& path,
    const std::string& text)
  {
    std::ofstream out(path, std::ios::out);
    if (!out) throw std::runtime_error("No se pudo abrir para escritura: " + path);
    out << text;
  }

  static std::string readTextFile(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("No se pudo abrir para lectura: " + path);
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
  }

  static void writeBinaryFile(const std::string& path,
    const std::string& data)
  {
    std::ofstream out(path, std::ios::binary);
    if (!out) throw std::runtime_error("No se pudo abrir para escritura: " + path);
    out.write(data.data(), data.size());
  }
};