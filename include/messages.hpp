#pragma once
#include <windows.h>

#include <array>
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
        // Example Aviary struct. These should be defined
        // for all message types.
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
#define STATIC_ANALYSIS_FLAG_1             (0x01)
#define STATIC_ANALYSIS_DEEPHASH_BLACKLIST (0x02) // Perform analysis using the Deephash Blacklist
#define STATIC_ANALYSIS_DEEPHASH_EVIDENCE  (0x04) // Populate the DeephashEvidence array - Probably used to obtain content for the Deephash Blacklist
#define STATIC_ANALYSIS_EXCERPT            (0x08)
#define STATIC_ANALYSIS_FLAG_64            (0x40)
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

    enum class VERDICT : uint8_t {
        Benign = 0,
    };

    typedef struct _STATIC_ANALYSIS_RESPONSE_DATA {
        uint32_t Flags;
        uint32_t ErrorCode;

        uint8_t IsFileHashMismatch;
        std::array<uint8_t, 32> ResponseFileHash;
        uint32_t RequestTrackingTag;

        // Flag(1) || Flag(0x40)
        uint8_t Verdict; // Possible values: 0, 1, 2, 3, 4, 5, 6
        uint8_t SubVerdict; // Possible values: 0, 1, 2, 3, 4, 5, 6, 7, 8
        float Score;
        std::array<uint8_t, 22412> _unk_56;
        uint32_t _unk_22468;
        uint32_t _unk_22472;
        uint32_t _unk_22476;

        uint32_t _unk_22480;
        uint32_t _unk_22484;
        uint8_t _unk_22488;
        uint32_t _unk_22492;
        uint8_t _unk_22496;

        // Flag(1) or [Flag(8) && !Flag(1)]
        uint64_t _unk_22504;
        uint16_t _unk_22512;
        uint32_t _unk_22516; // Default: 17

        // STATIC_ANALYSIS_DEEPHASH_BLACKLIST
        uint32_t BlacklistRevision; // Default: 0
        std::array<uint8_t, 32> BlacklistHash; // Only populated when BlacklistResult is 0 (indeterminate?)
        float BlacklistScore; // Default: 0.0
        uint32_t BlacklistResult; // Default: 1. If flag set, possible values: 0, 2, 4

        // STATIC_ANALYSIS_DEEPHASH_EVIDENCE
        std::array<uint8_t, 512> DeephashEvidence;
        uint32_t _unk_23076; // Default: 0. If flag set, always 1 (0x100401 >> 20)
        uint32_t _unk_23080; // Default: 1. If flag set, always 0

        std::array<uint8_t, 32> RequestFileHash;
        LARGE_INTEGER RequestSystemTime;
        uint64_t RequestTrackingId;
    } STATIC_ANALYSIS_RESPONSE_DATA;

    static_assert(sizeof(STATIC_ANALYSIS_RESPONSE_DATA) == 23136);
}