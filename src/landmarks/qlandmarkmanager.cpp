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

#include "qlandmarkmanager.h"

#include <QStringList>
#include <QString>
#include <QIODevice>


QTM_USE_NAMESPACE
/*!
    \class QLandmarkManager
    \brief The QLandmarkManager class provides an interface for storage
    and retrieval of landmarks from a landmark store.
*/

/*!
    \enum QLandmarkManager::Error
    Defines the possible errors for the landmark manager.
    \value NoError No error occurred.
    \value ReadOnly An attempt was made to modify/remove a read-only category.
*/

/*!
    \enum QLandmarkManager::Format
    Defines the various file formats for import and exporting landmarks.
    \value LandmarkExchange Nokia Landmark Exchange format(lmx)
    \value GPSExchange  GPS Exchange format(gpx)
    \value KeyholeMarkupLanguage Keyhole Markup Language format (kml)
    \value CommaSeparatedValues Comma separated values format (csv)
    \value Custom Custom landmark format
*/

/*!
    \internal
*/
QLandmarkManager::QLandmarkManager(const QString &name, QObject *parent)
{
    //TODO: implement
}

/*!
    Frees the memory used by the QLandmarkManager
*/
QLandmarkManager::~QLandmarkManager()
{
    //TODO: implement
}

/*!
    Adds the given \a landmark to the database if \a landmark has a
    default-constructed identifer, or an identifier with the manager
    URI set to the URI of this manager and an empty id.

    If the manager URI of the identifier of the \a landmark is neither
    empty nor equal to the URI of this manager, or the id member of the
    identifier is not empty, but does not exist in the manager,
    the operation will fail and calling error() will return
    \c QLandmarkManager::DoesNotExistError.

    Alternatively, the function will update the existing landmark in the
    database if \a landmark has a non-empty id and currently exists
    within the database.

    Returns false on failure or true on success.  On successful save
    of a landmark with an empty id, it will be assigned a valid
    id and have its manager URI set to the URI of this manager.
*/
bool QLandmarkManager::saveLandmark(QLandmark *landmark)
{
    return false; //TODO: implement
}

/*!
    Adds the list of \a landmarks to the database.
    Returns true if the landmarks were saved successfully, otherwise returns
    false.

    The manager might populate \a errorMap (the map of indices of the
    \a landmarks list to the error which occurred when saving the landmark
    at that index) for every index for which the landmark could not be
    saved.

    The \l QLandmarkManager::error() function will only return \c
    QLandmarkManager::NoError if all landmarks were saved successfully.

    For each newly saved landmark that was successful, the identifier
    of the landmark will be updated with a new value.  If a failure
    occurs when saving a new landmark, the identifier will be cleared
    (and become an invalid identifier).
*/
bool QLandmarkManager::saveLandmarks(QList<QLandmark *> landmarks,
                                     QMap<int, QLandmarkManager::Error> *errorMap )
{
}

/*!
    Remove the landmark identified by \a landmarkId from the database.

    Returns true if the landmark was removed successfully, otherwise
    returnse false.
*/
bool QLandmarkManager::removeLandmark(const QLandmarkId &landmarkId)
{
    return false; //TODO: implement
}

/*!
    Remove every landmark whose identifier is contained in the list
    of \a landmarkIds.  Returns true if all landmarks were removed
    successfully, otherwise false.

    The manager might populate \a errorMap (the map of indices of the
    \a landmarkIds list to the error which occurred when saving the landmark
    at that index) for every index for which the landmark could not be
    removed.
    The \l QLandmarkManager::error() function will only return
    \c QLandmarkManager::NoError if all landmarks were removed successfully.

    \sa QLandmarkManager::removeLandmark()
*/
bool QLandmarkManager::removeLandmarks(const QList<QLandmarkId> &landmarkIds,
                                QMap<int, QLandmarkManager::Error> *errorMap)
{
}


/*!
    Adds the given \a category to the database if \a category has a
    default-constructed identifier, or an identifier with the manager
    URI set to the URI of this manager and an empty id.

    If the manager URI of the identifier of the \a category is neither
    empty nor equal to the URI  of this manager, or the id member of the
    identifier is not empty, but does not exist in the manager,
    the operation will fail and calling error() will return
    \c QLandmarkManager::DoesNotExistError.

    Alternatively, the function will update the existing category in the
    database if \a category has a non-empty id and currently exists
    within the database.

    Returns false on failure or true on success.  On successful save
    of a landmark with an empty id, it will be assigned a valid
    id and have its manager URI set to the URI of this manager.
*/
bool QLandmarkManager::saveCategory(QLandmarkCategory *category)
{
    return false; //TODO: implement
}

/*!
    Remove the category identified by \a categoryId from the database.
    The categoryId is cleared(and becomes invalid) on successful
    removal.  An unsuccessful removal will leave the identifer alone.

    Returns true if the category was removed successfully, otherwise
    returnse false.
*/
bool QLandmarkManager::removeCategory(const QLandmarkCategoryId &categoryId)
{
    return false; //TODO: implement
}

