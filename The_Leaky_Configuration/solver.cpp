/*
Author : Cyber Assassin
*/
#include <iostream>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <cstdint>

/**
 * Performs a left rotation on a 32-bit word.
 */
uint32_t rotl(uint32_t x, uint32_t y) {
    return (x << (y & 31)) | (x >> (32 - (y & 31)));
}

/**
 * Performs a right rotation on a 32-bit word.
 */
uint32_t rotr(uint32_t x, uint32_t y) {
    return (x >> (y & 31)) | (x << (32 - (y & 31)));
}

/**
 * The key expansion function for RC5.
 */
void rc5_setup(const unsigned char* key, uint32_t* S) {
    const int r = 12; // Number of rounds
    const int b = 16; // Key size in bytes
    const int c = b / 4; // Key size in words
    const int t = 2 * (r + 1); // Size of the subkey array S
    uint32_t L[c] = {0};

    for (int i = b - 1; i >= 0; i--) {
        L[i / 4] = (L[i / 4] << 8) + key[i];
    }

    S[0] = 0xB7E15163; // P32 constant
    for (int i = 1; i < t; i++) {
        S[i] = S[i - 1] + 0x9E3779B9; // Q32 constant
    }

    uint32_t i = 0, j = 0, A = 0, B = 0;
    for (int k = 0; k < 3 * t; k++) {
        A = S[i] = rotl(S[i] + A + B, 3);
        B = L[j] = rotl(L[j] + A + B, A + B);
        i = (i + 1) % t;
        j = (j + 1) % c;
    }
}


void rc5_decrypt_block(const uint32_t* S, uint32_t* block) {
    const int r = 12;
    uint32_t A = block[0];
    uint32_t B = block[1];
    for (int i = r; i >= 1; i--) {
        B = rotr(B - S[2 * i + 1], A) ^ A;
        A = rotr(A - S[2 * i], B) ^ B;
    }
    block[1] = B - S[1];
    block[0] = A - S[0];
}


std::vector<unsigned char> rc5_decrypt(const std::vector<unsigned char>& ciphertext, const unsigned char* key) {
    uint32_t S[26]; // Subkey array for 12 rounds: 2 * (12 + 1) = 26
    rc5_setup(key, S);
    
    std::vector<unsigned char> data = ciphertext;

    // Process the data in 8-byte (64-bit) chunks
    for (size_t i = 0; i < data.size(); i += 8) {
        rc5_decrypt_block(S, reinterpret_cast<uint32_t*>(&data[i]));
    }
    

    if (!data.empty()) {
        char pad_value = data.back();
        if (pad_value > 0 && pad_value <= 8) {
            data.resize(data.size() - pad_value);
        }
    }
    
    return data;
}
std::string apply_repeating_xor(const std::string& input, const std::string& key) {
    if (key.empty()) {
        return "";
    }
    std::string output = input;
    for (size_t i = 0; i < output.length(); ++i) {
        output[i] ^= key[i % key.length()];
    }
    return output;
}
int main() {
    // The keys used for the XOR operations.
    std::string key1 = "EvadeDebuuger";
    std::string key2 = "RuntimeMisslead";
    std::string key3 = "SEHisAwesome";
    
    // The key for the RC5 encryption. Note: It's 16 bytes.
    std::string key4 = "Cyber_Assassin!!";

    // The final target vector that the transformed input must match.
    std::vector<int> target_vector = { 205, 430, 179, 116, 55, 8, 110, 240, 2, 61, 147, 60, 30, 255, 251, 8, 25, 7, 72, 10, 139, 90, 423, 21, 95, 55, 200, 145, 36, 21, 68, 230, 25, 73, 230, 114, 47, 211, 224, 181, 55, 421, 117, 183, 209, 134, 80, 54, 106, 66, 158, 178, 90, 143, 424, 133, 126, 101, 45, 56, 156, 429, 64, 182, 21, 248, 142, 48, 186, 89, 143, 134, 98, 247, 186, 109, 26, 96, 233, 212, 198, 52, 148, 220, 185, 189, 112, 93 };

    // The inverse operation is: (t ^ 0xAA) - 16
    std::vector<unsigned char> rc5_ciphertext;
    for (int val : target_vector) {
        unsigned char transformed_byte = (unsigned char)val;
        unsigned char original_byte = (transformed_byte ^ 0xAA) - 16;
        rc5_ciphertext.push_back(original_byte);
    }

    // Reverse the RC5 Encryption
    std::vector<unsigned char> xor_result_bytes = rc5_decrypt(rc5_ciphertext, (unsigned char*)key4.c_str());
    std::string xor_result_string(xor_result_bytes.begin(), xor_result_bytes.end());

    // Reverse the XOR ciphers in reverse order of application.
    // Original order: XOR key1 -> XOR key2 -> XOR key3
    // Reverse order:  XOR key3 -> XOR key2 -> XOR key1
    std::string temp_flag = apply_repeating_xor(xor_result_string, key3);
    
    temp_flag = apply_repeating_xor(temp_flag, key2);

    std::string final_flag = apply_repeating_xor(temp_flag, key1);
    
    std::cout << "========================================" << std::endl;
    std::cout << "          SOLVED! The flag is:" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << final_flag << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
