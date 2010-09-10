#include "qdeclarativelandmarkmodel_p.h"
#include <QTimer>

#ifdef QDECLARATIVE_LANDMARK_DEBUG
#include <QDebug>
#endif

QTM_BEGIN_NAMESPACE

QDeclarativeLandmarkAbstractModel::QDeclarativeLandmarkAbstractModel(QObject *parent) :
        QAbstractListModel(parent), m_manager(0),
        m_componentCompleted(false), m_updatePending(false), m_autoUpdate(true),
        m_limit(-1), m_offset(-1), m_sortingOrder(0), m_sortOrder(NoOrder), m_sortKey(NoSort)
{
}

QDeclarativeLandmarkAbstractModel::~QDeclarativeLandmarkAbstractModel()
{
    delete m_manager;
}

void QDeclarativeLandmarkAbstractModel::componentComplete()
{
    m_componentCompleted = true;
    if (!m_manager) {
        m_manager = new QLandmarkManager();
        connectManager();
    }
    if (m_autoUpdate) {
        scheduleUpdate();
    }
}

void QDeclarativeLandmarkAbstractModel::connectManager()
{
    if (!m_manager)
        return;
    connect(m_manager, SIGNAL(categoriesAdded(QList<QLandmarkCategoryId>)),
            this, SLOT(categoriesChanged(QList<QLandmarkCategoryId>)));
    connect(m_manager, SIGNAL(categoriesChanged(QList<QLandmarkCategoryId>)),
            this, SLOT(categoriesChanged(QList<QLandmarkCategoryId>)));
    connect(m_manager, SIGNAL(categoriesRemoved(QList<QLandmarkCategoryId>)),
            this, SLOT(categoriesChanged(QList<QLandmarkCategoryId>)));
    connect(m_manager, SIGNAL(landmarksAdded(QList<QLandmarkId>)),
            this, SLOT(landmarksChanged(QList<QLandmarkId>)));
    connect(m_manager, SIGNAL(landmarksChanged(QList<QLandmarkId>)),
            this, SLOT(landmarksChanged(QList<QLandmarkId>)));
    connect(m_manager, SIGNAL(landmarksRemoved(QList<QLandmarkId>)),
            this, SLOT(landmarksChanged(QList<QLandmarkId>)));
    connect(m_manager, SIGNAL(dataChanged()),
            this, SLOT(dataChanged()));
}

void QDeclarativeLandmarkAbstractModel::categoriesChanged(const QList<QLandmarkCategoryId>& ids)
{
    Q_UNUSED(ids)
    if (m_autoUpdate)
        update();
    emit databaseChanged();
}

void QDeclarativeLandmarkAbstractModel::landmarksChanged(const QList<QLandmarkId>& ids)
{
    Q_UNUSED(ids)
    if (m_autoUpdate)
        update();
    emit databaseChanged();
}

void QDeclarativeLandmarkAbstractModel::dataChanged()
{
    if (m_autoUpdate)
        update();
    emit databaseChanged();
}

void QDeclarativeLandmarkAbstractModel::setAutoUpdate(bool autoUpdate)
{
    if (autoUpdate == m_autoUpdate)
        return;
    m_autoUpdate = autoUpdate;
    emit autoUpdateChanged();
}

bool QDeclarativeLandmarkAbstractModel::autoUpdate() const
{
    return m_autoUpdate;
}

void QDeclarativeLandmarkAbstractModel::scheduleUpdate()
{
    if (!m_componentCompleted || m_updatePending)
        return;
    m_updatePending = true; // Disallow possible duplicate request triggering
    QMetaObject::invokeMethod(this, "startUpdate", Qt::QueuedConnection);
}

QString QDeclarativeLandmarkAbstractModel::error() const
{
    return m_error;
}

// This is purely for testing purposes, not part of public interface
void QDeclarativeLandmarkAbstractModel::setDbFileName(QString fileName)
{
    if (fileName == m_dbFileName)
        return;
    m_dbFileName = fileName;
    if (m_manager) {
        cancelUpdate();
        cancelImport();
        delete m_manager;
        m_manager = 0;
    }
    QMap<QString, QString> map;
    map["filename"] = m_dbFileName;
#ifdef Q_OS_SYMBIAN
    m_manager = new QLandmarkManager("com.nokia.qt.landmarks.engines.symbian", map);
#else
    m_manager = new QLandmarkManager("com.nokia.qt.landmarks.engines.sqlite", map);
#endif
    connectManager();
}

