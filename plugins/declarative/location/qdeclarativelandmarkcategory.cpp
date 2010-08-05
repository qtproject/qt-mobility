#include "qdeclarativelandmarkcategory_p.h"

QTM_BEGIN_NAMESPACE

QDeclarativeLandmarkCategory::QDeclarativeLandmarkCategory(QObject* parent) :
    QObject(parent)
{
}

QString QDeclarativeLandmarkCategory::name()
{
    return m_category.name();
}

void QDeclarativeLandmarkCategory::setName(const QString& name)
{
    if (name == m_category.name())
        return;
    m_category.setName(name);
    emit nameChanged(name);
}

QUrl QDeclarativeLandmarkCategory::iconUrl()
{
    return m_category.iconUrl();
}

void QDeclarativeLandmarkCategory::setIconUrl(const QUrl& iconUrl)
{
    if (iconUrl == m_category.iconUrl())
        return;
    m_category.setIconUrl(iconUrl);
    emit iconUrlChanged(iconUrl);
}

// Initializes this category from the given category
void QDeclarativeLandmarkCategory::setCategory(const QLandmarkCategory& category)
{
    m_category = category;
    // TODO update notifications when the whole scenario is more clear
}

#include "moc_qdeclarativelandmarkcategory_p.cpp"

QTM_END_NAMESPACE
