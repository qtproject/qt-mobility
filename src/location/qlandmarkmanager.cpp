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
#include "qlandmarkmanager_p.h"

#include "qlandmarkmanagerenginefactory_sqlite_p.h"

#include <QStringList>
#include <QString>
#include <QIODevice>
#include <QFile>
#include <QUrl>
#include "qlandmarkcategoryid.h"
#include "qlandmarkcategory.h"
#include "qlandmark.h"
#include "qlandmarkmanagerengine.h"


QTM_USE_NAMESPACE
/*!
    \class QLandmarkManager
    \brief The QLandmarkManager class provides an interface for storage
    and retrieval of landmarks from a landmark store.
    \ingroup location
*/

/*!
    \enum QLandmarkManager::Error
    Defines the possible errors for the landmark manager.
    \value NoError No error occurred.
    \value DoesNotExistError The most recent operation failed because the requested landmark or category does not exist.
    \value AlreadyExistsError The most recent operation failed because the specified landmark or category already exists.
    \value LockedError The most recent operation failed because the datastore specified is currently locked.
    \value PermissionsError The most recent operation failed because the caller does not have permission to perform the operation.
    \value OutOfMemoryError The most recent operation failed due to running out of memory.
    \value VersionMismatchError The most recent operation failed because the backend of the manager is not of the required version.
    \value NotSupportedError The most recent operation failed because the requested operation is not supported in the specified store.
    \value BadArgumentError The most recent operation failed because one or more of the parameters to the operation were invalid.
    \value UnknownError The most recent operation failed for an unknown reason.
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
    Constructs a QLandmarkManager whose implementation is identified by \a managerName with the given
    \a parameters.

    The \a parent QObject will be used as the parent of this QLandmarkManager.

    If an empty \a managerName is specified, the default implementation for the platform will be used.
*/
QLandmarkManager::QLandmarkManager(const QString &managerName, const QMap<QString, QString> &parameters, QObject *parent)
        : d_ptr(new QLandmarkManagerPrivate())
{
    Q_D(QLandmarkManager);

    if (!availableManagers().contains(managerName)) {
        d->errorCode = NotSupportedError;
        d->errorString = QString("The landmark engine %1 is not supported.").arg(managerName);
        d->engine = 0;
        return;
    }

    if (managerName == "com.nokia.qt.landmarks.engines.sqlite") {
        d->engine = QLandmarkManagerEngineFactorySqlite::engine(parameters,
                                                                &(d->errorCode),
                                                                &(d->errorString));
    } else {
        // set some kind of error? or use the else instead of the contains check?
        // should there be a static list of factories for this and for availableManagers()?
    }

}

/*!
  Constructs a QLandmarkManager whose backend has the name \a managerName and version \a implementationVersion, where the manager
  is constructed with the provided \a parameters.

  The \a parent QObject will be used as the parent of this QLandmarkManager.

  If an empty \a managerName is specified, the default implementation for the platform will be instantiated.
  If the specified implementation version is not available, the manager with the name \a managerName with the default implementation version is instantiated.
 */
QLandmarkManager::QLandmarkManager(const QString& managerName, int implementationVersion, const QMap<QString, QString>& parameters, QObject* parent)
        : d_ptr(new QLandmarkManagerPrivate())
{
    Q_D(QLandmarkManager);

    if (!availableManagers().contains(managerName)) {
        d->errorCode = NotSupportedError;
        d->errorString = QString("The landmark engine %1 is not supported.").arg(managerName);
        d->engine = 0;
        return;
    }

    if (managerName == "com.nokia.qt.landmarks.engines.sqlite") {
        d->engine = QLandmarkManagerEngineFactorySqlite::engine(parameters,
                                                                &(d->errorCode),
                                                                &(d->errorString));
        // TODO check version versus supported versions - seems kind of pointless unless the above takes a version argument
    } else {
        // set some kind of error? or use the else instead of the contains check?
        // should there be a static list of factories for this and for availableManagers()?
    }
}

