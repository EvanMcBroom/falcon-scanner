#define NOMINMAX
#include "libfalcon.hpp"
#include <algorithm>
#include <bcrypt.h>
#include <iomanip>
#include <limits>
#include <map>
#include <optional>
#include <set>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace {
    std::mutex scannerModulesMutex;
    std::set<HMODULE> scannerModules;

    void CheckResult(HRESULT result, const char* operation) {
        if (FAILED(result)) {
            std::ostringstream message;
            message << operation << " failed: 0x" << std::hex << std::setw(8)
                    << std::setfill('0') << static_cast<uint32_t>(result);
            throw std::runtime_error(message.str());
        }
    }

    std::array<uint8_t, 32> HashFileMapping(HANDLE mapping, uint32_t size) {
        struct ViewDeletor {
            void operator()(void* view) const noexcept {
                if (view)
                    UnmapViewOfFile(view);
            }
        };
        std::unique_ptr<void, ViewDeletor> view(MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, size));
        if (!view) {
            CheckResult(HRESULT_FROM_WIN32(::GetLastError()), "MapViewOfFile");
        }
        BCRYPT_ALG_HANDLE algorithm{ nullptr };
        CheckResult(BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0), "BCryptOpenAlgorithmProvider");
        struct AlgorithmGuard {
            BCRYPT_ALG_HANDLE handle;
            ~AlgorithmGuard() {
                BCryptCloseAlgorithmProvider(handle, 0);
            }
        } guard{ algorithm };
        std::array<uint8_t, 32> hash{};
        CheckResult(BCryptHash(algorithm, nullptr, 0, static_cast<PUCHAR>(view.get()), size, hash.data(), static_cast<ULONG>(hash.size())), "BCryptHash");
        return hash;
    }
}

STDMETHODIMP XMessageIoContext::QueryInterface(REFIID riid, void** ppvObject) {
    if (!ppvObject) {
        return E_INVALIDARG;
    }
    *ppvObject = nullptr;
    if (riid == __uuidof(Aviary::IXMessageIoContext) || riid == IID_IUnknown) {
        *ppvObject = this;
        this->AddRef();
        return S_OK;
    }
    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE XMessageIoContext::AddRef(void) {
    return _InterlockedIncrement(&refCount);
}

ULONG STDMETHODCALLTYPE XMessageIoContext::Release(void) {
    auto refCount{ _InterlockedDecrement(&this->refCount) };
    if (refCount == 0) {
        delete this;
    }
    return refCount;
}

void* XMessageIoContext::vfptr_24() {
    return reinterpret_cast<void*>(_unk_28.data());
}

void* XMessageIoContext::vfptr_32() {
    return reinterpret_cast<void*>(_unk_12.data());
}

uint32_t XMessageIoContext::GetErrorCode() {
    return errorCode;
}

uint16_t XMessageIoContext::vfptr_48() {
    return _unk_56;
}

void XMessageIoContext::vfptr_56() {
    if (_unk_48 && !CallbackMayRunLong(reinterpret_cast<TP_CALLBACK_INSTANCE*>(_unk_48))) {
        // Trace Message
    }
}

STDMETHODIMP XMessageService::QueryInterface(REFIID riid, void** ppvObject) {
    if (!ppvObject) {
        return E_INVALIDARG;
    }
    *ppvObject = nullptr;
    if (riid == __uuidof(Aviary::IXMessageService) || riid == IID_IUnknown) {
        *ppvObject = this;
        this->AddRef();
        return S_OK;
    }
    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE XMessageService::AddRef(void) {
    return ERROR_INVALID_FUNCTION;
}

ULONG STDMETHODCALLTYPE XMessageService::Release(void) {
    return ERROR_INVALID_FUNCTION;
}

STDMETHODIMP XMessageService::AttachMessageHandler(std::array<uint8_t, 16> const& key, Aviary::IXMessageHandler* message_handler, uint8_t** entry) {
    if (!message_handler || !entry) {
        return HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);
    }
    *entry = nullptr;
    try {
        std::lock_guard<std::mutex> guard(mutex);
        if (messageHandlers.contains(key)) {
            return HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS);
        }
        auto handler = std::make_unique<XMessageHandlerEntry>(key, message_handler);
        messageHandlers.emplace(key, handler.get());
        *entry = reinterpret_cast<uint8_t*>(&handler->messageHandler);
        handler.release();
    } catch (const std::bad_alloc&) {
        return HRESULT_FROM_WIN32(ERROR_OUTOFMEMORY);
    }
    return S_OK;
}

