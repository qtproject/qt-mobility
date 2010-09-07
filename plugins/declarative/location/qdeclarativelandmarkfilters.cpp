#include "qdeclarativelandmarkfilters_p.h"
#include "qdeclarativeposition_p.h"

QTM_BEGIN_NAMESPACE

QDeclarativeLandmarkNameFilter::QDeclarativeLandmarkNameFilter(QObject *parent) :
        QDeclarativeLandmarkFilterBase(parent)
{
}

QDeclarativeLandmarkNameFilter::~QDeclarativeLandmarkNameFilter()
{
}

QString QDeclarativeLandmarkNameFilter::name() const
{
    return m_name;
}

void QDeclarativeLandmarkNameFilter::setName(const QString& name)
{
    if (name == m_name)
        return;
    m_name = name;
    m_filter.setName(m_name);
    emit nameChanged();
}

QLandmarkFilter* QDeclarativeLandmarkNameFilter::filter()
{
    if (m_name.isEmpty())
        return 0;
    return &m_filter;
}

QDeclarativeLandmarkProximityFilter::QDeclarativeLandmarkProximityFilter(QObject *parent) :
        QDeclarativeLandmarkFilterBase(parent), m_radius(50), m_coordinate(0)
{
}

QDeclarativeLandmarkProximityFilter::~QDeclarativeLandmarkProximityFilter()
{
}

double QDeclarativeLandmarkProximityFilter::radius() const
{
    return m_radius;
}

void QDeclarativeLandmarkProximityFilter::setRadius(const double radius)
{
    if (radius == m_radius)
        return;
    m_radius = radius;
    emit radiusChanged();
}

QDeclarativeCoordinate* QDeclarativeLandmarkProximityFilter::coordinate() const
{
    return m_coordinate;
}

void QDeclarativeLandmarkProximityFilter::setCoordinate(QDeclarativeCoordinate* coordinate)
{
    m_coordinate = coordinate;
    m_filter.setCoordinate(m_coordinate->coordinate());
    emit coordinateChanged();
}

QLandmarkFilter* QDeclarativeLandmarkProximityFilter::filter()
{
    if (!m_coordinate)
        return 0;
    m_filter.setRadius(m_radius);
    return &m_filter;
}

QDeclarativeLandmarkUnionFilter::QDeclarativeLandmarkUnionFilter(QObject* parent)
        : QDeclarativeLandmarkFilterBase(parent)
{
}

QDeclarativeListProperty<QDeclarativeLandmarkFilterBase> QDeclarativeLandmarkUnionFilter::filters()
{
    return QDeclarativeListProperty<QDeclarativeLandmarkFilterBase>(this, m_filters);
}

QLandmarkFilter* QDeclarativeLandmarkUnionFilter::filter()
{
    if (m_filters.isEmpty())
        return 0;

    // Creates a Union filter of all filters.
    // This could be optimized such that the filters will be rebuilt when something
    // in filters really change, as opposed to rebuilding each time retrieved
    m_filter.clear();
    for (int i = 0; i < m_filters.count(); i++) {
        m_filter.append(*m_filters.at(i)->filter());
    }
    return &m_filter;
}

QDeclarativeLandmarkIntersectionFilter::QDeclarativeLandmarkIntersectionFilter(QObject* parent)
        : QDeclarativeLandmarkFilterBase(parent)
{
}

QDeclarativeListProperty<QDeclarativeLandmarkFilterBase> QDeclarativeLandmarkIntersectionFilter::filters()
{
    return QDeclarativeListProperty<QDeclarativeLandmarkFilterBase>(this, m_filters);
}

QLandmarkFilter* QDeclarativeLandmarkIntersectionFilter::filter()
{
    if (m_filters.isEmpty())
        return 0;

    // Creates a Intersection filter of all filters.
    // This could be optimized such that the filters will be rebuilt when something
    // in filters really change, as opposed to rebuilding each time retrieved
    m_filter.clear();
    for (int i = 0; i < m_filters.count(); i++) {
        m_filter.append(*m_filters.at(i)->filter());
    }
    return &m_filter;
}

#include "moc_qdeclarativelandmarkfilters_p.cpp"

QTM_END_NAMESPACE
