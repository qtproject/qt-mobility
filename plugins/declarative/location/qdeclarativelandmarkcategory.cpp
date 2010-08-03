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

QString QDeclarativeLandmarkCategory::description()
{
    return m_category.description();
}

void QDeclarativeLandmarkCategory::setDescription(const QString& description)
{
    if (description == m_category.description())
        return;
    m_category.setDescription(description);
    emit descriptionChanged(description);
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

#include "moc_qdeclarativelandmarkcategory_p.cpp"

QTM_END_NAMESPACE
