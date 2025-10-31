#define NOMINMAX
#include <windows.h>

#include "pyfalcon.hpp"
#include <codecvt>
#include <iostream>
#include <locale>
#include <pybind11/functional.h>
#include <string>

namespace py = pybind11;
using namespace Aviary;

template<typename Function>
inline auto LazyLoad(HMODULE library, const std::string& procName) {
    return (library) ? reinterpret_cast<Function*>(GetProcAddress(library, procName.data())) : nullptr;
}
#define LAZY_LOAD_PROC(LIBRARY, PROC) \
    (Lazy##PROC = LazyLoad<decltype(PROC)>(reinterpret_cast<HMODULE>(LIBRARY), #PROC))

namespace {
    auto utfConverter{ std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>() };
}

namespace PyFalcon {
    Exception::Exception(const char* message)
        : message(message) {
    }

    const char* Exception::what() const noexcept {
        return (!message.empty()) ? message.data() : "Unknown error.";
    }

    File::File(const std::string& path) {
        std::wstring utf16Path{ utfConverter.from_bytes(path) };
        handle = CreateFileW(utf16Path.data(), FILE_GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (handle == INVALID_HANDLE_VALUE) {
            throw Exception("Failed to load the file.");
        }
    }

    File::~File() {
        CloseHandle(handle);
    }

    Message::Message(IXMessage* message)
        : message(message) {
    }

    std::shared_ptr<StaticAnalysisResponse> Message::GetStaticAnalysisResponse() {
        if (*message->GetIid() == __uuidof(IStaticAnalysisResponse)) {
            auto response{ reinterpret_cast<STATIC_ANALYSIS_RESPONSE_DATA*>(message->GetData()) };
            auto staticAnalysisResponse{ std::make_shared<StaticAnalysisResponse>() };
            staticAnalysisResponse->Verdict = response->Verdict;
            staticAnalysisResponse->SubVerdict = response->SubVerdict;
            staticAnalysisResponse->Score = response->Score;
            return staticAnalysisResponse;
        }
        return nullptr;
    }

    Servlet::Servlet(const std::string& path) {
        std::wstring utf16Path{ utfConverter.from_bytes(path) };
        if (servlet = LoadLibraryW(utf16Path.data())) {
            if (LAZY_LOAD_PROC(servlet, CsBind) &&
                LAZY_LOAD_PROC(servlet, CsEntry)) {
                platform.iXMessageService = &messageService;
                platform.iXPlatformInformationService = &platformInformationService;
                platform.iXPlatformManagementService = &platformManagementService;
                messageService.callbacks.emplace_back([&](IXMessage* ixMessage) -> void {
                    if (logger) {
                        logger(Message{ ixMessage });
                    }
                });
                HRESULT result;
                if (FAILED(LazyCsBind(&platform)) ||
                    (FAILED(result = LazyCsEntry(1)) && result != HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED)) ||
                    FAILED(LazyCsEntry(2))) {
                    throw Exception("Servlet not loaded.");
                }
            }
        }
    }

    Servlet::~Servlet() {
        if (servlet &&
            LAZY_LOAD_PROC(servlet, CsEntry) &&
            LAZY_LOAD_PROC(servlet, CsUnbind)) {
            if (SUCCEEDED(LazyCsEntry(3))) {
                if (SUCCEEDED(LazyCsUnbind())) {
                    FreeLibrary(servlet);
                    servlet = nullptr;
                }
            }
        }
    }

    void Servlet::ScanFile(File& file) {
        auto fileSize{ GetFileSize(file.handle, 0) };
        auto fileMapping{ CreateFileMappingW(file.handle, nullptr, PAGE_READONLY, 0, fileSize, nullptr) };
        if (fileMapping && fileMapping != INVALID_HANDLE_VALUE) {
            for (auto const& [key, value] : messageService.messageHandlers) {
                value->messageHandler->AddRef();
                auto message{ TXMESSAGE(StaticAnalysisRequest, STATIC_ANALYSIS_REQUEST_DATA) };
                message.data.Flags = 0x41;
                message.data.TargetFileMapping = fileMapping;
                message.data.TargetFileSize = fileSize;
                value->messageHandler->Request(&message, &messageIoContext);
                value->messageHandler->Release();
            }
            // It appears the the falcon servlet takes ownership of the mapping
            // and will close it after the scan completes. The handle becomes
            // invalid and attempting to close it again would result in an error.
            // CloseHandle(fileMapping);
        } else {
            throw Exception("Could not map the file.");
        }
    }

    void Servlet::SetLogger(Logger logger) {
        this->logger = logger;
    }
}

PYBIND11_MODULE(pyfalcon, m) {
    // Attributes
    m.attr("__version__") = "1.0";

    // Classes
    py::class_<PyFalcon::File, std::shared_ptr<PyFalcon::File>>(m, "File", R"pbdoc(
            Manages file handles for scanning.
        )pbdoc")
        .def(py::init<std::string>(), py::arg("path"));

    py::class_<PyFalcon::Message, std::shared_ptr<PyFalcon::Message>>(m, "Message", R"pbdoc(
            Scan information provided to a callback function.
        )pbdoc")
        .def_property("static_analysis_response", &PyFalcon::Message::GetStaticAnalysisResponse, nullptr, R"pbdoc(
            The StaticAnalysisResponse data if a file scan was done, otherwise None.
        )pbdoc");

    py::class_<PyFalcon::Servlet, std::shared_ptr<PyFalcon::Servlet>>(m, "Servlet", R"pbdoc(
            Manages Falcon servlets for scanning data.
        )pbdoc")
        .def(py::init<std::string>(), py::arg("path"))
        .def("scan_file", &PyFalcon::Servlet::ScanFile, py::arg("file"))
        .def_property("logger", nullptr, &PyFalcon::Servlet::SetLogger);

    py::class_<PyFalcon::StaticAnalysisResponse, std::shared_ptr<PyFalcon::StaticAnalysisResponse>>(m, "StaticAnalysisResponse", R"pbdoc(
            The response of a file scan.
        )pbdoc")
        .def_readwrite("verdict", &PyFalcon::StaticAnalysisResponse::Verdict)
        .def_readwrite("sub_verdict", &PyFalcon::StaticAnalysisResponse::SubVerdict)
        .def_readwrite("score", &PyFalcon::StaticAnalysisResponse::Score);

    // Exceptions
    py::register_local_exception<PyFalcon::Exception>(m, "FalconException");
}
