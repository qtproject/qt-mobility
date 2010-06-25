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

#include <QStringList>
#include <QString>
#include <QIODevice>
#include <QFile>
#include <QUrl>
#include <QPluginLoader>
#include <QDebug>
#include "qlandmarkcategoryid.h"
#include "qlandmarkcategory.h"
#include "qlandmark.h"
#include "qlandmarkmanagerengine.h"
#include "qlandmarkmanagerenginefactory.h"
#include "qlandmarkidfilter.h"

QTM_USE_NAMESPACE
/*!
    \class QLandmarkManager
    \brief The QLandmarkManager class provides an interface for storage
    and retrieval of landmarks from a landmark store.

    \inmodule QtLocation
    
    \ingroup landmarks-main
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
    \value InvalidManagerError The most recent operation failed because the manager failed to initialize correctly and is invalid.
           This could be due using a manager name that is not recognised/available. A landmark request object will return this error if
           if is assigned a null manager pointer.
    \value UnknownError The most recent operation failed for an unknown reason.
*/

/*!
    \enum QLandmarkManager::LandmarkFeature
    Defines the possible features the landmark manager can support.
    \value GenericAttributes The manager supports landmarks and categories which have generic attributes
*/

/*!
    \enum QLandmarkManager::FilterSupportLevel
    Defines the possible support levels the manager can provide for a given filter.
    \value Native The manager natively supports the filter.
    \value Emulated The manager emulates the behaviour of the filter.  An emulated filter will inherently be slower than a natively supported filter.
    \value None The manager does not support the filter at all.
*/


/*!
    Constructs a QLandmarkManager. The default implementation for the platform will be used.

    The \a parent QObject will be used as the parent of this QLandmarkManager.
*/
QLandmarkManager::QLandmarkManager(QObject *parent)
        : QObject(parent),
          d_ptr(new QLandmarkManagerPrivate())
{
    Q_D(QLandmarkManager);
    d->q_ptr = this;
    QString managerName;

#ifdef Q_OS_SYMBIAN
#else
    managerName = "com.nokia.qt.landmarks.engines.sqlite";
#endif

    d->createEngine(managerName);
    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager, name: %1").arg(managerName);
        qWarning() << "Invalid QLandmarkManager instantiated with name: " << managerName;
    }
}

/*!
    Constructs a QLandmarkManager whose implementation is identified by \a managerName with the given
    \a parameters.

    The \a parent QObject will be used as the parent of this QLandmarkManager.

    If an empty \a managerName is specified, the default implementation for the platform will be used.
*/
QLandmarkManager::QLandmarkManager(const QString &managerName, const QMap<QString, QString> &parameters, QObject *parent)
        : QObject(parent),
          d_ptr(new QLandmarkManagerPrivate())
{
    Q_D(QLandmarkManager);
    d->q_ptr = this;
    d->createEngine(managerName, parameters);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager, name: %1").arg(managerName);
        qWarning() << "Invalid QLandmarkManager instantiated with name: " << managerName;
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
        : QObject(parent),
          d_ptr(new QLandmarkManagerPrivate())
{
    Q_D(QLandmarkManager);
    d->q_ptr = this;
    QMap<QString,QString> params = parameters;
    params[QString(QTLANDMARKS_IMPLEMENTATION_VERSION_NAME)] = QString::number(implementationVersion);
    d->createEngine(managerName, params);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager, name: %1").arg(managerName);
        qWarning() << "Invalid QLandmarkManager instantiated with name: " << managerName;
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

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return false;
    }

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

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return false;
    }

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

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return false;
    }

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

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return false;
    }

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

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return false;
    }

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

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return false;
    }

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

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return QLandmarkCategory();
    }

    QLandmarkCategory cat = d->engine->category(categoryId,
                            &(d->errorCode),
                            &(d->errorString));

    if (d->errorCode != NoError)
        return QLandmarkCategory();

    return cat;
}

/*!
    Returns a list of categories identified by \a categoryIds.
    The returned list is sorted in alphabetical order according to the
    category name.

    if \a categoryIds is empty, then all categories are returned.

    If any of the \a categoryIds are not found by the manager, they are simply ignored.
 */
