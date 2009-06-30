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

#ifndef QWMPPLAYERSERVICE_H
#define QWMPPLAYERSERVICE_H

#include <QtCore/qobject.h>

#include "qwmpevents.h"
#include "qwmpglobal.h"

#include <wmp.h>

class QMediaMetaData;
class QMediaPlayerControl;
class QMediaPlaylist;

class Q_WMP_EXPORT QMediaPlayerService : public QObject
{
    Q_OBJECT
public:
    QMediaPlayerService(QObject *parent = 0) : QObject(parent) {}

    virtual QWidget *createWidget() = 0;

    virtual QObject *videoOutput() const = 0;
    virtual void setVideoOutput(QObject *output) = 0;

    virtual QMediaPlayerControl *control() = 0;
    virtual QMediaMetaData *metaData() = 0;
    virtual QMediaPlaylist *playlist() = 0;
};

class QWmpMetaData;
class QWmpPlayerControl;
class QWmpPlaylist;

class Q_WMP_EXPORT QWmpPlayerService : public QMediaPlayerService, public QWmpEvents
{
    Q_OBJECT
public:
    QWmpPlayerService(QObject *parent = 0);
    ~QWmpPlayerService();

    QMediaPlayerControl *control();
    QMediaMetaData *metaData();
    QMediaPlaylist *playlist();

    QWidget *createWidget();

    QObject *videoOutput() const;
    void setVideoOutput(QObject *output);

    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **object);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IWMPEvents
    void STDMETHODCALLTYPE PlayStateChange(long NewState);
    void STDMETHODCALLTYPE Buffering(VARIANT_BOOL Start);
    void STDMETHODCALLTYPE PositionChange(double oldPosition, double newPosition);
    void STDMETHODCALLTYPE MediaChange(IDispatch *Item);

private:
    volatile LONG m_ref;
    IWMPPlayer4 *m_player;
    QObject *m_videoOutput;
    QWmpPlayerControl *m_control;
    QWmpMetaData *m_metaData;
    QWmpPlaylist *m_playlist;
    IConnectionPoint *m_connectionPoint;
    DWORD m_adviseCookie;


};

#endif