void QDeclarativeLandmarkAbstractModel::update()
{
    scheduleUpdate();
}

int QDeclarativeLandmarkAbstractModel::limit()
{
    return m_limit;
}

void QDeclarativeLandmarkAbstractModel::setLimit(int limit)
{
    if (limit == m_limit)
        return;
    m_limit = limit;
    if (m_autoUpdate) {
        scheduleUpdate();
    }
    emit limitChanged();
}

int QDeclarativeLandmarkAbstractModel::offset()
{
    return m_offset;
}

void QDeclarativeLandmarkAbstractModel::setOffset(int offset)
{
    if (offset == m_offset)
        return;
    m_offset = offset;
    if (m_autoUpdate) {
        scheduleUpdate();
    }
    emit offsetChanged();
}

QDeclarativeLandmarkAbstractModel::SortKey QDeclarativeLandmarkAbstractModel::sortBy() const
{
    return m_sortKey;
}

void QDeclarativeLandmarkAbstractModel::setSortBy(QDeclarativeLandmarkAbstractModel::SortKey key)
{
    if (key == m_sortKey)
        return;
    m_sortKey = key;
    if (m_autoUpdate) {
        scheduleUpdate();
    }
    emit sortByChanged();
}

QDeclarativeLandmarkAbstractModel::SortOrder QDeclarativeLandmarkAbstractModel::sortOrder() const
{
    return m_sortOrder;
}

void QDeclarativeLandmarkAbstractModel::setSortOrder(QDeclarativeLandmarkAbstractModel::SortOrder order)
{
    if (order == m_sortOrder)
        return;
    m_sortOrder = order;
    if (m_autoUpdate) {
        scheduleUpdate();
    }
    emit sortOrderChanged();
}


QDeclarativeLandmarkModel::QDeclarativeLandmarkModel(QObject *parent) :
        QDeclarativeLandmarkAbstractModel(parent),
        m_filter(0), m_fetchRequest(0), m_importRequest(0), m_importPending(false)
{
    // Establish role names so that they can be queried from this model
    QHash<int, QByteArray> roleNames;
    roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(LandmarkRole, "landmark");
    setRoleNames(roleNames);
}

QDeclarativeLandmarkModel::~QDeclarativeLandmarkModel()
{
    delete m_fetchRequest;
    delete m_sortingOrder;
    delete m_importRequest;
    qDeleteAll(m_landmarkMap.values());
    m_landmarkMap.clear();
}

// When the parent is valid it means that rowCount is returning the number of children of parent.
int QDeclarativeLandmarkModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_landmarks.count();
}

// Returns the stored under the given role for the item referred to by the index.
QVariant QDeclarativeLandmarkModel::data(const QModelIndex& index, int role) const
{
    QLandmark landmark = m_landmarks.value(index.row());

    switch (role) {
        case Qt::DisplayRole:
            return landmark.name();
        case LandmarkRole:
            if (m_landmarkMap.value(landmark.landmarkId().localId()))
                return QVariant::fromValue(m_landmarkMap.value(landmark.landmarkId().localId()));
            else
                return QVariant();
        }
    return QVariant();
}

QDeclarativeListProperty<QDeclarativeLandmark> QDeclarativeLandmarkModel::landmarks()
{
    return QDeclarativeListProperty<QDeclarativeLandmark>(this,
                                                          0, // opaque data parameter
                                                          landmarks_append,
                                                          landmarks_count,
                                                          landmarks_at,
                                                          landmarks_clear);
}

QDeclarativeLandmarkFilterBase* QDeclarativeLandmarkModel::filter()
{
    return m_filter;
}

void QDeclarativeLandmarkModel::setFilter(QDeclarativeLandmarkFilterBase* filter)
{
    m_filter = filter;
    // Connect to listen for filters' content changes
    if (filter)
        QObject::connect(filter, SIGNAL(filterContentChanged()), this, SLOT(filterContentChanged()));
    if (m_autoUpdate) {
        scheduleUpdate();
    }
    emit filterChanged();
}

void QDeclarativeLandmarkModel::filterContentChanged()
{
    if (m_autoUpdate) {
#ifdef QDECLARATIVE_LANDMARK_DEBUG
        qDebug() << "QDeclarativeLandmarkModel::filterContentChanged(), scheduling update.";
#endif
    scheduleUpdate();
    }
}

