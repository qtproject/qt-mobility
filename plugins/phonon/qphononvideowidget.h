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

#ifndef QGSTREAMERVIDEOWIDGET_H
#define QGSTREAMERVIDEOWIDGET_H

#include <qvideowidgetcontrol.h>
#include <qvideooutputcontrol.h>

#include <Phonon/VideoWidget>

class QPhononVideoWidget : public QVideoWidgetControl
{
    Q_OBJECT
public:
    QPhononVideoWidget(Phonon::VideoWidget *videoWidget, QObject *parent = 0);
    virtual ~QPhononVideoWidget();

    QWidget *videoWidget() { return m_videoWidget; }

    QVideoWidget::AspectRatioMode aspectRatioMode() const;
    void setAspectRatioMode(QVideoWidget::AspectRatioMode mode);

    bool isFullScreen() const;
    void setFullScreen(bool fullScreen);

    int brightness() const;
    void setBrightness(int brightness);

    int contrast() const;
    void setContrast(int contrast);

    int hue() const;
    void setHue(int hue);

    int saturation() const;
    void setSaturation(int saturation);

private:
    Phonon::VideoWidget *m_videoWidget;
    bool m_isFullscreen;
    QVideoWidget::AspectRatioMode m_aspectRatioMode;
};

class QPhononVideoOutputControl : public QVideoOutputControl
{
    Q_OBJECT
public:

    QPhononVideoOutputControl(QObject *parent = 0)
        :QVideoOutputControl(parent) {}
    ~QPhononVideoOutputControl() {}

    virtual QList<Output> availableOutputs() const
    {
        return QList<Output>() << WidgetOutput;
    }

    Output output() const { return WidgetOutput; }
    void setOutput(Output) {}
};

#endif // QGSTREAMERVIDEOWIDGET_H
