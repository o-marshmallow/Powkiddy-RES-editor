#ifndef RESPICTURE_H
#define RESPICTURE_H

#include <vector>
#include <string.h>
#include <QImage>
#include <QVector>
#include <QMessageBox>
#include <QObject>
#include "zlib_util.h"

#define PIC_HEADER_SIZE  8
#define IMG_HAS_ALPHA(TYPE) ((TYPE) & 8)

/* File headers in RES file */
typedef struct {
  uint32_t address;
  uint16_t size;
  uint8_t  type;
  char     name[8];
} FileHeader;

class ResPicture {
public:
    ResPicture();
    ResPicture(FileHeader, const uint8_t*);
    bool isEmpty();
    const QImage& getImage();
    const QString getName();
    const QString getInfo();

private:
    void parseImage(const int);
    void parseAlphaImage();
    void parseOpaqueImage();

    bool             m_parsed;
    FileHeader       m_header;
    QImage           m_image;
    QSize            m_dimension;
    std::vector<uint8_t> m_uncompressed;
    std::vector<uint8_t> m_compressed;
};

#endif // RESPICTURE_H
