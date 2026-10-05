#pragma once
#include "libfalcon.hpp"
#include <exception>
#include <pybind11/pybind11.h>

// A simple C++ API for Falcon to provide a pythonic interface via pybind11.
namespace PyFalcon {
    struct StaticAnalysisResponse;
    class Exception;
    class File;
    class Message;
    class Servlet;
    using Logger = std::function<void(Message message)>;

    struct StaticAnalysisResponse {
        Aviary::VERDICT Verdict{};
        uint8_t SubVerdict{};
        float Score{};
        int32_t Status{};
        bool HashMismatch{};
        std::string ResponseHash;
    };

    class Exception : public std::exception {
    public:
        explicit Exception(const char* message);
        const char* what() const noexcept override;

    private:
        std::string message;
    };

    class File {
        friend class Servlet;

    public:
        File(const std::string& path);
        ~File();

        // Make non-copyable.
        File(const File&) = delete;
        File& operator=(const File&) = delete;

    protected:
        HANDLE handle{ INVALID_HANDLE_VALUE };
    };

    class Message {
    public:
        Message(Aviary::IXMessage* message);
        std::shared_ptr<StaticAnalysisResponse> GetStaticAnalysisResponse();

    private:
        std::shared_ptr<StaticAnalysisResponse> response;
    };

    class Servlet {
    public:
        Servlet(const std::string& path);
        ~Servlet();

        // Make non-copyable.
        Servlet(const Servlet&) = delete;
        Servlet& operator=(const Servlet&) = delete;

        void ScanFile(File& file);
        void SetLogger(Logger logger);

    private:
        std::unique_ptr<Falcon::Scanner> scanner;
    };
}
