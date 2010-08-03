#include "qdeclarativelandmarksource_p.h"
#include <QTimer>

QTM_BEGIN_NAMESPACE

QDeclarativeLandmarkSource::QDeclarativeLandmarkSource(QObject *parent) :
    QAbstractListModel(parent), m_manager(0), m_fetchRequest(0), m_active(false)
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
    qDebug() << "Row count requested, returning: " <<  m_landmarks.count();
    return m_landmarks.count();
}

// Returns the stored under the given role for the item referred to by the index.
QVariant QDeclarativeLandmarkSource::data(const QModelIndex &index, int role) const
{
    qDebug() << "Requested index.row(), role is: " << index.row() << role;
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

void QDeclarativeLandmarkSource::update()
{
    if (!m_manager)
        return;
    // Clear any previous updates and request new
    cancelUpdate();
    m_fetchRequest = new QLandmarkFetchRequest(m_manager, this);
    QObject::connect(m_fetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)), this, SLOT(fetchRequestStateChanged(QLandmarkAbstractRequest::State)));

    // TODO Build filters/hints/sort orders for the request

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

void QDeclarativeLandmarkSource::convertLandmarksToDeclarative()
{
    qDebug("convertLandmarksToDeclarative enter");
    foreach (const QLandmark& landmark, m_landmarks) {
        if (!m_landmarkMap.contains(landmark.landmarkId().localId())) {
            qDebug() << "convertLandmarksToDeclarative entering new: " << landmark.name();
            QDeclarativeLandmark* declarativeLandmark = new QDeclarativeLandmark(this);
            declarativeLandmark->setLandmark(landmark);
            m_landmarkMap.insert(landmark.landmarkId().localId(), declarativeLandmark);
        } else {
            // The landmark exists already, update it
            qDebug() << "convertLandmarksToDeclarative updating existing: " << landmark.name();
            m_landmarkMap.value(landmark.landmarkId().localId())->setLandmark(landmark);
        }
    }
    qDebug("convertLandmarksToDeclarative exit");
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
        qDebug("got no error from fetch request, updating landmarks in the model");
        beginInsertRows(QModelIndex(), 0, m_landmarks.count());
        m_landmarks = m_fetchRequest->landmarks();
        endInsertRows();
    } else if (m_error != m_fetchRequest->errorString()) {
        qDebug("got error from fetch request, updating error");
        m_error = m_fetchRequest->errorString();
        emit errorChanged(m_error);
    }
    // Convert into declarative classes so that it is possible to return
    // landmarks in a list.
    convertLandmarksToDeclarative();
}

#include "moc_qdeclarativelandmarksource_p.cpp"

QTM_END_NAMESPACE
