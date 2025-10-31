// Falcon's Aviary API as determined by
// auditing the Falcon extension DLLs.
#pragma once
#pragma warning(disable : 4467) // Suppress: usage of ATL attributes is depreciated
#include <windows.h>

#include <array>
#include <atlbase.h>
#include <atlcom.h>
#include <comdef.h>
#include <stdio.h>
#include <string.h>

namespace Aviary {
    /// <summary>
    /// Message Base Interface
    /// </summary>
    EXTERN_C const IID IID_IXMessage;
    MIDL_INTERFACE("5937fbfd-61af-4b97-83f0-d84a53510730")
    IXMessage : public IUnknown {
        virtual IID const* STDMETHODCALLTYPE GetIid() = 0; // Name chosen to conform with IInspectable::GetIids
        virtual void* STDMETHODCALLTYPE GetData() = 0;
        virtual ~IXMessage() = 0 {}
    };

    /// <summary>
    /// Message I/O Assembler Base Interface
    /// </summary>
    EXTERN_C const IID IID_IXMessageAssembler;
    MIDL_INTERFACE("57373dc0-0032-4c54-a29d-9f8863cea300")
    IXMessageAssembler : public IUnknown {
        virtual IID const* STDMETHODCALLTYPE GetIid() = 0; // Name chosen to conform with IInspectable::GetIids
        virtual STDMETHODIMP ExtractMessage() = 0;
        virtual STDMETHODIMP CreateMessage(size_t size, void const* data, IXMessage** ppvMessage) = 0;
        virtual ~IXMessageAssembler() = 0;
    };

    /// <summary>
    /// Message I/O Context Base Interface
    /// </summary>
    EXTERN_C const IID IID_IXMessageIoContextr;
    MIDL_INTERFACE("ef5a5de4-db65-4233-80d8-67735c946750")
    IXMessageIoContext : public IUnknown {
        virtual void* vfptr_24() = 0;
        virtual void* vfptr_32() = 0;
        virtual uint32_t GetErrorCode() = 0;
        virtual uint16_t vfptr_48() = 0;
        virtual void vfptr_56() = 0;
        virtual ~IXMessageIoContext() = 0 {}
    };

    /// <summary>
    /// Message Handler Base Interface
    /// </summary>
    EXTERN_C const IID IID_IXMessageHandler;
    MIDL_INTERFACE("54ef8455-284e-46d7-bf86-6870aa170480")
    IXMessageHandler : public IUnknown {
        virtual void Request(IXMessage * message, IXMessageIoContext * context) = 0;
        virtual ~IXMessageHandler() = 0 {}
    };

    /// <summary>
    /// Message Service Base Interface
    /// </summary>
    EXTERN_C const IID IID_IXMessageService;
    MIDL_INTERFACE("C6E64D24-5513-47F8-A8D7-7B3FA532FA20")
    IXMessageService : public IUnknown {
        virtual STDMETHODIMP AttachMessageHandler(std::array<uint8_t, 16> const& key, IXMessageHandler* message_handler, uint8_t** entry) = 0;
        virtual STDMETHODIMP DetachMessageHandler(uint8_t* entry) = 0;
        virtual STDMETHODIMP MessageCallback(IXMessage * message, std::array<uint8_t, 16> const& _unk_2, uint8_t* entry, uint64_t _unk_4) = 0;
        virtual ~IXMessageService() = 0 {}
    };

    /// <summary>
    /// Platform Base Interface
    /// </summary>
    EXTERN_C const IID IID_IXPlatform;
    MIDL_INTERFACE("05840467-7f8c-49af-9394-d9ef731f9c70")
    IXPlatform : public IUnknown {
        virtual STDMETHODIMP GetLastError(void) = 0;
        virtual STDMETHODIMP GetService(REFIID riid, IUnknown * *service) = 0;
        virtual ~IXPlatform() = 0 {}
    };

