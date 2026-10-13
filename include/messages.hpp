#pragma once
#include <windows.h>

#include <array>
#include <cstddef>
#include <stdint.h>

namespace Aviary {
    // These will very likely change on all significant updates
    enum class MessageId {
        // FSML (Falcon Sensor ML)
        // Old IDs:
        // - StaticAnalysisRequest = 590,
        // - StaticAnalysisResponse = 591,
        // - DeepHashBlacklistUpdateRequest = 592,
        // - DeepHashBlacklistUpdateResponse = 593,
        StaticAnalysisRequest = 639,
        StaticAnalysisResponse = 640,
        DeepHashBlacklistUpdateRequest = 641,
        DeepHashBlacklistUpdateResponse = 642,
        RuleEngineLoadRulesRequest = 644,
        RuleEngineLoadRulesResponse = 645,
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
        // Example Aviary struct. These are fully defined
        // by the vendor for all message types.
    };

    typedef struct _DEEPHASH_BLACKLIST_UPDATE_REQUEST_DATA {
        uint32_t RequestTrackingId;
        uint32_t Revision; // Must be bigger than the existing revision
        HANDLE FileMapping;
        uint32_t NumberOfElements;
        uint32_t ProcessId;
    } DEEPHASH_BLACKLIST_UPDATE_REQUEST_DATA;

    static_assert(sizeof(DEEPHASH_BLACKLIST_UPDATE_REQUEST_DATA) == 24);

    typedef struct _DEEPHASH_BLACKLIST_UPDATE_RESPONSE_DATA {
        uint32_t ErrorCode;
        uint32_t ResponseRevision;
        uint32_t RequestTrackingId;
        uint32_t RequestRevision;
    } DEEPHASH_BLACKLIST_UPDATE_RESPONSE_DATA;

    static_assert(sizeof(DEEPHASH_BLACKLIST_UPDATE_RESPONSE_DATA) == 16);

    typedef struct _RULE_ENGINE_LOAD_RULES_REQUEST_DATA {
        HANDLE FileMapping;
        uint32_t FileSize;
        uint32_t ProcessId;
    } RULE_ENGINE_LOAD_RULES_REQUEST_DATA;

    static_assert(sizeof(RULE_ENGINE_LOAD_RULES_REQUEST_DATA) == 16);

    typedef struct _RULE_ENGINE_LOAD_RULES_RESPONSE_DATA {
        uint32_t ProcessId;
        uint32_t ErrorCode;
        uint32_t _unk_16;
    } RULE_ENGINE_LOAD_RULES_RESPONSE_DATA;

    static_assert(sizeof(RULE_ENGINE_LOAD_RULES_RESPONSE_DATA) == 12);

    typedef struct _STATIC_ANALYSIS_REQUEST_DATA {
#define STATIC_ANALYSIS_PUBLIC_PE_RESULT   0x01 // Whether to produce a VERDICT
#define STATIC_ANALYSIS_DEEPHASH_BLACKLIST 0x02 // Deephash blacklist is an auxiliary raw-byte scorer
#define STATIC_ANALYSIS_DEEPHASH_EVIDENCE  0x04 // Evidence is a 512-byte artifact
#define STATIC_ANALYSIS_EXCERPT            0x08
#define STATIC_ANALYSIS_POLICY_METADATA    0x40
        uint32_t Flags;
        std::array<uint8_t, 32> FileHash;
        uint64_t RequestTrackingId;
        HANDLE TargetFileMapping;
        uint32_t TargetFileSize;
        uint32_t RequestTrackingTag;
        LARGE_INTEGER SystemTime;
        uint32_t ProcessId;
        HANDLE ExcerptFileMapping; // STATIC_ANALYSIS_EXCERPT
    } STATIC_ANALYSIS_REQUEST_DATA;

    static_assert(sizeof(STATIC_ANALYSIS_REQUEST_DATA) == 88);
    static_assert(offsetof(STATIC_ANALYSIS_REQUEST_DATA, TargetFileMapping) == 0x30);
    static_assert(offsetof(STATIC_ANALYSIS_REQUEST_DATA, TargetFileSize) == 0x38);

    inline constexpr uint32_t DefaultScanFlags = STATIC_ANALYSIS_PUBLIC_PE_RESULT | STATIC_ANALYSIS_POLICY_METADATA;

