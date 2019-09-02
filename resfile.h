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
#include <stdint.h>
#include "respicture.h"

#define RES_HEADER_SIZE 16

class ResFile : public QFile {
public:
    ResFile(const QString& path);
    ~ResFile();
    bool resFileOk();
    const QStringList getFileList();
    const QImage& getImage(const int);
    const QString getImageInfo(const int);

private:

    /* Private members */
    QByteArray          m_data;
    bool                m_parsed;
    QVector<ResPicture> m_images;

    void parseHeaders(const QByteArray&);
};

#endif // RESFILE_H
