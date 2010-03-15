/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef CAMERAFORMATCONVERTER_H
#define CAMERAFORMATCONVERTER_H

#include <QtCore/qlist.h>
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
