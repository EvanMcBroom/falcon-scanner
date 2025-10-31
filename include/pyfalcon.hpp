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
        uint8_t Verdict;
        uint8_t SubVerdict;
        float Score;
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

    protected:
        HANDLE handle;
    };

    class Message {
    public:
        Message(Aviary::IXMessage* message);
        std::shared_ptr<StaticAnalysisResponse> GetStaticAnalysisResponse();

    private:
        Aviary::IXMessage* message;
    };

    class Servlet {
    public:
        Servlet(const std::string& path);
        ~Servlet();
        void ScanFile(File& file);
        void SetLogger(Logger logger);

    private:
        decltype(CsBind)* LazyCsBind;
        decltype(CsEntry)* LazyCsEntry;
        decltype(CsUnbind)* LazyCsUnbind;
        Logger logger;
        XMessageIoContext messageIoContext;
        XPlatform platform;
        XMessageService messageService;
        XPlatformInformationService platformInformationService;
        XPlatformManagementService platformManagementService;
        HMODULE servlet{ nullptr };
    };
}