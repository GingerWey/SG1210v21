// Copyright 2026 Wey. Silver Grid. All rights reserved.
// CSG Toolkits — Decoder implementation
// ---------------------------------------------------------------------------

#include "CSGDecoder.h"
#include "Compress/RLE.h"
#include "Compress/Huffman.h"
#include "Compress/MiniLZ77.h"
#include <cstring>
#include <algorithm>
#include "CSGCommon.h"

// ============================================================================
// Construction
// ============================================================================

CSGDecoder::CSGDecoder() = default;

// ============================================================================
// DecompressData — decompress picture data to raw pixel bytes
// ============================================================================

CSG_ErrCode CSGDecoder::DecompressData(const uint8_t* compData,
                                        size_t compLen,
                                        const CSGPicture& header,
                                        std::vector<uint8_t>& rawOutput) {
    auto algo = header.GetCompressAlgorithm();
    int cal  = header.GetCompressLevel();
    auto cm  = static_cast<ColorMode>(header.colorMode);
    int crn  = header.crn;
    int pixelCount = header.width * header.height;
    int bpc  = BytesPerColor(cm);
    int bpi  = (crn > 0) ? BitsPerIndex(crn) : 0;

    int rawStride = (crn > 0) ? 1 : bpc;
    int expectedRawSize = pixelCount * rawStride;
    rawOutput.resize(expectedRawSize);

    if (compLen == 0)
        return CSG_ErrCode::kErrCompStreamEmpty;

    if (algo == CompressAlgorithm::kNone) {
        // No compression — raw bit-packed (CRN>0) or CRM bytes (CRN=0)
        if (crn > 0) {
            // Bit-unpack CRI indices
            BitUnpacker bu(compData, compLen);
            for (int i = 0; i < pixelCount; ++i) {
                if (bu.BitsRemaining() < static_cast<size_t>(bpi))
                    return CSG_ErrCode::kErrBitpackTrunc;
                rawOutput[i] = bu.UnpackBits(bpi);
            }
        } else {
            // Raw CRM bytes
            if (compLen < static_cast<size_t>(expectedRawSize))
                return CSG_ErrCode::kErrBitpackTrunc;
            std::memcpy(rawOutput.data(), compData, expectedRawSize);
        }
        return CSG_ErrCode::kOk;
    }

    if (algo == CompressAlgorithm::kDEFLATE) {
        // DEFLATE = MiniLZ77 output bytes → Huffman (VP-style)
        auto lzBytes = HuffmanCodec::DecompressRaw(compData, compLen);
        if (lzBytes.empty()) return CSG_ErrCode::kErrCompStreamEmpty;

        MiniLZ77Codec lz77;
        return lz77.Decompress(lzBytes.data(), lzBytes.size(),
                               rawOutput.data(), header.width, header.height,
                               header.crn, static_cast<ColorMode>(header.colorMode));
    }

    // Standalone codecs (RLE, Huffman, MiniLZ77)
    auto codec = CreateCodec(algo, static_cast<uint8_t>(cal));
    if (!codec)
        return CSG_ErrCode::kErrCasInvalid;

    return codec->Decompress(compData, compLen, rawOutput.data(),
                              header.width, header.height,
                              crn, cm);
}

// ============================================================================
// ConvertToRGBA — raw CRM/CRI + palette → RGBA pixel array
// ============================================================================

CSG_ErrCode CSGDecoder::ConvertToRGBA(
    const uint8_t* rawData,
    const CSGPicture& header,
    const std::vector<uint8_t>& palette,
    std::vector<uint8_t>& rgbaOutput) {

    auto cm = static_cast<ColorMode>(header.colorMode);
    int crn = header.crn;
    int pixelCount = header.width * header.height;
    int bpc = BytesPerColor(cm);

    rgbaOutput.resize(pixelCount * 4);

    for (int i = 0; i < pixelCount; ++i) {
        VxARGB px;
        bool isTransparent = false;

        if (crn > 0) {
            // Palette mode: rawData[i] is a CRI palette index
            uint8_t cri = rawData[i];
            if (cri == 0) {
                // CRI=0 is transparent
                isTransparent = true;
            }
            // Convert CRI to palette entry
            int palOffset = cri * bpc;
            if (palOffset + bpc <= static_cast<int>(palette.size())) {
                px = UnpackColor(palette.data() + palOffset, cm);
            }
        } else {
            // True-color mode: rawData[i*bpc] is CRM-encoded
            int offset = i * bpc;
            px = UnpackColor(rawData + offset, cm);

            // Transparent if pixel matches palette[0]
            if (palette.size() >= static_cast<size_t>(bpc)) {
                VxARGB transColor = UnpackColor(palette.data(), cm);
                if (px == transColor)
                    isTransparent = true;
            }
        }

        // Write RGBA
        uint8_t* out = rgbaOutput.data() + i * 4;
        out[0] = px.r;
        out[1] = px.g;
        out[2] = px.b;
        out[3] = isTransparent ? 0 : px.a;
    }

    return CSG_ErrCode::kOk;
}

