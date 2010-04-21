/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT7VIDEOOUTPUTCONTROL_H
#define QT7VIDEOOUTPUTCONTROL_H

#include <QtCore/qobject.h>

#include <qvideooutputcontrol.h>
#include <qvideowindowcontrol.h>
#include <qvideowidgetcontrol.h>
#include <qvideorenderercontrol.h>
#include <qmediaplayer.h>

#include <QtGui/qmacdefines_mac.h>


QT_BEGIN_NAMESPACE

class QMediaPlaylist;
class QMediaPlaylistNavigator;
class QT7PlayerSession;
class QT7PlayerService;


class QT7VideoOutput {
public:
    virtual ~QT7VideoOutput() {}
    virtual void setEnabled(bool enabled) = 0;
    virtual void setMovie(void *movie) = 0;
};

class QT7VideoWindowControl : public QVideoWindowControl, public QT7VideoOutput
{
public:
    virtual ~QT7VideoWindowControl() {}

protected:
    QT7VideoWindowControl(QObject *parent)
        :QVideoWindowControl(parent)
    {}
};

class QT7VideoRendererControl : public QVideoRendererControl, public QT7VideoOutput
{
public:
    virtual ~QT7VideoRendererControl() {}

protected:
    QT7VideoRendererControl(QObject *parent)
        :QVideoRendererControl(parent)
    {}
};

class QT7VideoWidgetControl : public QVideoWidgetControl, public QT7VideoOutput
{
public:
    virtual ~QT7VideoWidgetControl() {}

protected:
    QT7VideoWidgetControl(QObject *parent)
        :QVideoWidgetControl(parent)
    {}
};

class QT7VideoOutputControl : public QVideoOutputControl
{
Q_OBJECT
public:
    QT7VideoOutputControl(QObject *parent = 0);
    ~QT7VideoOutputControl();

    void setSession(QT7PlayerSession *session);

    QList<Output> availableOutputs() const;
    void enableOutput(Output);

    Output output() const;
    void setOutput(Output output);

signals:
    void videoOutputChanged(QVideoOutputControl::Output);
    
private:
    QT7PlayerSession *m_session;
    Output m_output;
    QList<Output> m_outputs;
};

QT_END_NAMESPACE

#endif
