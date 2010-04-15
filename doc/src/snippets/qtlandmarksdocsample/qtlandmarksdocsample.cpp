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

#include "requestexample.h"

#include <qlandmark.h>
#include <qlandmarkmanager.h>
#include <qgeoaddress.h>
#include <qlandmarknamefilter.h>
#include <qlandmarknamesort.h>
#include <qlandmarkproximityfilter.h>
#include <qlandmarkdistancesort.h>
#include <qlandmarkcategoryid.h>
#include <qgeocoordinate.h>
#include <qlandmarkcategory.h>
#include <qlandmarkboxfilter.h>
#include <qlandmarkfetchrequest.h>
#include <qlandmarkcategorysaverequest.h>
#include <qlandmarkcategoryfilter.h>

#include <QCoreApplication>
#include <QTimer>
#include <QUrl>

#include <QDebug>

QTM_USE_NAMESPACE
QLandmarkCategoryId categoryId;


//! [Add category asynchronously req]
void RequestExample::categorySaveRequest()
{
    QLandmarkCategory cafes;
    cafes.setName("Cafes");
    cafes.setDescription("Small diners");
    cafes.setIconUrl(QUrl("cafe.png"));

    //catSaveRequest was created with catSaveRequest = new QLandmarkCategorySaveRequest()
    //in the ctor
    catSaveRequest->setManager(lmManager); //lmManager is a QLandmarkManager*
    catSaveRequest->appendCategory(cafes);

    connect(catSaveRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)), this,
            SLOT(categorySaveRequestHandler(QLandmarkAbstractRequest::State)));
    if (!catSaveRequest->start())
        qDebug() << "Unable to save category error code: " << catSaveRequest->error();
    else
        qDebug() << "Saveing category; awaiting results...";
}
//! [Add category asynchronously req]

//! [Add category asynchronously handler]
void RequestExample::categorySaveRequestHandler(QLandmarkAbstractRequest::State state)
{
    if (state == QLandmarkAbstractRequest::FinishedState) {
        if (catSaveRequest->error() == QLandmarkManager::NoError) {
            qDebug() << "Category save succesfully completed";
            catSaveRequest->clearCategories();
        }
        else {
            qDebug() << "Category save was unsuccessful";
        }
    }
}
//! [Add category asynchronously handler]

//! [Add landmark asynchronously]
void RequestExample::landmarkSaveRequest()
{
    //Creating and saving a landmark
    QLandmark monks;
    monks.setName("Monk's cafe");
    monks.setCoordinate(QGeoCoordinate(40.81, 73.97));

    QGeoAddress address;
    address.setThoroughfareNumber("2880");
    // ...
    address.setCountryCode("US");
    monks.setAddress(address);

    //lmSaveRequest was created with lmSaveRequest = new QLandmarkSaveRequest()
    //in the ctor
    lmSaveRequest->setManager(lmManager); //lmManager is a QLandmarkManager*
    lmSaveRequest->appendLandmark(monks);

    monks.setDescription("Jerry's favourite diner");

    connect(lmSaveRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)), this,
            SLOT(landmarkSaveRequestHandler(QLandmarkAbstractRequest::State)));
    if (!lmSaveRequest->start())
        qDebug() << "Unable to save landmark error code: " << lmSaveRequest->error();
    else
        qDebug() << "Saving landmark; awaiting results...";
}
//! [Add landmark asynchronously]

//! [Add landmark asynchronously handler]
void RequestExample::landmarkSaveRequestHandler(QLandmarkAbstractRequest::State state)
{
    if (state == QLandmarkAbstractRequest::FinishedState) {
        if (lmSaveRequest->error() == QLandmarkManager::NoError) {
            qDebug() << "Landmark save succesfully completed";
            lmSaveRequest->clearLandmarks();
        }
        else {
            qDebug() << "Landmark save was unsuccessful";
        }
    }
}
//! [Add landmark asynchronously handler]

