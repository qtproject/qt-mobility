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

#ifndef V4LVIDEOBUFFER_H
#define V4LVIDEOBUFFER_H

#include <QtCore/qsize.h>
#include <QtMultimedia/QAbstractVideoBuffer>

#include <linux/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

class V4LVideoBuffer : public QAbstractVideoBuffer
{
public:
    V4LVideoBuffer(unsigned char *buffer, int fd, v4l2_buffer buf);
    ~V4LVideoBuffer();

    MapMode mapMode() const;

    uchar *map(MapMode mode, int *numBytes, int *bytesPerLine);
    void unmap();

    void setBytesPerLine(int bytesPerLine);

private:
    unsigned char *m_buffer;
    int m_length;
    int m_fd;
    int m_bytesPerLine;
    MapMode m_mode;
    v4l2_buffer m_buf;
};


#endif