void QDeclarativeLandmarkModel::startUpdate()
{
    if (!m_manager) {
        m_updatePending = false;
        return;
    }
    // Clear any previous updates and request new
    cancelUpdate();
    m_fetchRequest = new QLandmarkFetchRequest(m_manager, this);
    QObject::connect(m_fetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)), this, SLOT(fetchRequestStateChanged(QLandmarkAbstractRequest::State)));
    // Set filters, orders, and limits
    if (m_filter) {
        QLandmarkFilter* filter = m_filter->filter();
        if (filter)
            m_fetchRequest->setFilter(*filter);
    }
    setFetchRange();
    setFetchOrder();
#ifdef QDECLARATIVE_LANDMARK_DEBUG
    qDebug() << "============ Calling start for the fetch request: " << m_fetchRequest << " whose manager is: " << m_manager;
#endif
    m_fetchRequest->start();
    m_updatePending = false; // Allow requesting updates again
}

void QDeclarativeLandmarkModel::cancelUpdate()
{
    if (m_fetchRequest) {
#ifdef QDECLARATIVE_LANDMARK_DEBUG
        qDebug() << "QDeclarativeLandmarkModel::cancelUpdate() deleting m_fetchRequest:" << m_fetchRequest;
#endif
        delete m_fetchRequest;
        m_fetchRequest = 0;
    }
    if (m_sortingOrder) {
        delete m_sortingOrder;
        m_sortingOrder = 0;
    }
}

void QDeclarativeLandmarkModel::importLandmarks()
{
    scheduleImport();
}

void QDeclarativeLandmarkModel::cancelImport()
{
    if (m_importRequest) {
#ifdef QDECLARATIVE_LANDMARK_DEBUG
    qDebug() << "QDeclarativeLandmarkModel::cancelImport() deleting m_importRequest:" << m_importRequest;
#endif
        delete m_importRequest;
        m_importRequest = 0;
    }
}

int QDeclarativeLandmarkModel::count()
{
    return m_landmarks.count();
}

void QDeclarativeLandmarkModel::setFetchRange()
{
    if (!m_fetchRequest || ((m_limit <= 0) && (m_offset <= 0)))
        return;
    if (m_limit > 0)
        m_fetchRequest->setLimit(m_limit);
    if ((m_offset > 0))
        m_fetchRequest->setOffset(m_offset);
}

void QDeclarativeLandmarkModel::setFetchOrder()
{
    if (!m_fetchRequest || ((m_sortKey == NoSort) && (m_sortOrder = NoOrder)))
        return;
    if (m_sortingOrder)
        delete m_sortingOrder;
    if (m_sortKey == NameSort) {
        m_sortingOrder = new QLandmarkNameSort(); // Only supported sort type
    } else {
        m_sortingOrder = new QLandmarkSortOrder();
    }
    if (m_sortOrder != NoOrder)
        m_sortingOrder->setDirection((Qt::SortOrder)m_sortOrder);
    m_fetchRequest->setSorting(*m_sortingOrder);
}

void QDeclarativeLandmarkModel::landmarks_append(QDeclarativeListProperty<QDeclarativeLandmark>* prop, QDeclarativeLandmark* landmark)
{
    Q_UNUSED(prop);
    Q_UNUSED(landmark);
    qWarning() << "LandmarkModel: appending landmarks is not currently supported";
}

int QDeclarativeLandmarkModel::landmarks_count(QDeclarativeListProperty<QDeclarativeLandmark>* prop)
{
    // The 'prop' is in a sense 'this' for this static function (as given in landmarks() function)
    return static_cast<QDeclarativeLandmarkModel*>(prop->object)->m_landmarkMap.values().count();
}

QDeclarativeLandmark* QDeclarativeLandmarkModel::landmarks_at(QDeclarativeListProperty<QDeclarativeLandmark>* prop, int index)
{
    return static_cast<QDeclarativeLandmarkModel*>(prop->object)->m_landmarkMap.values().at(index);
}

void QDeclarativeLandmarkModel::landmarks_clear(QDeclarativeListProperty<QDeclarativeLandmark>* prop)
{
    QDeclarativeLandmarkModel* model = static_cast<QDeclarativeLandmarkModel*>(prop->object);
    QMap<QString, QDeclarativeLandmark*>* landmarkMap = &model->m_landmarkMap;
    qDeleteAll(landmarkMap->values());
    landmarkMap->clear();
    model->m_landmarks.clear();
    emit model->landmarksChanged();
}