/*!
    Frees the memory used by the QLandmarkManager
*/
QLandmarkManager::~QLandmarkManager()
{
    Q_D(QLandmarkManager);

    if (d->engine)
        delete d->engine;

    delete d;
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
    Q_D(QLandmarkManager);

    if (!d->engine)
        return false;

    return d->engine->saveLandmark(landmark,
                                   &(d->errorCode),
                                   &(d->errorString));
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
bool QLandmarkManager::saveLandmarks(QList<QLandmark> *landmarks,
                                     QMap<int, QLandmarkManager::Error> *errorMap)
{
    Q_D(QLandmarkManager);

    if (!d->engine)
        return false;

    return d->engine->saveLandmarks(landmarks,
                                    errorMap,
                                    &(d->errorCode),
                                    &(d->errorString));
}

/*!
    Remove the landmark identified by \a landmarkId from the database.

    Returns true if the landmark was removed successfully, otherwise
    returnse false.
*/
bool QLandmarkManager::removeLandmark(const QLandmarkId &landmarkId)
{
    Q_D(QLandmarkManager);

    if (!d->engine)
        return false;

    return d->engine->removeLandmark(landmarkId,
                                     &(d->errorCode),
                                     &(d->errorString));
}

/*!
    Removes every landmark whose identifier is contained in the list
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
    Q_D(QLandmarkManager);

    if (!d->engine)
        return false;

    return d->engine->removeLandmarks(landmarkIds,
                                      errorMap,
                                      &(d->errorCode),
                                      &(d->errorString));
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
    of a category with an invalid id, it will be assigned a valid
    id and have its manager URI set to the URI of this manager.
*/
bool QLandmarkManager::saveCategory(QLandmarkCategory *category)
{
    Q_D(QLandmarkManager);

    if (!d->engine)
        return false;

    return d->engine->saveCategory(category,
                                   &(d->errorCode),
                                   &(d->errorString));
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
    Q_D(QLandmarkManager);

    if (!d->engine)
        return false;

    return d->engine->removeCategory(categoryId,
                                     &(d->errorCode),
                                     &(d->errorString));
}

/*!
    Returns the cateory in the database identified by \a categoryId.
*/
QLandmarkCategory QLandmarkManager::category(const QLandmarkCategoryId &categoryId) const
{
    Q_D(const QLandmarkManager);

    if (!d->engine)
        return QLandmarkCategory();

    QLandmarkCategory cat = d->engine->category(categoryId,
                                                &(d->errorCode),
                                                &(d->errorString));

    if (d->errorCode != NoError)
        return QLandmarkCategory();

    return cat;
}

/*!
    Returns a list of categories identified by \a categoryIds.

    If any of the \a categoryIds are not found by the manager, they are simply ignored.
 */
QList<QLandmarkCategory> QLandmarkManager::categories(const QList<QLandmarkCategoryId> &categoryIds) const
{
    Q_D(const QLandmarkManager);

    if (!d->engine)
        return QList<QLandmarkCategory>();

    QList<QLandmarkCategory> cats = d->engine->categories(categoryIds,
                          &(d->errorCode),
                          &(d->errorString));

    if (d->errorCode != NoError)
        return QList<QLandmarkCategory>();

    return cats;
}

/*!
    Returns a list of all category identifiers.
*/
QList<QLandmarkCategoryId> QLandmarkManager::categoryIds() const
{
    Q_D(const QLandmarkManager);

    if (!d->engine)
        return QList<QLandmarkCategoryId>();

    QList<QLandmarkCategoryId> ids = d->engine->categoryIds(&(d->errorCode),
                                                            &(d->errorString));

    if (d->errorCode != NoError)
        return QList<QLandmarkCategoryId>();

    return ids;
}

/*!
    Returns the landmark in the database identified by \a landmarkId
*/
QLandmark QLandmarkManager::landmark(const QLandmarkId &landmarkId) const
{
    Q_D(const QLandmarkManager);

    if (!d->engine)
        return QLandmark();

    QLandmark lm = d->engine->landmark(landmarkId,
                                       &(d->errorCode),
                                       &(d->errorString));

    if (d->errorCode != NoError)
        return QLandmark();

    return lm;
}

/*!
    Returns a list of landmarks which match the given \a filter and are sorted according to the \a sortOrders.
*/
QList<QLandmark> QLandmarkManager::landmarks(const QLandmarkFilter &filter, const QList<QLandmarkSortOrder> &sortOrders) const
{
    Q_D(const QLandmarkManager);

    if (!d->engine)
        return QList<QLandmark>();

    QList<QLandmark> lms = d->engine->landmarks(filter,
                                                sortOrders,
                                                &(d->errorCode),
                                                &(d->errorString));

    if (d->errorCode != NoError)
        return QList<QLandmark>();

    return lms;
}

/*!
    Returns a list of landmarks which match the given \a landmarkIds.

*/
QList<QLandmark> QLandmarkManager::landmarks(const QList<QLandmarkId> &landmarkIds) const
{
    Q_D(const QLandmarkManager);

    if (!d->engine)
        return QList<QLandmark>();

    // use the error map to add to the error string?
    // or use it to remove the landmarks which had errors?
    QList<QLandmark> lms = d->engine->landmarks(landmarkIds,
                                                0,
                                                &(d->errorCode),
                                                &(d->errorString));
    if (d->errorCode != NoError)
        return QList<QLandmark>();

    return lms;
}

/*!
    Returns a list of landmark identifiers of landmarks that match the given \a filter, sorted
    according to the given \a sortOrders.
*/
QList<QLandmarkId> QLandmarkManager::landmarkIds(const QLandmarkFilter &filter,
        const QList<QLandmarkSortOrder> &sortOrders) const
{
    Q_D(const QLandmarkManager);

    if (!d->engine)
        return QList<QLandmarkId>();

    QList<QLandmarkId> ids = d->engine->landmarkIds(filter,
                                                    sortOrders,
                                                    &(d->errorCode),
                                                    &(d->errorString));

    if (d->errorCode != NoError)
        return QList<QLandmarkId>();

    return ids;
}

/*!
    Reads landmarks from the given \a device and saves them.  The data from the \a device
    is expected to adhere to the provided \a format.

    Returns true if all landmarks could be imported, otherwise
    returns false.  It may be possible that only a subset of
    landmarks are imported.
*/
bool QLandmarkManager::importLandmarks(QIODevice *device, Format format)
{
    Q_D(QLandmarkManager);

    if (!d->engine)
        return false;

    return d->engine->importLandmarks(device,
                                      format,
                                      &(d->errorCode),
                                      &(d->errorString));
}

/*!
    Convenience function that will read landmarks from \a fileName in
    the expected \a format.  Internally a QFile is opened with
    QIODevice::ReadOnly permissions.

    Returns true if all landmarks could be imported, otherwise
    returns false.  It may be possible that only a subset of landmarks
    are imported.
*/
bool QLandmarkManager::importLandmarks(const QString &fileName, Format format)
{
    QFile file(fileName);

    if (!file.exists()) {
        // TODO set error
        return false;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        // TODO set error
        return false;
    }

    return importLandmarks(&file, format);
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
    Q_D(QLandmarkManager);

    if (!d->engine)
        return false;

    return d->engine->exportLandmarks(device,
                                      format,
                                      landmarkIds,
                                      &(d->errorCode),
                                      &(d->errorString));
}

/*!
    Convenience function that will write landmarks to \a fileName in the expected
    \a format. Internally a QFile is opened with QIODevice::WriteOnly permissions.
    If \a landmarkIds is empty, then all landmarks will be exported, otherwise
    only those landmarks that match \a landmarkIds will be exported.

    Returns true if all specified landmarks were successfully exported,
    otherwise returns false.  It may be possible that only a subset
    of landmarks are exported.
*/
bool QLandmarkManager::exportLandmarks(const QString &fileName, Format format, QList<QLandmarkId> landmarkIds)
{
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly)) {
        // TODO set error
        return false;
    }

    return exportLandmarks(&file, format, landmarkIds);
}