STDMETHODIMP XMessageService::DetachMessageHandler(uint8_t* entry) {
    if (!entry) {
        return HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);
    }
    std::unique_ptr<XMessageHandlerEntry> handler;
    {
        std::lock_guard<std::mutex> guard(mutex);
        auto found = std::find_if(messageHandlers.begin(), messageHandlers.end(), [entry](const auto& item) {
            return reinterpret_cast<uint8_t*>(&item.second->messageHandler) == entry;
        });
        if (found == messageHandlers.end()) {
            return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        }
        handler.reset(found->second);
        messageHandlers.erase(found);
    }
    return S_OK;
}

STDMETHODIMP XMessageService::MessageCallback(Aviary::IXMessage* message, std::array<uint8_t, 16> const& _unk_2, uint8_t* entry, uint64_t _unk_4) {
    try {
        for (auto const& callback : callbacks) {
            callback(message);
        }
        return S_OK;
    } catch (...) {
        if (!callbackError) {
            callbackError = std::current_exception();
        }
        return E_FAIL;
    }
}

XMessageService::~XMessageService() {
    ClearHandlers();
}

void XMessageService::ClearHandlers() {
    decltype(messageHandlers) handlers;
    {
        std::lock_guard<std::mutex> guard(mutex);
        handlers.swap(messageHandlers);
    }
    for (auto const& [key, value] : handlers) {
        delete value;
    }
}

CComPtr<Aviary::IXMessageHandler> XMessageService::GetScanHandler() {
    std::lock_guard<std::mutex> guard(mutex);
    if (messageHandlers.size() != 1) {
        throw std::runtime_error("CsMl must register exactly one scan handler.");
    }
    return CComPtr<Aviary::IXMessageHandler>(messageHandlers.begin()->second->messageHandler);
}

