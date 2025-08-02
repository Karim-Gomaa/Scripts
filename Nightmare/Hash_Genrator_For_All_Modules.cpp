/*
Author : Cyber Assassin
*/
#include <iostream>
#include <windows.h>
#include <winternl.h>
#include <iomanip>
#include <string>
#include <vector>

namespace Illusionist {

    constexpr uint32_t HASH_SEED = 0x4A53C5A7;
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

    inline uint32_t IllusionistHashA(const char* str) {
        if (!str) return 0;
        uint32_t hash = HASH_SEED;
        while (*str) {
            char c = *str;
            if (c >= 'a' && c <= 'z') c -= 32; // Uppercase
            hash = (hash << 7) + (hash >> 3) + c;
            str++;
        }
        hash = hash ^ 0xDEADBEEF;
        hash = (~hash) + (hash << 21);
        hash = hash ^ (hash >> 12);
        uint8_t byte1 = (hash >> 24) & 0xFF;
        uint8_t byte2 = (hash >> 16) & 0xFF;
        uint8_t byte3 = (hash >> 8)  & 0xFF;
        uint8_t byte4 = (hash) & 0xFF;
        return (S_BOX[byte1] << 24) | (S_BOX[byte2] << 16) | (S_BOX[byte3] << 8) | S_BOX[byte4];
    }
}

void printHash(const std::string& name, uint32_t hash, const std::string& indent = "") {
    std::cout << indent
              << std::left << std::setw(40) << name
              << " -> 0x" << std::hex << std::setfill('0') << std::setw(8) << hash
              << std::dec << std::endl; // Reset to decimal for next std::setw
}

void processModuleExports(const BYTE* fileBuffer, DWORD fileSize) {
    // The file buffer acts as the base address for our calculations.
    
    // Boundary Check: Ensure buffer is large enough for a DOS header
    if (fileSize < sizeof(IMAGE_DOS_HEADER)) return;
    auto dosHeader = reinterpret_cast<const IMAGE_DOS_HEADER*>(fileBuffer);
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        return; // Not a valid PE file
    }

    // Boundary Check: Ensure the NT header offset is within the file bounds
    if (dosHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS) > fileSize) return;
    auto ntHeaders = reinterpret_cast<const IMAGE_NT_HEADERS*>(fileBuffer + dosHeader->e_lfanew);
    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
        return; // Not a valid PE file
    }
    
    // Locate the export directory
    auto exportDirRVA = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    if (exportDirRVA == 0) {
        return; // No exports in this module
    }

    // Boundary Check: Ensure the export directory is within the file bounds
    if (exportDirRVA + sizeof(IMAGE_EXPORT_DIRECTORY) > fileSize) return;
    auto exportDir = reinterpret_cast<const IMAGE_EXPORT_DIRECTORY*>(fileBuffer + exportDirRVA);

    // Boundary Check: Ensure the name table is within file bounds
    if (exportDir->AddressOfNames + (exportDir->NumberOfNames * sizeof(DWORD)) > fileSize) return;
    const DWORD* namesRVA = reinterpret_cast<const DWORD*>(fileBuffer + exportDir->AddressOfNames);
    
    // Iterate through all the exported function names
    for (DWORD i = 0; i < exportDir->NumberOfNames; ++i) {
        // Boundary Check: Ensure the specific name RVA is within file bounds
        if (namesRVA[i] > fileSize) continue;
        
        const char* funcName = reinterpret_cast<const char*>(fileBuffer + namesRVA[i]);
        
        // Final sanity check for null terminator within bounds, though less critical
        // This just prevents reading too far if a string is not properly terminated.
        size_t nameOffset = namesRVA[i];
        size_t maxLen = fileSize - nameOffset;
        if (strnlen(funcName, maxLen) >= maxLen) continue; // Malformed name, skip it

        // Calculate and print the hash for the function name
        uint32_t hash = Illusionist::IllusionistHashA(funcName);
        printHash(funcName, hash, "    [API] ");
    }
}

int main() {
    char systemPath[MAX_PATH];
    
    // Get the path to the C:\Windows\System32 directory
    GetSystemDirectoryA(systemPath, MAX_PATH);

    std::string searchPath = std::string(systemPath) + "\\*.dll";
    
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        std::cerr << "Error: Could not find files in the System32 directory. Error code: " << GetLastError() << std::endl;
        return 1;
    }

    std::cout << "--- Traversing System32 Modules and Their Exports ---\n" << std::endl;

    do {
        // Construct the full path to the current DLL
        std::string moduleName = findData.cFileName;
        std::string fullPath = std::string(systemPath) + "\\" + moduleName;

        // Calculate and print the hash for the module name
        uint32_t moduleHash = Illusionist::IllusionistHashA(moduleName.c_str());
        printHash("[Module] " + moduleName, moduleHash);

        // --- Start of safe file reading process ---
        HANDLE hFile = CreateFileA(fullPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            std::cout << "    [!] Could not open file. Error: " << GetLastError() << std::endl;
            continue;
        }

        DWORD fileSize = GetFileSize(hFile, NULL);
        if (fileSize == INVALID_FILE_SIZE) {
             std::cout << "    [!] Could not get file size. Error: " << GetLastError() << std::endl;
            CloseHandle(hFile);
            continue;
        }
        
        // Allocate a buffer to hold the file's contents
        BYTE* fileBuffer = new (std::nothrow) BYTE[fileSize];
        if (!fileBuffer) {
            std::cout << "    [!] Memory allocation failed." << std::endl;
            CloseHandle(hFile);
            continue;
        }

        DWORD bytesRead = 0;
        if (!ReadFile(hFile, fileBuffer, fileSize, &bytesRead, NULL) || bytesRead != fileSize) {
            std::cout << "    [!] Failed to read file. Error: " << GetLastError() << std::endl;
            delete[] fileBuffer;
            CloseHandle(hFile);
            continue;
        }
        
        // We're done with the file handle, close it immediately
        CloseHandle(hFile);

        // Process the exports from the in-memory buffer, passing the size for safety
        processModuleExports(fileBuffer, fileSize);
        
        // Clean up the allocated buffer
        delete[] fileBuffer;
        // --- End of safe file reading process ---

        std::cout << std::endl; // Add a newline for better readability

    } while (FindNextFileA(hFind, &findData) != 0);

    // Clean up the find handle
    FindClose(hFind);
    
    std::cout << "--- Traversal Complete ---" << std::endl;
    return 0;
}
