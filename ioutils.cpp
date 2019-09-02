#include "ioutils.h"

IOUtils::IOUtils(){}

void IOUtils::pushLittleEndianValue(vector<uint8_t>& stream, uint16_t value) {
    stream.push_back(value & 0xff);
    stream.push_back((value >> 8) & 0xff);
}

void IOUtils::pushLittleEndianValue(vector<uint8_t>& stream, uint32_t value) {
    stream.push_back(value & 0xff);
    stream.push_back((value >>  8) & 0xff);
    stream.push_back((value >> 16) & 0xff);
    stream.push_back((value >> 24) & 0xff);
}

void IOUtils::concatVector(vector<uint8_t>& v1, const vector<uint8_t>& v2) {
    v1.insert(v1.end(), v2.begin(), v2.end());
}
