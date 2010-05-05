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

#include "qlandmark.h"
#include "qlandmarkmanager.h"
#include "qgeoaddress.h"
#include "qlandmarknamefilter.h"
#include "qlandmarknamesort.h"
#include "qlandmarkproximityfilter.h"
#include "qlandmarkdistancesort.h"
#include "qlandmarkcategoryid.h"
#include "qgeocoordinate.h"
#include "qlandmarkcategory.h"
#include "qlandmarkboxfilter.h"
#include "qlandmarkfetchrequest.h"
#include "qlandmarkcategorysaverequest.h"
#include "qlandmarkcategoryfilter.h"

#include <QCoreApplication>
#include <QTimer>
#include <QUrl>

#include <QDebug>

QTM_USE_NAMESPACE
QLandmarkCategoryId categoryId;
QLandmark landmark;
QLandmarkCategory category;

//! [Add category asynchronously req]
void RequestExample::categorySaveRequest()
{
    QLandmarkCategory cafes;
    cafes.setName("Cafes");
    cafes.setDescription("Small diners");
    cafes.setIconUrl(QUrl("cafe.png"));

    //catSaveRequest was created with catSaveRequest = new QLandmarkCategorySaveRequest(lmManager)
    //in the ctor, where lmManager is a QLandmarkManager *
    catSaveRequest->setCategory(cafes);

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

    //lmSaveRequest was created with lmSaveRequest = new QLandmarkSaveRequest(lmManager)
    //in the ctor, where lmManager is a QLandamrkManager constructor
    lmSaveRequest->setLandmark(monks);

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
    //catFetchRequest was created with catFetchRequest = new QLandmarkCategoryFetchRequest(lmManager)
    //in the ctor, where lmManager is a QLandmarkManager*

    connect(catFetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)),
            this, SLOT(categoryFetchRequestHandler(QLandmarkAbstractRequest::State)));

    if(!catFetchRequest->start()) {
        qDebug() << "Unable to request categories, error code:" << catFetchRequest->error();
        QCoreApplication::exit(0);
    } else {
        qDebug() << "Requested categories, awaiting results...";
    }
    //! [Retrieve categories asynchronously]
}

//! [Retrieve categories asynchronously handler]
void RequestExample::categoryFetchRequestHandler(QLandmarkAbstractRequest::State state)
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
    QLandmarkNameSort sort(Qt::AscendingOrder);
    //categoryId is a previously retrieved QLandmarkCategoryId
    filter.setCategoryId(categoryId);

    //lmFetchRequest was created with lmFetchRequest = new QLandmarkFetchRequest(lmManager)
    //in the ctor, where lmManager is a QLandmarkManger *
    lmFetchRequest->setFilter(filter);
    lmFetchRequest->setSorting(sort);

    connect(lmFetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)),
            this, SLOT(landmarkFetchRequestHandler(QLandmarkAbstractRequest::State)));

    if(!lmFetchRequest->start()) {
        qDebug() << "Unable to request landmarks, error code:" << lmFetchRequest->error();
        QCoreApplication::exit(0);
    } else {
        qDebug() << "Requested landmarks, awaiting results...";
    }
    //! [Retrieve landmarks asynchronously]
}

//! [Retrieve landmarks asynchronously handler]
void RequestExample::landmarkFetchRequestHandler(QLandmarkAbstractRequest::State state)
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

//! [Remove category asynchronously]
void RequestExample::categoryRemoveRequest()
{
    //catRemoveRequest was created with catRemoveRequest = new QLandmarkCategoryRemoveRequest(lmManager)
    //in the ctor, where lmManager is a QLandmarkManager*
    catRemoveRequest->setCategoryId(category.id()); //category is a previously retrieved QLandmarkCategory

    connect(catRemoveRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)),
        this, SLOT(categoryRemoveRequestHandler(QLandmarkAbstractRequest::State)));

    if(!catRemoveRequest->start()) {
        qDebug() << "Unable to request category removal, error code:" << catRemoveRequest->error();
        QCoreApplication::exit(0);
    } else {
        qDebug() << "Requested category removal, awaiting results...";
    }
}
//! [Remove category asynchronously]

//! [Remove category asynchronously handler]
void RequestExample::categoryRemoveRequestHandler(QLandmarkAbstractRequest::State state)
{
    if (state == QLandmarkAbstractRequest::FinishedState) {
        if (catRemoveRequest->error() == QLandmarkManager::NoError) {
            qDebug() << "Category remove succesfully completed";
        }
        else {
            qDebug() << "Category remove was unsuccessful";
        }
    }
}
//! [Remove category asynchronously handler]

//! [Remove landmark asynchronously]
void RequestExample::landmarkRemoveRequest()
{
    //lmRemoveRequest was created with lmRemoveRequest = new QLandmarkSaveRequest(lmManager)
    //in the ctor, where lmManager is a QLandmarkManager*
    lmRemoveRequest->setLandmarkId(landmark.id());  //landmark is a previously retrieved QLandmark

    connect(lmRemoveRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)), this,
            SLOT(landmarkRemoveRequestHandler(QLandmarkAbstractRequest::State)));
    if (!lmRemoveRequest->start())
        qDebug() << "Unable to remove landmark, error code: " << lmSaveRequest->error();
    else
        qDebug() << "Removing landmark; awaiting results...";
}
//! [Remove landmark asynchronously]

//! [Remove landmark asynchronously handler]
void RequestExample::landmarkRemoveRequestHandler(QLandmarkAbstractRequest::State state)
{
    if (state == QLandmarkAbstractRequest::FinishedState) {
        if (lmRemoveRequest->error() == QLandmarkManager::NoError) {
            qDebug() << "Landmark removal succesfully completed";
        }
        else {
            qDebug() << "Landmark removal was unsuccessful";
        }
    }
}
//! [Remove landmark asynchronously handler]

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
    QLandmarkNameSort sortOrder(Qt::AscendingOrder);
    landmarkIds = lm->landmarkIds(filter, sortOrder);
    foreach(QLandmarkId id, landmarkIds) {
        qDebug() << "Found landmark:" << lm->landmark(id).name();
    }
    //! [Retrieve landmarks synchronously by id]

    //! [Retrieve landmarks synchronously]
    QList<QLandmark> landmarks;
    landmarks = lm->landmarks(filter, sortOrder);
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
    QLandmarkDistanceSort sort(origin, Qt::AscendingOrder);
    QList<QLandmarkId> matchingIds = lm->landmarkIds(filter, sort);

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

void deleteLandmarkAndCategory(QLandmarkManager *lm)
{
    //! [Remove category synchronously]
    //category is a previously retrieved QLandmarkCategory object
    lm->removeCategory(category.id());
    //! [Remove category synchronously]

    //! [Remove landmark synchronously]
    //landmark is a previously retrieved QLandmark object
    lm->removeLandmark(landmark.id());
    //! [Remove landmark synchronously]
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
    QTimer::singleShot(10, &re, SLOT(categoryRemoveRequest()));
    QTimer::singleShot(10, &re, SLOT(landmarkRemoveRequest()));

    app.exec();

    delete lm;
    return 0;
}