void QDeclarativeLandmarkModel::convertLandmarksToDeclarative()
{
    QList<QString> landmarksToRemove = m_landmarkMap.keys();

    foreach(const QLandmark& landmark, m_landmarks) {
        if (!m_landmarkMap.contains(landmark.landmarkId().localId())) {
            QDeclarativeLandmark* declarativeLandmark = new QDeclarativeLandmark(this);
            declarativeLandmark->setLandmark(landmark);
            m_landmarkMap.insert(landmark.landmarkId().localId(), declarativeLandmark);
        } else {
            // The landmark exists already, update it
            m_landmarkMap.value(landmark.landmarkId().localId())->setLandmark(landmark);
            // Item is still valid, remove it from the list of removables
            landmarksToRemove.removeOne(landmark.landmarkId().localId());
        }
    }
    foreach (const QString removable, landmarksToRemove) {
        delete m_landmarkMap.value(removable);
        m_landmarkMap.remove(removable);
    }
    emit landmarksChanged();
}

QString QDeclarativeLandmarkModel::importFile() const
{
    return m_importFile;
}

void QDeclarativeLandmarkModel::setImportFile(QString importFile)
{
    if (importFile == m_importFile)
        return;
    m_importFile = importFile;
    if (m_importRequest) {
        delete m_importRequest;
        m_importRequest = 0;
    }
    if (m_autoUpdate)
        scheduleImport();
    emit importFileChanged();
}

void QDeclarativeLandmarkModel::scheduleImport()
{
    if (!m_componentCompleted || m_importPending)
        return;
    m_importPending = true; // Disallow possible duplicate request triggering
    QMetaObject::invokeMethod(this, "startImport", Qt::QueuedConnection);
}

void QDeclarativeLandmarkModel::componentComplete()
{
    m_componentCompleted = true;
    if (!m_manager) {
        m_manager = new QLandmarkManager();
        connectManager();
    }
    if (m_autoUpdate) {
        if (!m_importFile.isEmpty())
            scheduleImport();
        scheduleUpdate();
    }
}

void QDeclarativeLandmarkModel::startImport()
{
    if (!m_manager || m_importFile.isEmpty()) {
        m_importPending = false;
        return;
    }
    if (m_importRequest)
        delete m_importRequest;
    m_importRequest = new QLandmarkImportRequest(m_manager);
    m_importRequest->setFileName(m_importFile);
#ifdef QDECLARATIVE_LANDMARK_DEBUG
    qDebug() << "============ Calling start for the import request: " << m_importRequest << " whose manager is: " << m_manager << "and file is:" << m_importFile;

#endif
    m_importRequest->start(); // If successful, will result in landmark/category added signals
    m_importPending = false;
}


QList<QLandmark> QDeclarativeLandmarkModel::landmarkList()
{
    return m_landmarks;
}

void QDeclarativeLandmarkModel::fetchRequestStateChanged(QLandmarkAbstractRequest::State state)
{
#ifdef QDECLARATIVE_LANDMARK_DEBUG
    qDebug() << "QDeclarativeLandmarkModel::Fetch request finished with state: " << state;
    if (m_fetchRequest) {
        qDebug() << "and related request error code is: " << m_fetchRequest->errorString();
        qDebug() << "and received this many landmarks:" << m_fetchRequest->landmarks().count();
    }
#endif
    if (!m_fetchRequest || state != QLandmarkAbstractRequest::FinishedState)
        return;

    if (m_fetchRequest->error() == QLandmarkManager::NoError) {
        // Later improvement item is to make udpate incremental by connecting to resultsAvailable() -function.
        beginResetModel();
        int oldCount = m_landmarks.count();
        m_landmarks = m_fetchRequest->landmarks();
        // Convert into declarative classes
        convertLandmarksToDeclarative();
        endResetModel();
        if (!(oldCount == 0 && m_landmarks.count() == 0))
            emit modelChanged();
        if (oldCount != m_landmarks.count())
            emit countChanged();
    } else if (m_error != m_fetchRequest->errorString()) {        
        m_error = m_fetchRequest->errorString();
        emit errorChanged();
    }
}

#include "moc_qdeclarativelandmarkmodel_p.cpp"

QTM_END_NAMESPACE
