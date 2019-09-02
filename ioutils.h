#ifndef IOUTILS_H
#define IOUTILS_H

#include <stdint.h>
#include <vector>

using namespace std;

class IOUtils
{
public:
    static void concatVector(vector<uint8_t>&, const vector<uint8_t>&);
    static void pushLittleEndianValue(vector<uint8_t>&, uint16_t);
    static void pushLittleEndianValue(vector<uint8_t>&, uint32_t);

private:
    IOUtils();
};

#endif // IOUTILS_H