QList<QLandmarkCategory> QLandmarkManager::categories(const QList<QLandmarkCategoryId> &categoryIds) const
{
    Q_D(const QLandmarkManager);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return QList<QLandmarkCategory>();
    }

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

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return QList<QLandmarkCategoryId>();
    }

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

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return QLandmark();
    }

    QLandmark lm = d->engine->landmark(landmarkId,
                                       &(d->errorCode),
                                       &(d->errorString));

    if (d->errorCode != NoError)
        return QLandmark();

    return lm;
}

/*!
    Returns a list of landmarks which match the given \a filter and are sorted according to the \a sortOrders.
    Various fetch operation parameters may be specified by \a fetchHint.
*/
QList<QLandmark> QLandmarkManager::landmarks(const QLandmarkFilter &filter, const QList<QLandmarkSortOrder> &sortOrders,
                                             const QLandmarkFetchHint &fetchHint) const
{
    Q_D(const QLandmarkManager);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return QList<QLandmark>();
    }

    QList<QLandmark> lms = d->engine->landmarks(filter,
                           sortOrders,
                           fetchHint,
                           &(d->errorCode),
                           &(d->errorString));

    if (d->errorCode != NoError)
        return QList<QLandmark>();

    return lms;
}

/*!
    Returns a list of landmarks which match the given \a filter and are sorted according to the given \a sortOrder.
    Various fetch operation parameters may be specified by \a fetchHint.
*/
QList<QLandmark> QLandmarkManager::landmarks(const QLandmarkFilter &filter, const QLandmarkSortOrder &sortOrder,
                                             const QLandmarkFetchHint &fetchHint) const
{
    Q_D(const QLandmarkManager);

     if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return QList<QLandmark>();
    }

    QList<QLandmarkSortOrder> sortOrders;
    if (sortOrder.type() != QLandmarkSortOrder::DefaultSort)
        sortOrders.append(sortOrder);

    QList<QLandmark> lms = d->engine->landmarks(filter,
                           sortOrders,
                           fetchHint,
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

     if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return QList<QLandmark>();
    }

    QLandmarkIdFilter idFilter(landmarkIds);
    idFilter.setMatchingScheme(QLandmarkIdFilter::MatchAll);
    QList<QLandmarkSortOrder> sortOrders;

    // use the error map to add to the error string?
    // or use it to remove the landmarks which had errors?

    QList<QLandmark> lms = d->engine->landmarks(idFilter,
                                                sortOrders,
                                                QLandmarkFetchHint(),
                                                &(d->errorCode),
                                                &(d->errorString));

    if (d->errorCode != NoError)
        return QList<QLandmark>();

    return lms;
}

/*!
    Returns a list of landmark identifiers of landmarks that match the given \a filter, sorted
    according to the given \a sortOrders.  Various fetch operation parameters may be specified by \a fetchHint.
*/
QList<QLandmarkId> QLandmarkManager::landmarkIds(const QLandmarkFilter &filter,
                                                 const QList<QLandmarkSortOrder> &sortOrders,
                                                 const QLandmarkFetchHint &fetchHint) const
{
    Q_D(const QLandmarkManager);

     if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return QList<QLandmarkId>();
    }

    QList<QLandmarkId> ids = d->engine->landmarkIds(filter,
                             sortOrders,
                             fetchHint,
                             &(d->errorCode),
                             &(d->errorString));

    if (d->errorCode != NoError)
        return QList<QLandmarkId>();

    return ids;
}

/*!
    Convenience function for returning a list of landmark identifiers of landmarks that match the given \a filter, sorted
    according to the given \a sortOrder. Various fetch operation parameters may be specified by \a fetchHint.

    This is a convenience function.
*/
QList<QLandmarkId> QLandmarkManager::landmarkIds(const QLandmarkFilter &filter, const QLandmarkSortOrder &sortOrder,
                                                 const QLandmarkFetchHint &fetchHint) const
{
    Q_D(const QLandmarkManager);

     if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return QList<QLandmarkId>();
    }

    QList<QLandmarkSortOrder> sortOrders;
    sortOrders.append(sortOrder);

    QList<QLandmarkId> ids = d->engine->landmarkIds(filter,
                             sortOrders,
                             fetchHint,
                             &(d->errorCode),
                             &(d->errorString));

    if (d->errorCode != NoError)
        return QList<QLandmarkId>();

    return ids;
}

