// The implementations of Falcon's Aviary API which
// are designed to emulate Falcon itself.
//
// The Reset functions are possibly virtual deconstructors
// in the original code.
#pragma once
#include "libaviary.hpp"
#include "messages.hpp"
#include <array>
#include <functional>
#include <mutex>
#include <unordered_map>

namespace std {
    template<>
    struct hash<std::array<uint8_t, 16>> {
        uint64_t operator()(std::array<uint8_t, 16> const& arr) const noexcept {
            return fnv1a(&arr[0]) ^ fnv1a(&arr[8]);
        }

    private:
        /// <summary>
        /// The "alternate" form of the Fowler–Noll–Vo (fnv) hash function:
        /// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
        /// </summary>
        constexpr uint64_t fnv1a(uint8_t const key[8]) const {
            uint64_t hash{ 0xcbf29ce484222325ull }; // The fnv offset basis
            for (size_t index{ 0 }; index < 8; index++) {
                hash ^= static_cast<uint64_t>(key[index]);
                hash *= 0x100000001b3ll; // The fnv prime
            }
            return hash;
        }
    };
}

struct XMessageHandlerEntry;
struct XMessageIoContext;
struct XMessageService;
struct XPlatform;
struct XPlatformInformationService;
struct XPlatformManagementService;

// Example:
//   ComIid = &struct__GUID_const_IID_IArchiveScanFinalizationRequest
//   ComInterface = struct_IArchiveScanFinalizationRequest
//   MessageData = struct__ARCHIVE_SCAN_FINALIZATION_REQUEST_DATA
//   Id = ArchiveScanFinalizationRequest
// First 2 args are likely instatiated using IID_PPV_ARGS.
template<IID const* ComIid, class ComInterface, typename MessageData, Aviary::MessageId Id,
    typename = std::enable_if_t<std::is_base_of_v<Aviary::IXMessage, ComInterface>>>
struct TXMessage : public ComInterface {
    MessageData data;
    uint32_t refCount;

    TXMessage()
        : TXMessage(nullptr) {
    }

    TXMessage(MessageData const* src)
        : refCount(1) {
        if (!src)
            std::memset(reinterpret_cast<void*>(&data), 0, sizeof(MessageData));
        else
            std::memcpy(reinterpret_cast<void*>(&data), reinterpret_cast<void const*>(src), sizeof(MessageData));
    }

    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) {
        if (!ppvObject) {
            return E_INVALIDARG;
        }
        *ppvObject = nullptr;
        if (riid == *ComIid || riid == __uuidof(Aviary::IXMessage)) {
            *ppvObject = this;
            this->AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef(void) {
        return _InterlockedIncrement(&refCount);
    }

    ULONG STDMETHODCALLTYPE Release(void) {
        uint32_t refCount = _InterlockedDecrement(&refCount);
        if (refCount == 0)
            delete this;
        return refCount;
    }

    // ComInterface methods
    IID const* STDMETHODCALLTYPE GetIid() {
        return ComIid;
    }
    void* STDMETHODCALLTYPE GetData() {
        return reinterpret_cast<void*>(&data);
    }
};

#define TXMESSAGETYPE(TYPE, DATA) TXMessage<&IID_I##TYPE, I##TYPE, DATA, MessageId::##TYPE>
#define TXMESSAGE(TYPE, DATA)     TXMESSAGETYPE(TYPE, DATA)()

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
template<IID const* ComIid, class ComInterface, typename TXMessageType, typename MessageData,
    typename = std::enable_if_t<std::is_base_of_v<Aviary::IXMessage, ComInterface>>>
struct TXMessageAssembler : public Aviary::IXMessageAssembler {
    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) {
        if (!ppvObject) {
            return E_INVALIDARG;
        }
        *ppvObject = nullptr;
        if (riid == __uuidof(Aviary::IXMessageAssembler)) {
            *ppvObject = this;
            this->AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef(void) {
        return ERROR_INVALID_FUNCTION;
    }
    ULONG STDMETHODCALLTYPE Release(void) {
        return ERROR_INVALID_FUNCTION;
    }

    // IXMessageAssembler methods
    IID const* STDMETHODCALLTYPE GetIid() {
        return ComIid;
    }

    STDMETHODIMP ExtractMessage(Aviary::IXMessage* message, uint32_t* size, void* data) {
        if (!message || !size || !data)
            return HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);

        if (*size < sizeof(MessageData)) {
            *size = sizeof(MessageData);
            return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        }

        TXMessageType* object = nullptr;
        HRESULT result = message->QueryInterface(*ComIid, reinterpret_cast<void**>(&object));

        if (result >= 0) {
            std::memcpy(data, object->GetData(), sizeof(MessageData));
            result = S_OK;
        }

        if (object != nullptr)
            object->Release();

        return result;
    }

    STDMETHODIMP CreateMessage(size_t size, void const* data, Aviary::IXMessage** ppvMessage) {
        if (!data || !ppvMessage)
            return HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);

        *ppvMessage = nullptr;

        if (size < sizeof(MessageData))
            return HRESULT_FROM_WIN32(ERROR_INVALID_DATA);

        auto message = new TXMessageType(reinterpret_cast<MessageData const*>(data));

        if (message == nullptr)
            return HRESULT_FROM_WIN32(ERROR_OUTOFMEMORY);

        *ppvMessage = message;
        return S_OK;
    }
};

