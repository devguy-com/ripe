//
//  Ripe tool
//
//  Copyright © 2017 Muflihun.com. All rights reserved.
//

#include <cstring>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "include/RipeCrypto.h"
#include "include/Ripe.h"

void displayUsage()
{
    std::cout << "ripe [-d | -e | -g] [--in <input_file_path>] [--key <key>] [--in-key <file_path>] [--out-public <output_file_path>] [--out-private <output_file_path>] [--iv <init vector>] [--base64] [--rsa] [--length <key_length>] [--out <output_file_path>] [--clean] [--aes [<key_length>]]" << std::endl;
}

void displayVersion()
{
    std::cout << "Ripe - 256-bit security tool" << std::endl << "Version: " << RIPE_VERSION << std::endl << "http://muflihun.com" << std::endl;
}

void encryptAES(std::string& data, const std::string& key, const std::string& clientId, const std::string& outputFile)
{
    std::cout << Ripe::encryptAES(data, key, clientId, outputFile);
}

void decryptAES(std::string& data, const std::string& key, std::string& iv, bool isBase64)
{
    std::cout << Ripe::decryptAES(data, key, iv, isBase64);
}

void generateAESKey(int length)
{
    if (length == 0 || length == 2048) {
        std::cout << "ERROR: Please provide valid key length" << std::endl;
        return;
    }
    std::cout << RipeCrypto::generateNewKey(length / 8);
}

void encodeBase64(std::string& data)
{
    std::cout << Ripe::base64Encode(data);
}

void decodeBase64(std::string& data)
{
    std::cout << Ripe::base64Decode(data);
}

void encryptRSA(std::string& data, const std::string& key, const std::string& outputFile, std::size_t length)
{
    std::cout << Ripe::encryptRSA(data, key, outputFile, length);
}

void decryptRSA(std::string& data, const std::string& key, bool isBase64, std::size_t length)
{
    std::cout << Ripe::decryptRSA(data, key, isBase64, length);
}

void writeRSAKeyPair(const std::string& publicFile, const std::string& privateFile, std::size_t length)
{
    Ripe::writeRSAKeyPair(publicFile, privateFile, length);
}

void generateRSAKeyPair(std::size_t length)
{
    std::cout << Ripe::generateRSAKeyPair(length);
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        displayUsage();
        return 1;
    }

    if (strcmp(argv[1], "--version") == 0) {
        displayVersion();
        return 0;
    }

    // This is quick check for args, use getopt in future
    int type = -1; // Decryption or encryption

    std::string key;
    std::string publicKeyFile;
    std::string privateKeyFile;
    std::string iv;
    int keyLength = 2048;
    std::string data;
    std::string clientId;
    bool isAES = false;
    bool isBase64 = false;
    bool clean = false;
    bool isRSA = false;
    std::string outputFile;
    bool fileArgSpecified = false;

    for (int i = 0; i < argc; i++) {
        std::string arg(argv[i]);
        bool hasNext = i + 1 < argc;
        if (arg == "-d" && type == -1) {
            type = 1;
        } else if (arg == "-e" && type == -1) {
            type = 2;
        } else if (arg == "-g" && type == -1) {
            type = 3;
        } else if (arg == "--base64") {
            isBase64 = true;
        } else if (arg == "--rsa") {
            isRSA = true;
        } else if (arg == "--key" && hasNext) {
            key = argv[++i];
        } else if (arg == "--aes") {
            isAES = true;
            if (i + 1 < argc) {
                int k = atoi(argv[++i]);
                if (k > 0) {
                    keyLength = k;
                } else {
                    --i;
                }
            }
        } else if (arg == "--length" && hasNext) {
            keyLength = atoi(argv[++i]);
        } else if (arg == "--clean") {
            clean = true;
        } else if (arg == "--out-public" && hasNext) {
            publicKeyFile = argv[++i];
        } else if (arg == "--out-private" && hasNext) {
            privateKeyFile = argv[++i];
        } else if (arg == "--in-key" && hasNext) {
            std::fstream fs;
            // Do not increment i here as we are only changing 'data'
            fs.open (argv[i + 1], std::fstream::binary | std::fstream::in);
            key = std::string((std::istreambuf_iterator<char>(fs)),
                            (std::istreambuf_iterator<char>()));
            fs.close();
        } else if (arg == "--out" && hasNext) {
            outputFile = argv[++i];
        } else if (arg == "--iv" && hasNext) {
            iv = argv[++i];
        } else if (arg == "--client-id" && hasNext) {
            clientId = argv[++i];
        } else if (arg == "--in" && hasNext) {
            fileArgSpecified = true;
            std::fstream fs;
            // Do not increment i here as we are only changing 'data'
            fs.open (argv[i + 1], std::fstream::binary | std::fstream::in);
            data = std::string((std::istreambuf_iterator<char>(fs) ),
                            (std::istreambuf_iterator<char>()));
            fs.close();
        }
    }

    if ((type == 1 || type == 2) && !fileArgSpecified) {
        std::stringstream ss;
        for (std::string line; std::getline(std::cin, line);) {
            ss << line << std::endl;
        }
        data = ss.str();
        // Remove last 'new line'
        data.erase(data.size() - 1);
    }

    if (isBase64 && clean) {
        data.erase(0, data.find_first_of(':') + 1);
    }
    if (type == 1) { // Decrypt / Decode
        if (isBase64 && key.empty() && iv.empty()) {
            // base64 decode
            decodeBase64(data);
        } else if (isRSA) {
            // RSA decrypt (base64-flexiable)
            decryptRSA(data, key, isBase64, keyLength);
        } else {
            // AES decrypt (base64-flexible)
            decryptAES(data, key, iv, isBase64);
        }
    } else if (type == 2) { // Encrypt / Encode
        if (isBase64 && key.empty() && iv.empty()) {
            encodeBase64(data);
        } else if (isRSA) {
            encryptRSA(data, key, outputFile, keyLength);
        } else {
            encryptAES(data, key, clientId, outputFile);
        }
    } else if (type == 3) { // Generate
        if (isRSA) {
            if (publicKeyFile.empty() && privateKeyFile.empty()) {
                generateRSAKeyPair(keyLength);
            } else if (publicKeyFile.empty() || privateKeyFile.empty()) {
                std::cout << "ERROR: Please provide both private and public key files [out-public] and [out-private]" << std::endl;
            } else {
                writeRSAKeyPair(publicKeyFile, privateKeyFile, keyLength);
            }
        } else if (isAES) {
            generateAESKey(keyLength);
        } else {
            std::cout << "ERROR: Please provide method (you probably forgot '--rsa' or '--aes')" << std::endl;
        }
    } else {
        displayUsage();
        return 1;
    }
    return 0;
}