//Fetch categories asynchronously
void RequestExample::categoryFetchRequest()
{
    //! [Retrieve categories asynchronously]
    //catFetchRequest was created with catFetchRequest = new QLandmarkCategoryFetchRequest()
    //in the ctor
    catFetchRequest->setManager(lmManager); //lmManager is a QLandmarkManager*

    connect(catFetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)),
            this, SLOT(categoryFetchHandlerRequest(QLandmarkAbstractRequest::State)));

    if(!catFetchRequest->start()) {
        qDebug() << "Unable to request categories, error code:" << catFetchRequest->error();
        QCoreApplication::exit(0);
    } else {
        qDebug() << "Requested categories, awaiting results...";
    }
    //! [Retrieve categories asynchronously]
}

//! [Retrieve categories asynchronously handler]
void RequestExample::categoryFetchHandlerRequest(QLandmarkAbstractRequest::State state)
{
    if (state == QLandmarkAbstractRequest::FinishedState) {
        previousLastIndex = 0;
        if (catFetchRequest->error() == QLandmarkManager::NoError) {
            QList<QLandmarkCategory> categories = catFetchRequest->categories();
            qDebug() << "Category fetch succesfully completed";
            for(int i=0; i < categories.count(); ++i) {
                qDebug() << categories[i].name();
            }
        }
        else {
            qDebug() << "Category fetch was unsuccessful";
        }
    }
}
//! [Retrieve categories asynchronously handler]

void RequestExample::landmarkFetchRequest()
{
    //! [Retrieve landmarks asynchronously]
    QLandmarkCategoryFilter filter;
    //categoryId is a previously retrieved QLandmarkCategoryId
    filter.setCategoryId(categoryId);

    //lmFetchRequest was created with lmFetchRequest = new QLandmarkFetchRequest()
    //in the ctor
    lmFetchRequest->setManager(lmManager); //lmManager is a QLandmarkManager*
    lmFetchRequest->setFilter(filter);
    lmFetchRequest->setSorting(QLandmarkNameSort(Qt::AscendingOrder));

    connect(lmFetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)),
            this, SLOT(landmarkFetchHandlerRequest(QLandmarkAbstractRequest::State)));

    if(!lmFetchRequest->start()) {
        qDebug() << "Unable to request landmarks, error code:" << lmFetchRequest->error();
        QCoreApplication::exit(0);
    } else {
        qDebug() << "Requested landmarks, awaiting results...";
    }
    //! [Retrieve landmarks asynchronously]
}

//! [Retrieve landmarks asynchronously handler]
void RequestExample::landmarkFetchHandlerRequest(QLandmarkAbstractRequest::State state)
{
    if (state == QLandmarkAbstractRequest::FinishedState) {
        previousLastIndex = 0;
        if (lmFetchRequest->error() == QLandmarkManager::NoError) {
            qDebug() << "Landmark fetch succesfully completed";
            QList<QLandmark> landmarks = lmFetchRequest->landmarks();
            for(int i=0; i < landmarks.count(); ++i) {
                qDebug() << landmarks[i].name();
            }
        }
        else {
            qDebug() << "Landmark fetch was unsuccessful";
        }
    }
}
//! [Retrieve landmarks asynchronously handler]

void addLandmarkAndCategory(QLandmarkManager *lm)
{
//! [Add category synchronously]
    QLandmarkCategory cafes;
    cafes.setName("Cafes");
    cafes.setDescription("Small diners");
    cafes.setIconUrl(QUrl("cafe.png"));
    lm->saveCategory(&cafes);  //lm is a QLandmarkManager *
//! [Add category synchronously]

//! [Add landmark synchronously]
    QLandmark monks;
    monks.setName("Monk's cafe");
    monks.setCoordinate(QGeoCoordinate(40.81, 73.97));

    QGeoAddress address;
    address.setThoroughfareNumber("2880");
    address.setThoroughfareName("112th Street");
    address.setCity("New York City");
    address.setState("New York");
    address.setCountry("United States");
    address.setCountryCode("US");
    monks.setAddress(address);

    monks.setDescription("Jerry's favourite diner");
    monks.addCategory(cafes.id());

    lm->saveLandmark(&monks); //lm  is a QLandmarkManager*
    //! [Add landmark synchronously]
}