#define TXMESSAGEASSEMBLERTYPE(TYPE, DATA) Aviary::TXMessageAssembler<&IID_I##TYPE, I##TYPE, TXMESSAGETYPE(TYPE, DATA), decltype(DATA)>
#define TXMESSAGEASSEMBLER(TYPE, DATA)     TXMESSAGEASSEMBLERTYPE(TYPE, DATA)()

struct XMessageHandlerEntry {
    std::array<uint8_t, 16> key;
    Aviary::IXMessageHandler* messageHandler;

    XMessageHandlerEntry(std::array<uint8_t, 16> const& key, Aviary::IXMessageHandler* message_handler)
        : key(key), messageHandler(message_handler) {
        messageHandler->AddRef();
    }

    virtual ~XMessageHandlerEntry() {
        messageHandler->Release();
    }
};

struct XMessageIoContext : public Aviary::IXMessageIoContext {
    uint32_t refCount{ 1 };
    std::array<uint8_t, 16> _unk_12 = { 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 };
    std::array<uint8_t, 16> _unk_28 = { 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48 };
    uint32_t errorCode{ 0 };
    uint64_t _unk_48{ 0 };
    uint16_t _unk_56{ 0 };

    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

    // IXMessageIoContext methods
    void* vfptr_24();
    void* vfptr_32();
    uint32_t GetErrorCode();
    uint16_t vfptr_48();
    void vfptr_56();
};

struct XMessageService : public Aviary::IXMessageService {
    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

    // IXMessageService methods
    STDMETHODIMP AttachMessageHandler(std::array<uint8_t, 16> const& key, Aviary::IXMessageHandler* message_handler, uint8_t** entry);
    STDMETHODIMP DetachMessageHandler(uint8_t* entry);
    STDMETHODIMP MessageCallback(Aviary::IXMessage* message, std::array<uint8_t, 16> const& _unk_2, uint8_t* entry, uint64_t _unk_4);
    ~XMessageService();

    std::mutex mutex;
    std::unordered_map<std::array<uint8_t, 16>, XMessageHandlerEntry*> messageHandlers;
    std::vector<std::function<void(Aviary::IXMessage*)>> callbacks;
};

struct XPlatform : public Aviary::IXPlatform {
    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

    // IXPlatform methods
    STDMETHODIMP GetLastError(void);
    STDMETHODIMP GetService(REFIID riid, IUnknown** service);
    ~XPlatform();

    // The name and purpose of this is completely guessed
    // based off of programming design elsewhere.
    DWORD error = 0;
    XMessageService* iXMessageService = nullptr;
    XPlatformInformationService* iXPlatformInformationService = nullptr;
    XPlatformManagementService* iXPlatformManagementService = nullptr;
};

struct XPlatformInformationService : public Aviary::IXPlatformInformationService {
    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

    // IXPlatformInformationService methods
    void* STDMETHODCALLTYPE GetKey();

    std::array<uint8_t, 16> key = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
};

struct XPlatformManagementService : public Aviary::IXPlatformManagementService {
    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

    // IXPlatformManagementService methods
    size_t STDMETHODCALLTYPE CallInternalMethod();

    size_t(STDMETHODCALLTYPE* unknown1)(void*) = nullptr;
    void* unknown2 = nullptr; // Likely a this variable
};