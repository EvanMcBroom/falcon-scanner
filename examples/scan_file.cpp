#include <Windows.h>
#include <libfalcon.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

template<typename Function>
inline auto LazyLoad(HMODULE library, const std::string& procName) {
    return (library) ? reinterpret_cast<Function*>(GetProcAddress(library, procName.data())) : nullptr;
}
#define LAZY_LOAD_PROC(LIBRARY, PROC) \
    auto Lazy##PROC{ LazyLoad<decltype(PROC)>(LIBRARY, #PROC) };

std::string ReadFile(std::filesystem::path path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    const auto size{ std::filesystem::file_size(path) };
    std::string result(size, '\0');
    file.read(result.data(), size);
    return result;
}

void ScanFile(HMODULE servlet, std::string& sample) {
    LAZY_LOAD_PROC(servlet, CsBind);
    LAZY_LOAD_PROC(servlet, CsEntry);
    LAZY_LOAD_PROC(servlet, CsUnbind);

    XPlatform platform;
    XMessageService messageService;
    XPlatformInformationService platformInformationService;
    XPlatformManagementService platformManagementService;
    platform.iXMessageService = &messageService;
    platform.iXPlatformInformationService = &platformInformationService;
    platform.iXPlatformManagementService = &platformManagementService;
    auto status{ LazyCsBind(&platform) };
    std::cout << "Bind: " << status << std::endl;

    status = LazyCsEntry((uint32_t)CsEntryArg1::Unknown1, 0, nullptr, nullptr);
    std::cout << "Entry: " << status << std::endl;
    status = LazyCsEntry((uint32_t)CsEntryArg1::Unknown2, 0, nullptr, nullptr);
    std::cout << "Entry: " << status << std::endl;
    status = LazyCsEntry((uint32_t)CsEntryArg1::Unknown3, 0, nullptr, nullptr);
    std::cout << "Entry: " << status << std::endl;

    status = LazyCsUnbind();
    std::cout << "Unbind: " << status << std::endl;
}

int wmain(int argc, wchar_t** argv) {
    if (argc > 2) {
        auto servlet{ LoadLibraryW(argv[1]) };
        if (servlet) {
            std::string sample;
            try {
                sample = ReadFile(argv[2]);
                std::wcout << L"File -> " << argv[2] << std::endl;
            } catch (...) {
                std::cout << "Could not read sample file." << std::endl;
            }
            ScanFile(servlet, sample);
            FreeLibrary(servlet);
        }
    } else {
        std::wcout << argv[0] << L" <path to falcon servlet> <path to file>" << std::endl;
    }
}