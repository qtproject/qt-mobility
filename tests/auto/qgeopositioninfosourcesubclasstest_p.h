/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QGEOPOSITIONINFOSOURCESUBCLASSTEST_H
#define QGEOPOSITIONINFOSOURCESUBCLASSTEST_H

#include <qgeopositioninfosource.h>

#include <QIODevice>
#include <QObject>
#include <QList>
#include <QPointer>

class QGeoCoordinate;

class QGeoPositionInfoSourceProxy
{
public:
    QGeoPositionInfoSourceProxy() {}
    virtual ~QGeoPositionInfoSourceProxy() {}

    virtual QGeoPositionInfoSource *source() const = 0;
    virtual void feedUpdate(const QDateTime &dt) = 0;

    virtual int updateIntervalErrorMargin() const { return 50; }
};


class QGeoPositionInfoSourceProxyFactory
{
public:
    QGeoPositionInfoSourceProxyFactory() {}
    virtual ~QGeoPositionInfoSourceProxyFactory() {}

    virtual QGeoPositionInfoSourceProxy *createProxy() = 0;
};



class QGeoPositionInfoSourceSubclassTest : public QObject
{
    Q_OBJECT
public:
    QGeoPositionInfoSourceSubclassTest(QGeoPositionInfoSourceProxyFactory *factory, QObject *parent = 0);

private slots:
    void initTestCase();
    void init();
    void cleanup();

    void setUpdateInterval();
    void setUpdateInterval_data();
    void setUpdateInterval_delayedUpdate();
    void setUpdateInterval_expectLatestUpdateOnly();

    void lastKnownPosition();

    void requestUpdate();
    void requestUpdate_after_start();

    void startUpdates();
    void startUpdates_data();
    void startUpdates_startNotCalled();

    void stopUpdates();
    void stopUpdates_restart();

protected:
    inline QGeoPositionInfoSourceProxyFactory *factory() const { return m_factory; }

private:
    QList<QDateTime> createDateTimes(int count) const;

    QGeoPositionInfoSourceProxyFactory *m_factory;
    QGeoPositionInfoSourceProxy *m_helper;
    QGeoPositionInfoSource *m_source;
};


#endif