// ============================================================================
// DecodePicture — decode a single CSGPicture from encoded bytes
// ============================================================================

DecoderResult CSGDecoder::DecodePicture(const uint8_t* data, size_t len) {
    DecoderResult result;

    // Parse picture header
    CSGPicture header;
    CSG_ErrCode err = DecodePictureHeader(data, len, header);
    if (err != CSG_ErrCode::kOk) {
        result.error = err;
        result.errorDetail = CsgGetErrStr(err);
        return result;
    }

    result.width     = header.width;
    result.height    = header.height;
    result.colorMode = static_cast<ColorMode>(header.colorMode);
    result.crn       = header.crn;
    result.algo      = header.GetCompressAlgorithm();
    result.cal       = header.GetCompressLevel();

    int bpc = BytesPerColor(static_cast<ColorMode>(header.colorMode));
    int palBytes = (header.crn > 0)
        ? header.crn * bpc
        : bpc;

    // Read palette
    uint32_t ppos = header.ppos;
    if (ppos + palBytes > len) {
        result.error = CSG_ErrCode::kErrPaletteRead;
        result.errorDetail = "Palette data beyond buffer";
        return result;
    }
    result.palette.assign(data + ppos, data + ppos + palBytes);

    // Decompress pixel data
    uint32_t dpos = header.dpos;
    size_t compLen = header.size - dpos;
    if (dpos + compLen > len) {
        result.error = CSG_ErrCode::kErrDposOverflow;
        result.errorDetail = "Pixel data beyond buffer";
        return result;
    }

    std::vector<uint8_t> rawData;
    err = DecompressData(data + dpos, compLen, header, rawData);
    if (err != CSG_ErrCode::kOk) {
        result.error = err;
        result.errorDetail = CsgGetErrStr(err);
        return result;
    }

    // Convert to RGBA
    err = ConvertToRGBA(rawData.data(), header, result.palette, result.pixels);
    if (err != CSG_ErrCode::kOk) {
        result.error = err;
        result.errorDetail = CsgGetErrStr(err);
        return result;
    }

    result.error = CSG_ErrCode::kOk;
    return result;
}

// ============================================================================
// DecodeAtlasPicture — decode one picture from an atlas
// ============================================================================

DecoderResult CSGDecoder::DecodeAtlasPicture(const uint8_t* atlasData,
                                              size_t atlasLen,
                                              int pictureIndex) {
    DecoderResult result;

    CSGHeader header;
    CSG_ErrCode err = DecodeAtlasHeader(atlasData, atlasLen, header);
    if (err != CSG_ErrCode::kOk) {
        result.error = err;
        result.errorDetail = CsgGetErrStr(err);
        return result;
    }

    if (pictureIndex < 0
        || pictureIndex >= static_cast<int>(header.GetPicCount())) {
        result.error = CSG_ErrCode::kErrOutOfRange;
        result.errorDetail = "Picture index out of range";
        return result;
    }

    uint32_t offset = header.GetPicOffset(pictureIndex);
    if (offset >= atlasLen) {
        result.error = CSG_ErrCode::kErrOffsetTableBroken;
        result.errorDetail = "Offset beyond file bounds";
        return result;
    }

    return DecodePicture(atlasData + offset, atlasLen - offset);
}

// ============================================================================
// DecodeAtlas — decode all pictures from an atlas
// ============================================================================

std::vector<DecoderResult> CSGDecoder::DecodeAtlas(const uint8_t* atlasData,
                                                    size_t atlasLen) {
    std::vector<DecoderResult> results;

    CSGHeader header;
    CSG_ErrCode err = DecodeAtlasHeader(atlasData, atlasLen, header);
    if (err != CSG_ErrCode::kOk) {
        DecoderResult r;
        r.error = err;
        r.errorDetail = CsgGetErrStr(err);
        results.push_back(r);
        return results;
    }

    int picCount = static_cast<int>(header.GetPicCount());
    for (int i = 0; i < picCount; ++i) {
        uint32_t offset = header.GetPicOffset(i);
        if (offset >= atlasLen) {
            DecoderResult r;
            r.error = CSG_ErrCode::kErrOffsetTableBroken;
            r.errorDetail = "Offset " + std::to_string(offset)
                          + " beyond bounds";
            results.push_back(r);
            continue;
        }
        results.push_back(DecodePicture(atlasData + offset,
                                        atlasLen - offset));
    }

    return results;
}

