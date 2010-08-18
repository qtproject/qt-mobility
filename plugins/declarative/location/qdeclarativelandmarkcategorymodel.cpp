#include "qdeclarativelandmarkcategorymodel_p.h"
#include <QTimer>

#ifdef QDECLARATIVE_LANDMARK_DEBUG
#include <QDebug>
#endif

QTM_BEGIN_NAMESPACE

QDeclarativeLandmarkCategoryModel::QDeclarativeLandmarkCategoryModel(QObject *parent) :
        QAbstractListModel(parent), m_manager(0), m_fetchRequest(0), m_autoUpdate(false)
{
    // Establish role names so that they can be queried from this model
    QHash<int, QByteArray> roleNames;
    roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(NameRole, "name");
    roleNames.insert(DescriptionRole, "description");
    roleNames.insert(IconSourceRole, "iconSource");
    setRoleNames(roleNames);
    // Instantiate default manager
    m_manager = new QLandmarkManager();
}

QDeclarativeLandmarkCategoryModel::~QDeclarativeLandmarkCategoryModel()
{
    delete m_manager;
    delete m_fetchRequest;
}

// When the parent is valid it means that rowCount is returning the number of children of parent.
int QDeclarativeLandmarkCategoryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_categories.count();
}

// Returns the stored under the given role for the item referred to by the index.
QVariant QDeclarativeLandmarkCategoryModel::data(const QModelIndex &index, int role) const
{
    QLandmarkCategory category = m_categories.value(index.row());

    switch (role) {
        case NameRole:
            return category.name();
        case IconSourceRole:
            return category.iconUrl();
    }
    return QVariant();
}

int QDeclarativeLandmarkCategoryModel::count()
{
    return m_categories.count();
}

QString QDeclarativeLandmarkCategoryModel::error()
{
    return m_error;
}

void QDeclarativeLandmarkCategoryModel::setAutoUpdate(bool autoUpdate)
{
    if (autoUpdate == m_autoUpdate)
        return;
    if (m_autoUpdate)
        QTimer::singleShot(0, this, SLOT(update())); // delay ensures all properties have been set
    else
        cancelUpdate();
}

bool QDeclarativeLandmarkCategoryModel::autoUpdate() const
{
    return m_autoUpdate;
}

void QDeclarativeLandmarkCategoryModel::update()
{
#ifdef QDECLARATIVE_LANDMARK_DEBUG
    qDebug("QDeclarativeLandmarkCategoryModel::update()");
#endif
    if (!m_manager)
        return;
    // Clear any previous updates and request new
    cancelUpdate();
    m_fetchRequest = new QLandmarkCategoryFetchRequest(m_manager, this);
    QObject::connect(m_fetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)), this, SLOT(fetchRequestStateChanged(QLandmarkAbstractRequest::State)));
    m_fetchRequest->start();
}

void QDeclarativeLandmarkCategoryModel::cancelUpdate()
{
    if (m_fetchRequest) {
        delete m_fetchRequest;
        m_fetchRequest = 0;
    }
}

void QDeclarativeLandmarkCategoryModel::convertCategoriesToDeclarative()
{
    foreach(const QLandmarkCategory& category, m_categories) {
        if (!m_categoryMap.contains(category.categoryId().localId())) {
            QDeclarativeLandmarkCategory* declarativeLandmarkCategory = new QDeclarativeLandmarkCategory(this);
            declarativeLandmarkCategory->setCategory(category);
            m_categoryMap.insert(category.categoryId().localId(), declarativeLandmarkCategory);
        } else {
            // The landmark exists already, update it
            m_categoryMap.value(category.categoryId().localId())->setCategory(category);
        }
    }
}

void QDeclarativeLandmarkCategoryModel::fetchRequestStateChanged(QLandmarkAbstractRequest::State state)
{
#ifdef QDECLARATIVE_LANDMARK_DEBUG
    qDebug() << "QDeclarativeLandmarkCategoryModel::Fetch request finished with state: " << state;
    if (m_fetchRequest)
        qDebug() << "and related request error code is: " << m_fetchRequest->errorString();
#endif
    if (!m_fetchRequest || state != QLandmarkAbstractRequest::FinishedState)
        return;

    if (m_fetchRequest->error() == QLandmarkManager::NoError) {
        // TODO Later improvement item is to make udpate incremental by connecting to resultsAvailable() -function.
        beginInsertRows(QModelIndex(), 0, m_categories.count());
        int oldCount = m_categories.count();
        m_categories = m_fetchRequest->categories();
        endInsertRows();
        if (oldCount != m_categories.count())
            emit countChanged(m_categories.count());
    } else if (m_error != m_fetchRequest->errorString()) {
        m_error = m_fetchRequest->errorString();
        emit errorChanged(m_error);
    }
    // Convert into declarative classes --> possible to return categories in a list in QML
    convertCategoriesToDeclarative();
}

#include "moc_qdeclarativelandmarkcategorymodel_p.cpp"

QTM_END_NAMESPACE
