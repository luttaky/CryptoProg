#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <CommonCrypto/CommonCryptor.h>
#include <CommonCrypto/CommonKeyDerivation.h>

void printUsage(const char* prog) {
    std::cerr << "Использование: " << prog
               << " -m encrypt|decrypt -i <входной файл> -o <выходной файл> -p <пароль>\n";
    std::cerr << "Пример: " << prog
               << " -m encrypt -i secret.txt -o secret.enc -p МойПароль123\n";
}

int main(int argc, char* argv[]) {
    std::string mode, inputFile, outputFile, password;

    for (int i = 1; i < argc - 1; ++i) {
        std::string arg = argv[i];
        if (arg == "-m") mode = argv[++i];
        else if (arg == "-i") inputFile = argv[++i];
        else if (arg == "-o") outputFile = argv[++i];
        else if (arg == "-p") password = argv[++i];
    }

    if (mode.empty() || inputFile.empty() || outputFile.empty() || password.empty()) {
        printUsage(argv[0]);
        return 1;
    }
    if (mode != "encrypt" && mode != "decrypt") {
        printUsage(argv[0]);
        return 1;
    }

    // Читаем входной файл целиком
    std::ifstream in(inputFile, std::ios::binary);
    if (!in) {
        std::cerr << "Ошибка: не удалось открыть входной файл\n";
        return 1;
    }
    std::vector<uint8_t> inData((std::istreambuf_iterator<char>(in)),
                                  std::istreambuf_iterator<char>());
    in.close();

    // Фиксированная соль для детерминированной выработки ключа и IV из пароля
    const uint8_t salt[] = { 0x43, 0x72, 0x79, 0x70, 0x74, 0x6f, 0x50, 0x72, 0x6f, 0x67 };

    // Выработка ключа (32 байта, AES-256) и вектора инициализации (16 байт) из пароля через PBKDF2
    const size_t keyLen = kCCKeySizeAES256;
    const size_t ivLen = kCCBlockSizeAES128;
    std::vector<uint8_t> derived(keyLen + ivLen);

    int kdfStatus = CCKeyDerivationPBKDF(
        kCCPBKDF2,
        password.c_str(), password.size(),
        salt, sizeof(salt),
        kCCPRFHmacAlgSHA256,
        10000,
        derived.data(), derived.size()
    );
    if (kdfStatus != 0) {
        std::cerr << "Ошибка: не удалось выработать ключ из пароля\n";
        return 1;
    }

    uint8_t key[keyLen];
    uint8_t iv[ivLen];
    std::memcpy(key, derived.data(), keyLen);
    std::memcpy(iv, derived.data() + keyLen, ivLen);

    // Буфер под результат (с запасом на блок для PKCS7-дополнения)
    std::vector<uint8_t> outData(inData.size() + kCCBlockSizeAES128);
    size_t outMoved = 0;

    CCOperation op = (mode == "encrypt") ? kCCEncrypt : kCCDecrypt;

    CCCryptorStatus status = CCCrypt(
        op,
        kCCAlgorithmAES,
        kCCOptionPKCS7Padding,   // без kCCOptionECBMode -> режим CBC
        key, keyLen,
        iv,
        inData.data(), inData.size(),
        outData.data(), outData.size(),
        &outMoved
    );

    if (status != kCCSuccess) {
        std::cerr << "Ошибка: операция шифрования/расшифрования не выполнена (код " << status << ")\n";
        return 1;
    }

    std::ofstream out(outputFile, std::ios::binary);
    if (!out) {
        std::cerr << "Ошибка: не удалось открыть выходной файл\n";
        return 1;
    }
    out.write(reinterpret_cast<char*>(outData.data()), outMoved);
    out.close();

    if (mode == "encrypt")
        std::cout << "Файл успешно зашифрован: " << outputFile << std::endl;
    else
        std::cout << "Файл успешно расшифрован: " << outputFile << std::endl;

    return 0;
}