/*!
    Returns the error code of the most recent operation
*/
QLandmarkManager::Error QLandmarkManager::error() const
{
    Q_D(const QLandmarkManager);
    return d->errorCode;
}

/*!
    Returns a short human-readable description of the error that occurred
    in the most recent operation.
*/
QString QLandmarkManager::errorString() const
{
    Q_D(const QLandmarkManager);
    return d->errorString;
}

/*!
    Returns true if the given \a filterType is supported
    natively by the manager, else false if the filter behaviour would be emulated.

    Note: In some cases, the behaviour of an unsupprted filter cannot be emulated.
    In these cases the filter will fail.
*/
bool QLandmarkManager::isFilterSupported(QLandmarkFilter::FilterType filterType) const
{
    Q_D(const QLandmarkManager);

    if (!d->engine)
        return false;

    return d->engine->isFilterSupported(filterType);
}

/*!
    Returns the manager name for this QLandmarkManager
*/
QString QLandmarkManager::managerName() const
{
    Q_D(const QLandmarkManager);

    if (!d->engine)
        return QString();

    return d->engine->managerName();
}

/*!
    Return the parameters relevant to the creation of this QLandmarkManager
*/
QMap<QString, QString> QLandmarkManager::managerParameters() const
{
    Q_D(const QLandmarkManager);

    if (!d->engine)
        return QMap<QString, QString>();

    return d->engine->managerParameters();
}

