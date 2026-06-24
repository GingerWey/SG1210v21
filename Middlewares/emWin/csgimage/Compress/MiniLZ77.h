// Copyright 2026 Wey. Silver Grid. All rights reserved.
// CSG Toolkits — MiniLZ77 dictionary compression codec
// ---------------------------------------------------------------------------
// MiniLZ77.h — Lightweight LZ77-style compressor / decompressor designed
//              for resource-constrained MCU decoding.
//
//              Output format (CAS=3, standalone):
//                [1-bit flag] + [data]
//                flag=0 → literal:  8 bits of pixel byte
//                flag=1 → match:    Dist_Bits bits offset
//                                 + Len_Bits bits length code
//              All fields are byte-aligned, fixed-length.
//
//              In DEFLATE mode (CAS=2), outputs raw symbol stream
//              (literal, length, distance symbols) for Huffman coding.
// ---------------------------------------------------------------------------

#ifndef TOOLKITS_INCLUDE_COMPRESS_MINILZ77_H_
#define TOOLKITS_INCLUDE_COMPRESS_MINILZ77_H_

#include "CompressionBase.h"
#include <cstdint>
#include <vector>

class MiniLZ77Codec : public CompressionBase {
public:
    MiniLZ77Codec();

    const char* Name() const override { return "MiniLZ77"; }
    CompressAlgorithm Algorithm() const override {
        return CompressAlgorithm::kMiniLZ77;
    }

    std::vector<uint8_t> Compress(
        const uint8_t* pixels,
        int width,
        int height,
        int crn,
        ColorMode cm) override;

    CSG_ErrCode Decompress(
        const uint8_t* input,
        size_t inputLen,
        uint8_t* output,
        int width,
        int height,
        int crn,
        ColorMode cm) override;

    // ---- DEFLATE symbol-stream interface ----
    // These output raw literal/length/distance symbols for downstream
    // Huffman encoding (not byte-packed).

    struct Lz77Symbol {
        bool isLiteral;         // true = literal byte, false = match pair
        uint8_t literal;        // Literal byte value (only if isLiteral)
        uint16_t distance;      // Match distance (only if !isLiteral)
        uint16_t length;        // Match length  (only if !isLiteral)
    };

    /// Compress to a sequence of LZ77 symbols for DEFLATE pipeline.
    std::vector<Lz77Symbol> CompressToSymbols(
        const uint8_t* pixels,
        int width,
        int height);

    // ---- Accessors for current CAL parameters ----
    const CSG_LZ77_Cfg& GetCfg() const;
    int LenBits() const;

private:
    // Get CAL configuration
    void UpdateCfg();

    CSG_LZ77_Cfg cfg_;
    int lenBits_;   // Length field bit-width per CAL
};

#endif  // TOOLKITS_INCLUDE_COMPRESS_MINILZ77_H_
