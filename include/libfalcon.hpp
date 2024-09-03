// The implementations of Falcon's Aviary API which
// are designed to emulate Falcon itself.
//
// The Reset functions are possibly virtual deconstructors
// in the original code.
#pragma once
#include <libaviary.hpp>

struct XMessageService : public Aviary::IXMessageService {
    XMessageService();
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

    // Interface
    void STDMETHODCALLTYPE VirtualFunction4(void* p1, void* p2, void** p3);
    void STDMETHODCALLTYPE VirtualFunction5();
    void STDMETHODCALLTYPE VirtualFunction6();
    void STDMETHODCALLTYPE Reset(bool release = false);
    
    size_t unknown1 = 0;
    DWORD error = 0xc000a003;
    size_t unknown2 = 0;
    size_t unknown3 = 0;
    size_t unknown4 = 0;
    size_t unknown5 = 0;
    size_t unknown6 = 0;
    size_t unknown7 = 0x3f800000;
    void* unknown8 = nullptr; // 0x28 size buffer
    size_t unknown9 = 0;
    size_t unknown10 = 0;
    size_t unknown11 = 0;
    size_t unknown12 = 0;
    size_t unknown13 = 0;
    size_t unknown14 = 0;
};

struct XPlatformInformationService : public Aviary::IXPlatformInformationService {
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

    // Interface
    void* STDMETHODCALLTYPE GetData();
    void STDMETHODCALLTYPE Reset(bool release = false);

    size_t unknown1;
    size_t unknown2;
};

struct XPlatformManagementService : public Aviary::IXPlatformManagementService {
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

    // Interface
    size_t STDMETHODCALLTYPE CallInternalMethod();
    void STDMETHODCALLTYPE Reset(bool release = false);

    size_t (*unknown1)(void*) = nullptr;
    void* unknown2 = nullptr; // Likely a this variable
};

struct XPlatform : public Aviary::IXPlatform {
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

    // Interface
    DWORD STDMETHODCALLTYPE GetError(void);
    STDMETHODIMP GetService(REFIID riid, IUnknown** service);
    void STDMETHODCALLTYPE Reset(bool release = false);

    // The name and purpose of this is completely guessed
    // based off of programming design elsewhere.
    DWORD error = 0;
    XMessageService* iXMessageService = nullptr;
    XPlatformInformationService* iXPlatformInformationService = nullptr;
    XPlatformManagementService* iXPlatformManagementService = nullptr;
};

// Example:
//   ComIid = &struct__GUID_const_IID_IArchiveScanFinalizationRequest
//   ComInterface = struct_IArchiveScanFinalizationRequest
//   MessageData = struct__ARCHIVE_SCAN_FINALIZATION_REQUEST_DATA
//   Id = ArchiveScanFinalizationRequest
// First 2 args are likely instatiated using IID_PPV_ARGS.
template<typename ComIid, class ComInterface, typename MessageData, Aviary::MessageId Id>
struct TXMessage : public ComInterface {
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) {
        if (ppvObject == nullptr) {
            return E_INVALIDARG;
        }
        *ppvObject = nullptr;
        if (riid == ComIid || riid == Aviary::IXMessage) {
            *ppvObject = this;
            this->AddRef();
            return ERROR_SUCCESS;
        }
        return E_NOINTERFACE;
    }

    STDMETHODIMP AddRef(void) {
        return ERROR_INVALID_FUNCTION;
    }

    STDMETHODIMP Release(void) {
        return ERROR_INVALID_FUNCTION;
    }

    // Interface
    uint64_t STDMETHODCALLTYPE GetId() {
        return ((uint64_t[2])ComIid)[0];
    }

    void* STDMETHODCALLTYPE GetData() {
        return this + 1;
    }

    void STDMETHODCALLTYPE Reset(bool release = false) {
        this = ComInterface; // Reset vtable
        if (release) {
            delete this;
        }
    }
};


// Example:
//   ComIid = &struct__GUID_const_IID_IArchiveScanFinalizationRequest
//   ComInterface = struct_IArchiveScanFinalizationRequest
//   MessageData = TXMessage<
//     &struct__GUID_const_IID_IArchiveScanFinalizationRequest,
//     struct_IArchiveScanFinalizationRequest,
//     struct__ARCHIVE_SCAN_FINALIZATION_REQUEST_DATA,
//     ArchiveScanFinalizationRequest
//   >
//   Id = ArchiveScanFinalizationRequest
// First 2 args are likely instatiated using IID_PPV_ARGS.
template<typename ComIid, class ComInterface, typename TXMessageType, Aviary::MessageId Id>
struct TXMessageAssembler : public Aviary::IXMessageAssembler {
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject);
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);

    // Interface
    // ...
};