void categoryFetch(QLandmarkManager *lm)
{
    //! [Retrieve categories synchronously by id]
    QList<QLandmarkCategoryId> categoryIds = lm->categoryIds();
    foreach(QLandmarkCategoryId id, categoryIds) {
        qDebug() << "Found category: " << lm->category(id).name();
    }
    //! [Retrieve categories synchronously by id]

    //! [Retrieve categories synchronously]
    QList<QLandmarkCategory> categories = lm->categories();
    foreach(QLandmarkCategory category, categories) {
        qDebug() << "Found category: " << category.name();
    }
    //! [Retrieve categories synchronously]
}

void landmarkFetch(QLandmarkManager *lm)
{
    //! [Retrieve landmarks synchronously by id]
    QLandmarkCategoryFilter filter;
    //categoryId is a previously retrieved QLandmarkCategoryId
    filter.setCategoryId(categoryId);

    //retrieval via ids
    QList<QLandmarkId> landmarkIds;
    landmarkIds = lm->landmarkIds(filter, QLandmarkNameSort(Qt::AscendingOrder));
    foreach(QLandmarkId id, landmarkIds) {
        qDebug() << "Found landmark:" << lm->landmark(id).name();
    }
    //! [Retrieve landmarks synchronously by id]

    //! [Retrieve landmarks synchronously]
    QList<QLandmark> landmarks;
    landmarks = lm->landmarks(filter, QLandmarkNameSort(Qt::AscendingOrder));
    foreach(QLandmark landmark, landmarks) {
        qDebug() << "Found landmark:" << landmark.name();
    }
    //! [Retrieve landmarks synchronously]
}

void filterByName(QLandmarkManager *lm)
{
    QLandmarkNameFilter filter("Monk's cafe");
    QList<QLandmarkId> matchingIds = lm->landmarkIds(filter);

    if (matchingIds.count() == 0) {
        qDebug() << "No matches found";
    } else {
        QLandmark match = lm->landmark(matchingIds.at(0));
        qDebug() << "Match found coordinates are:" << match.coordinate().toString();
    }
}

void filterByProximity(QLandmarkManager *lm)
{
    QGeoCoordinate origin(41,74);
    QLandmarkProximityFilter filter(origin, 5000);
    QList<QLandmarkId> matchingIds = lm->landmarkIds(filter, QLandmarkDistanceSort(origin, Qt::AscendingOrder));

    if(matchingIds.count() == 0) {
        qDebug() << "No matches found";
    } else {
        QLandmark matchingLandmark;
        foreach(QLandmarkId id, matchingIds) {
            matchingLandmark = lm->landmark(id);
            qDebug() << "Match found, name: " << matchingLandmark.name();
        }
    }
}

void listAllCategories(QLandmarkManager *lm)
{
    QList<QLandmarkCategoryId> categoryIds = lm->categoryIds();
    foreach(QLandmarkCategoryId id, categoryIds) {
        qDebug() << lm->category(id).name();
    }

    QList<QLandmarkCategory> categories = lm->categories();
    foreach(QLandmarkCategory category, categories) {
        qDebug() << category.name();
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QLandmarkManager *lm = new QLandmarkManager();
    //Synchronous API examples
    addLandmarkAndCategory(lm);
    categoryFetch(lm);
    landmarkFetch(lm);
    filterByName(lm);
    filterByProximity(lm);

    //Asynchronous API example
    RequestExample re;
    QTimer::singleShot(10, &re, SLOT(categorySaveRequest()));
    QTimer::singleShot(10, &re, SLOT(landmarkSaveRequest()));
    QTimer::singleShot(10, &re, SLOT(categoryFetchRequest()));
    QTimer::singleShot(10, &re, SLOT(landmarkFetchRequest()));

    app.exec();

    delete lm;
    return 0;
}