/*!
  Return the uri describing this QLandmarkManager, consisting of the manager name and any parameters.
 */
QString QLandmarkManager::managerUri() const
{
    Q_D(const QLandmarkManager);

    if (!d->engine)
        return QString();

    return d->engine->managerUri();
}

/*!
  Returns the engine backend implementation version number.
*/
int QLandmarkManager::managerVersion() const
{
    Q_D(const QLandmarkManager);

    if (!d->engine)
        return 0;

    return d->engine->managerVersion();
}

/*!
    Returns a list of available manager names that can
    be used when constructing a QLandmarkManager
*/
QStringList QLandmarkManager::availableManagers()
{
    QStringList names;
    names << "com.nokia.qt.landmarks.engines.sqlite";
    return names;
}

/*! Returns a URI that completely describes a manager implementation, datastore,
    and the parameters with which to instantiate the manager,
    from the given \a managerName, \a params and an optional \a implementationVersion */
QString QLandmarkManager::buildUri(const QString& managerName, const QMap<QString, QString>& params, int implementationVersion)
{
    QUrl url;
    url.setHost(managerName);
    url.setPath(QString::number(implementationVersion));

    QList<QString> keys = params.keys();
    for (int i = 0; i < keys.size(); ++i) {
        QString key = keys.at(i);
        url.addQueryItem(key, params.value(key));
    }

    return url.toString();
}

/*!
  Constructs a QLandmarkManager whose implementation, store and parameters are specified in the given \a storeUri,
  and whose parent object is \a parent.
 */
QLandmarkManager* QLandmarkManager::fromUri(const QString& storeUri, QObject* parent)
{
    return NULL;
    QUrl url = QUrl::fromEncoded(storeUri.toUtf8(), QUrl::StrictMode);
    if (!url.isValid()) {
        return NULL;
    }

    QString managerName = url.host();

    bool versionOk = false;
    int version = -1;
    if (!url.path().isEmpty()) {
        version = url.path().toInt(&versionOk);
        if (!ok)
            return NULL;
    }

    QMap<QString, QString> parameters;

    // TODO error on duplicate keys?
    QList<QPair<QString, QString> > items = url.queryItems();
    for (int i = 0; i < items.size(); ++i) {
        parameters[items.at(i).first] = items.at(i).second;
    }

    if (versionOk) {
        return new QLandmarkManager(managerName, paramaters, parent);
    } else {
        return new QLandmarkManager(managerName, version, parameters, parent);
    }
}

