#define NOMINMAX
#include "libfalcon.hpp"
#include <iomanip>
#include <iostream>
#include <magic_enum.hpp>
#include <stdexcept>
#include <string>

int wmain(int argc, wchar_t** argv) {
    if (argc == 3) {
        try {
            Falcon::Scanner scanner(argv[1]);
            auto file{ CreateFileW(argv[2], FILE_GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr) };
            if (file != INVALID_HANDLE_VALUE) {
                struct FileGuard {
                    HANDLE handle;
                    ~FileGuard() {
                        CloseHandle(handle);
                    }
                } guard{ file };
                auto response{ scanner.ScanFile(file) };
                auto verdictName{ magic_enum::enum_name(static_cast<Aviary::VERDICT>(response.Verdict)) };
                std::cout << "Static analysis:\n  Response Hash: " << std::hex << std::setfill('0');
                for (auto byte : response.ResponseHash) {
                    std::cout << std::setw(2) << static_cast<unsigned>(byte);
                }
                std::cout << std::dec << "\n  Verdict      : " << static_cast<unsigned>(response.Verdict)
                          << " (" << (verdictName.empty() ? "Unknown" : verdictName) << ")"
                          << "\n  Subverdict   : " << static_cast<unsigned>(response.SubVerdict)
                          << "\n  Score        : " << std::setprecision(9) << response.Score
                          << "\n  Status       : " << response.Status
                          << "\n  Hash mismatch: " << static_cast<unsigned>(response.HashMismatch)
                          << "\nScan completed.\n";
                return 0;
            } else {
                throw std::runtime_error("Could not open sample file (Win32 error " + std::to_string(GetLastError()) + ").");
            }
        } catch (const std::exception& error) {
            std::cerr << error.what() << '\n';
            return 1;
        }
    } else {
        std::wcerr << argv[0] << L" <path to CsMl.dll> <path to PE file>\n";
        return 1;
    }
}
