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
    emit filterContentChanged();
}

QLandmarkFilter* QDeclarativeLandmarkNameFilter::filter()
{
    if (m_name.isEmpty())
        return 0;
    return &m_filter;
}

QDeclarativeLandmarkCategoryFilter::QDeclarativeLandmarkCategoryFilter(QObject* parent) :
        QDeclarativeLandmarkFilterBase(parent)
{
}

QDeclarativeLandmarkCategoryFilter::~QDeclarativeLandmarkCategoryFilter()
{
}

QDeclarativeLandmarkCategory* QDeclarativeLandmarkCategoryFilter::category() const
{
    return m_category;
}

void QDeclarativeLandmarkCategoryFilter::setCategory(QDeclarativeLandmarkCategory* category)
{
    m_category = category;
    m_filter.setCategoryId(m_category->category().categoryId());
    emit categoryChanged();
    emit filterContentChanged();
}

QLandmarkFilter* QDeclarativeLandmarkCategoryFilter::filter()
{
    if (!m_category)
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
    emit filterContentChanged();
}

QDeclarativeCoordinate* QDeclarativeLandmarkProximityFilter::coordinate() const
{
    return m_coordinate;
}

void QDeclarativeLandmarkProximityFilter::setCoordinate(QDeclarativeCoordinate* coordinate)
{
    m_coordinate = coordinate;
    QObject::connect(m_coordinate, SIGNAL(latitudeChanged(double)), this, SIGNAL(filterContentChanged()));
    QObject::connect(m_coordinate, SIGNAL(longitudeChanged(double)), this, SIGNAL(filterContentChanged()));
    emit coordinateChanged();
    emit filterContentChanged();
}

QLandmarkFilter* QDeclarativeLandmarkProximityFilter::filter()
{
    if (!m_coordinate)
        return 0;
    // Populate filter only now in case their contents have changed.
    m_filter.setRadius(m_radius);
    m_filter.setCoordinate(m_coordinate->coordinate());
    return &m_filter;
}


QDeclarativeLandmarkCompoundFilter::QDeclarativeLandmarkCompoundFilter(QObject* parent)
        : QDeclarativeLandmarkFilterBase(parent)
{
}

QDeclarativeListProperty<QDeclarativeLandmarkFilterBase> QDeclarativeLandmarkCompoundFilter::filters()
{
    return QDeclarativeListProperty<QDeclarativeLandmarkFilterBase>(this,
                                                          0, // opaque data parameter
                                                          filters_append,
                                                          filters_count,
                                                          filters_at,
                                                          filters_clear);
}

void QDeclarativeLandmarkCompoundFilter::filters_append(QDeclarativeListProperty<QDeclarativeLandmarkFilterBase>* prop, QDeclarativeLandmarkFilterBase* filter)
{
    QDeclarativeLandmarkCompoundFilter* compoundFilter = static_cast<QDeclarativeLandmarkCompoundFilter*>(prop->object);
    compoundFilter->m_filters.append(filter);
    QObject::connect(filter, SIGNAL(filterContentChanged()), compoundFilter, SIGNAL(filterContentChanged()));
    emit compoundFilter->filterContentChanged();
}

int QDeclarativeLandmarkCompoundFilter::filters_count(QDeclarativeListProperty<QDeclarativeLandmarkFilterBase>* prop)
{
    // The 'prop' is in a sense 'this' for this static function (as given in filters() function)
    return static_cast<QDeclarativeLandmarkCompoundFilter*>(prop->object)->m_filters.count();
}

QDeclarativeLandmarkFilterBase* QDeclarativeLandmarkCompoundFilter::filters_at(QDeclarativeListProperty<QDeclarativeLandmarkFilterBase>* prop, int index)
{
    return static_cast<QDeclarativeLandmarkCompoundFilter*>(prop->object)->m_filters.at(index);
}

void QDeclarativeLandmarkCompoundFilter::filters_clear(QDeclarativeListProperty<QDeclarativeLandmarkFilterBase>* prop)
{
    QDeclarativeLandmarkCompoundFilter* filter = static_cast<QDeclarativeLandmarkCompoundFilter*>(prop->object);
    qDeleteAll(filter->m_filters);
    filter->m_filters.clear();
}

template <class T>
        bool QDeclarativeLandmarkCompoundFilter::appendFilters(T* compoundFilter)
{
    // Creates a T type compound filter of all filters.
    if (m_filters.isEmpty())
        return false;
    compoundFilter->clear();
    for (int i = 0; i < m_filters.count(); i++) {
        compoundFilter->append(*m_filters.at(i)->filter());
    }
    return true;
}

QLandmarkFilter* QDeclarativeLandmarkUnionFilter::filter()
{
    return appendFilters<QLandmarkUnionFilter>(&m_filter) ? &m_filter : 0;
}

QDeclarativeLandmarkUnionFilter::QDeclarativeLandmarkUnionFilter(QObject* parent)
        : QDeclarativeLandmarkCompoundFilter(parent)
{
}

QDeclarativeLandmarkIntersectionFilter::QDeclarativeLandmarkIntersectionFilter(QObject* parent)
        : QDeclarativeLandmarkCompoundFilter(parent)
{
}

QLandmarkFilter* QDeclarativeLandmarkIntersectionFilter::filter()
{
    return appendFilters<QLandmarkIntersectionFilter>(&m_filter) ? &m_filter : 0;
}

#include "moc_qdeclarativelandmarkfilters_p.cpp"

QTM_END_NAMESPACE
