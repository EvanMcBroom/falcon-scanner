#include <windows.h>

#include "libfalcon.hpp"
#include "messages.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <winternl.h>

using namespace Aviary;

template<typename Function>
inline auto LazyLoad(HMODULE library, const std::string& procName) {
    return (library) ? reinterpret_cast<Function*>(GetProcAddress(library, procName.data())) : nullptr;
}
#define LAZY_LOAD_PROC(LIBRARY, PROC) \
    auto Lazy##PROC{ LazyLoad<decltype(PROC)>(LIBRARY, #PROC) };

void ScanFile(HMODULE servlet, HANDLE sampleFileMapping, DWORD sampleSize) {
    LAZY_LOAD_PROC(servlet, CsBind);
    LAZY_LOAD_PROC(servlet, CsEntry);
    LAZY_LOAD_PROC(servlet, CsUnbind);
    if (LazyCsBind && LazyCsEntry && LazyCsUnbind) {
        XPlatform platform;
        XMessageService messageService;
        XPlatformInformationService platformInformationService;
        XPlatformManagementService platformManagementService;
        platform.iXMessageService = &messageService;
        platform.iXPlatformInformationService = &platformInformationService;
        platform.iXPlatformManagementService = &platformManagementService;
        XMessageIoContext messageIoContext;
        messageService.callbacks.push_back([&](IXMessage* message) -> void {
            if (*message->GetIid() == IID_IRuleEngineLoadRulesResponse) {
                auto response{ reinterpret_cast<RULE_ENGINE_LOAD_RULES_RESPONSE_DATA*>(message->GetData()) };
            } else if (*message->GetIid() == __uuidof(IStaticAnalysisResponse)) {
                auto response{ reinterpret_cast<STATIC_ANALYSIS_RESPONSE_DATA*>(message->GetData()) };
                std::cout << "Static analysis response!" << std::endl;
                std::cout << "  Response Hash: ";
                std::stringstream stream;
                stream << std::setw(2) << std::setfill('0') << std::hex;
                for (const auto& item : response->ResponseFileHash) {
                    stream << static_cast<unsigned>(item);
                }
                std::cout << stream.str() << std::endl;
                std::cout << "  Verdict      : " << std::to_string(response->Verdict) << std::endl;
                std::cout << "  Subverdict   : " << std::to_string(response->SubVerdict) << std::endl;
                std::cout << "  Score        : " << std::to_string(response->Score) << std::endl;
            }
        });
        HRESULT result;
        if (SUCCEEDED(result = LazyCsBind(&platform))) {
            if (SUCCEEDED(result = LazyCsEntry(1)) || result == HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED)) {
                if (SUCCEEDED(result = LazyCsEntry(2))) {
                    for (auto const& [key, value] : messageService.messageHandlers) {
                        value->messageHandler->AddRef();
                        auto message{ TXMESSAGE(StaticAnalysisRequest, STATIC_ANALYSIS_REQUEST_DATA) };
                        message.data.Flags = 0x41;
                        message.data.TargetFileMapping = sampleFileMapping;
                        message.data.TargetFileSize = sampleSize;
                        value->messageHandler->Request(&message, &messageIoContext);
                        std::cout << std::endl
                                  << "Scan completed!" << std::endl;
                        value->messageHandler->Release();
                    }
                    if (FAILED(result = LazyCsEntry(3))) {
                        printf("CsEntry(3) failed with error code: %08x\n", result);
                    }
                    if (FAILED(result = LazyCsUnbind())) {
                        printf("CsUnbind failed with error code: %08x\n", result);
                    }
                } else {
                    printf("CsEntry(2) failed with error code: %08x\n", result);
                }
            } else {
                printf("CsEntry(1) failed with error code: %08x\n", result);
            }
        } else {
            printf("CsBind failed with error code: 0x%08x\n", result);
        }
    } else {
        std::wcerr << L"Could not resolve all required servlet exports." << std::endl;
    }
}

int wmain(int argc, wchar_t** argv) {
    if (argc > 2) {
        auto servlet{ LoadLibraryW(argv[1]) };
        if (servlet) {
            auto sampleFile{ CreateFileW(argv[2], FILE_GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr) };
            if (sampleFile != INVALID_HANDLE_VALUE) {
                std::wcout << L"File -> " << argv[2] << std::endl
                           << std::endl;
                auto sampleSize{ GetFileSize(sampleFile, 0) };
                auto sampleFileMapping{ CreateFileMappingW(sampleFile, nullptr, PAGE_READONLY, 0, sampleSize, nullptr) };
                if (sampleFileMapping && sampleFileMapping != INVALID_HANDLE_VALUE) {
                    ScanFile(servlet, sampleFileMapping, sampleSize);
                    // It appears the the falcon servlet takes ownership of the mapping
                    // and will close it after the scan completes. The handle becomes
                    // invalid and attempting to close it again would result in an error.
                    // CloseHandle(sampleFileMapping);
                } else {
                    std::wcerr << L"Could not map the sample file." << std::endl;
                }
                CloseHandle(sampleFile);
            } else {
                std::wcerr << L"Could not open the sample file." << std::endl;
            }
            FreeLibrary(servlet);
        } else {
            std::wcerr << L"Could not load the falcon CsMl servlet." << std::endl;
        }
    } else {
        std::wcout << argv[0] << L" <path to a falcon CsMl servlet> <path to file>" << std::endl;
    }
}