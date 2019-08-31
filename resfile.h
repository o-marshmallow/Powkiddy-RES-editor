#ifndef RESFILE_H
#define RESFILE_H

#include <QFile>
#include <QVector>
#include <QHash>
#include <QImage>
#include <QMessageBox>
#include <QByteArray>
#include <stdint.h>

#define RES_HEADER_SIZE 16
#define PIC_HEADER_SIZE  8

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
      uint8_t  unknown;
      char     name[8];
    } FileHeader;

    /* Private members */
    QByteArray          m_data;
    bool                m_parsed;
    QVector<FileHeader> m_headers;
    QHash<int, QImage>  m_images;

    void parseHeaders(const QByteArray&);
    void parseImage(const int);
};

#endif // RESFILE_H
