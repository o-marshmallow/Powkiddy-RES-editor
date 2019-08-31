#ifndef RESFILE_H
#define RESFILE_H

#include <QFile>
#include <QVector>
#include <QHash>
#include <QImage>
#include <QMessageBox>
#include <QByteArray>
#include <QSize>
#include <stdint.h>

#define RES_HEADER_SIZE 16
#define PIC_HEADER_SIZE  8
#define IMG_HAS_ALPHA(TYPE) ((TYPE) & 8)

class ResFile : public QFile {
public:
    ResFile(const QString& path);
    ~ResFile();
    bool resFileOk();
    const QStringList getFileList();
    const QImage& getImage(int);

private:
    /* File headers in RES file */
    typedef struct {
      uint32_t address;
      uint16_t size;
      uint8_t  type;
      char     name[8];
    } FileHeader;

    /* Private members */
    QByteArray          m_data;
    bool                m_parsed;
    QVector<FileHeader> m_headers;
    QHash<int, QImage>  m_images;

    void parseHeaders(const QByteArray&);
    void parseImage(const int);
    QImage rawToAlphaImage(const uint8_t*, const uint32_t, const QSize&);
    QImage rawToOpaqueImage(const uint8_t*, const uint32_t, const QSize&);
};

#endif // RESFILE_H
