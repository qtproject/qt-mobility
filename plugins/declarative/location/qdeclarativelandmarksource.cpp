#include "qdeclarativelandmarksource_p.h"
#include <QTimer>

#ifdef QDECLARATIVE_LANDMARK_DEBUG
#include <QDebug>
#endif

QTM_BEGIN_NAMESPACE

QDeclarativeLandmarkSource::QDeclarativeLandmarkSource(QObject *parent) :
    QAbstractListModel(parent), m_manager(0), m_nameFilter(0), m_proximityFilter(0),
    m_fetchRequest(0), m_active(false), m_maxItems(-1)
{
    // Establish role names so that they can be queried from this model
    QHash<int, QByteArray> roleNames;
    roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(NameRole, "name");
    roleNames.insert(PhoneRole, "phone");
    roleNames.insert(DescriptionRole, "description");
    roleNames.insert(RadiusRole, "radius");
    roleNames.insert(IconUrlRole, "iconUrl");
    roleNames.insert(UrlRole, "url");
    roleNames.insert(LatitudeRole, "latitude");
    roleNames.insert(LongitudeRole, "longitude");
    setRoleNames(roleNames);
    // Instantiate default manager
    m_manager = new QLandmarkManager();
}

QDeclarativeLandmarkSource::~QDeclarativeLandmarkSource()
{
    delete m_manager; 
    delete m_fetchRequest;
}

// When the parent is valid it means that rowCount is returning the number of children of parent.
int QDeclarativeLandmarkSource::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_landmarks.count();
}

// Returns the stored under the given role for the item referred to by the index.
QVariant QDeclarativeLandmarkSource::data(const QModelIndex &index, int role) const
{
    QLandmark landmark = m_landmarks.value(index.row());
    switch (role) {
    //case Qt::DisplayRole:
    //    return landmark.name();
    case NameRole:
        return landmark.name();
    case PhoneRole:
        return landmark.phone();
    case DescriptionRole:
        return landmark.description();
    case RadiusRole:
        return landmark.radius();
    case IconUrlRole:
        return landmark.iconUrl();
    case UrlRole:
        return landmark.url();
    case LatitudeRole:
        return landmark.coordinate().latitude();
    case LongitudeRole:
        return landmark.coordinate().longitude();
    }
    return QVariant();
}

QString QDeclarativeLandmarkSource::name()
{
    if (m_manager)
        return m_manager->managerName() ;
    return QString();
}

QString QDeclarativeLandmarkSource::error()
{
    return m_error;
}

void QDeclarativeLandmarkSource::setActive(bool active)
{
    if (active == m_active)
        return;
    if (active)
        QTimer::singleShot(0, this, SLOT(update())); // delay ensures all properties have been set
    else
        cancelUpdate();
}

bool QDeclarativeLandmarkSource::isActive() const
{
    return m_active;
}

QObject* QDeclarativeLandmarkSource::nameFilter()
{
    return m_nameFilter;
}

void QDeclarativeLandmarkSource::setFilter(QObject* filter)
{
    if (qobject_cast<QDeclarativeLandmarkNameFilter*>(filter)) {
        m_nameFilter = qobject_cast<QDeclarativeLandmarkNameFilter*>(filter);
        QObject::connect(m_nameFilter, SIGNAL(filterChanged()), this, SLOT(update()));
    } else if (qobject_cast<QDeclarativeLandmarkProximityFilter*>(filter)) {
        m_proximityFilter = qobject_cast<QDeclarativeLandmarkProximityFilter*>(filter);
        QObject::connect(m_proximityFilter, SIGNAL(filterChanged()), this, SLOT(update()));
    }
}

QObject* QDeclarativeLandmarkSource::proximityFilter()
{
    return m_proximityFilter;
}

void QDeclarativeLandmarkSource::update()
{
#ifdef QDECLARATIVE_LANDMARK_DEBUG
    qDebug("QDeclarativeLandmarkSource::update()");
#endif
    if (!m_manager)
        return;
    // Clear any previous updates and request new
    cancelUpdate();
    m_fetchRequest = new QLandmarkFetchRequest(m_manager, this);
    QObject::connect(m_fetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)), this, SLOT(fetchRequestStateChanged(QLandmarkAbstractRequest::State)));
    // Set filters and hints. Fetch orders are not currently supported.
    if (m_nameFilter)
        m_fetchRequest->setFilter(*m_nameFilter->filter());
    if (m_proximityFilter)
        m_fetchRequest->setFilter(*m_proximityFilter->filter());
    setFetchHints();
    m_fetchRequest->start();
    if (!m_active) {
        m_active = true;
        emit activeChanged(m_active);
    }
}

void QDeclarativeLandmarkSource::cancelUpdate()
{
    if (m_fetchRequest) {
        delete m_fetchRequest;
        m_fetchRequest = 0;
    }
}

int QDeclarativeLandmarkSource::maxItems()
{
    return m_maxItems;
}

void QDeclarativeLandmarkSource::setMaxItems(int maxItems)
{
    if (maxItems == m_maxItems)
        return;
    m_maxItems = maxItems;
    emit maxItemsChanged(maxItems);
}

void QDeclarativeLandmarkSource::setFetchHints()
{
//TODO: replace fetch hint with limit and offset

/*    if (!m_fetchRequest || m_maxItems <= 0)
        return;
    m_fetchHint.setMaxItems(m_maxItems);
    m_fetchRequest->setFetchHint(m_fetchHint);
*/
}

void QDeclarativeLandmarkSource::convertLandmarksToDeclarative()
{
    foreach (const QLandmark& landmark, m_landmarks) {
        if (!m_landmarkMap.contains(landmark.landmarkId().localId())) {
            QDeclarativeLandmark* declarativeLandmark = new QDeclarativeLandmark(this);
            declarativeLandmark->setLandmark(landmark);
            m_landmarkMap.insert(landmark.landmarkId().localId(), declarativeLandmark);
        } else {
            // The landmark exists already, update it
            m_landmarkMap.value(landmark.landmarkId().localId())->setLandmark(landmark);
        }
    }
}

void QDeclarativeLandmarkSource::fetchRequestStateChanged(QLandmarkAbstractRequest::State state)
{
#ifdef QDECLARATIVE_LANDMARK_DEBUG
    qDebug() << "QDeclarativeLandmarkSource::Fetch request finished with state: " << state;
    if (m_fetchRequest)
        qDebug() << "and related request error code is: " << m_fetchRequest->errorString();
#endif
    if (!m_fetchRequest || state != QLandmarkAbstractRequest::FinishedState)
        return;

    if (m_fetchRequest->error() == QLandmarkManager::NoError) {
        // TODO Later improvement item is to make udpate incremental by connecting to resultsAvailable() -function.
        beginInsertRows(QModelIndex(), 0, m_landmarks.count());
        m_landmarks = m_fetchRequest->landmarks();
        endInsertRows();
    } else if (m_error != m_fetchRequest->errorString()) {
        m_error = m_fetchRequest->errorString();
        emit errorChanged(m_error);
    }
    // Convert into declarative classes --> possible to return landmarks in a list in QML
    convertLandmarksToDeclarative();
}

#include "moc_qdeclarativelandmarksource_p.cpp"

QTM_END_NAMESPACE
