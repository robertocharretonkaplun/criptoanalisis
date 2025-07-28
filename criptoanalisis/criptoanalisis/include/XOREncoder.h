#pragma once
#include "Prerequisites.h"

class XOREncoder {
public:
  XOREncoder() = default;
  ~XOREncoder() = default;

  // --- Texto en memoria ---
  std::string encode(const std::string& input, const std::string& key) const {
    std::string output = input;
    for (size_t i = 0; i < input.size(); ++i) {
      output[i] = input[i] ^ key[i % key.size()];
    }
    return output;
  }

  // --- I/O de archivos ---
  void encryptFile(const std::string& inPath,
    const std::string& outPath,
    const std::string& key) const
  {
    auto buf = readFile(inPath);
    std::string plain(reinterpret_cast<const char*>(buf.data()), buf.size());
    std::string cipher = encode(plain, key);
    writeFile(outPath,
      std::vector<unsigned char>(cipher.begin(), cipher.end()));
  }

  // Para XOR, encriptar y desencriptar es la misma operación
  void decryptFile(const std::string& inPath,
    const std::string& outPath,
    const std::string& key) const
  {
    encryptFile(inPath, outPath, key);
  }

  // --- Fuerza bruta 1 byte sobre archivo ---
  // Genera un archivo descifrado por cada posible valor de byte [0..255]
  void bruteForce1ByteFile(const std::string& inPath,
    const std::string& outDir) const
  {
    auto buf = readFile(inPath);
    fs::create_directories(outDir);
    for (int k = 0; k < 256; ++k) {
      std::string key(1, static_cast<char>(k));
      std::string decoded;
      decoded.reserve(buf.size());
      for (auto b : buf) decoded += static_cast<char>(b ^ k);

      // Solo guardamos si el texto resultante parece legible
      if (isValidText(decoded)) {
        std::ostringstream fname;
        fname << outDir << "/xor1b_0x"
          << std::hex << std::setw(2) << std::setfill('0') << k
          << ".bin";
        writeFile(fname.str(),
          std::vector<unsigned char>(decoded.begin(), decoded.end()));
        std::cout << "Guardado: " << fname.str() << "\n";
      }
    }
  }

  // --- Fuerza bruta 2 bytes sobre archivo ---
  void bruteForce2ByteFile(const std::string& inPath,
    const std::string& outDir) const
  {
    auto buf = readFile(inPath);
    fs::create_directories(outDir);
    for (int b1 = 0; b1 < 256; ++b1) {
      for (int b2 = 0; b2 < 256; ++b2) {
        std::string decoded;
        decoded.reserve(buf.size());
        for (size_t i = 0; i < buf.size(); ++i) {
          int keyByte = (i % 2 == 0 ? b1 : b2);
          decoded += static_cast<char>(buf[i] ^ keyByte);
        }
        if (isValidText(decoded)) {
          std::ostringstream fname;
          fname << outDir << "/xor2b_0x"
            << std::hex << std::setw(2) << std::setfill('0') << b1
            << "_0x" << std::setw(2) << b2 << ".bin";
          writeFile(fname.str(),
            std::vector<unsigned char>(decoded.begin(), decoded.end()));
          std::cout << "Guardado: " << fname.str() << "\n";
        }
      }
    }
  }

  // --- Fuerza bruta con diccionario sobre archivo ---
  void bruteForceDictionaryFile(const std::string& inPath,
    const std::string& outDir) const
  {
    static const std::vector<std::string> comunes = {
        "clave","admin","1234","root","test","abc","hola",
        "user","pass","12345","0000","password","default"
    };
    auto buf = readFile(inPath);
    fs::create_directories(outDir);
    for (auto& key : comunes) {
      std::string decoded;
      decoded.reserve(buf.size());
      for (size_t i = 0; i < buf.size(); ++i) {
        decoded += static_cast<char>(buf[i] ^ key[i % key.size()]);
      }
      if (isValidText(decoded)) {
        std::ostringstream fname;
        fname << outDir << "/xor_dict_" << key << ".bin";
        writeFile(fname.str(),
          std::vector<unsigned char>(decoded.begin(), decoded.end()));
        std::cout << "Guardado: " << fname.str()
          << "  (key='" << key << "')\n";
      }
    }
  }

private:
  // --- Lectura / escritura binaria ---
  static std::vector<unsigned char> readFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) throw std::runtime_error("No se pudo abrir: " + path);
    std::vector<unsigned char> buf((std::istreambuf_iterator<char>(in)),
      std::istreambuf_iterator<char>());
    return buf;
  }

  static void writeFile(const std::string& path,
    const std::vector<unsigned char>& buf)
  {
    std::ofstream out(path, std::ios::binary);
    if (!out) throw std::runtime_error("No se pudo escribir: " + path);
    out.write(reinterpret_cast<const char*>(buf.data()), buf.size());
  }

  // --- Valida texto legible ASCII ---
  bool isValidText(const std::string& data) const {
    return std::all_of(data.begin(), data.end(), [](unsigned char c) {
      return std::isprint(c) || std::isspace(c) || c == '\n' || c == '\r';
      });
  }
};