STDMETHODIMP XPlatform::QueryInterface(REFIID riid, void** ppvObject) {
    if (!ppvObject) {
        return E_INVALIDARG;
    }
    *ppvObject = nullptr;
    if (riid == __uuidof(Aviary::IXPlatform) || riid == IID_IUnknown) {
        *ppvObject = this;
        this->AddRef();
        return S_OK;
    }
    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE XPlatform::AddRef(void) {
    return ERROR_INVALID_FUNCTION;
}

ULONG STDMETHODCALLTYPE XPlatform::Release(void) {
    return ERROR_INVALID_FUNCTION;
}

STDMETHODIMP XPlatform::GetLastError(void) {
    return error;
}

HRESULT STDMETHODCALLTYPE XPlatform::GetService(REFIID riid, IUnknown** service) {
    if (service) {
        *service = nullptr;
        if (riid == __uuidof(Aviary::IXMessageService) || riid == IID_IUnknown) {
            *service = iXMessageService;
        } else if (riid == __uuidof(Aviary::IXPlatformInformationService) || riid == IID_IUnknown) {
            *service = iXPlatformInformationService;
        } else if (riid == __uuidof(Aviary::IXPlatformManagementService) || riid == IID_IUnknown) {
            *service = iXPlatformManagementService;
        } else {
            return E_NOINTERFACE;
        }
        if (*service) {
            (*service)->AddRef();
        }
        return (*service) ? S_OK : E_NOINTERFACE;
    }
    return E_INVALIDARG;
}

XPlatform::~XPlatform() {
    if (iXMessageService != nullptr) {
        iXMessageService->Release();
        iXMessageService = nullptr;
    }
    if (iXPlatformInformationService != nullptr) {
        iXPlatformInformationService->Release();
        iXPlatformInformationService = nullptr;
    }
    if (iXPlatformManagementService != nullptr) {
        iXPlatformManagementService->Release();
        iXPlatformManagementService = nullptr;
    }
}

STDMETHODIMP XPlatformInformationService::QueryInterface(REFIID riid, void** ppvObject) {
    if (!ppvObject) {
        return E_INVALIDARG;
    }
    *ppvObject = nullptr;
    if (riid == __uuidof(Aviary::IXPlatformInformationService) || riid == IID_IUnknown) {
        *ppvObject = this;
        this->AddRef();
        return S_OK;
    }
    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE XPlatformInformationService::AddRef(void) {
    return ERROR_INVALID_FUNCTION;
}

ULONG STDMETHODCALLTYPE XPlatformInformationService::Release(void) {
    return ERROR_INVALID_FUNCTION;
}

void* STDMETHODCALLTYPE XPlatformInformationService::GetKey() {
    return reinterpret_cast<void*>(key.data());
}

STDMETHODIMP XPlatformManagementService::QueryInterface(REFIID riid, void** ppvObject) {
    if (!ppvObject) {
        return E_INVALIDARG;
    }
    *ppvObject = nullptr;
    if (riid == __uuidof(Aviary::IXPlatformManagementService) || riid == IID_IUnknown) {
        *ppvObject = this;
        this->AddRef();
        return S_OK;
    }
    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE XPlatformManagementService::AddRef(void) {
    return ERROR_INVALID_FUNCTION;
}

ULONG STDMETHODCALLTYPE XPlatformManagementService::Release(void) {
    return ERROR_INVALID_FUNCTION;
}

size_t STDMETHODCALLTYPE XPlatformManagementService::CallInternalMethod(void) {
    return S_OK; // unknown1(unknown1);
}

struct Falcon::Scanner::State {
    XMessageService messageService;
    XPlatformInformationService informationService;
    XPlatformManagementService managementService;
    XPlatform platform; // Destroyed before the above services.
    HMODULE module = nullptr;
    decltype(CsBind)* bind = nullptr;
    decltype(CsEntry)* entry = nullptr;
    decltype(CsUnbind)* unbind = nullptr;
    bool claimed = false;
    bool bound = false;
    bool initializationAttempted = false;
    bool scanning = false;
    std::recursive_mutex mutex;
    std::optional<Aviary::STATIC_ANALYSIS_RESPONSE_DATA> response;
    std::function<void(Aviary::IXMessage*)> logger;

    bool Close() noexcept {
        logger = nullptr;
        if (bound) {
            if (initializationAttempted) {
                (void)entry(static_cast<uint32_t>(CsEntryOperation::Shutdown));
            }
            if (FAILED(unbind())) {
                return false;
            }
            bound = false;
        }
        // Release any remaining DLL objects while their code is still mapped.
        messageService.ClearHandlers();
        if (claimed) {
            std::lock_guard<std::mutex> guard(scannerModulesMutex);
            scannerModules.erase(module);
            claimed = false;
        }
        if (module) {
            FreeLibrary(module);
        }
        module = nullptr;
        return true;
    }
};

Falcon::Scanner::Scanner(const std::wstring& path)
    : state(std::make_unique<State>()) {
    auto& s{ *state };
    try {
        s.module = LoadLibraryW(path.c_str());
        if (!s.module) {
            CheckResult(HRESULT_FROM_WIN32(::GetLastError()), "LoadLibraryW");
        }
        s.bind = reinterpret_cast<decltype(CsBind)*>(GetProcAddress(s.module, "CsBind"));
        s.entry = reinterpret_cast<decltype(CsEntry)*>(GetProcAddress(s.module, "CsEntry"));
        s.unbind = reinterpret_cast<decltype(CsUnbind)*>(GetProcAddress(s.module, "CsUnbind"));
        if (!s.bind || !s.entry || !s.unbind) {
            throw std::runtime_error("Scanner DLL must export CsBind, CsEntry, and CsUnbind.");
        }
        {
            std::lock_guard<std::mutex> lock(scannerModulesMutex);
            if (!scannerModules.insert(s.module).second) {
                throw std::runtime_error("This CsMl DLL already has an active scanner. Reuse that scanner.");
            }
            s.claimed = true;
        }
        s.platform.iXMessageService = &s.messageService;
        s.platform.iXPlatformInformationService = &s.informationService;
        s.platform.iXPlatformManagementService = &s.managementService;
        s.messageService.callbacks.emplace_back([&s](Aviary::IXMessage* message) {
            if (!message || !message->GetIid()) {
                throw std::runtime_error("Scanner returned an invalid message.");
            }
            if (*message->GetIid() == __uuidof(Aviary::IStaticAnalysisResponse)) {
                if (!message->GetData()) {
                    throw std::runtime_error("Scanner returned an empty response.");
                }
                Aviary::STATIC_ANALYSIS_RESPONSE_DATA response{};
                std::memcpy(&response, message->GetData(), Aviary::StaticAnalysisResponseResultSize);
                s.response = response;
            }
            if (s.logger) {
                s.logger(message);
            }
        });
        CheckResult(s.bind(&s.platform), "CsBind");
        s.bound = true;
        auto result = s.entry(1);
        if (result != HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED)) {
            CheckResult(result, "CsEntry(1)");
        }
        s.initializationAttempted = true;
        CheckResult(s.entry(static_cast<uint32_t>(CsEntryOperation::Initialize)), "CsEntry(2)");
        if (s.messageService.callbackError) {
            std::rethrow_exception(s.messageService.callbackError);
        }
        (void)s.messageService.GetScanHandler();
    } catch (...) {
        // If unbinding fails, retain the platform and DLL because the DLL may
        // still hold references to host objects. Normal shutdown frees both.
        if (!s.Close()) {
            (void)state.release();
        }
        throw;
    }
}

Falcon::Scanner::~Scanner() {
    if (state && !state->Close()) {
        (void)state.release();
    }
}

void Falcon::Scanner::SetLogger(std::function<void(Aviary::IXMessage*)> logger) {
    std::lock_guard<std::recursive_mutex> lock(state->mutex);
    if (state->scanning) {
        throw std::runtime_error("Cannot change the logger during a scan.");
    }
    state->logger = std::move(logger);
}

Aviary::STATIC_ANALYSIS_RESPONSE_DATA Falcon::Scanner::ScanFile(HANDLE file) {
    using namespace Aviary;
    auto& s{ *state };
    std::lock_guard<std::recursive_mutex> lock(s.mutex);
    if (s.scanning) {
        throw std::runtime_error("A scan callback cannot start another scan on this scanner.");
    }
    LARGE_INTEGER size{};
    if (!GetFileSizeEx(file, &size)) {
        CheckResult(HRESULT_FROM_WIN32(::GetLastError()), "GetFileSizeEx");
    }
    if (size.QuadPart <= 0 || static_cast<uint64_t>(size.QuadPart) > std::numeric_limits<uint32_t>::max()) {
        throw std::runtime_error("Input must be between 1 and the max limit for uint32_t (e.g. 4294967295) bytes.");
    }
    auto handler{ s.messageService.GetScanHandler() };
    struct HandleDeletor {
        void operator()(void* handle) const noexcept {
            if (handle)
                CloseHandle(handle);
        }
    };
    std::unique_ptr<void, HandleDeletor> mapping(CreateFileMappingW(file, nullptr, PAGE_READONLY, 0, 0, nullptr));
    if (!mapping) {
        CheckResult(HRESULT_FROM_WIN32(::GetLastError()), "CreateFileMappingW");
    }
    CComPtr<TXMESSAGETYPE(StaticAnalysisRequest, Aviary::STATIC_ANALYSIS_REQUEST_DATA)> message;
    message.Attach(new TXMESSAGETYPE(StaticAnalysisRequest, Aviary::STATIC_ANALYSIS_REQUEST_DATA));
    CComPtr<XMessageIoContext> context;
    context.Attach(new XMessageIoContext);
    message->data.Flags = Aviary::DefaultScanFlags;
    message->data.TargetFileMapping = mapping.get();
    message->data.TargetFileSize = static_cast<uint32_t>(size.QuadPart);
    message->data.FileHash = HashFileMapping(mapping.get(), message->data.TargetFileSize);
    s.response.reset();
    s.messageService.callbackError = nullptr;
    s.scanning = true;
    struct ScanGuard {
        bool& active;
        ~ScanGuard() {
            active = false;
        }
    } guard{ s.scanning };
    // The Falcon servlet takes ownership of the mapping when the request is
    // sent and closes it after scanning, including on scan errors. The handle
    // becomes invalid, so it should not be closed again. RAII is used to
    // close it if something failed before the request is sent.
    (void)mapping.release();
    handler->Request(message, context);
    if (s.messageService.callbackError) {
        auto error = std::exchange(s.messageService.callbackError, nullptr);
        std::rethrow_exception(error);
    }
    if (!s.response) {
        throw std::runtime_error("No static analysis response received from scanner.");
    }
    if (s.response->Status != 0) {
        std::ostringstream message;
        message << "Static analysis failed: status 0x" << std::hex
                << static_cast<uint32_t>(s.response->Status);
        throw std::runtime_error(message.str());
    }
    if (s.response->HashMismatch) {
        throw std::runtime_error("Scanner reported a file hash mismatch.");
    }
    return *s.response;
}
