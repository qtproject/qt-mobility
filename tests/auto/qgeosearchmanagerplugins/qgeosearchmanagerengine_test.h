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

#ifndef QGEOSEARCHMANAGERENGINE_TEST_H
#define QGEOSEARCHMANAGERENGINE_TEST_H

#include <qgeoserviceprovider.h>
#include <qgeosearchmanagerengine.h>
#include <QLocale>
#include <qlandmarkmanager.h>
#include <qgeoaddress.h>
#include <qgeoplace.h>
#include <qgeosearchreply.h>


QTM_USE_NAMESPACE


class SearchReplyTest :public QGeoSearchReply
{
    Q_OBJECT
public:
    SearchReplyTest(QObject *parent=0):QGeoSearchReply (parent) {}

    void  callAddPlace ( const QGeoPlace & place ) {addPlace(place);}
    void  callSetError ( Error error, const QString & errorString ) {setError(error, errorString);}
    void  callSetFinished ( bool finished ) {setFinished(finished);}
    void  callSetLimit ( int limit ) {setLimit(limit);}
    void  callSetOffset ( int offset ) {setOffset(offset);}
    void  callSetPlaces ( const QList<QGeoPlace> & places ) {setPlaces(places);}
    void  callSetViewport ( QGeoBoundingArea * viewport ) {setViewport(viewport);}

};

class QGeoSearchManagerEngineTest: public QGeoSearchManagerEngine

{
Q_OBJECT
public:
    QGeoSearchManagerEngineTest(const QMap<QString, QVariant> &parameters,
        QGeoServiceProvider::Error *error, QString *errorString) :
        QGeoSearchManagerEngine(parameters)
    {
        Q_UNUSED(error)
        Q_UNUSED(errorString)
        setSupportedSearchTypes(QGeoSearchManager::SearchGeocode);
        setSupportsGeocoding(true);
        setSupportsReverseGeocoding(true);
        setLocale(*(new QLocale (QLocale::German, QLocale::Germany)));

        QLandmarkManager *landmarkmanager = new QLandmarkManager();
        setDefaultLandmarkManager(landmarkmanager);


    }

    QGeoSearchReply*  search ( const QString & searchString, QGeoSearchManager::SearchTypes searchTypes, int limit, int offset, QGeoBoundingArea * bounds )
    {
        SearchReplyTest *searchreply = new SearchReplyTest();
        searchreply->callSetLimit(limit);
        searchreply->callSetOffset(offset);
        searchreply->callSetViewport(bounds);
        searchreply->callSetError(QGeoSearchReply::NoError,searchString);
        searchreply->callSetFinished(true);
        emit(this->finished(searchreply));

        return static_cast<QGeoSearchReply*>(searchreply);

    }

    QGeoSearchReply*  geocode ( const QGeoAddress & address, QGeoBoundingArea * bounds )
    {
        SearchReplyTest *searchreply = new SearchReplyTest();
        searchreply->callSetViewport(bounds);
        searchreply->callSetError(QGeoSearchReply::NoError,address.city());
        searchreply->callSetFinished(true);
        emit(this->finished(searchreply));

        return static_cast<QGeoSearchReply*>(searchreply);
    }

    QGeoSearchReply*  reverseGeocode ( const QGeoCoordinate & coordinate, QGeoBoundingArea * bounds )
    {
        SearchReplyTest *searchreply = new SearchReplyTest();
        searchreply->callSetViewport(bounds);
        searchreply->callSetError(QGeoSearchReply::NoError,coordinate.toString());
        searchreply->callSetFinished(true);
        emit(this->finished(searchreply));
        return static_cast<QGeoSearchReply*>(searchreply);
    }




};

#endif