/*!
    Reads landmarks from the given \a device and saves them.  The data from the \a device
    is expected to adhere to the provided \a format.  If no \a format is provided,
    the manager tries to auto detect the \a format.

    Returns true if all landmarks could be imported, otherwise
    returns false.  It may be possible that only a subset of
    landmarks are imported.
*/
bool QLandmarkManager::importLandmarks(QIODevice *device, const QByteArray &format)
{
    Q_D(QLandmarkManager);

     if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return false;
    }

    return d->engine->importLandmarks(device,
                                      format,
                                      &(d->errorCode),
                                      &(d->errorString));
}

/*!
    Convenience function that will read landmarks from \a fileName in
    the expected \a format. If no \a format is provided, the manager tries
    to auto detect the \a format.  Internally a QFile is opened with
    QIODevice::ReadOnly permissions.

    Returns true if all landmarks could be imported, otherwise
    returns false.  It may be possible that only a subset of landmarks
    are imported.
*/
bool QLandmarkManager::importLandmarks(const QString &fileName, const QByteArray &format)
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
bool QLandmarkManager::exportLandmarks(QIODevice *device, const QByteArray &format, QList<QLandmarkId> landmarkIds)
{
    Q_D(QLandmarkManager);

     if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return false;
    }

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
bool QLandmarkManager::exportLandmarks(const QString &fileName, const QByteArray &format, QList<QLandmarkId> landmarkIds)
{
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly)) {
        // TODO set error
        return false;
    }

    return exportLandmarks(&file, format, landmarkIds);
}

/*!
    Returns the error code of the most recent operation.  All functions will modify the error based on whether the
    operation successfully or not.
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
    Returns whether the manager supports the given \a feature.
*/
bool QLandmarkManager::isFeatureSupported(QLandmarkManager::LandmarkFeature feature) const
{
    Q_D(const QLandmarkManager);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return false;
    }

    return d->engine->isFeatureSupported(feature);
}

/*!
    Returns the support level the manager provides for the given \a filter.
*/
QLandmarkManager::FilterSupportLevel QLandmarkManager::filterSupportLevel(const QLandmarkFilter &filter) const
{
    Q_D(const QLandmarkManager);

     if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return QLandmarkManager::None;
    }

    d->errorCode = QLandmarkManager::NoError;
    d->errorString = "";
    return d->engine->filterSupportLevel(filter);
}

/*!
    Returns true if the manager is entirely read-only.  Meaning
    landmarks and categories cannot be added, modified or removed.
*/
bool QLandmarkManager::isReadOnly() const
{
    Q_D(const QLandmarkManager);
    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return true;
    }

    d->errorCode = QLandmarkManager::NoError;
    d->errorString = "";
    return d->engine->isReadOnly();
}

/*!
    Returns true if the landmark identified by \a landmarkId
    considered read-only by the manager.

    If the \a landmarkId does not refer to an existing landmark,
    it is considered writable unless the manager is exclusively read-only.
*/
bool QLandmarkManager::isReadOnly(const QLandmarkId &landmarkId) const
{
    Q_D(const QLandmarkManager);
    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return true;
    }

    return d->engine->isReadOnly(landmarkId);
}

/*!
    Returns true if the category identified by \a categoryId is
    considered read-only by the manager.

    If \a categoryId does not refer to an existing category,
    it is considered writable unless the manager is exclusively read-only.
*/
bool QLandmarkManager::isReadOnly(const QLandmarkCategoryId &categoryId) const
{
    Q_D(const QLandmarkManager);
    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return true;
    }

    return d->engine->isReadOnly(categoryId);
}

