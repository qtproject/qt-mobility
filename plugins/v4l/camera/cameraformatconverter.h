#ifndef CAMERAFORMATCONVERTER_H
#define CAMERAFORMATCONVERTER_H

#include <QList>
#include <QtMultimedia/qvideoframe.h>

/*
 * convert something to RGB32 / RGB16
 *
 */
class CameraFormatConverter
{
public:

    virtual ~CameraFormatConverter() {}

    virtual unsigned char* convert(unsigned char* src, int len) = 0;

    static CameraFormatConverter* createFormatConverter(QVideoFrame::PixelFormat format, int width, int height);
    static void releaseFormatConverter(CameraFormatConverter* converter);
    static QList<QVideoFrame::PixelFormat> supportedFormats();
};


class NullConverter : public CameraFormatConverter
{
public:

    virtual unsigned char* convert(unsigned char* src, int len);
};


class YUVConverter : public CameraFormatConverter
{
public:
    YUVConverter(QVideoFrame::PixelFormat type, int width, int height);
    virtual ~YUVConverter();

    virtual unsigned char* convert(unsigned char* src, int len);

private:

    QVideoFrame::PixelFormat    m_type;
    int             m_width;
    int             m_height;
    unsigned char*  m_buf;
    int m_ui,m_vi,m_y1i,m_y2i;

};

class BayerConverter : public CameraFormatConverter
{
public:
    BayerConverter(int width, int height);
    virtual ~BayerConverter();

    virtual unsigned char* convert(unsigned char* src, int len);

private:

    int             m_width;
    int             m_height;
    unsigned char*  m_buf;
};

#endif
