#include <libfalcon.hpp>

STDMETHODIMP XPlatform::QueryInterface(REFIID riid, void** ppvObject) {
    if (riid == __uuidof(Aviary::IXPlatform)) {
        return E_INVALIDARG;
    }
    *ppvObject = this;
    this->AddRef();
    return ERROR_SUCCESS;
}

ULONG STDMETHODCALLTYPE XPlatform::AddRef(void) {
    return ERROR_INVALID_FUNCTION;
}

ULONG STDMETHODCALLTYPE XPlatform::Release(void) {
    return ERROR_INVALID_FUNCTION;
}

DWORD STDMETHODCALLTYPE XPlatform::GetError(void) {
    return error;
}

HRESULT STDMETHODCALLTYPE XPlatform::GetService(REFIID riid, IUnknown** service) {
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
    return (service) ? ERROR_SUCCESS : E_NOINTERFACE;
}

void STDMETHODCALLTYPE XPlatform::Reset(bool release) {
    // Reset vtable, then:
    if (iXMessageService) {
        iXMessageService->Release();
        iXMessageService = nullptr;
    }
    if (iXPlatformInformationService) {
        iXPlatformInformationService->Release();
        iXPlatformInformationService = nullptr;
    }
    if (iXPlatformManagementService) {
        iXPlatformManagementService->Release();
        iXPlatformManagementService = nullptr;
    }
    if (release) {
        delete this;
    }
}

XMessageService::XMessageService() {
    unknown8 = malloc(0x28);
}

STDMETHODIMP XMessageService::QueryInterface(REFIID riid, void** ppvObject) {
    if (riid == __uuidof(IXMessageService)) {
        return E_INVALIDARG;
    }
    *ppvObject = this;
    this->AddRef();
    return ERROR_SUCCESS;
}

ULONG STDMETHODCALLTYPE XMessageService::AddRef(void) {
    return ERROR_INVALID_FUNCTION;
}

ULONG STDMETHODCALLTYPE XMessageService::Release(void) {
    return ERROR_INVALID_FUNCTION;
}

void STDMETHODCALLTYPE XMessageService::VirtualFunction4(void* p1, void* p2, void** p3) {
    return;
}

void STDMETHODCALLTYPE XMessageService::VirtualFunction5(void) {
    return;
}

void STDMETHODCALLTYPE XMessageService::VirtualFunction6(void) {
    return;
}

void STDMETHODCALLTYPE XMessageService::Reset(bool release) {
    // Reset vtable, then:
    // A lot more processing occurs before this...
    if (release) {
        delete this;
    }
}

STDMETHODIMP XPlatformInformationService::QueryInterface(REFIID riid, void** ppvObject) {
    if (riid == __uuidof(IXPlatformInformationService)) {
        return E_INVALIDARG;
    }
    *ppvObject = this;
    this->AddRef();
    return ERROR_SUCCESS;
}

ULONG STDMETHODCALLTYPE XPlatformInformationService::AddRef(void) {
    return ERROR_INVALID_FUNCTION;
}

ULONG STDMETHODCALLTYPE XPlatformInformationService::Release(void) {
    return ERROR_INVALID_FUNCTION;
}

void* STDMETHODCALLTYPE XPlatformInformationService::GetData(void) {
    return &unknown1;
}

void STDMETHODCALLTYPE XPlatformInformationService::Reset(bool release) {
    // Reset vtable, then:
    if (release) {
        delete this;
    }
}

STDMETHODIMP XPlatformManagementService::QueryInterface(REFIID riid, void** ppvObject) {
    if (riid == __uuidof(Aviary::IXPlatformManagementService)) {
        return E_INVALIDARG;
    }
    *ppvObject = this;
    this->AddRef();
    return ERROR_SUCCESS;
}

ULONG STDMETHODCALLTYPE XPlatformManagementService::AddRef(void) {
    return ERROR_INVALID_FUNCTION;
}

ULONG STDMETHODCALLTYPE XPlatformManagementService::Release(void) {
    return ERROR_INVALID_FUNCTION;
}

size_t STDMETHODCALLTYPE XPlatformManagementService::CallInternalMethod(void) {
    return unknown1(unknown1);
}

void STDMETHODCALLTYPE XPlatformManagementService::Reset(bool release) {
    // Reset vtable, then:
    if (release) {
        delete this;
    }
}