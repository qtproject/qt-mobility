#include "qdeclarativelandmarkmanager_p.h"

QTM_BEGIN_NAMESPACE

QDeclarativeLandmarkManager::QDeclarativeLandmarkManager(QObject *parent) :
    QAbstractListModel(parent)
{
    m_manager = new QLandmarkManager(); // Default manager
}

QDeclarativeLandmarkManager::~QDeclarativeLandmarkManager()
{
    delete m_manager;
}

// When the parent is valid it means that rowCount is returning the number of children of parent.
int QDeclarativeLandmarkManager::rowCount(const QModelIndex &parent) const
{
    return -1;
}

// Returns the data stored under the given role for the item referred to by the index.
QVariant QDeclarativeLandmarkManager::data(const QModelIndex &index, int role) const
{
    return QVariant();
}

QString QDeclarativeLandmarkManager::name()
{
    if (m_manager)
        return m_manager->managerName() ;
}


#include "moc_qdeclarativelandmarkmanager_p.cpp"

QTM_END_NAMESPACE

