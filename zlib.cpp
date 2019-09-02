#include "zlib_util.h"

Zlib::Zlib()
{
}

int Zlib::def(std::vector<uint8_t>& decompressed,
              std::vector<uint8_t>& compressed,
              int level) {
    int ret;
    z_stream strm;

    strm.zalloc   = Z_NULL;
    strm.zfree    = Z_NULL;
    strm.opaque   = Z_NULL;
    ret = deflateInit(&strm, level);
    if (ret != Z_OK)
        return ret;

    strm.avail_in = decompressed.size();
    strm.next_in  = decompressed.data();

    strm.avail_out = compressed.size();
    strm.next_out  = compressed.data();

    ret = deflate(&strm, Z_FINISH);
    assert(ret != Z_STREAM_ERROR);
    switch (ret) {
        case Z_NEED_DICT:
            ret = Z_DATA_ERROR;
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
            deflateEnd(&strm);
            return ret;
    }

    if (ret == Z_STREAM_END)
        compressed.resize(strm.total_out);

    deflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

int Zlib::inf(std::vector<uint8_t>& compressed,
              std::vector<uint8_t>& decompressed) {
    int ret;
    z_stream strm;

    strm.zalloc   = Z_NULL;
    strm.zfree    = Z_NULL;
    strm.opaque   = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK)
        return ret;

    strm.avail_in = compressed.size();
    if (strm.avail_in == 0)
        return Z_DATA_ERROR;
    strm.next_in = compressed.data();

    strm.avail_out = decompressed.size();
    strm.next_out = decompressed.data();
    ret = inflate(&strm, Z_NO_FLUSH);
    assert(ret != Z_STREAM_ERROR);
    switch (ret) {
        case Z_NEED_DICT:
            ret = Z_DATA_ERROR;
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
            inflateEnd(&strm);
            return ret;
    }

    inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}