    // Aviary gives scanned PEs an internal maliciousness verdict from 1-6.
    // It will return the score when requested after swapping around the
    // order of the numbers. Below is the public/swapped ordering of the
    // numbers with names applied. These names are based on the words of
    // estimative probability. The original names were not identified. The
    // percentage needed to get a certain score will vary between model
    // versions, but generally the top 3 scores will be >90%, the middle
    // score will be roughly around 20%, and the bottom 3 will be <1%.
    // clang-format off
    enum class VERDICT : uint8_t {
                                          // Words of estimative probability:
        Malicious = 3,                    // Certain
        CertainlyMalicious = 2,           // Almost certain
        ProbablyMalicious = 1,            // Probable
        ChancesAboutEvenItsMalicious = 4, // Chances about even,
        ProbablyNotMalicious = 6,         // Probably not
        CertainlyNotMalicious = 5,        // Almost certainly not
        NotMalicious = 0                  // "Impossible" (benign)
    };
    // clang-format on

    typedef struct _STATIC_ANALYSIS_RESPONSE_DATA {
        // The following are stable across versions.
        uint32_t Flags;
        uint32_t Status;
        uint8_t HashMismatch;
        std::array<uint8_t, 32> ResponseHash;
        uint32_t RequestTrackingTag;
        // Flag(1) || Flag(0x40)
        uint8_t Verdict; // 0-6
        uint8_t SubVerdict; // 0-8: a second verdict based on a loaded ruleset
        std::array<uint8_t, 2> Reserved1;
        float Score;
        std::array<uint8_t, 22412> Reserved2;
        uint32_t Reserved3;
        uint32_t Reserved4;
        uint32_t Reserved5;
        uint32_t Reserved6;
        uint32_t Reserved7;
        uint8_t Reserved8;
        uint32_t Reserved9;
        uint8_t Reserved10;
        // Flag(1) or [Flag(8) && !Flag(1)]
        uint64_t Reserved11;
        uint16_t Reserved12;
        uint32_t Reserved13; // Default: 17
        // STATIC_ANALYSIS_DEEPHASH_BLACKLIST
        uint32_t BlacklistRevision; // Default: 0
        std::array<uint8_t, 32> BlacklistHash; // Only populated when BlacklistResult is 0 (indeterminate?)
        float BlacklistScore; // Default: 0.0
        uint32_t BlacklistResult; // Default: 1. If flag set, possible values: 0, 2, 4
        // STATIC_ANALYSIS_DEEPHASH_EVIDENCE
        std::array<uint8_t, 512> DeephashEvidence;
        uint32_t Reserved14; // Default: 0. If flag set, always 1 (0x100401 >> 20)
        uint32_t Reserved15; // Default: 1. If flag set, always 0
        std::array<uint8_t, 32> RequestFileHash;
        LARGE_INTEGER RequestSystemTime;
        uint64_t RequestTrackingId;
    } STATIC_ANALYSIS_RESPONSE_DATA;

    inline constexpr size_t StaticAnalysisResponseResultSize = offsetof(STATIC_ANALYSIS_RESPONSE_DATA, Score) + sizeof(STATIC_ANALYSIS_RESPONSE_DATA::Score);

    // The length of STATIC_ANALYSIS_RESPONSE_DATA changes between
    // module versions, but was 23136 bytes for the audited version.
    static_assert(sizeof(STATIC_ANALYSIS_RESPONSE_DATA) == 23136);
    // The fields through Score are stable across versions and can be used to get results.
    static_assert(offsetof(STATIC_ANALYSIS_RESPONSE_DATA, Status) == 0x04);
    static_assert(offsetof(STATIC_ANALYSIS_RESPONSE_DATA, HashMismatch) == 0x08);
    static_assert(offsetof(STATIC_ANALYSIS_RESPONSE_DATA, ResponseHash) == 0x09);
    static_assert(offsetof(STATIC_ANALYSIS_RESPONSE_DATA, Verdict) == 0x30);
    static_assert(offsetof(STATIC_ANALYSIS_RESPONSE_DATA, SubVerdict) == 0x31);
    static_assert(offsetof(STATIC_ANALYSIS_RESPONSE_DATA, Score) == 0x34);
}