/*!
    Returns the manager name for this QLandmarkManager.

    The manager name usually takes the format of a reverse domain string.  An example
    of a manager name is \c com.nokia.qt.landmarks.engines.sqlite
*/
QString QLandmarkManager::managerName() const
{
    Q_D(const QLandmarkManager);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return QString();
    }

    d->errorCode = QLandmarkManager::NoError;
    d->errorString = "";
    return d->engine->managerName();
}

/*!
    Return the parameters relevant to the creation of this QLandmarkManager.

    The parameters may be viewed as a set of key-value pairs.  Each manager
    may have a different set of parameters depending upon its backend implementation.
*/
QMap<QString, QString> QLandmarkManager::managerParameters() const
{
    Q_D(const QLandmarkManager);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return QMap<QString, QString>();
    }

    d->errorCode = QLandmarkManager::NoError;
    d->errorString = "";
    return d->engine->managerParameters();
}

/*!
  Return the uri describing this QLandmarkManager, consisting of the manager name and any parameters.
 */
QString QLandmarkManager::managerUri() const
{
    Q_D(const QLandmarkManager);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return QString();
    }

    d->errorCode = QLandmarkManager::NoError;
    d->errorString = "";
    return d->engine->managerUri();
}

/*!
  Returns the engine backend implementation version number.
*/
int QLandmarkManager::managerVersion() const
{
    Q_D(const QLandmarkManager);

    if (!d->engine) {
        d->errorCode = QLandmarkManager::InvalidManagerError;
        d->errorString = QString("Invalid Manager");
        return 0;
    }

    d->errorCode = QLandmarkManager::NoError;
    d->errorString = "";
    return d->engine->managerVersion();
}

/*!
    Returns a list of available manager names that can
    be used when constructing a QLandmarkManager
*/
QStringList QLandmarkManager::availableManagers()
{
    return QLandmarkManagerPrivate::factories().keys();
}

/*! Returns a URI that completely describes a manager implementation, datastore,
    and the parameters with which to instantiate the manager,
    from the given \a managerName, \a params and an optional \a implementationVersion */
QString QLandmarkManager::buildUri(const QString& managerName, const QMap<QString, QString>& params, int implementationVersion)
{
    QString ret(QLatin1String("qtlandmarks:%1:%2"));
    // we have to escape each param
    QStringList escapedParams;
    QStringList keys = params.keys();
    for (int i=0; i < keys.size(); i++) {
        QString key = keys.at(i);
        QString arg = params.value(key);
        arg = arg.replace(QLatin1Char('&'), QLatin1String("&amp;"));
        arg = arg.replace(QLatin1Char('='), QLatin1String("&equ;"));
        key = key.replace(QLatin1Char('&'), QLatin1String("&amp;"));
        key = key.replace(QLatin1Char('='), QLatin1String("&equ;"));
        key = key + QLatin1Char('=') + arg;
        escapedParams.append(key);
    }

    if (implementationVersion != -1) {
        QString versionString = QString(QLatin1String(QTLANDMARKS_IMPLEMENTATION_VERSION_NAME));
        versionString += QString::fromAscii("=");
        versionString += QString::number(implementationVersion);
        escapedParams.append(versionString);
    }

    return ret.arg(managerName, escapedParams.join(QLatin1String("&")));
}

/*!
  Constructs a QLandmarkManager whose implementation, store and parameters are specified in the given \a storeUri,
  and whose parent object is \a parent.
 */
QLandmarkManager* QLandmarkManager::fromUri(const QString& storeUri, QObject* parent)
{
    if (storeUri.isEmpty()) {
        return new QLandmarkManager(QString(), QMap<QString, QString>(), parent);
    } else {
        QString id;
        QMap<QString, QString> parameters;
        if (parseUri(storeUri, &id, &parameters)) {
            return new QLandmarkManager(id, parameters, parent);
        } else {
            // invalid
            return NULL;
        }
    }
}

/*!
  Splits the given \a uri into the manager, store, and parameters that it describes, and places the information
  into the memory addressed by \a pManagerId and \a pParams respectively.  Returns true if \a uri could be split successfully,
  otherwise returns false
 */