/*!
    Returns the cateory in the database identified by \a categoryId.
*/
QLandmarkCategory QLandmarkManager::category(const QLandmarkCategoryId &categoryId)
{
}

/*!
    Returns a list of all category identifiers.
*/
QList<QLandmarkCategoryId> QLandmarkManager::categoryIds() const
{
}

/*!
    Returns the landmark in the database identified by \a landmarkId
*/
QLandmark QLandmarkManager::landmark(const QLandmarkId &landmarkId)
{
}

/*!
    Returns a list of landmark ids that match the given \a filter, sorted
    according to the given \a sortOrder.
*/
QList<QLandmarkId> QLandmarkManager::landmarkIds( const QLandmarkFilter &filter,
                            const QLandmarkSortOrder &sortOrder) const
{
    return QList<QLandmarkId>(); //TODO: implement
}

/*!
    Reads landmarks from the given \a device.  The data from the \a device
    is expected to adhere to the provided \a format.

    Returns true if all landmarks could be imported, otherwise
    returns false.  It may be possible that only a subset of
    landmarks are imported.
*/
bool QLandmarkManager::importLandmarks(QIODevice *device, Format format)
{
    return false;//TODO: implement
}

/*!
    Writes landmarks to the given \a device.  The landmarks will be written
    according to the specified \a format.  If  \a landmarkIds is empty, then
    all landmarks will be exported, otherwise only those landmarks that
    match \a landmarkIds will be exported.

    Returns true if all specified landmarks were successfully exported,
    otherwise returns false.  It may be possible that only a subset
    of landmarks are exported.
*/
bool QLandmarkManager::exportLandmarks(QIODevice *device, Format format, QList<QLandmarkId> landmarkIds)
{
    return false; //TODO: implement
}

/*!
    Returns the error code of the most recent operation
*/
QLandmarkManager::Error QLandmarkManager::error() const
{
    return NoError; //TODO: implement
}

/*!
    Returns a short human-readable description of the error that occurred
    in the most recent operation.
*/
QString QLandmarkManager::errorString() const
{
    return QString(); //TODO: implement
}

/*!
    Returns a list of available manager names that can
    be used when constructing a QLandmarkManager
*/
QStringList QLandmarkManager::availableManagers()
{
    return QStringList(); //TODO: implement
}

/*!
    Registers a manager \a name so that QLandmarkManager instances
    based on it can be created.

    If the \a uri refers to a local database that does not
    exist, a database is created.

    Returns true if the manager was successfully added, otherwise
    returns false.
*/
bool QLandmarkManager::addManager(const QString &name, const QString &uri)
{
    return false; //TODO: impelment
}

/*!
    Unregisters a manager \a name.  If the manager's URI points
    to a local database then the database is deleted.

    Returns true if the manager was removed, otherwise
    returns false.
*/
bool QLandmarkManager::removeManager(const QString &name)
{
}

/*!
    Returns the default landmark manager's name
*/
QString QLandmarkManager::defaultManager()
{
    return QString(); //TODO: implement
}

/*!
    Sets the default manager to be the one identified
    by \a name.

    Returns true if a default was set, otherwise returnse false.
*/
bool QLandmarkManager::setDefaultManager(const QString &name)
{
    return false; //TODO: impelement
}

/*!
    \fn void QLandmarkManager::landmarkAdded(const QLandmarkId &landmarkId)

    This signal is emitted when a new landmark has been added to the
    database.   \a landmarkId is the identifier of new landmark.

    This signal will not be emitted as a result of importing landmarks.
    \sa landmarkUpdated(), landmarkRemoved()
*/

/*!
    \fn void QLandmarkManager::landmarkUpdated(const QLandmarkId &landmarkId)

    This signal is emitted when an existing landmark has had its details
    updated.  \a landmarkId is the identifier of the updated landmark.
*/

/*!
    \fn void QLandmarkManager::landmarkRemoved(const QLandmarkId &landmarkId)

    This signal is emitted when a landmark has been removed from the
    database.  \a landmarkId is the identifier of the removed landmark.
*/

/*!
    \fn void QLandmarkManager::categoryAdded(const QLandmarkCategoryId &categoryId)

    This signal is emitted when a new landmark has been added to the
    database.  \a categoryId is the identifier of new landmark.

    This signal will not be emitted as a result of importing landmarks.
    \sa landmarkUpdated(), landmarkRemoved()
*/

/*!
    \fn void QLandmarkManager::categoryUpdated(const QLandmarkCategoryId &categoryId)

    This signal is emitted when an existing landmark has had its details
    updated.  \a categoryId is the identifier of the updated landmark.
*/

/*!
    \fn void QLandmarkManager::categoryRemoved(const QLandmarkCategoryId &categoryId)

    This signal is emitted when a landmark has been removed from the
    database.  \a categoryId is the identifier of the removed landmark.
*/

#include "moc_qlandmarkmanager.cpp"
