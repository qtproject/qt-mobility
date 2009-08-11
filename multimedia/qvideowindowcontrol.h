/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QVIDEOOVERLAYENDPOINT_H
#define QVIDEOOVERLAYENDPOINT_H

#include "qabstractmediacontrol.h"

#include <QtGui/qwidget.h>

class QVideoWindowControlPrivate;

class Q_MEDIA_EXPORT QVideoWindowControl : public QAbstractMediaControl
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QVideoWindowControl)
public:
    QVideoWindowControl(QObject *parent = 0);
    ~QVideoWindowControl();

    WId winId() const;
    virtual void setWinId(WId id);

    QRect displayRect() const;
    virtual void setDisplayRect(const QRect &rect);

    bool isFullscreen() const;
    virtual void setFullscreen(bool fullscreen);

    virtual void repaint();

    virtual QSize nativeSize() const = 0;

    int brightness() const;
    virtual void setBrightness(int brightness);

    int contrast() const;
    virtual void setContrast(int contrast);

    int hue() const;
    virtual void setHue(int hue);

    int saturation() const;
    virtual void setSaturation(int saturation);

Q_SIGNALS:
    void fullscreenChanged(bool fullscreen);
    void brightnessChanged(int brightness);
    void contrastChanged(int contrast);
    void hueChanged(int hue);
    void saturationChanged(int saturation);
    void nativeSizeChanged();
};

#define QVideoWindowControl_iid "com.nokia.Qt.QVideoWindowControl/1.0"

Q_MEDIA_DECLARE_CONTROL(QVideoWindowControl, QVideoWindowControl_iid)

#endif
