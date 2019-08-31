#include "zlib_util.h"

Zlib::Zlib()
{
}

int Zlib::def(uint8_t* in, uint32_t size_in, uint8_t* out, uint32_t size_out, int level) {
    return Z_DATA_ERROR;
}

int Zlib::inf(uint8_t* in, uint32_t size_in, uint8_t* out, uint32_t size_out) {
    int ret;
    z_stream strm;

    /* allocate inflate state */
    strm.zalloc   = Z_NULL;
    strm.zfree    = Z_NULL;
    strm.opaque   = Z_NULL;
    strm.avail_in = 0;
    strm.next_in  = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK)
        return ret;

    /* decompress until deflate stream ends or end of file */
    strm.avail_in = size_in;
    if (strm.avail_in == 0)
        return Z_DATA_ERROR;
    strm.next_in = in;

    /* run inflate() on input */
    strm.avail_out = size_out;
    strm.next_out = out;
    ret = inflate(&strm, Z_NO_FLUSH);
    assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
    switch (ret) {
        case Z_NEED_DICT:
            ret = Z_DATA_ERROR;     /* and fall through */
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
            (void)inflateEnd(&strm);
            return ret;
    }

    /* clean up and return */
    (void)inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}
