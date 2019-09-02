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
    uint size  = static_cast<uint>((rawdata[addr + 7] << 24) |
                                   (rawdata[addr + 6] << 16) |
                                   (rawdata[addr + 5] <<  8) |
                                   (rawdata[addr + 4]));

    if (header.type == 0x5) {
        /* No size in file header type */
        size = header.size;
    }

    m_dimension = QSize(width, height);
    const uint8_t *img = rawdata + addr + PIC_HEADER_SIZE;

    /* Store compressed image data */
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

FileHeader ResPicture::getHeader() {
    return m_header;
}

vector<uint8_t> ResPicture::getCompressedData() {
    vector<uint8_t> finaldata;

    const uint16_t width  = static_cast<uint16_t>(m_dimension.width());
    const uint16_t height = static_cast<uint16_t>(m_dimension.height());
    const uint32_t size   = static_cast<uint32_t>(m_compressed.size());

    IOUtils::pushLittleEndianValue(finaldata, width);
    IOUtils::pushLittleEndianValue(finaldata, height);
    IOUtils::pushLittleEndianValue(finaldata, size);
    IOUtils::concatVector(finaldata, m_compressed);

    return finaldata;
}

const QSize& ResPicture::getSize() {
    return m_dimension;
}

void ResPicture::replaceImage(const QImage& img) {
    int r, g, b, a;
    const float ratio = 8.2258f;

    m_uncompressed.clear();

    for (int i = 0; i < img.height(); i++) {
        for (int j = 0; j < img.width(); j++) {
            const QColor color = img.pixelColor(j, i);
            color.getRgb(&r, &g, &b, &a);

            uint8_t red   = static_cast<uint8_t>(r/ratio);
            uint8_t green = static_cast<uint8_t>(g/ratio);
            uint8_t blue  = static_cast<uint8_t>(b/ratio);

            uint16_t bytes = ((red   & 0x1f) << 11 |
                              (green & 0x1f) <<  6 |
                              (blue  & 0x1f) <<  0);

            IOUtils::pushLittleEndianValue(m_uncompressed, bytes);

            if (IMG_HAS_ALPHA(m_header.type))
                m_uncompressed.push_back(static_cast<uint8_t>(a));
        }
    }

    m_compressed.clear();
    m_compressed.resize(m_uncompressed.size());
    Zlib::def(m_uncompressed, m_compressed, Z_DEFAULT_COMPRESSION);

    m_image = img;
}

