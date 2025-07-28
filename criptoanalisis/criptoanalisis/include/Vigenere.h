#pragma once
#include "Prerequisites.h"


class Vigenere {
public:
  Vigenere() = default;

  Vigenere(const std::string& rawKey)
    : key(normalizeKey(rawKey))
  {
    if (key.empty()) {
      throw std::invalid_argument("La clave no puede estar vacía o sin letras.");
    }
  }

  // --- Normalización de la clave: solo letras mayúsculas ---
  static std::string 
  normalizeKey(const std::string& rawKey) {
    std::string k;
    for (char c : rawKey) {
      if (std::isalpha(static_cast<unsigned char>(c))) {
        k += std::toupper(static_cast<unsigned char>(c));
      }
    }
    return k;
  }

  // --- Encriptar/desencriptar texto en memoria ---
  std::string 
  encode(const std::string& text) const {
    return transform(text, /*encode=*/true);
  }

  std::string 
  decode(const std::string& text) const {
    return transform(text, /*encode=*/false);
  }

  // --- I/O de archivos ---
  void 
  encryptFile(const std::string& inputPath,
    const std::string& outputPath) const
  {
    std::string plain = readFile(inputPath);
    std::string cipher = encode(plain);
    writeFile(outputPath, cipher);
  }

  void 
  decryptFile(const std::string& inputPath,
    const std::string& outputPath) const
  {
    std::string cipher = readFile(inputPath);
    std::string plain = decode(cipher);
    writeFile(outputPath, plain);
  }

  // --- Fuerza bruta para texto en memoria ---
  // Devuelve la mejor clave encontrada hasta maxKeyLength
  static std::string 
  breakEncode(const std::string& text,
    int maxKeyLength)
  {
    double bestScore = -std::numeric_limits<double>::infinity();
    std::string bestKey, trailKey, bestPlain;

    auto dfs = [&](auto&& self, int pos, int len) -> void {
      if (pos == len) {
        Vigenere v(trailKey);
        std::string candidate = v.decode(text);
        double score = fitness(candidate);
        if (score > bestScore) {
          bestScore = score;
          bestKey = trailKey;
          bestPlain = candidate;
        }
        return;
      }
      for (char c = 'A'; c <= 'Z'; ++c) {
        trailKey[pos] = c;
        self(self, pos + 1, len);
      }
      };

    for (int L = 1; L <= maxKeyLength; ++L) {
      trailKey.assign(L, 'A');
      dfs(dfs, 0, L);
    }

    std::cout << "*** Vigenère Brute-Force ***\n"
      << "Clave encontrada:   " << bestKey << "\n"
      << "Texto descifrado:   " << bestPlain << "\n\n";

    return bestKey;
  }

  // --- Fuerza bruta aplicada a archivo ---
  // Lee todo el archivo, lo descifra con cada clave hasta maxKeyLength,
  // imprime en consola los N mejores candidatos según fitness.
  static void 
  breakFile(const std::string& inputPath,
    int maxKeyLength,
    int topCandidates = 5)
  {
    std::string cipher = readFileStatic(inputPath);
    struct Candidate { std::string key, plain; double score; };
    std::vector<Candidate> results;

    auto dfs = [&](auto&& self, int pos, int len, std::string& trailKey) -> void {
      if (pos == len) {
        Vigenere v(trailKey);
        std::string candPlain = v.decode(cipher);
        double scr = fitness(candPlain);
        results.push_back({ trailKey, candPlain, scr });
        return;
      }
      for (char c = 'A'; c <= 'Z'; ++c) {
        trailKey[pos] = c;
        self(self, pos + 1, len, trailKey);
      }
      };

    for (int L = 1; L <= maxKeyLength; ++L) {
      std::string trailKey(L, 'A');
      dfs(dfs, 0, L, trailKey);
    }

    // Ordenamos y mostramos los N mejores
    std::sort(results.begin(), results.end(),
      [](auto& a, auto& b) { return a.score > b.score; });
    std::cout << "=== Top " << topCandidates << " claves ===\n";
    for (int i = 0; i < std::min((int)results.size(), topCandidates); ++i) {
      std::cout << i + 1 << ") Clave: " << results[i].key
        << " | Score: " << results[i].score << "\n"
        << "   Texto: " << results[i].plain << "\n\n";
    }
  }

private:
  std::string key;

  // --- Transformación común ---
  std::string 
  transform(const std::string& text, bool encode) const {
    std::string res;
    res.reserve(text.size());
    unsigned int ki = 0;

    for (char c : text) {
      if (std::isalpha(static_cast<unsigned char>(c))) {
        bool isLower = std::islower(static_cast<unsigned char>(c));
        char base = isLower ? 'a' : 'A';
        int  shift = key[ki % key.size()] - 'A';
        int  dir = encode ? +1 : -1;
        char out = char(((c - base) + dir * shift + 26 * shift) % 26 + base);
        res += out;
        ++ki;
      }
      else {
        res += c;
      }
    }
    return res;
  }

  // --- Fitness (mismo de tu versión) ---
  static double 
  fitness(const std::string& text) {
    static const std::vector<std::string> comunes = {
        " DE ", " LA ", " EL ", " QUE ", " Y ",
        " A ", " EN ", " UN ", " PARA ", " CON ",
        " POR ", " COMO ", " SU ", " AL ", " DEL ",
        " LOS ", " SE ", " NO ", " MAS ", " O ",
        " SI ", " YA ", " TODO ", " ESTA ", " HAY ",
        " ESTO ", " SON ", " TIENE ", " HACE ", " SUS ",
        " VIDA ", " NOS ", " TE ", " LO ", " ME ",
        " ESTE ", " ESA ", " ESE ", " BIEN ", " MUY ",
        " PUEDE ", " TAMBIEN ", " AUN ", " MI ", " DOS ",
        " UNO ", " OTRO ", " NUEVO ", " SIN ", " ENTRE ",
        " SOBRE "
    };
    double score = 0;
    for (auto& w : comunes) {
      size_t pos = 0;
      while ((pos = text.find(w, pos)) != std::string::npos) {
        score += w.length();
        pos += w.length();
      }
    }
    return score;
  }

  // --- Lectura/escritura de archivos ---
  static std::string 
  readFileStatic(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) throw std::runtime_error("No se pudo abrir: " + path);
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
  }

  std::string 
  readFile(const std::string& path) const {
    return readFileStatic(path);
  }

  void 
  writeFile(const std::string& path, const std::string& data) const {
    std::ofstream out(path, std::ios::binary);
    if (!out) throw std::runtime_error("No se pudo escribir: " + path);
    out << data;
  }
};


/*
#include "Prerequisites.h"
#include "Vigenere.h"

int main() {
	std::string text = "Hola este mensaje otorga una decima";
	std::string key = "RobertoCharreton00";

	std::cout << "Texto original: " << text << std::endl;
	std::cout << "Clave: " << key << std::endl;

	Vigenere vigenere(key);
	std::string encrypted = vigenere.encode(text);
	std::cout << "Texto cifrado: " << encrypted << std::endl;

	std::string decrypted = vigenere.decode(encrypted);
	std::cout << "Texto descifrado: " << decrypted << std::endl;


	return 0;
}

*/