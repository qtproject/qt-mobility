#include "qdeclarativelandmarkfilters_p.h"
#include "qdeclarativeposition_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass LandmarkNameFilter QDeclarativeLandmarkNameFilter
    \brief The LandmarkNameFilter element specifies a name filter for landmark model.

    This element is part of the \bold{QtMobility.location 1.1} module.

    Logical combinations of this and other landmark filters can be
    formed using \l LandmarkIntersectionFilter and \l LandmarkUnionFilter.

    The following example creates a filter which filters for landmarks named "Undercity".

    \qml
    LandmarkNameFilter {
        id: landmarkFilterUndercity
        name: "Undercity"
    }
    \endqml

    \sa LandmarkModel

*/

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

/*!
    \qmlproperty QString LandmarkNameFilter::name
    The name string to match landmark names against.
    Currently regular expression (/wildcard) matching is not supported.
*/

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

/*!
    \qmlclass LandmarkCategoryFilter QDeclarativeLandmarkCategoryFilter
    \brief The LandmarkCategoryFilter element specifies a category filter for landmark model.

    This element is part of the \bold{QtMobility.location 1.1} module.

    Logical combinations of this and other landmark filters can be
    formed using \l LandmarkIntersectionFilter and \l LandmarkUnionFilter.

    The following example creates a filter which filters for landmarks which belong to
    categories "Cities" and "Sunshine cities". Notably the categories used must come from
    databases because their internal category identifiers need to be correct (not exposed
    to QML).

    \qml

    TODO

    \endqml

    \sa LandmarkModel LandmarkCategoryModel

*/

QDeclarativeLandmarkCategoryFilter::QDeclarativeLandmarkCategoryFilter(QObject* parent) :
        QDeclarativeLandmarkFilterBase(parent), m_category(0)
{
}

QDeclarativeLandmarkCategoryFilter::~QDeclarativeLandmarkCategoryFilter()
{
}

QDeclarativeLandmarkCategory* QDeclarativeLandmarkCategoryFilter::category() const
{
    return m_category;
}

/*!
    \qmlproperty LandmarkCategory LandmarkCategoryFilter::category
    Category for whose landmarks should be filtered.
*/

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


/*!
    \qmlclass LandmarkBoxFilter QDeclarativeLandmarkBoxFilter
    \brief The LandmarkBoxFilter element specifies a box (rectangle) filter for landmark model.

    This element is part of the \bold{QtMobility.location 1.1} module.

    Logical combinations of this and other landmark filters can be
    formed using \l LandmarkIntersectionFilter and \l LandmarkUnionFilter.

    The following example creates a filter which filters for landmarks located within
    the given box (rectangle between top left and bottom right coordinates).

    TODO verify the filter when example app is ready (there seems to be slightly excessive amount
    of landmarks atm).

    \qml
    Map {
        id: map
        zoomLevel: 15
        size.width: parent.width
        size.height: parent.height
        center: myPositionSource.position.coordinate
    }

    LandmarkBoxFilter {
        id: boxFilter
        topLeft: map.toCoordinate(Qt.point(0,0))
        bottomRight: map.toCoordinate(Qt.point(map.width, map.height))
    }
    \endqml

    \sa LandmarkModel

*/

QDeclarativeLandmarkBoxFilter::QDeclarativeLandmarkBoxFilter(QObject* parent) :
        QDeclarativeLandmarkFilterBase(parent), m_topLeft(0), m_bottomRight(0)
{
}

QDeclarativeLandmarkBoxFilter::~QDeclarativeLandmarkBoxFilter()
{
}

QDeclarativeCoordinate* QDeclarativeLandmarkBoxFilter::topLeft() const
{
    return m_topLeft;
}

/*!
    \qmlproperty Coordinate LandmarkBoxFilter::topLeft
    Top left coordinate of the box to filter (landmarks within the boundaries).
*/

void QDeclarativeLandmarkBoxFilter::setTopLeft(QDeclarativeCoordinate* coordinate)
{
    m_topLeft = coordinate;
    if (m_topLeft && m_bottomRight)
        m_filter.setBoundingBox(QGeoBoundingBox(m_topLeft->coordinate(), m_bottomRight->coordinate()));
    emit topLeftChanged();
    emit filterContentChanged();
}

/*!
    \qmlproperty Coordinate LandmarkBoxFilter::bottomRight
    Bottom right coordinate of the box to filter (landmarks within the boundaries).
*/

QDeclarativeCoordinate* QDeclarativeLandmarkBoxFilter::bottomRight() const
{
    return m_bottomRight;
}

void QDeclarativeLandmarkBoxFilter::setBottomRight(QDeclarativeCoordinate* coordinate)
{
    m_bottomRight = coordinate;
    if (m_topLeft && m_bottomRight)
        m_filter.setBoundingBox(QGeoBoundingBox(m_topLeft->coordinate(), m_bottomRight->coordinate()));
    emit bottomRightChanged();
    emit filterContentChanged();
}

