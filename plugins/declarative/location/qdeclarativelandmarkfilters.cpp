#include "qdeclarativelandmarkfilters_p.h"

QTM_BEGIN_NAMESPACE

QDeclarativeLandmarkNameFilter::QDeclarativeLandmarkNameFilter(QObject *parent) :
    QObject(parent)
{
}

QString QDeclarativeLandmarkNameFilter::name()
{
    return m_filter.name();
}

void QDeclarativeLandmarkNameFilter::setName(const QString& name)
{
    if (name == m_filter.name())
        return;
    m_filter.setName(name);
    emit nameChanged(name);
    emit filterChanged();
}

QLandmarkNameFilter* QDeclarativeLandmarkNameFilter::filter()
{
    return &m_filter;
}

QDeclarativeLandmarkProximityFilter::QDeclarativeLandmarkProximityFilter(QObject *parent) :
    QObject(parent)
{
}

double QDeclarativeLandmarkProximityFilter::latitude() const
{
    return m_filter.coordinate().latitude();
}

void QDeclarativeLandmarkProximityFilter::setLatitude(double latitude)
{
    if (m_filter.coordinate().latitude() == latitude)
        return;
    m_coordinate.setLatitude(latitude);
    m_filter.setCoordinate(m_coordinate);
    emit latitudeChanged(latitude);
    emit filterChanged();
}

double QDeclarativeLandmarkProximityFilter::longitude() const
{
    return m_filter.coordinate().longitude();
}

void QDeclarativeLandmarkProximityFilter::setlongitude(double longitude)
{
    if (longitude == m_filter.coordinate().longitude())
        return;
    m_coordinate.setLongitude(longitude);
    m_filter.setCoordinate(m_coordinate);
    emit longitudeChanged(longitude);
    emit filterChanged();
}

double QDeclarativeLandmarkProximityFilter::radius() const
{
    return m_filter.radius();
}
void QDeclarativeLandmarkProximityFilter::setRadius(double radius)
{
    if (radius == m_filter.radius())
        return;
    m_filter.setRadius(radius);
    emit radiusChanged(radius);
    emit filterChanged();
}

QLandmarkProximityFilter* QDeclarativeLandmarkProximityFilter::filter()
{
    return &m_filter;
}

#include "moc_qdeclarativelandmarkfilters_p.cpp"

QTM_END_NAMESPACE
