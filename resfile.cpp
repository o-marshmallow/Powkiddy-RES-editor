#include "resfile.h"
#include "zlib_util.h"
#include <stdio.h>

ResFile::ResFile(const QString& path) : QFile(path), m_data(), m_parsed(false)
{
    if (!exists())
        return;

    /* Open and read the file */
    if (!this->open(QIODevice::ReadOnly))
        return;

    m_data = this->readAll();

    /* Check read data and expected header */
    const QByteArray header("RES\x19", 4);
    if (m_data.isNull() || m_data.isEmpty() || !m_data.startsWith(header))
        return;

    parseHeaders(m_data);
}

ResFile::~ResFile() { }

bool ResFile::resFileOk() {
    return m_parsed;
}

const QStringList ResFile::getFileList() {
    QStringList names;

    for(int i = 0; i < m_images.size(); i++) {
        names.push_back(m_images[i].getName());
    }

    return names;
}

const QImage& ResFile::getImage(const int index) {
    if (index < m_images.size())
        return m_images[index].getImage();
    else
        return QImage();
}


const QString ResFile::getImageInfo(const int index) {
    if (index < m_images.size())
        return m_images[index].getInfo();
    else
        return QString();
}

bool ResFile::importSizeMatch(int index, const QSize& size) {
    if (index < m_images.size())
        return m_images[index].getSize() == size;
    else
        return false;
}

bool ResFile::importImage(int index, const QImage& image) {
    if (index < m_images.size()) {
        m_images[index].replaceImage(image);
        return true;
    } else {
        return false;
    }
}


void ResFile::parseHeaders(const QByteArray& bytes) {
  /* This first raw first pointer will be used to parse the headers */
  const uint8_t *data     = (const uint8_t*) bytes.data();
  /* this second one will be passed to the ResPicture class */
  const uint8_t *rawbytes = data;
  int count = (data[5] << 8) | data[4];

  /* Clear images already created */
  m_images.clear();

  /* Skip RES file header */
  data += RES_HEADER_SIZE;

  for (int i = 0; i < count; i++) {
    FileHeader header;
    header.address = static_cast<u_int32_t>(
                         (data[3] << 24) | (data[2] << 16) |
                         (data[1] << 8)  | (data[0] << 0));
    header.size = static_cast<uint16_t>((data[5] << 8) | data[4]);
    header.type = static_cast<uint8_t>(data[6]);
    memcpy(header.name, &data[7], sizeof(header.name));

    /* Create the image from the given header */
    m_images.push_back(ResPicture(header, rawbytes));

    data += RES_HEADER_SIZE;
  }

  m_parsed = true;
}

bool ResFile::save(const QString& path) {
    bool ok = true;
    vector<uint8_t> resheader {'R', 'E', 'S', 0x19};
    vector<uint8_t> endheader {0x00, 0xC0, 0x00, 0x02, 0x00,
                               0x00, 0x00, 0x00, 0x00, 0x00};
    vector<uint8_t> allfiles;

    /* Retrieve all the compressed data */
    uint16_t imgcount = static_cast<uint16_t>(m_images.size());
    IOUtils::pushLittleEndianValue(resheader, imgcount);
    IOUtils::concatVector(resheader, endheader);

    uint32_t next_image_addr = RES_HEADER_SIZE +
              (static_cast<uint32_t>(m_images.size()) * RES_HEADER_SIZE);

    for (int i = 0; i < m_images.size(); i++) {
        /* Generate the compressed data for the image, including its header! */
        const std::vector<uint8_t> compressed =
            m_images[i].getCompressedData();

        /* Generate the header */
        FileHeader header = m_images[i].getHeader();
        header.address = next_image_addr;
        header.size    = static_cast<uint16_t>(compressed.size());
        writeHeader(header, resheader);

        /* Add compressed data to the final vector */
        IOUtils::concatVector(allfiles, compressed);

        next_image_addr += compressed.size();
    }

    /* Open the file and the stream associated to it */
    QFile file(path);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QDataStream stream(&file);

    stream.writeRawData((const char*) resheader.data(), resheader.size());
    stream.writeRawData((const char*) allfiles.data(), allfiles.size());

    file.close();
    return ok;
}

void ResFile::writeHeader(const FileHeader& header, vector<uint8_t>& stream) {
    IOUtils::pushLittleEndianValue(stream, header.address);
    IOUtils::pushLittleEndianValue(stream, header.size);
    stream.push_back(header.type);
    stream.insert(stream.end(), begin(header.name), end(header.name));
}
