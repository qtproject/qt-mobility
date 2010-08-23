#include "qdeclarativelandmarkfilters_p.h"
#include "qdeclarativeposition_p.h"

QTM_BEGIN_NAMESPACE

QDeclarativeLandmarkFilter::QDeclarativeLandmarkFilter(QObject *parent) :
        QDeclarativeLandmarkFilterBase(parent), m_type(Name), m_filter(0)
{
}

QDeclarativeLandmarkFilter::~QDeclarativeLandmarkFilter()
{
    delete m_filter;
}

QDeclarativeLandmarkFilter::FilterType QDeclarativeLandmarkFilter::type() const
{
    return m_type;
}

void QDeclarativeLandmarkFilter::setType(QDeclarativeLandmarkFilter::FilterType type)
{
    if (type == m_type)
        return;
    m_type = type;
    if (m_filter) {
        delete m_filter;
        m_filter = 0;
    }
    switch (m_type)
    {
    case Name:
        m_filter = new QLandmarkNameFilter();
        break;
    case Proximity:
        m_filter = new QLandmarkProximityFilter();
        break;
    default:
        return;
    }
    emit typeChanged();
}

QVariant QDeclarativeLandmarkFilter::value() const
{
    return m_value;
}

void QDeclarativeLandmarkFilter::setValue(const QVariant& value)
{
    if (value == m_value)
        return;
    m_value = value;
    emit valueChanged();
}

QLandmarkFilter* QDeclarativeLandmarkFilter::filter()
{
    qDebug() << "QDeclarativeLandmarkFilter filter() enter";
    if (!m_filter || !m_value.isValid())
        return 0;
    // Set value for filter here so we are not dependant of in
    // which order the 'type' and 'value' were set.
    switch (m_filter->type())
    {
    case QLandmarkFilter::NameFilter:
        {
        QLandmarkNameFilter* filter = static_cast<QLandmarkNameFilter*>(m_filter);
        filter->setName(m_value.toString());
        }
        break;
    case QLandmarkFilter::ProximityFilter:
        {
        QLandmarkProximityFilter* filter = static_cast<QLandmarkProximityFilter*>(m_filter);
        QDeclarativePosition* position = qobject_cast<QDeclarativePosition*>(m_value.value<QObject*>());
        qDebug() << "QDeclarativeLandmarkFilter filter(), value for proximity: " << m_value.value<QObject*>();
        qDebug() << "QDeclarativeLandmarkFilter filter() lat lon rad: " << position->latitude() << position->longitude() << position->radius();
        filter->setCoordinate(QGeoCoordinate(position->latitude(), position->longitude()));
        filter->setRadius(position->radius());
        }
        break;
    default:
        // Other filters are not currently supported
        return 0;
    }
    qDebug() << "QDeclarativeLandmarkFilter filter() exit. Was there a match?";
    return m_filter;
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
    qDebug() << "QDeclarativeLandmarkUnionFilter:: filter() number of filters is: " << m_filters.count();
    if (m_filters.isEmpty()) {
        qDebug("QDeclarativeLandmarkUnionFilter::filter() returning zero because no filters. Will this crash?");
        return 0;
    }
    // Creates a Union filter of all filters.
    // This could be optimized such that the filters will be rebuilt when something
    // in filters really change, as opposed to rebuilding each time retrieved
    m_filter.clear();
    for (int i = 0; i < m_filters.count(); i++) {
        m_filter.append(*m_filters.at(i)->filter());
    }
    qDebug("QDeclarativeLandmarkUnionFilter::filter() exit.");
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
    qDebug() << "QDeclarativeLandmarkIntersectionFilter:: filter() number of filters is: " << m_filters.count();
    if (m_filters.isEmpty()) {
        qDebug("QDeclarativeLandmarkIntersectionFilter::filter() returning zero because no filters. Will this crash?");
        return 0;
    }
    // Creates a Intersection filter of all filters.
    // This could be optimized such that the filters will be rebuilt when something
    // in filters really change, as opposed to rebuilding each time retrieved
    m_filter.clear();
    for (int i = 0; i < m_filters.count(); i++) {
        m_filter.append(*m_filters.at(i)->filter());
    }
    qDebug("QDeclarativeLandmarkIntersectionFilter::filter() exit.");
    return &m_filter;
}

#include "moc_qdeclarativelandmarkfilters_p.cpp"

QTM_END_NAMESPACE
