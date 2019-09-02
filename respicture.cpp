#include "respicture.h"

ResPicture::ResPicture()
{

}

ResPicture::ResPicture(FileHeader header, const uint8_t* rawdata) :
    m_header(header), m_uncompressed(), m_compressed()
{
    /* Read image data from raw bytes */
    const uint32_t addr = header.address;
    const int width  = rawdata[addr + 1] << 8 | rawdata[addr];
    const int height = rawdata[addr + 3] << 8 | rawdata[addr + 2];
    const uint size  = static_cast<uint>((rawdata[addr + 7] << 24) |
                                         (rawdata[addr + 6] << 16) |
                                         (rawdata[addr + 5] <<  8) |
                                         (rawdata[addr + 4]));
    m_dimension = QSize(width, height);

    /* Store compressed image data */
    const uint8_t *img = rawdata + addr + PIC_HEADER_SIZE;
    m_compressed.resize(size);
    memcpy(&m_compressed[0], img, size);

    /* Decompress data and store the result */
    uint size_out = static_cast<uint>(width * height);
    if (IMG_HAS_ALPHA(header.type))
        size_out *= 3;
    else
        size_out *= 2;
    m_uncompressed.resize(size_out);
    Zlib::inf(m_compressed, m_uncompressed);
}


bool ResPicture::isEmpty(){
    return !m_parsed;
}

const QImage& ResPicture::getImage() {
    if (!m_parsed) {

        if (m_compressed.size() != (m_header.size - PIC_HEADER_SIZE)) {
            QMessageBox::warning(nullptr, QObject::tr("Wrong image format"),
             QObject::tr("The image size in the header and metadata header don't match.\n"
               "The program will continue taking in account the metadata size"));
        }

        if (IMG_HAS_ALPHA(m_header.type))
            parseAlphaImage();
        else
            parseOpaqueImage();

        m_parsed = true;
    }

    return m_image;
}

const QString ResPicture::getName() {
    return QString(m_header.name);
}

const QString ResPicture::getInfo() {
    return QString(m_header.name) + "\tSize: " +
                   QString::number(m_dimension.width()) +
                   "x" +
                   QString::number(m_dimension.height());
}

void ResPicture::parseAlphaImage() {
    m_image = QImage(m_dimension, QImage::Format::Format_RGBA8888);
    int x = 0;
    int y = 0;

    for (uint32_t i = 0; i < m_uncompressed.size(); i += 3) {
      uint8_t  alpha = m_uncompressed[i + 2];
      uint16_t bytes = static_cast<uint16_t>(m_uncompressed[i + 1] << 8 |
                                             m_uncompressed[i]);

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

      m_image.setPixelColor(x, y,
                            QColor(red, green, blue, alpha));
      x++;
      if (x == m_dimension.width()) {
          x = 0;
          y++;
      }
    }
}

void ResPicture::parseOpaqueImage() {
    m_image = QImage(m_dimension, QImage::Format::Format_RGB32);
    int x = 0;
    int y = 0;

    for (uint32_t i = 0; i < m_uncompressed.size(); i += 2) {
      uint16_t bytes = static_cast<uint16_t>(m_uncompressed[i + 1] << 8 |
                                             m_uncompressed[i]);

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

      m_image.setPixelColor(x, y, QColor(red, green, blue));
      x++;
      if (x == m_dimension.width()) {
          x = 0;
          y++;
      }
    }
}