// ============================================================================
// Streaming decoder API (for MCU use)
// ============================================================================

// Streaming decoder API (for MCU use)
// ============================================================================
// Usage:
//   CSGDecoderState st;
//   csgDecodeInit(&st, &pic, lineBuf);
//   st.stream = compressedData;   // point to compressed data in Flash
//   st.window = historyBuf;       // point to output-history buffer
//   st.windowSize = sizeof(historyBuf);  // must be >= image total pixels
//   while (st.pixelsDecoded < total) {
//       csgDecodePixels(&st, out, 32);   // decode up to 32 pixels at a time
//   }

CSG_ErrCode CsgDecodeInit(CSGDecoderState* state,
                           const CSGPicture* pic,
                           uint8_t* lineBuf,
                           const uint8_t* palette,
                           ColorMode outMode) {
    if (!state || !pic || !lineBuf)
        return CSG_ErrCode::kErrSrcImgNull;

    state->stream  = nullptr;
    state->lineBuf = lineBuf;
    state->width   = pic->width;
    state->height  = pic->height;
    state->cal     = pic->GetCompressLevel();
    state->pixelsDecoded = 0;
    state->bitBuf   = 0;
    state->bitCount = 8;
    state->linePos  = 0;
    state->rowCount = 0;
    state->windowPos = 0;

    state->bpc     = BytesPerColor(outMode);
    state->palette = palette;
    state->crn     = pic->crn;
    state->bpi     = (pic->crn > 0) ? BitsPerIndex(pic->crn) : 0;

    // Store transparent color
    if (palette && state->bpc <= 4) {
        for (int i = 0; i < state->bpc; ++i)
            state->transparent[i] = palette[i];  // palette[0] = transparent
    } else {
        state->transparent[0] = state->transparent[1] = 0;
        state->transparent[2] = state->transparent[3] = 0;
    }

    return CSG_ErrCode::kOk;
}

CSG_ErrCode CsgDecodePixels(CSGDecoderState* state,
                             uint8_t* output,
                             int requestedPixels) {
    if (!state || !output || !state->stream || !state->window)
        return CSG_ErrCode::kErrSrcImgNull;
    if (requestedPixels <= 0)
        return CSG_ErrCode::kOk;

    int totalPixels = state->width * state->height;
    int remaining   = totalPixels - state->pixelsDecoded;
    int toDecode    = Min(requestedPixels, remaining);
    if (toDecode == 0) return CSG_ErrCode::kOk;

    const uint8_t* input = state->stream;
    int     ip      = state->windowPos;
    uint8_t ctrl    = static_cast<uint8_t>(state->bitBuf);
    int     tokenIdx = state->bitCount;
    int     outPos  = state->pixelsDecoded;
    uint8_t* win    = state->window;
    int     bpc     = state->bpc;
    const uint8_t* pal = state->palette;
    int     dec     = 0;   // pixels decoded this call

    while (dec < toDecode) {
        if (tokenIdx >= 8) {
            ctrl     = input[ip++];
            tokenIdx = 0;
        }

        uint8_t cri;
        if (ctrl & (1u << tokenIdx)) {
            int encLen   = input[ip++];
            int encDist  = input[ip++];
            int matchLen = encLen + 3;
            int dist     = encDist + 1;
            if (dist > outPos) return CSG_ErrCode::kErrLz77DistErr;
            int ref = outPos - dist;
            for (int j = 0; j < matchLen && dec < toDecode; ++j) {
                cri = win[ref + j];
                win[outPos] = cri;
                // CRM lookup
                if (pal && bpc > 0) {
                    for (int k = 0; k < bpc; ++k)
                        output[dec * bpc + k] = pal[cri * bpc + k];
                } else {
                    output[dec] = cri;
                }
                ++outPos; ++dec;
            }
        } else {
            cri = input[ip++];
            win[outPos] = cri;
            if (pal && bpc > 0) {
                for (int k = 0; k < bpc; ++k)
                    output[dec * bpc + k] = pal[cri * bpc + k];
            } else {
                output[dec] = cri;
            }
            ++outPos; ++dec;
        }
        ++tokenIdx;
    }

    state->pixelsDecoded = outPos;
    state->bitBuf   = ctrl;
    state->bitCount = tokenIdx;
    state->windowPos = ip;
    state->linePos += dec;
    if (state->linePos >= state->width) {
        state->linePos -= state->width;
        ++state->rowCount;
    }

    return CSG_ErrCode::kOk;
}
