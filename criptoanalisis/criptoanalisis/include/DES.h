#pragma once
#include "Prerequisites.h"

class DES {
public:
  // --- Constructores y generación de subclaves ---
  DES() = default;
  DES(const std::bitset<64>& key_) : key(key_) {
    generateSubkeys();
  }
  ~DES() = default;

  void setKey(const std::bitset<64>& key_) {
    key = key_;
    subkeys.clear();
    generateSubkeys();
  }

  // --- Bloque de 64 bits (1 bloque) ---
  std::bitset<64> 
  encodeBlock(const std::bitset<64>& plaintext) {
    auto data = iPermutation(plaintext);
    auto left = std::bitset<32>((data.to_ullong() >> 32));
    auto right = std::bitset<32>(data.to_ullong());

    for (int round = 0; round < 16; ++round) {
      auto f = feistel(right, subkeys[round]);
      auto newR = left ^ f;
      left = right;
      right = newR;
    }
    uint64_t combined = (uint64_t(right.to_ullong()) << 32) | left.to_ullong();
    return fPermutation(std::bitset<64>(combined));
  }

  std::bitset<64> 
  decodeBlock(const std::bitset<64>& ciphertext) {
    auto data = iPermutation(ciphertext);
    auto left = std::bitset<32>((data.to_ullong() >> 32));
    auto right = std::bitset<32>(data.to_ullong());

    for (int round = 15; round >= 0; --round) {
      auto f = feistel(right, subkeys[round]);
      auto newR = left ^ f;
      left = right;
      right = newR;
    }
    uint64_t combined = (uint64_t(right.to_ullong()) << 32) | left.to_ullong();
    return fPermutation(std::bitset<64>(combined));
  }

  // --- Encriptar / Decriptar archivos ---
  void 
  encryptFile(const std::string& inPath,
    const std::string& outPath)
  {
    processFile(inPath, outPath, /*encrypt=*/true);
  }

  void 
  decryptFile(const std::string& inPath,
    const std::string& outPath)
  {
    processFile(inPath, outPath, /*encrypt=*/false);
  }

  // --- Brute force de demostración ---
  // 1) Sobre un solo bloque conocido: recorre hasta maxKeys y retorna las claves que coinciden
  std::vector<uint64_t> 
  bruteForceKnownPlaintextBlock(
    const std::bitset<64>& cipherBlock,
    const std::bitset<64>& knownPlain,
    uint64_t maxKeys = (1ULL << 20)  // por defecto: 2^20 claves
  ) {
    std::vector<uint64_t> found;
    for (uint64_t k = 0; k < maxKeys; ++k) {
      setKey(std::bitset<64>(k));
      if (decodeBlock(cipherBlock) == knownPlain) {
        found.push_back(k);
      }
    }
    return found;
  }

  // 2) Sobre un archivo con bloque conocido al inicio:
  //    genera archivos descifrados con cada clave hasta maxKeys
  void 
  bruteForceFile(const std::string& inPath,
    const std::string& outDir,
    uint64_t maxKeys = (1ULL << 20))
  {
    // leer todo el archivo
    auto data = readFile(inPath);
    // extraer primer bloque de 8 bytes
    std::bitset<64> cipherBlock = readBlock(data, 0);

    for (uint64_t k = 0; k < maxKeys; ++k) {
      setKey(std::bitset<64>(k));
      // si el primer bloque descifra a algo legible (aquí sin chequear),
      // de todas formas guardamos el intento:
      std::string out = processBuffer(data, /*encrypt=*/false);
      std::ostringstream oss;
      oss << outDir << "/decrypted_" << k << ".bin";
      writeFile(oss.str(), out);
    }
  }

private:
  std::bitset<64> key;
  std::vector<std::bitset<48>> subkeys;

  // --- Tablas (simplificadas en tu código original) ---
  const int EXPANSION_TABLE[48] = { /* … tu tabla … */ };
  const int P_TABLE[32] = { /* … tu tabla … */ };
  const int SBOX[4][16] = { /* … tu SBOX … */ };

  void 
  generateSubkeys() {
    for (int i = 0; i < 16; ++i) {
      std::bitset<48> sub((key.to_ullong() >> i) & 0xFFFFFFFFFFFF);
      subkeys.push_back(sub);
    }
  }

  std::bitset<64> 
  iPermutation(const std::bitset<64>& in) {
    // identidad en tu ejemplo
    return in;
  }
  std::bitset<64> 
  fPermutation(const std::bitset<64>& in) {
    // identidad en tu ejemplo
    return in;
  }
  std::bitset<48> 
  expand(const std::bitset<32>& half) {
    std::bitset<48> out;
    for (int i = 0; i < 48; ++i)
      out[i] = half[32 - EXPANSION_TABLE[i]];
    return out;
  }
  std::bitset<32> 
  substitute(const std::bitset<48>& in) {
    std::bitset<32> out;
    for (int i = 0; i < 8; ++i) {
      int row = (in[i * 6] << 1) | in[i * 6 + 5];
      int col = (in[i * 6 + 1] << 3) | (in[i * 6 + 2] << 2) | (in[i * 6 + 3] << 1) | in[i * 6 + 4];
      int val = SBOX[row % 4][col % 16];
      for (int j = 0; j < 4; ++j)
        out[i * 4 + j] = (val >> (3 - j)) & 1;
    }
    return out;
  }
  std::bitset<32> 
  permuteP(const std::bitset<32>& in) {
    std::bitset<32> out;
    for (int i = 0; i < 32; ++i)
      out[i] = in[32 - P_TABLE[i]];
    return out;
  }
  std::bitset<32> 
  feistel(const std::bitset<32>& r,
    const std::bitset<48>& k)
  {
    return permuteP(substitute(expand(r) ^ k));
  }

  // --- I/O de archivos y padding cero ---
  std::string 
  readFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) throw std::runtime_error("No se pudo abrir: " + path);
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
  }

  void 
  writeFile(const std::string& path, const std::string& data) {
    std::ofstream out(path, std::ios::binary);
    if (!out) throw std::runtime_error("No se pudo escribir: " + path);
    out.write(data.data(), data.size());
  }

  // Procesa el buffer completo (pad con ceros hasta múltiplo de 8)
  void 
  processFile(const std::string& inPath,
    const std::string& outPath,
    bool encrypt)
  {
    auto buf = readFile(inPath);
    auto out = processBuffer(buf, encrypt);
    writeFile(outPath, out);
  }

  std::string 
  processBuffer(const std::string& buf, bool encrypt) {
    std::string result;
    size_t n = buf.size();
    size_t padded = ((n + 7) / 8) * 8;
    result.resize(padded, '\0');

    // copiar original y zeros
    std::memcpy(result.data(), buf.data(), n);

    // por cada bloque de 8 bytes
    for (size_t offset = 0; offset < padded; offset += 8) {
      auto block = readBlock(result, offset);
      auto outBlk = encrypt
        ? encodeBlock(block)
        : decodeBlock(block);
      writeBlock(result, offset, outBlk);
    }
    return result;
  }

  std::bitset<64> 
  readBlock(const std::string& buf, size_t off) {
    uint64_t val = 0;
    for (int i = 0; i < 8; ++i)
      val |= uint64_t(uint8_t(buf[off + i])) << ((7 - i) * 8);
    return std::bitset<64>(val);
  }
  void 
  writeBlock(std::string& buf, size_t off,
    const std::bitset<64>& bits)
  {
    uint64_t val = bits.to_ullong();
    for (int i = 0; i < 8; ++i)
      buf[off + i] = char((val >> ((7 - i) * 8)) & 0xFF);
  }
};


/*
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

*/