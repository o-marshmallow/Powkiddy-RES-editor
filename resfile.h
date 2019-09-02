#ifndef RESFILE_H
#define RESFILE_H

#include <QObject>
#include <QFile>
#include <QVector>
#include <QHash>
#include <QImage>
#include <QMessageBox>
#include <QByteArray>
#include <QSize>
#include <QDataStream>

#include <stdint.h>
#include <vector>

#include "respicture.h"
#include "ioutils.h"

#define RES_HEADER_SIZE 16U

using namespace std;

class ResFile : public QFile {
public:
    ResFile(const QString& path);
    ~ResFile();
    bool resFileOk();
    bool save(const QString&);
    const QStringList getFileList();
    const QImage& getImage(const int);
    const QString getImageInfo(const int);
    bool importSizeMatch(int, const QSize&);
    bool importImage(int, const QImage&);

private:

    /* Private members */
    QByteArray          m_data;
    bool                m_parsed;
    QVector<ResPicture> m_images;

    void parseHeaders(const QByteArray&);
    static void writeHeader(const FileHeader&, vector<uint8_t>&);
};

#endif // RESFILE_H
