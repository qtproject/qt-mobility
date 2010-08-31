#include "qdeclarativelandmark_p.h"

QTM_BEGIN_NAMESPACE

QDeclarativeLandmark::QDeclarativeLandmark(QObject* parent) :
        QObject(parent)
{
}

QString QDeclarativeLandmark::name()
{
    return m_landmark.name();
}

void QDeclarativeLandmark::setName(const QString& name)
{
    if (name == m_landmark.name())
        return;
    m_landmark.setName(name);
    emit nameChanged();
}

QString QDeclarativeLandmark::phoneNumber()
{
    return m_landmark.phoneNumber();
}

void QDeclarativeLandmark::setPhoneNumber(const QString& phoneNumber)
{
    if (phoneNumber == m_landmark.phoneNumber())
        return;
    m_landmark.setPhoneNumber(phoneNumber);
    emit phoneNumberChanged();
}

QString QDeclarativeLandmark::description()
{
    return m_landmark.description();
}

void QDeclarativeLandmark::setDescription(const QString& description)
{
    if (description == m_landmark.description())
        return;
    m_landmark.setDescription(description);
    emit descriptionChanged();
}

double QDeclarativeLandmark::radius()
{
    return m_landmark.radius();
}

void QDeclarativeLandmark::setRadius(const double& radius)
{
    if (radius == m_landmark.radius())
        return;
    m_landmark.setRadius(radius);
    emit radiusChanged();
}

QUrl QDeclarativeLandmark::iconSource()
{
    return m_landmark.iconUrl();
}

void QDeclarativeLandmark::setIconSource(const QUrl& iconSource)
{
    if (iconSource == m_landmark.iconUrl())
        return;
    m_landmark.setIconUrl(iconSource);
    emit iconSourceChanged();
}

QUrl QDeclarativeLandmark::url()
{
    return m_landmark.url();
}

void QDeclarativeLandmark::setUrl(const QUrl& url)
{
    if (url == m_landmark.url())
        return;
    m_landmark.setUrl(url);
    emit urlChanged();
}

// Initializes this landmark from the given landmark
void QDeclarativeLandmark::setLandmark(const QLandmark& landmark)
{
    m_landmark = landmark;
    // TODO update notifications when the whole scenario is more clear
}

QList<QLandmarkCategoryId> QDeclarativeLandmark::categoryIds () const
{
    return m_landmark.categoryIds();
}

#include "moc_qdeclarativelandmark_p.cpp"

QTM_END_NAMESPACE