QLandmarkFilter* QDeclarativeLandmarkBoxFilter::filter()
{
    if (!m_topLeft || !m_bottomRight)
        return 0;
    return &m_filter;
}

/*!
    \qmlclass LandmarkProximityFilter QDeclarativeLandmarkProximityFilter
    \brief The LandmarkProximityFilter element specifies a proximity filter for landmark model.

    This element is part of the \bold{QtMobility.location 1.1} module.

    Logical combinations of this and other landmark filters can be
    formed using \l LandmarkIntersectionFilter and \l LandmarkUnionFilter.

    The following example creates a filter which filters for landmarks located within
    1500 metres of the current position.

    \qml
    PositionSource {
        id: myPositionSource
        updateInterval: 5000
        active: true
    }

    LandmarkProximityFilter {
        id: proximityFilter
        center: myPositionSource.position.coordinate
        radius: 1500
    }
    \endqml

    \sa LandmarkModel
*/

QDeclarativeLandmarkProximityFilter::QDeclarativeLandmarkProximityFilter(QObject *parent) :
        QDeclarativeLandmarkFilterBase(parent), m_radius(0), m_coordinate(0)
{
}

QDeclarativeLandmarkProximityFilter::~QDeclarativeLandmarkProximityFilter()
{
}

double QDeclarativeLandmarkProximityFilter::radius() const
{
    return m_radius;
}

/*!
    \qmlproperty double LandmarkProximityFilter::radius
    Radius (in metres) from the \l center coordinate to filter
    (landmarks within that area).

    \sa center
*/

void QDeclarativeLandmarkProximityFilter::setRadius(const double radius)
{
    if (radius == m_radius)
        return;
    m_radius = radius;
    emit radiusChanged();
    emit filterContentChanged();
}

QDeclarativeCoordinate* QDeclarativeLandmarkProximityFilter::center() const
{
    return m_coordinate;
}

/*!
    \qmlproperty Coordinate LandmarkProximityFilter::center
    Center coordinate in whose proximity landmarks should be filtered.

    \sa radius
*/

void QDeclarativeLandmarkProximityFilter::setCenter(QDeclarativeCoordinate* coordinate)
{
    m_coordinate = coordinate;
    QObject::connect(m_coordinate, SIGNAL(latitudeChanged(double)), this, SIGNAL(filterContentChanged()));
    QObject::connect(m_coordinate, SIGNAL(longitudeChanged(double)), this, SIGNAL(filterContentChanged()));
    emit centerChanged();
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

/*!
    \qmlproperty list<Landmark*Filter> LandmarkUnionFilter::filters
    \default

    The filters to OR together.
*/

/*!
    \qmlproperty list<Landmark*Filter> LandmarkIntersectionFilter::filters
    \default

    The filters to AND together.
*/

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

/*!
    \qmlclass LandmarkUnionFilter QDeclarativeLandmarkUnionFilter
    \brief The LandmarkUnionFilter element specifies a union of landmark filters.

    This element is part of the \bold{QtMobility.location 1.1} module.

    Logical OR combinations of landmark filters can be formed using LandmarkUnionFilter.

    The following example creates a filter which filters for landmarks which are
    either named \c Darwin, or located within 500 metres from coordinate (20,10)
    (or both).

    \qml
    LandmarkUnionFilter {
        id: landmarkUnionFilter
        LandmarkNameFilter {
            name: "Darwin"
        }
        LandmarkProximityFilter {
            coordinate: Coordinate {
                latitude: 20
                longitude:  10
            }
            radius: 500
        }
    }
    \endqml

    \sa LandmarkIntersectionFilter LandmarkModel

*/

QLandmarkFilter* QDeclarativeLandmarkUnionFilter::filter()
{
    return appendFilters<QLandmarkUnionFilter>(&m_filter) ? &m_filter : 0;
}

QDeclarativeLandmarkUnionFilter::QDeclarativeLandmarkUnionFilter(QObject* parent)
        : QDeclarativeLandmarkCompoundFilter(parent)
{
}

/*!
    \qmlclass LandmarkIntersectionFilter QDeclarativeLandmarkIntersectionFilter
    \brief The LandmarkIntersectionFilter element specifies an insection of landmark filters.

    This element is part of the \bold{QtMobility.location 1.1} module.

    Logical AND combinations of LandmarkFilters can be formed using LandmarkIntersectionFilter.

    The following example creates a filter which filters for landmarks named \c Darwin within
    500 metres of coordinate (20,10).

    \qml
    LandmarkIntersectionFilter {
        id: landmarkIntersectionFilter
        LandmarkNameFilter {
            name: "Darwin"
        }
        LandmarkProximityFilter {
            coordinate: Coordinate {
                latitude: 20
                longitude:  10
            }
            radius: 500
        }
    }
    \endqml

    \sa LandmarkUnionFilter LandmarkModel
*/

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
