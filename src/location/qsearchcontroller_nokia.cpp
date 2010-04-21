/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qsearchcontroller_nokia_p.h"
#include "qsearchrequest_nokia.h"

QTM_BEGIN_NAMESPACE

QSearchControllerNokia::QSearchControllerNokia()
    : searchService(new QGeocodingServiceNokia)
{
    searchResponse=NULL;
    QObject::connect(searchService, SIGNAL(finished(QSearchResponse*)),
                     this, SLOT(codingReplyFinished(QSearchResponse*)));
}

QSearchControllerNokia::QSearchControllerNokia(QGeocodingServiceNokia* service)
    : searchService(service)
{
    searchResponse=NULL;
    QObject::connect(searchService, SIGNAL(finished(QSearchResponse*)),
                     this, SLOT(codingReplyFinished(QSearchResponse*)));
}

QSearchControllerNokia::~QSearchControllerNokia()
{
    delete searchService;
}

void QSearchControllerNokia::geocode(QSearchRequest& request)
{
    request.searchText = static_cast<QSearchRequestNokia*>(&request)->requestGeocodingString(searchService->host());
    searchService->geocode(request);
    emit searchRequestStarted(&request);
}

void QSearchControllerNokia::reverseGeocode(QSearchRequest& request)
{
    request.searchText = static_cast<QSearchRequestNokia*>(&request)->requestReverseGeocodingString(searchService->host());
    searchService->reverseGeocode(request);
    emit searchRequestStarted(&request);
}

void QSearchControllerNokia::POISearch(QSearchRequest& request)
{
    emit searchRequestStarted(&request);
    clear();
    //TODO: emit with error code? NotSupported? Null for now.
    emit searchRequestFinished(searchResponse);
}

void QSearchControllerNokia::search(QSearchRequest& request)
{   
    emit searchRequestStarted(&request);
    clear();
    //TODO: emit with error code? NotSupported? Null for now.
    emit searchRequestFinished(searchResponse);
}

void QSearchControllerNokia::nextPage()
{
    //TODO: emit with error code? NotSupported? Null for now.
    emit searchRequestFinished(searchResponse);
}

void QSearchControllerNokia::previousPage()
{
    //TODO: emit with error code? NotSupported? Null for now.
    emit searchRequestFinished(searchResponse);
}

void QSearchControllerNokia::clear()
{
    emit searchResponseCleared(searchResponse);
    searchResponse->deleteLater();
    searchResponse = NULL;
}

void QSearchControllerNokia::codingReplyFinished(QSearchResponse* response)
{
    clear();
    searchResponse = response;
    emit searchRequestFinished(response);
}

#include "moc_qsearchcontroller_nokia_p.cpp"

QTM_END_NAMESPACE
