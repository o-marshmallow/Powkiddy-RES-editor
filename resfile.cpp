#include "resfile.h"
#include "zlib_util.h"
#include <stdio.h>

ResFile::ResFile(const QString& path) : QFile(path), m_data(), m_parsed(false), m_headers()
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

    for(int i = 0; i < m_headers.size(); i++) {
        names.push_back(m_headers[i].name);
    }

    return names;
}

const QImage& ResFile::getImage(const int index) {
    if (m_images[index].isNull())
        parseImage(index);

    return m_images[index];
}

void ResFile::parseImage(const int index) {
    const FileHeader header = m_headers[index];
    const uint32_t addr = header.address;
    uint8_t* data = (uint8_t*) m_data.data();

    const uint16_t width = static_cast<uint16_t>(data[addr + 1] << 8 |
                                                 data[addr]);
    const uint16_t height = static_cast<uint16_t>(data[addr + 3] << 8 |
                                                  data[addr + 2]);
    const uint32_t size = static_cast<uint16_t>(data[addr + 7] << 24 |
                                                data[addr + 6] << 16 |
                                                data[addr + 5] << 8  |
                                                data[addr + 4]);
    if (size != (header.size - PIC_HEADER_SIZE)) {
        QMessageBox::warning(nullptr, tr("Wrong image format"),
        tr("The image size in the header and metadata header don't match.\n"
           "The program will continue taking in account the metadata size"));
    }

    /* Decompress data */
    uint8_t *img = data + addr + PIC_HEADER_SIZE;
    const uint32_t size_out = width * height * 3;
    uint8_t *uncompressed = new uint8_t[size_out];
    Zlib::inf(img, size, uncompressed, size_out);
    img = uncompressed;

    QImage qimg(width, height, QImage::Format::Format_RGBA8888);
    int x = 0;
    int y = 0;

    for (uint32_t i = 0; i < size_out; i += 3) {
      uint8_t  alpha = static_cast<uint8_t>(img[i + 2]);
      uint16_t bytes = static_cast<uint16_t>((img[i + 1] << 8) | img[i]);

      uint8_t blue   = static_cast<uint8_t>(bytes & 0x1f);
      /* We have to dump the fifth bit, so let's shift the byte by
       * 6 instead of 5. */
      uint8_t green = static_cast<uint8_t>((bytes >> 6)  & 0x1f);
      uint8_t red   = static_cast<uint8_t>((bytes >> 11) & 0x1f);

      /* Convert R, G and B to 8-bit values */
      const float ratio = 8.2258f;
      blue  = static_cast<uint8_t>(blue  * ratio);
      green = static_cast<uint8_t>(green * ratio);
      red   = static_cast<uint8_t>(red   * ratio);

      qimg.setPixelColor(x, y,
                         QColor(red, green, blue, alpha));
      x++;
      if (x == width) {
          x = 0;
          y++;
      }
    }

    delete img;
    m_images[index] = qimg;
}

void ResFile::parseHeaders(const QByteArray& bytes) {
  uint16_t i = 0;
  const uint8_t *data = (const uint8_t*) bytes.data();
  int count = (data[5] << 8) | data[4];

  m_headers.clear();

  /* Skip RES file header */
  data += RES_HEADER_SIZE;

  for (i = 0; i < count; i++) {
    FileHeader header;
    header.address = static_cast<u_int32_t>(
                         (data[3] << 24) | (data[2] << 16) |
                         (data[1] << 8)  | (data[0] << 0));
    header.size = static_cast<uint16_t>((data[5] << 8) | data[4]);
    header.unknown = static_cast<uint8_t>(data[6]);
    memcpy(header.name, &data[7], sizeof(header.name));
    m_headers.push_back(header);
    data += RES_HEADER_SIZE;
  }

  m_parsed = true;
}