bool QLandmarkManager::parseUri(const QString& uri, QString* pManagerId, QMap<QString, QString>* pParams)
{
    // Format: qtlandmarks:<managerid>:<key>=<value>&<key>=<value>
    // - it is assumed the prefix(qtlandmarks) and managerid cannot contain ':'
    // - it is assumed keys and values do not contain '=' or '&'
    //   but can contain &amp; and &equ;
    QStringList colonSplit = uri.split(QLatin1Char(':'));
    QString prefix = colonSplit.value(0);

    if (prefix != QLatin1String("qtlandmarks"))
        return false;

    QString managerName = colonSplit.value(1);

    if (managerName.trimmed().isEmpty())
        return false;

    QString firstParts = prefix + QLatin1Char(':') + managerName + QLatin1Char(':');
    QString paramString = uri.mid(firstParts.length());

    QMap<QString, QString> outParams;

    // Now we have to decode each parameter
    if (!paramString.isEmpty()) {
        QStringList params = paramString.split(QRegExp(QLatin1String("&(?!(amp;|equ;))")), QString::KeepEmptyParts);
        // If we have an empty string for paramstring, we get one entry in params,
        // so skip that case.
        for(int i = 0; i < params.count(); i++) {
            /* This should be something like "foo&amp;bar&equ;=grob&amp;" */
            QStringList paramChunk = params.value(i).split(QLatin1String("="), QString::KeepEmptyParts);

            if (paramChunk.count() != 2)
                return false;

            QString arg = paramChunk.value(0);
            QString param = paramChunk.value(1);
            arg.replace(QLatin1String("&equ;"), QLatin1String("="));
            arg.replace(QLatin1String("&amp;"), QLatin1String("&"));
            param.replace(QLatin1String("&equ;"), QLatin1String("="));
            param.replace(QLatin1String("&amp;"), QLatin1String("&"));
            if (arg.isEmpty())
                return false;
            outParams.insert(arg, param);
        }
    }

    if (pParams)
        *pParams = outParams;
    if (pManagerId)
        *pManagerId = managerName;
    return true;
}

QLandmarkManagerEngine *QLandmarkManager::engine()
{
    return d_ptr->engine;
}

/*!
    \fn void QLandmarkManager::landmarksAdded(const QList<QLandmarkId> &landmarkIds)

    This signal is emitted when landmarks (identified by \a landmarkIds) have been added to the datastore managed by this manager.

    \sa landmarksChanged(), landmarksRemoved()
*/

/*!
    \fn void QLandmarkManager::landmarksChanged(const QList<QLandmarkId> &landmarkIds)

    This signal is emitted when landmarks (identified by \a landmarkIds) have been modified in the datastore managed by this manager.

    \sa landmarksAdded(), landmarksRemoved()
*/

/*!
    \fn void QLandmarkManager::landmarksRemoved(const QList<QLandmarkId> &landmarkIds)

    This signal is emitted when landmarks (identified by \a landmarkIds) have been removed from the datastore managed by this manager.

    \sa landmarksAdded(), landmarksChanged()
*/

/*!
    \fn void QLandmarkManager::categoriesAdded(const QList<QLandmarkCategoryId> &categoryIds)

    This signal is emitted when categories (identified by \a categoryIds) have been added to the datastore managed by this manager.

    \sa categoriesChanged(), categoriesRemoved()
*/

/*!
    \fn void QLandmarkManager::categoriesChanged(const QList<QLandmarkCategoryId> &categoryIds)

    This signal is emitted when categories (identified by \a categoryIds) have been modified in the datastore managed by this manager.

    \sa categoriesAdded(), categoriesRemoved()
*/

/*!
    \fn void QLandmarkManager::categoriesRemoved(const QList<QLandmarkCategoryId> &categoryIds)

    This signal is emitted when categories (identified by \a categoryIds) have been removed from the datastore managed by this manager.

    \sa categoriesAdded(), categoriesChanged()
*/

#include "moc_qlandmarkmanager.cpp"
