// Falcon's Aviary API as determined by
// auditing the Falcon extension DLLs.
#pragma once
#pragma warning(disable : 4467) // Suppress: usage of ATL attributes is depreciated
#include <Windows.h>
#include <atlbase.h>
#include <atlcom.h>
#include <comdef.h>
#include <stdio.h>
#include <string.h>

namespace Aviary {
    enum class MessageId {
        // FSML (Falcon Sensor ML)
        StaticAnalysisRequest = 590,
        StaticAnalysisResponse = 591,
        DeepHashBlacklistUpdateRequest = 592,
        DeepHashBlacklistUpdateResponse = 593,
        // Network Scanner
        NetworkScanServletRequest = 608,
        NetworkScanServletStatusUpdate = 609,
        // Memory Scanner
        MemoryScanRequest = 984,
        MemoryScanContinueRequest = 985,
        MemoryScanInitRequest = 986,
        MemoryScanInitResponse = 987,
        MemoryScanGetMoreMemory = 988,
        MemoryScanResponse = 989,
        MemoryScanReply = 990,
        // OFP (Office File Prevention)
        OfficeFileAnalysisRequest = 993,
        OfficeFileAnalysisResponse = 994,
        // All servlets
        ServletControlRequest = 1069,
        ServletControlResponse = 1070,
        ServletLoadConfigRequest = 1078,
        ServletLoadConfigResponse = 1079,
        ServletTelemetryRequest = 1080,
        ServletTelemetryResponse = 1081,
        // IDP (IDP protocol analyzer)
        IdentityProtectionConnectRequest = 1100,
        IdentityProtectionConnectResponse = 1101,
        // Decompress
        DecompressionRequest = 1268,
        DecompressionResponse = 1269,
        // FileMetadata
        FileMetadataGetRequest = 1270,
        FileMetadataSetRequest = 1271,
        FileMetadataResponse = 1272,
        // SLP (Sensor Source Language)
        SlpRequest = 1274,
        SlpResponse = 1275,
        // Decompress, continued
        ArchiveScanFinalizationRequest = 1279,
        ArchiveScanFinalizationResponse = 1279,
        ArchiveScanInitializationRequest = 1279,
        ArchiveScanInitializationResponse = 1279,
        ArchiveScanRequest = 1279,
        ArchiveScanResponse = 1280,
        // VCI (unknown acronym)
        SpotlightProbeWmiInitRequest = 1281,
        SpotlightProbeWmiUnInitRequest = 1281,
        SpotlightProbeWmiInitResponse = 1282,
        SpotlightProbeWmiUnInitResponse = 1282,
        SpotlightProbeWqlColumnInfoRequest = 1283,
        SpotlightProbeWqlColumnInfoResponse = 1284,
        SpotlightProbeWqlExecuteRequest = 1285,
        SpotlightProbeWqlExecuteResponse = 1286,
        // ContentClassifier
        ContentClassificationGeneralConfig = 1333,
        ContentClassificationTextExtractorConfig = 1334,
        ContentClassificationTextAnalyzerGeneralConfig = 1335,
        ContentClassificationBigIdClassifier = 1336,
        ContentClassificationCommitConfiguration = 1337,
        ContentClassificationRequest = 1338,
        ContentClassificationResponse = 1339,
        ContentClassificationCancelRequest = 1340,
        ContentClassificationAck = 1341,
        ContentClassificationNewConfig = 1342,
        ContentClassificationCustomClassifier = 1343,
        ContentClassificationError = 1344,
    };

    struct _ARCHIVE_SCAN_FINALIZATION_REQUEST_DATA {
        // Example Aviary struct. These should be defined
        // for all message types.
    };

    EXTERN_C const IID IID_IXMessage;
    MIDL_INTERFACE("5937fbfd-61af-4b97-83f0-d84a53510730")
    IXMessage : public IUnknown {
        virtual uint64_t STDMETHODCALLTYPE GetId() = 0;
        virtual void* STDMETHODCALLTYPE GetData() = 0;
        virtual void STDMETHODCALLTYPE Reset(bool release) = 0;
    };

    EXTERN_C const IID IID_IXMessageAssembler;
    MIDL_INTERFACE("57373dc0-0032-4c54-a29d-9f8863cea300")
    IXMessageAssembler : public IUnknown{
        // ...
    };

    EXTERN_C const IID IID_IXMessageService;
    MIDL_INTERFACE("C6E64D24-5513-47F8-A8D7-7B3FA532FA20")
    IXMessageService : public IUnknown {
        // TBD
        virtual void STDMETHODCALLTYPE VirtualFunction4(void* p1, void* p2, void** p3) = 0;
        virtual void STDMETHODCALLTYPE VirtualFunction5() = 0;
        virtual void STDMETHODCALLTYPE VirtualFunction6() = 0;
        virtual void STDMETHODCALLTYPE Reset(bool release) = 0;
    };

    EXTERN_C const IID IID_IXPlatform;
    MIDL_INTERFACE("05840467-7f8c-49af-9394-d9ef731f9c70")
    IXPlatform : public IUnknown {
        virtual DWORD STDMETHODCALLTYPE GetError(void) = 0;
        virtual STDMETHODIMP GetService(REFIID riid, IUnknown * *service) = 0;
        virtual void STDMETHODCALLTYPE Reset(bool release) = 0;
    };

    EXTERN_C const IID IID_IXPlatformInformationService;
    MIDL_INTERFACE("FAD3EB88-63BE-4608-AB1D-7E9B773027C0")
    IXPlatformInformationService : public IUnknown {
        virtual void* STDMETHODCALLTYPE GetData() = 0;
        virtual void STDMETHODCALLTYPE Reset(bool release) = 0;
    };

    EXTERN_C const IID IID_IXPlatformInformationService;
    MIDL_INTERFACE("19F7D9E5-98DA-4A18-A796-01AFBCA6BC60")
    IXPlatformManagementService : public IUnknown {
        virtual size_t STDMETHODCALLTYPE CallInternalMethod() = 0;
        virtual void STDMETHODCALLTYPE Reset(bool release) = 0;
    };

// All messages wrap IXMessage to allow their IIDs to be different
#define DEFINE_MESSAGE_IID(ID, NAME) \
    EXTERN_C const IID IID_##NAME;   \
    MIDL_INTERFACE(ID)               \
    NAME:                            \
public                               \
    IXMessage{};

    // The DEFINE_MESSAGE macro should be used to setup all messages.
    DEFINE_MESSAGE_IID("29B6DF03-E939-4039-8777-89B909889DD0", IArchiveScanFinalizationRequest);
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
[[maybe_unused]] STDMETHODIMP CsBind(void* platform);

/// <summary>
///     ...
/// </summary>
/// <returns>
///     ...
/// </returns>
[[maybe_unused]] STDMETHODIMP CsEntry(uint32_t, size_t, void*, void*);

/// <summary>
///     Removes all internal references to IXPlatform and the service vtables that acquired via CsBind.
///     Updates the internal BindingState variable as needed.
/// </summary>
/// <returns>
///     ERROR_SUCCESS on success.
///     __HRESULT_FROM_WIN32(ERROR_INVALID_STATE) on failure.
/// </returns>
[[maybe_unused]] STDMETHODIMP CsUnbind();
}