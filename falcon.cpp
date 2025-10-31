#include "libfalcon.hpp"
#include <map>

STDMETHODIMP XMessageIoContext::QueryInterface(REFIID riid, void** ppvObject) {
    if (!ppvObject) {
        return E_INVALIDARG;
    }
    *ppvObject = nullptr;
    if (riid == __uuidof(Aviary::IXMessageIoContext)) {
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
    uint32_t refCount = _InterlockedDecrement(&refCount);
    if (refCount == 0)
        delete this;
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
    if (riid == __uuidof(Aviary::IXMessageService)) {
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
    if (!message_handler || !entry)
        return HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);

    std::lock_guard<std::mutex> g(mutex);

    if (messageHandlers.contains(key))
        return HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS);

    auto obj_ref = new XMessageHandlerEntry(key, message_handler);

    if (obj_ref == nullptr)
        return HRESULT_FROM_WIN32(ERROR_OUTOFMEMORY);

    messageHandlers[key] = obj_ref;
    *entry = reinterpret_cast<uint8_t*>(&obj_ref->messageHandler);

    return S_OK;
}

STDMETHODIMP XMessageService::DetachMessageHandler(uint8_t* entry) {
    if (!entry)
        return HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);

    auto obj_ref = reinterpret_cast<XMessageHandlerEntry*>(entry - offsetof(XMessageHandlerEntry, messageHandler));

    if (obj_ref == nullptr)
        return HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);

    std::lock_guard<std::mutex> g(mutex);

    if (!messageHandlers.contains(obj_ref->key))
        return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);

    messageHandlers.erase(obj_ref->key);
    delete obj_ref;

    return S_OK;
}

STDMETHODIMP XMessageService::MessageCallback(Aviary::IXMessage* message, std::array<uint8_t, 16> const& _unk_2, uint8_t* entry, uint64_t _unk_4) {
    for (auto const& callback : callbacks)
        callback(message);

    return 0;
}

XMessageService::~XMessageService() {
    for (auto const& [key, value] : messageHandlers)
        delete value;
}

STDMETHODIMP XPlatform::QueryInterface(REFIID riid, void** ppvObject) {
    if (!ppvObject) {
        return E_INVALIDARG;
    }
    *ppvObject = nullptr;
    if (riid == __uuidof(Aviary::IXPlatform)) {
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
        if (riid == __uuidof(Aviary::IXMessageService)) {
            *service = iXMessageService;
        } else if (riid == __uuidof(Aviary::IXPlatformInformationService)) {
            *service = iXPlatformInformationService;
        } else if (riid == __uuidof(Aviary::IXPlatformManagementService)) {
            *service = iXPlatformManagementService;
        } else {
            return E_NOINTERFACE;
        }
        if (service) {
            (*service)->AddRef();
        }
        return (service) ? S_OK : E_NOINTERFACE;
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
    if (riid == __uuidof(Aviary::IXPlatformInformationService)) {
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
    if (riid == __uuidof(Aviary::IXPlatformManagementService)) {
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