/*!
  Splits the given \a uri into the manager, store, and parameters that it describes, and places the information
  into the memory addressed by \a pManagerId and \a pParams respectively.  Returns true if \a uri could be split successfully,
  otherwise returns false
 */
bool QLandmarkManager::parseUri(const QString& uri, QString* pManagerId, QMap<QString, QString>* pParams)
{
    QUrl url = QUrl::fromEncoded(uri.toUtf8(), QUrl::StrictMode);
    if (!url.isValid()) {
        return false;
    }

    *pManagerId = url.host();

    // TODO error on duplicate keys?
    QList<QPair<QString, QString> > items = url.queryItems();
    for (int i = 0; i < items.size(); ++i) {
        pParams->insert(items.at(i).first, items.at(i).second);
    }

    return true;
}

/*!
    \fn QLandmarkManager::dataChanged()
    This signal is emitted by the manager if its internal state changes and it is unable to precisely determine
    the changes which occurred, or if the manager considers the changes to be radical enough to require clients to reload
    all data.  If the signal is emitted, no other signals will be emitted for the associated changes.
*/

/*!
    \fn void QLandmarkManager::landmarksAdded(const QList<QLandmarkId> &landmarkIds)

    This signal is emitted when landmarks (identified by \a landmarkIds) have been added to the datastore managed by this manager.
    This signal is not emitted if the dataChanged() signal was previously emitted for these changes.

    \sa landmarksChanged(), landmarksRemoved()
*/

/*!
    \fn void QLandmarkManager::landmarksChanged(const QList<QLandmarkId> &landmarkIds)

    This signal is emitted when landmarks (identified by \a landmarkIds) have been modified in the datastore managed by this manager.
    This signal is not emitted if the dataChanged() signal was previously emitted for these changes.

    \sa landmarksAdded(), landmarksRemoved()
*/

/*!
    \fn void QLandmarkManager::landmarksRemoved(const QList<QLandmarkId> &landmarkIds)

    This signal is emitted when landmarks (identified by \a landmarkIds) have been removed from the datastore managed by this manager.
    This signal is not emitted if the dataChanged() signal was previously emitted for these changes.

    \sa landmarksAdded(), landmarksChanged()
*/

/*!
    \fn void QLandmarkManager::categoriesAdded(const QList<QLandmarkCategoryId> &categoryIds)

    This signal is emitted when categories (identified by \a categoryIds) have been added to the datastore managed by this manager.
    This signal is not emitted if the dataChanged() signal was previously emitted for these changes.

    \sa categoriesChanged(), categoriesRemoved()
*/

/*!
    \fn void QLandmarkManager::categoriesChanged(const QList<QLandmarkCategoryId> &categoryIds)

    This signal is emitted when categories (identified by \a categoryIds) have been modified in the datastore managed by this manager.
    This signal is not emitted if the dataChanged() signal was previously emitted for these changes.

    \sa categoriesAdded(), categoriesRemoved()
*/

/*!
    \fn void QLandmarkManager::categoriesRemoved(const QList<QLandmarkCategoryId> &categoryIds)

    This signal is emitted when categories (identified by \a categoryIds) have been removed from the datastore managed by this manager.
    This signal is not emitted if the dataChanged() signal was previously emitted for these changes.

    \sa categoriesAdded(), categoriesChanged()
*/

/*******************************************************************************
*******************************************************************************/

QLandmarkManagerPrivate::QLandmarkManagerPrivate()
    : engine(0),
    errorCode(QLandmarkManager::NoError)
{
}

#include "moc_qlandmarkmanager.cpp"