    /// <summary>
    /// Platform Information Service Base Interface
    /// </summary>
    EXTERN_C const IID IID_IXPlatformInformationService;
    MIDL_INTERFACE("FAD3EB88-63BE-4608-AB1D-7E9B773027C0")
    IXPlatformInformationService : public IUnknown {
        virtual void* STDMETHODCALLTYPE GetKey() = 0;
        virtual ~IXPlatformInformationService() = 0 {}
    };

    /// <summary>
    /// Platform Management Service Base Interface
    /// </summary>
    EXTERN_C const IID IID_IXPlatformManagementService;
    MIDL_INTERFACE("19F7D9E5-98DA-4A18-A796-01AFBCA6BC60")
    IXPlatformManagementService : public IUnknown {
        virtual size_t STDMETHODCALLTYPE CallInternalMethod() = 0;
        virtual ~IXPlatformManagementService() = 0 {}
    };

// All messages wrap IXMessage to allow their IIDs to be different
#define DEFINE_MESSAGE_IID(ID, NAME) \
    MIDL_INTERFACE(ID)               \
    NAME:                            \
public                               \
    IXMessage{};                     \
    constexpr IID const IID_##NAME = __uuidof(NAME);

    // The DEFINE_MESSAGE macro should be used to setup all messages.
    DEFINE_MESSAGE_IID("29B6DF03-E939-4039-8777-89B909889DD0", IArchiveScanFinalizationRequest);
    DEFINE_MESSAGE_IID("5d350b4e-c03c-4f72-9d41-ceb956ad94d0", IDeepHashBlacklistUpdateRequest)
    DEFINE_MESSAGE_IID("b7da2930-63a7-4067-8ff8-0fb3f6057b00", IDeepHashBlacklistUpdateResponse)
    DEFINE_MESSAGE_IID("5e96bd8c-6489-4f81-a618-d811cf45e211", IRuleEngineLoadRulesRequest)
    DEFINE_MESSAGE_IID("61848ed7-a79f-40f7-8fc3-db10e5a28cc6", IRuleEngineLoadRulesResponse)
    DEFINE_MESSAGE_IID("9099e661-bfbd-4605-b323-02b9aa88b910", IStaticAnalysisRequest)
    DEFINE_MESSAGE_IID("68f71fa1-5236-420a-9e03-4548939fb5e0", IStaticAnalysisResponse)
    // ...
}

// Guessed name
enum class CsBindingState {
    Unbinding = -2, // A call to CsUnbind is currently being performed
    Unbound = -1, // A call to CsUnbind completed
    Initial = 0, // Neither CsBind or CsUnbind has been called
    Binding = 1, // A call to CsBind is currently being performed
    Bound = 2, // A call to CsBind completed
};

// Placeholder name
enum class CsEntryArg1 : uint32_t {
    Unknown1 = 1,
    Unknown2 = 2,
    Unknown3 = 3
};

/// <summary>
/// Extension Library Exports
/// </summary>
extern "C" {
/// <summary>
///     Stores the provided platform argument and the service vtables it maintains.
///     Updates the internal BindingState variable as needed.
/// </summary>
/// <param name='platform'>
///     A COM object that implements the IXPlatform interface.
/// </param>
/// <returns>
///     ERROR_SUCCESS on success.
///     E_INVALIDARG or __HRESULT_FROM_WIN32(ERROR_INVALID_STATE) on failure.
/// </returns>
[[maybe_unused]] STDMETHODIMP CsBind(Aviary::IXPlatform*);

/// <summary>
///     ...
/// </summary>
/// <returns>
///     ...
/// </returns>
[[maybe_unused]] STDMETHODIMP CsEntry(uint32_t); //, size_t, void*, void*);

/// <summary>
///     Removes all internal references to IXPlatform and the service vtables that acquired via CsBind.
///     Updates the internal BindingState variable as needed.
/// </summary>
/// <returns>
///     ERROR_SUCCESS on success.
///     __HRESULT_FROM_WIN32(ERROR_INVALID_STATE) on failure.
/// </returns>
[[maybe_unused]] STDMETHODIMP CsUnbind(void);
}