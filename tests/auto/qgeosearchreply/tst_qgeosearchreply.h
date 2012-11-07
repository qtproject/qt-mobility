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

#ifndef TST_QGEOSEARCHREPLY_H
#define TST_QGEOSEARCHREPLY_H

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QMetaType>
#include <QSignalSpy>

#include "../qgeocoordinate/qlocationtestutils_p.h"

#include <qgeosearchreply.h>
#include <qgeoplace.h>
#include <qgeoaddress.h>
#include <qgeocoordinate.h>
#include <qgeoboundingbox.h>

QTM_USE_NAMESPACE
class SubSearchReply : public QGeoSearchReply
{
    Q_OBJECT
public:
    SubSearchReply():QGeoSearchReply() {}

    void  callAddPlace ( const QGeoPlace & place ) {addPlace(place);}
    void  callSetError ( Error error, const QString & errorString ) {setError(error, errorString);}
    void  callSetFinished ( bool finished ) {setFinished(finished);}
    void  callSetLimit ( int limit ) {setLimit(limit);}
    void  callSetOffset ( int offset ) {setOffset(offset);}
    void  callSetPlaces ( const QList<QGeoPlace> & places ) {setPlaces(places);}
    void  callSetViewport ( QGeoBoundingArea * viewport ) {setViewport(viewport);}

};

class tst_QGeoSearchReply :public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    //Start Unit Test for QGeoSearchReply
private slots:
    void constructor();
    void constructor_error();
    void constructor_error_data();
    void destructor();
    void destructor_data();
    void abort();
    void error();
    void error_data();
    void finished();
    void limit();
    void offset();
    void places();
    void viewport();

    //End Unit Test for QGeoSearchReply



private:
    QSignalSpy *signalerror;
    QSignalSpy *signalfinished;
    SubSearchReply* reply;
    QGeoPlace *qgeoplace;
    QGeoBoundingBox *qgeoboundingbox;
};

Q_DECLARE_METATYPE( QList<double>);
Q_DECLARE_METATYPE( QGeoSearchReply::Error);

#endif // TST_QGEOSEARCHREPLY_H

