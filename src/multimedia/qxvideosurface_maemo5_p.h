/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QXVIDEOSURFACE_MAEMO5_H
#define QXVIDEOSURFACE_MAEMO5_H

#include <QtCore/qhash.h>
#include <QtGui/qwidget.h>
#include <qabstractvideosurface.h>

#include <X11/Xlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xv.h>
#include <X11/extensions/Xvlib.h>

QT_USE_NAMESPACE

class QXVideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    QXVideoSurface(QObject *parent = 0);
    ~QXVideoSurface();

    WId winId() const;
    void setWinId(WId id);

    QRect displayRect() const;
    void setDisplayRect(const QRect &rect);

    QColor colorKey() const;
    void setColorKey(QColor key);

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;

    QVideoFrame lastFrame() const { return m_lastFrame; }

public slots:
    bool start(const QVideoSurfaceFormat &format);
    void stop();

    bool present(const QVideoFrame &frame);
    void repaintLastFrame();

private:
    WId m_winId;
    XvPortID m_portId;
    int m_xvFormatId;
    GC m_gc;
    XvImage *m_image;
    XShmSegmentInfo	m_shminfo;
    QList<QVideoFrame::PixelFormat> m_supportedPixelFormats;
    QVector<int> m_formatIds;
    QRect m_viewport;
    QRect m_displayRect;
    QColor m_colorKey;

    QVideoFrame m_lastFrame;

    bool findPort();
    void querySupportedFormats();

    int getAttribute(const char *attribute) const;
    void setAttribute(const char *attribute, int value);
};

Q_DECLARE_METATYPE(XvImage*)

#endif
