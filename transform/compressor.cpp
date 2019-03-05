#include "compressor.h"
#include "encoder.h"

#include <iostream>

#include "variant_digest_manager.h"

#include <xmmintrin.h>

namespace pil {

int ZstdCompressor::Compress(std::shared_ptr<ColumnSet> cset, const PIL_CSTORE_TYPE& field_type, const int compression_level) {
    if(cset.get() == nullptr) return(-1);

    int ret = 0;
    if(field_type == PIL_CSTORE_COLUMN) {
       //std::cerr << "in cstore col: n=" << cset->size() << std::endl;
       for(int i = 0; i < cset->size(); ++i) {
           std::shared_ptr<ColumnStore> tgt = cset->columns[i];

           int64_t in_size = tgt->buffer.length();
           int ret2 = Compress(
                   tgt->buffer.mutable_data(),
                   tgt->buffer.length(),
                   compression_level);

           tgt->compressed_size = ret2;
           memcpy(tgt->buffer.mutable_data(), buffer->mutable_data(), ret2);
           tgt->buffer.UnsafeSetLength(ret2);
           tgt->transformation_args.push_back(std::make_shared<TransformMeta>(PIL_COMPRESS_ZSTD, in_size, ret2));
           tgt->transformation_args.back()->ComputeChecksum(tgt->buffer.mutable_data(), ret2);
           ret += ret2;
       }
       return(ret);
    } else if(field_type == PIL_CSTORE_TENSOR) {
        std::cerr << "not allowed yet" << std::endl;
        exit(1);
    }
    return(ret);
}

int ZstdCompressor::Compress(std::shared_ptr<ColumnSet> cset, const DictionaryFieldType& field, const int compression_level) {
    return(Compress(cset, field.cstore, compression_level));
}

int ZstdCompressor::Compress(const uint8_t* src, const uint32_t n_src, const int compression_level) {
    if(buffer.get() == nullptr) {
        assert(AllocateResizableBuffer(pool_, n_src + 16384, &buffer) == 1);
    }

    if(buffer->capacity() < n_src + 16384){
        //std::cerr << "here in limit=" << n*sizeof(T) << "/" << buffer->capacity() << std::endl;
        assert(buffer->Reserve(n_src + 16384) == 1);
    }

    const int32_t ret = ZSTD_compress(buffer->mutable_data(), buffer->capacity(), src, n_src, compression_level);
    if(ZSTD_isError(ret)){
        std::cerr << "zstd error: " << ZSTD_getErrorString(ZSTD_getErrorCode(ret)) << std::endl;
        return(-1);
    }

    return(ret);
}


int ZstdCompressor::UnsafeDecompress(std::shared_ptr<ColumnStore> cstore, const bool back_copy) {
	const size_t ret = ZSTD_decompress(
							   buffer->mutable_data(),
							   buffer->capacity(),
							   cstore->mutable_data(),
							   cstore->compressed_size);

	if(ZSTD_isError(ret)){
		std::cerr << ZSTD_getErrorString(ZSTD_getErrorCode(ret)) << std::endl;
		return(false);
	}

	if(back_copy) {
	    memcpy(cstore->mutable_data(), buffer->mutable_data(), ret);
	    cstore->uncompressed_size = ret;
	}
	return(ret);
}

int ZstdCompressor::Decompress(std::shared_ptr<ColumnStore> cstore, std::shared_ptr<TransformMeta> meta, const bool back_copy) {
    if(meta->ctype != PIL_COMPRESS_ZSTD) return(-1);

    if(buffer.get() == nullptr) {
        assert(AllocateResizableBuffer(pool_, meta->u_sz + 16384, &buffer) == 1);
    }

    if(buffer->capacity() < meta->u_sz + 8196) {
        buffer->Reserve(meta->u_sz + 8196);
    }

    return(UnsafeDecompress(cstore, back_copy));
}


// quality

int QualityCompressor::Compress(std::shared_ptr<ColumnSet> cset, PIL_CSTORE_TYPE cstore) {
    if(cset.get() == nullptr) return(-1);

    int ret = 0;
    if(cstore == PIL_CSTORE_COLUMN) {
        //std::cerr << "in cstore col" << std::endl;
        for(int i = 0; i < cset->columns.size(); ++i) {
            if(cset->columns[i].get() == nullptr) return(-2);
            std::shared_ptr<ColumnStore> tgt = cset->columns[i];
            uint32_t n_l = tgt->buffer.length();
            int64_t n_in = tgt->buffer.length();
            int ret2 = Compress(tgt->buffer.mutable_data(), tgt->buffer.length(), &n_l, 1);
            tgt->compressed_size = ret2;
            memcpy(tgt->buffer.mutable_data(), buffer->mutable_data(), ret2);
            tgt->buffer.UnsafeSetLength(ret2);
            tgt->transformation_args.push_back(std::make_shared<TransformMeta>(PIL_COMPRESS_RC_QUAL, n_in, ret2));
            tgt->transformation_args.back()->ComputeChecksum(tgt->buffer.mutable_data(), ret2);
            ret += ret2;
        }
    } else if(cstore == PIL_CSTORE_TENSOR) {
        if(cset->size() != 2) return(-3);
        if(cset->columns[0].get() == nullptr || cset->columns[1].get() == nullptr) return(-2);

        static_cast<DeltaEncoder*>(static_cast<Transformer*>(this))->UnsafeEncode(cset->columns[0]);

        // temp
#define compress2 1
#ifdef compress2
        ret += Compress2(cset,cstore);
#else
        //

        // Compress QUALs from Column 1 with the SEQ lengths from Column 0
        int64_t n_in = cset->columns[1]->buffer.length();
        int ret2 = Compress(cset->columns[1]->buffer.mutable_data(),
                        cset->columns[1]->buffer.length(),
                        reinterpret_cast<uint32_t*>(cset->columns[0]->buffer.mutable_data()),
                        cset->columns[0]->n_records);

        memcpy(cset->columns[1]->buffer.mutable_data(), buffer->mutable_data(), ret2);
        cset->columns[1]->compressed_size = ret2;
        cset->columns[1]->buffer.UnsafeSetLength(ret2);
        cset->columns[1]->transformation_args.push_back(std::make_shared<TransformMeta>(PIL_COMPRESS_RC_QUAL, n_in, ret2));
        cset->columns[1]->transformation_args.back()->ComputeChecksum(cset->columns[1]->buffer.mutable_data(), ret2);

        ret += ret2;
#endif

        int64_t n_in0 = cset->columns[0]->buffer.length();
        int ret1 = reinterpret_cast<ZstdCompressor*>(this)->Compress(
                                       cset->columns[0]->buffer.mutable_data(),
                                       cset->columns[0]->buffer.length(),
                                       PIL_ZSTD_DEFAULT_LEVEL);

        memcpy(cset->columns[0]->buffer.mutable_data(), buffer->mutable_data(), ret1);
        cset->columns[0]->compressed_size = ret1;
        cset->columns[0]->buffer.UnsafeSetLength(ret1);
        cset->columns[0]->transformation_args.push_back(std::make_shared<TransformMeta>(PIL_COMPRESS_ZSTD, n_in0, ret1));
        cset->columns[0]->transformation_args.back()->ComputeChecksum(cset->columns[0]->buffer.mutable_data(), ret1);
        ret += ret1;

        // Compress the Nullity bitmap
        if(cset->columns[0]->nullity.get() == nullptr) return(-5); // malformed data

        const uint32_t n_nullity = std::ceil((float)cset->columns[0]->n_records / 32);
        int retNull = static_cast<ZstdCompressor*>(static_cast<Transformer*>(this))->Compress(
                                     cset->columns[0]->nullity->mutable_data(),
                                     n_nullity,
                                     PIL_ZSTD_DEFAULT_LEVEL);

        if(retNull < 0) return(-6); // compression failure
        cset->columns[0]->nullity_u = n_nullity;
        cset->columns[0]->nullity_c = retNull;
        memcpy(cset->columns[0]->nullity->mutable_data(), buffer->mutable_data(), retNull);
        ret += retNull;
        //std::cerr << ">>>>>>>>>>NULLITY=" << n_nullity << "->" << retNull << std::endl;

    } else {
        //std::cerr << "unknown cstore type" << std::endl;
        return(-1);
    }

    return(ret);
}

int QualityCompressor::Compress(const uint8_t* qual,
             const uint32_t n_src,
             uint32_t qlevel,
             RangeCoder* rc,
             FrequencyModel<QMAX>* model_qual)
{
    uint32_t last = 0;
    int delta = 5;
    int i, len2 = n_src;
    int q1 = 0, q2 = 0;
    // Todo: fix illumina hack
    bool illumina_fastq = true;
    int val0 = illumina_fastq ? 59 : 33;
    //int val0 = 0;

    /* Removing "Killer Bees" */
    while (len2 > 0 && qual[len2 - 1] == '#') len2--;

    for (i = 0; i < len2; i++) {
        //std::cerr << i << ": " << (char)qual[i] << "->" << (int)qual[i] << std::endl;
        const uint8_t q = (qual[i] - val0) & (QMAX - 1); // normalize quality value and mask out upper bits
        model_qual[last].EncodeSymbol(rc, q); // encode this symbol in RC

        // previous 2-3 bytes
        //if (QBITS == 12) {
            last = ((MAX(q1, q2) << 6) + q) & ((1 << QBITS) - 1);
        //} else {
        //    last = ((last << 6) + q) & ((1 << QBITS) - 1);
        //}

        //std::cerr << "first last=" << (int)last << std::endl;

        // Branchless tricks.
        if (qlevel > 1) {
            last  += (q1 == q2) << QBITS; // shift in equality between q1 and q2
            delta += (q1 > q) * (q1 - q); // if q1 > q then add the delta (q1 - q)
            last  += (MIN(7 * 8, delta) & 0xf8) << (QBITS - 2); // 0xf8 = 0b11111000
            //std::cerr << "q1 last=" << (int)last << " delta=" << (int)delta << std::endl;
        }

        if (qlevel > 2) {
            last += (MIN(i + 15, 127) & (15 << 3)) << (QBITS + 1);
            //std::cerr << "q2 add=" << (int)(MIN(i + 15, 127) & (15 << 3)) << " and shift= " << (int)(QBITS+1) << std::endl;
            //std::cerr << "q2 total=" << (int)((MIN(i + 15, 127) & (15 << 3)) << (QBITS + 1)) << "/" << (int)(QSIZE*16) << std::endl;
            //std::cerr << "q2 last=" << (int)last << std::endl;
        }

        _mm_prefetch((const char *)&model_qual[last], _MM_HINT_T0);
        q2 = q1; q1 = q;

        //std::cerr << (int)last << "/" << (QSIZE*16) << std::endl;
        assert(last < (QSIZE*16));
    }

    if (n_src != len2)
        model_qual[last].EncodeSymbol(rc, QMAX-1); /* terminator */

    return(1);
}

int QualityCompressor::Compress(uint8_t* qual, const uint32_t n_src, const uint32_t* lengths, const uint32_t n_lengths) {
    if(buffer.get() == nullptr) {
        assert(AllocateResizableBuffer(pool_, n_src + 16384, &buffer) == 1);
    }

    if(buffer->capacity() < n_src + 16384){
        assert(buffer->Reserve(n_src + 16384) == 1);
    }

    // temp
    //return(Compress2(qual, n_src));
    //

    int qlevel = 2; // should be parameterized
    int qsize = QSIZE;
    if (qlevel > 1) qsize *= 16;
    if (qlevel > 2) qsize *= 16;
    FrequencyModel<QMAX>* model_qual = new FrequencyModel<QMAX>[qsize];

    RangeCoder rc;
    rc.StartEncode();
    rc.SetOutput(reinterpret_cast<char*>(buffer->mutable_data()));

    uint32_t cum_offset = 0;
    for(int i = 0; i < n_lengths; ++i){
        Compress(&qual[cum_offset], lengths[i], qlevel, &rc, model_qual);
        cum_offset += lengths[i];
    }
    //Compress(qual, n_src, qlevel, &rc, model_qual);

    rc.FinishEncode();
    //std::cerr << "qual encodings=" << n_src << "->" << rc.size_out() << " (" << (float)n_src/rc.size_out() << "-fold)" << std::endl;

    delete[] model_qual;

    return(rc.OutSize());
}

int QualityCompressor::Compress2(std::shared_ptr<ColumnSet> cset, PIL_CSTORE_TYPE cstore) {
    if(cset.get() == nullptr) return(-1);
    if(cset->size() != 2) return(-2);
    if(cset->columns[0].get() == nullptr || cset->columns[1].get() == nullptr) return(-3);

    if(buffer.get() == nullptr) {
       assert(AllocateResizableBuffer(pool_, cset->columns[1]->buffer.length()*1.1 + 16384, &buffer) == 1);
    }

    if(buffer->capacity() < cset->columns[1]->buffer.length()*1.1 + 16384){
       assert(buffer->Reserve(cset->columns[1]->buffer.length()*1.1 + 16384) == 1);
    }

    int vers = 4;

    size_t out_size = 0;
    std::cerr << "entering qual compressor2: " << vers << "," << 0 << "," << (cset.get() == nullptr) << "," << out_size << std::endl;
    FqzCompQual fqz;
    int64_t n_in = cset->columns[1]->buffer.length();
    std::cerr << "in-size=" << n_in << std::endl;
    fqz.Compress(vers, 0, cset, buffer, out_size);
    std::cerr << "out-size=" << out_size << std::endl;
    memcpy(cset->columns[1]->mutable_data(), buffer->mutable_data(), out_size);
    cset->columns[1]->compressed_size = out_size;
    cset->columns[1]->buffer.UnsafeSetLength(out_size);
    cset->columns[1]->transformation_args.push_back(std::make_shared<TransformMeta>(PIL_COMPRESS_RC_QUAL, n_in, out_size));
    cset->columns[1]->transformation_args.back()->ComputeChecksum(cset->columns[1]->buffer.mutable_data(), out_size);

    return(out_size);
}

int QualityCompressor::Decompress2(std::shared_ptr<ColumnSet> cset, PIL_CSTORE_TYPE cstore) {
    if(cset.get() == nullptr) return(-1);
    if(cset->size() != 2) return(-2);
    if(cset->columns[0].get() == nullptr || cset->columns[1].get() == nullptr) return(-3);
    if(cset->columns[1]->transformation_args.back()->ctype != PIL_COMPRESS_RC_QUAL) return(-4);

    if(buffer.get() == nullptr) {
        assert(AllocateResizableBuffer(pool_, cset->columns[1]->transformation_args.back()->u_sz + 16384, &buffer) == 1);
    }

    if(buffer->capacity() < cset->columns[1]->transformation_args.back()->u_sz + 16384){
        assert(buffer->Reserve(cset->columns[1]->transformation_args.back()->u_sz + 16384) == 1);
    }

    FqzCompQual fqz;

    size_t ret = cset->columns[1]->transformation_args.back()->u_sz;
    fqz.Decompress(cset, buffer, ret);
    memcpy(cset->columns[1]->mutable_data(), buffer->mutable_data(), ret);
    cset->columns[1]->buffer.UnsafeSetLength(ret);

    return(ret);
}

// sequence

int SequenceCompressor::Compress(std::shared_ptr<ColumnSet> cset, PIL_CSTORE_TYPE cstore) {
    if(cset.get() == nullptr) return(-1);
    std::cerr << "sequence compressor" << std::endl;

    int ret = 0;
    if(cstore == PIL_CSTORE_COLUMN) {
        //std::cerr << "in cstore col: COMPUTING BASES" << std::endl;
        for(int i = 0; i < cset->columns.size(); ++i) {
            if(cset->columns[i].get() == nullptr) return(-2);

            std::shared_ptr<ColumnStore> tgt = cset->columns[i];
            uint32_t n_l = tgt->buffer.length();
            int64_t n_in = tgt->buffer.length();
            int ret2 = Compress(tgt->buffer.mutable_data(), tgt->buffer.length(), &n_l, 1);
            tgt->compressed_size = ret2;
            memcpy(tgt->buffer.mutable_data(), buffer->mutable_data(), ret2);
            tgt->buffer.UnsafeSetLength(ret2);
            tgt->transformation_args.push_back(std::make_shared<TransformMeta>(PIL_COMPRESS_RC_BASES, n_in, ret2));
            tgt->transformation_args.back()->ComputeChecksum(tgt->buffer.mutable_data(), ret2);
            ret += ret2;
        }
    } else if(cstore == PIL_CSTORE_TENSOR) {
        if(cset->size() != 2) return(-3);
        if(cset->columns[0].get() == nullptr || cset->columns[1].get() == nullptr) return(-2);

        static_cast<DeltaEncoder*>(static_cast<Transformer*>(this))->UnsafeEncode(cset->columns[0]);

        int64_t n_in = cset->columns[1]->buffer.length();
        int ret2 = Compress(cset->columns[1]->buffer.mutable_data(),
                            cset->columns[1]->buffer.length(),
                            reinterpret_cast<uint32_t*>(cset->columns[0]->buffer.mutable_data()),
                            cset->columns[0]->n_records);

        cset->columns[1]->compressed_size = ret2;
        cset->columns[1]->buffer.UnsafeSetLength(ret2);
        cset->columns[1]->transformation_args.push_back(std::make_shared<TransformMeta>(PIL_COMPRESS_RC_BASES,n_in,ret2));
        memcpy(cset->columns[1]->buffer.mutable_data(), buffer->mutable_data(), ret2);
        cset->columns[1]->transformation_args.back()->ComputeChecksum(cset->columns[1]->buffer.mutable_data(), ret2);
        ret += ret2;

        int64_t n_in0 = cset->columns[0]->buffer.length();
        int ret1 = reinterpret_cast<ZstdCompressor*>(this)->Compress(
                                       cset->columns[0]->buffer.mutable_data(),
                                       cset->columns[0]->buffer.length(),
                                       PIL_ZSTD_DEFAULT_LEVEL);

        memcpy(cset->columns[0]->buffer.mutable_data(), buffer->mutable_data(), ret1);
        cset->columns[0]->compressed_size = ret1;
        cset->columns[0]->buffer.UnsafeSetLength(ret1);
        cset->columns[0]->transformation_args.push_back(std::make_shared<TransformMeta>(PIL_COMPRESS_ZSTD, n_in0, ret1));
        cset->columns[0]->transformation_args.back()->ComputeChecksum(cset->columns[0]->buffer.mutable_data(), ret1);

        ret += ret1;

        // Compress the Nullity bitmap
        if(cset->columns[0]->nullity.get() == nullptr) return(-5); // malformed data

        const uint32_t n_nullity = std::ceil((float)cset->columns[0]->n_records / 32);
        int retNull = static_cast<ZstdCompressor*>(static_cast<Transformer*>(this))->Compress(
                                     cset->columns[0]->nullity->mutable_data(),
                                     n_nullity,
                                     PIL_ZSTD_DEFAULT_LEVEL);

        if(retNull < 0) return(-6); // compression failure
        cset->columns[0]->nullity_u = n_nullity;
        cset->columns[0]->nullity_c = retNull;
        memcpy(cset->columns[0]->nullity->mutable_data(), buffer->mutable_data(), retNull);
        ret += retNull;

    } else {
        //std::cerr << "unknown cstore type" << std::endl;
        return(-1);
    }

    std::cerr << "done sequence compressor" << std::endl;

    return(ret);
}

int SequenceCompressor::Compress(const uint8_t* bases, const uint32_t n_src, const uint32_t* lengths, const uint32_t n_lengths) {
    if(buffer.get() == nullptr) {
        assert(AllocateResizableBuffer(pool_, n_src + 65536, &buffer) == 1);
    }

    if(buffer->capacity() < n_src + 65536) {
        assert(buffer->Reserve(n_src + 65536) == 1);
    }

    // slevel in range [1,9);
    int slevel = 3; // Number of bases of sequence context.
    int NS = 7 + slevel;
    const int NS_MASK = ((1 << (2*NS)) - 1);
    /* Corresponds to a 12-mer word that doesn't occur in human genome. */
    int last  = 0x7616c7 & NS_MASK;
    int last2 = (0x2c6b62ff >> (32 - 2*NS)) & NS_MASK;
    BaseModel<uint16_t>* model_seq16 = new BaseModel<uint16_t>[1 << (2 * NS)];
    FrequencyModel<2> model_null(2);

    int L[256];
    /* ACGTN* */
    for (int i = 0; i < 256; i++) L[i] = 0;
    L['A'] = L['a'] = 0;
    L['C'] = L['c'] = 1;
    L['G'] = L['g'] = 2;
    L['T'] = L['t'] = 3;
    L['N'] = L['n'] = 4; // is this correct?

    RangeCoder rc;
    rc.StartEncode();
    rc.SetOutput(reinterpret_cast<char*>(buffer->mutable_data()));

    uint32_t cum_offset = 0;
    for(int i = 1; i < n_lengths; ++i) {

        for (int j = 0; j < lengths[i]; ++j) {
            //if(i < 3) {
            //    std::cerr << bases[cum_offset + j];
            //}
            const uint8_t b = L[(uint8_t)bases[cum_offset + j]];
            if(b == 4) model_null.EncodeSymbol(&rc, 1);
            else {
                model_null.EncodeSymbol(&rc, 0);
                //const uint8_t b = L[(uint8_t)bases[cum_offset + j]];
                model_seq16[last].EncodeSymbol(&rc, b);
                last = (last*4 + b) & NS_MASK;
                //volatile int p = *(int *)&model_seq16[last];
                _mm_prefetch((const char *)&model_seq16[last], _MM_HINT_T0);
            }
        }
        //if(i < 3) {
        //    std::cerr << std::endl;
        //}
        //last  = 0x7616c7 & NS_MASK;
        cum_offset += lengths[i];
    }

   rc.FinishEncode();
   //std::cerr << "BASES encodings=" << n_src << "->" << rc.OutSize() << " (" << (float)n_src/rc.OutSize() << "-fold)" << std::endl;
   delete[] model_seq16;

   return(rc.OutSize());
}

int SequenceCompressor::Decompress(std::shared_ptr<ColumnSet> cset, const DictionaryFieldType& field) {
    if(cset.get() == nullptr) return(-1);
    if(cset->size() != 2) return(-2);
    if(cset->columns[0].get() == nullptr || cset->columns[1].get() == nullptr) return(-3);
    if(cset->columns[1]->transformation_args.back()->ctype != PIL_COMPRESS_RC_BASES) return(-4);

    if(buffer.get() == nullptr) {
        assert(AllocateResizableBuffer(pool_, cset->columns[1]->transformation_args.back()->u_sz + 16384, &buffer) == 1);
    }

    if(buffer->capacity() < cset->columns[1]->transformation_args.back()->u_sz + 16384){
        //std::cerr << "here in limit=" << n*sizeof(T) << "/" << buffer->capacity() << std::endl;
        assert(buffer->Reserve(cset->columns[1]->transformation_args.back()->u_sz + 16384) == 1);
    }


    int slevel = 3; // Number of bases of sequence context.
    int NS = 7 + slevel;
    const int NS_MASK = ((1 << (2*NS)) - 1);
    BaseModel<uint16_t>* model_seq16 = new BaseModel<uint16_t>[1 << (2 * NS)];
    FrequencyModel<2> model_null(2);
    uint8_t* out = buffer->mutable_data();

    const char* dec = "ACGTN";
    int last  = 0x7616c7 & NS_MASK;
    int last2 = (0x2c6b62ff >> (32 - 2*NS)) & NS_MASK;

    const uint32_t u_sz = cset->columns[1]->transformation_args.back()->u_sz;

    RangeCoder rc;
    rc.SetInput(reinterpret_cast<char*>(cset->columns[1]->mutable_data()));
    rc.StartDecode();

    for (int i = 0; i < u_sz; i++) {
        //if(i % 100 == 0 && i != 0) last  = 0x7616c7 & NS_MASK;
        //if(i == 100) std::cerr << std::endl;

        const uint8_t null = model_null.DecodeSymbol(&rc);
        if(null == 0) {
            const uint8_t b = model_seq16[last].DecodeSymbol(&rc);
            out[i] = dec[b];

            //if(i < 200) std::cerr << dec[b];
            last = (last*4 + b) & NS_MASK;
            _mm_prefetch((const char *)&model_seq16[last], _MM_HINT_T0);
        } else {
            out[i] = 'N';
            //if(i < 200) std::cerr << 'N';
        }
        /*
        if (both_strands) {
            int b2 = last2 & 3;
            last2 = last2/4 + ((3-b) << (2*NS-2));
            _mm_prefetch((const char *)&model_seq16[last2], _MM_HINT_T0);
            model_seq16[last2].UpdateSymbol(b2);
        }
        */

    }
    rc.FinishDecode();
    delete[] model_seq16;
    std::cerr << std::endl;

    memcpy(cset->columns[1]->mutable_data(), buffer->mutable_data(), u_sz);
    cset->columns[1]->buffer.UnsafeSetLength(u_sz);

    return(u_sz);
}


}
