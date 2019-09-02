#ifndef RESPICTURE_H
#define RESPICTURE_H

#include <QImage>
#include <QVector>
#include <QMessageBox>
#include <QObject>

#include <vector>
#include <string.h>

#include "zlib_util.h"
#include "ioutils.h"

#define PIC_HEADER_SIZE  8U
#define IMG_HAS_ALPHA(TYPE) ((TYPE) == 8U)

using namespace std;

/* File headers in RES file */
typedef struct {
  uint32_t address;
  uint16_t size;
  uint8_t  type;
  char     name[9];
} FileHeader;

class ResPicture {
public:
    ResPicture();
    ResPicture(FileHeader, const uint8_t*);
    bool isEmpty();
    const QImage& getImage();
    const QString getName();
    const QString getInfo();
    FileHeader    getHeader();
    const QSize&  getSize();
    vector<uint8_t> getCompressedData();
    void replaceImage(const QImage&);

private:
    void parseAlphaImage();
    void parseOpaqueImage();

    bool            m_parsed;
    FileHeader      m_header;
    QImage          m_image;
    QSize           m_dimension;
    vector<uint8_t> m_uncompressed;
    vector<uint8_t> m_compressed;
};

#endif // RESPICTURE_H
