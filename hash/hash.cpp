#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <CommonCrypto/CommonDigest.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Использование: " << argv[0] << " <файл>\n";
        std::cerr << "Пример: " << argv[0] << " test.txt\n";
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary);
    if (!file) {
        std::cerr << "Ошибка: не удалось открыть файл\n";
        return 1;
    }

    CC_SHA256_CTX ctx;
    CC_SHA256_Init(&ctx);

    std::vector<char> buffer(4096);
    while (file.read(buffer.data(), buffer.size()) || file.gcount() > 0) {
        CC_SHA256_Update(&ctx, buffer.data(), (CC_LONG)file.gcount());
    }

    unsigned char digest[CC_SHA256_DIGEST_LENGTH];
    CC_SHA256_Final(digest, &ctx);

    std::ostringstream oss;
    for (unsigned char b : digest) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)b;
    }

    std::cout << oss.str() << std::endl;
    return 0;
}