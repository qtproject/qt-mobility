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
    emit nameChanged(name);
}

QString QDeclarativeLandmark::phone()
{
    return m_landmark.phoneNumber();
}

void QDeclarativeLandmark::setPhone(const QString& phone)
{
    if (phone == m_landmark.phoneNumber())
        return;
    m_landmark.setPhoneNumber(phone);
    emit phoneChanged(phone);
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
    emit descriptionChanged(description);
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
    emit radiusChanged(radius);
}

QUrl QDeclarativeLandmark::iconUrl()
{
    return m_landmark.iconUrl();
}

void QDeclarativeLandmark::setIconUrl(const QUrl& iconUrl)
{
    if (iconUrl == m_landmark.iconUrl())
        return;
    m_landmark.setIconUrl(iconUrl);
    emit iconUrlChanged(iconUrl);
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
    emit urlChanged(url);
}

// Initializes this landmark from the given landmark
void QDeclarativeLandmark::setLandmark(const QLandmark& landmark)
{
    m_landmark = landmark;
    // TODO update notifications when the whole scenario is more clear
}

#include "moc_qdeclarativelandmark_p.cpp"

QTM_END_NAMESPACE
