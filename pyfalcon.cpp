#define NOMINMAX
#include "pyfalcon.hpp"
#include <filesystem>
#include <iomanip>
#include <magic_enum.hpp>
#include <pybind11/functional.h>
#include <sstream>

namespace py = pybind11;

namespace PyFalcon {
    Exception::Exception(const char* message)
        : message(message) {
    }

    const char* Exception::what() const noexcept {
        return (!message.empty()) ? message.c_str() : "Unknown error.";
    }

    File::File(const std::string& path) {
        auto utf16Path = std::filesystem::u8path(path).wstring();
        handle = CreateFileW(utf16Path.c_str(), FILE_GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (handle == INVALID_HANDLE_VALUE) {
            auto code = GetLastError();
            throw Exception(("Could not open file: " + path + " (Win32 error " + std::to_string(code) + ")").c_str());
        }
    }

    File::~File() {
        if (handle != INVALID_HANDLE_VALUE) {
            CloseHandle(handle);
        }
    }

    Message::Message(Aviary::IXMessage* message) {
        if (message && message->GetIid()) {
            if (*message->GetIid() == __uuidof(Aviary::IStaticAnalysisResponse)) {
                if (message->GetData()) {
                    Aviary::STATIC_ANALYSIS_RESPONSE_DATA data{};
                    std::memcpy(&data, message->GetData(), Aviary::StaticAnalysisResponseResultSize);
                    response = std::make_shared<StaticAnalysisResponse>();
                    response->Verdict = static_cast<Aviary::VERDICT>(data.Verdict);
                    response->SubVerdict = data.SubVerdict;
                    response->Score = data.Score;
                    response->Status = data.Status;
                    response->HashMismatch = data.HashMismatch != 0;
                    std::ostringstream hash;
                    hash << std::hex << std::setfill('0');
                    for (auto byte : data.ResponseHash) {
                        hash << std::setw(2) << static_cast<unsigned>(byte);
                    }
                    response->ResponseHash = hash.str();
                } else {
                    throw Exception("Empty static analysis response.");
                }
            }
        } else {
            throw Exception("Invalid scanner message.");
        }
    }

    std::shared_ptr<StaticAnalysisResponse> Message::GetStaticAnalysisResponse() {
        return response;
    }

    Servlet::Servlet(const std::string& path) {
        try {
            scanner = std::make_unique<Falcon::Scanner>(std::filesystem::u8path(path).wstring());
        } catch (const std::exception& error) {
            throw Exception(error.what());
        }
    }

    Servlet::~Servlet() = default;

    void Servlet::ScanFile(File& file) {
        try {
            // The shared scanner transfers ownership of the file mapping to
            // CsMl, which closes it after the scan. Closing it again here would
            // use an invalid handle; only the File's original handle is ours.
            (void)scanner->ScanFile(file.handle);
        } catch (const py::error_already_set&) {
            throw;
        } catch (const std::exception& error) {
            throw Exception(error.what());
        }
    }

    void Servlet::SetLogger(Logger logger) {
        try {
            // clang-format off
            scanner->SetLogger(logger ? std::function<void(Aviary::IXMessage*)>(
                [logger = std::move(logger)](Aviary::IXMessage* message) {
                    logger(Message{ message });
                }
            ) : nullptr);
            // clang-format on
        } catch (const std::exception& error) {
            throw Exception(error.what());
        }
    }
}

PYBIND11_MODULE(pyfalcon, m) {
    // Attributes
    m.attr("__version__") = "1.1";

    // Classes and enums
    py::class_<PyFalcon::File, std::shared_ptr<PyFalcon::File>>(m, "File", R"pbdoc(
            Manages file handles for scanning.
        )pbdoc")
        .def(py::init<std::string>(), py::arg("path"));

    py::class_<PyFalcon::Message, std::shared_ptr<PyFalcon::Message>>(m, "Message", R"pbdoc(
            Scan information provided to a callback function.
        )pbdoc")
        .def_property_readonly("static_analysis_response", &PyFalcon::Message::GetStaticAnalysisResponse, R"pbdoc(
            The StaticAnalysisResponse data if a file scan response was received, otherwise None.
        )pbdoc");

    py::class_<PyFalcon::Servlet, std::shared_ptr<PyFalcon::Servlet>>(m, "Servlet", R"pbdoc(
            Manages Falcon servlets for scanning data.
        )pbdoc")
        .def(py::init<std::string>(), py::arg("path"))
        .def("scan_file", &PyFalcon::Servlet::ScanFile, py::arg("file"))
        .def_property("logger", nullptr, &PyFalcon::Servlet::SetLogger);

    m.attr("Scanner") = m.attr("Servlet");

    py::class_<PyFalcon::StaticAnalysisResponse, std::shared_ptr<PyFalcon::StaticAnalysisResponse>>(m, "StaticAnalysisResponse", R"pbdoc(
            The response of a file scan.
        )pbdoc")
        .def_readwrite("verdict", &PyFalcon::StaticAnalysisResponse::Verdict, R"pbdoc(
            A Verdict enum; use .name for its name or int(verdict) for its numeric value.
        )pbdoc")
        .def_readwrite("subverdict", &PyFalcon::StaticAnalysisResponse::SubVerdict)
        .def_readwrite("sub_verdict", &PyFalcon::StaticAnalysisResponse::SubVerdict)
        .def_readwrite("score", &PyFalcon::StaticAnalysisResponse::Score)
        .def_readwrite("status", &PyFalcon::StaticAnalysisResponse::Status)
        .def_readwrite("hash_mismatch", &PyFalcon::StaticAnalysisResponse::HashMismatch)
        .def_readwrite("response_hash", &PyFalcon::StaticAnalysisResponse::ResponseHash);

    // Add the C++ verdict names directly so each name is registered once.
    auto verdict = py::enum_<Aviary::VERDICT>(m, "Verdict", py::arithmetic());
    for (const auto& [value, name] : magic_enum::enum_entries<Aviary::VERDICT>()) {
        verdict.value(name.data(), value);
    }

    // Exceptions
    py::register_local_exception<PyFalcon::Exception>(m, "FalconException");
}
