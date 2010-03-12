
#include "qmaneuver.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QManeuver
    \brief The QManeuver class represents the maneuver component of a QRouteReply.
    \ingroup location

    This class represents a maneuver as part of a route reply.

    \sa QRouteReply
*/

/*!
    Default constructor.
*/
QManeuver::QManeuver() : dur(0), dist(0), traffDir(0), icn(0) {}

/*!
    Constructs a QManeuver from \a maneuver.
*/
QManeuver::QManeuver(const QManeuver& maneuver)
        : wPoints(maneuver.wPoints), mPoints(maneuver.mPoints)
{
    descr = maneuver.descr;
    act = maneuver.act;
    dist = maneuver.dist;
    dur = maneuver.dur;
    trn = maneuver.trn;
    stName = maneuver.stName;
    rtName = maneuver.rtName;
    nxtStreetName = maneuver.nxtStreetName;
    sgnPost = maneuver.sgnPost;
    traffDir = maneuver.traffDir;
    icn = maneuver.icn;
}

/*!
* Assignment operator.
* @param maneuver The maneuver to be assigned from.
* @return A reference to this maneuver.
*/
QManeuver& QManeuver::operator=(const QManeuver & maneuver)
{
    wPoints = maneuver.wPoints;
    mPoints = maneuver.mPoints;

    descr = maneuver.descr;
    act = maneuver.act;
    dist = maneuver.dist;
    dur = maneuver.dur;
    trn = maneuver.trn;
    stName = maneuver.stName;
    rtName = maneuver.rtName;
    nxtStreetName = maneuver.nxtStreetName;
    sgnPost = maneuver.sgnPost;
    traffDir = maneuver.traffDir;
    icn = maneuver.icn;

    return *this;
}

/*!
    Returns a textual representation of the maneuver that has to be performed to follow the route.
*/
QString QManeuver::description() const
{
    return descr;
}

/*!
    Returns a string constant that describes the essential action that has to be taken on this maneuver.
*/

QString QManeuver::action() const
{
    return act;
}
/*!
    Returns the distance that is covered by this single maneuver in meters.
*/

quint32 QManeuver::distance() const
{
    return dist;
}

/*!
    Returns the estimated duration that performing the whole maneuver will take.
    The duration will be in the xsd:duration format.
*/
qint32 QManeuver::duration() const
{
    return dur;
}

/*!
    Returns the turn to be taken in this maneuver.
*/
QString QManeuver::turn() const
{
    return trn;
}

/*!
    Returns the name of the street involved.
*/
QString QManeuver::streetName() const
{
    return stName;
}

/*!
    Returns the name of the route.
*/
QString QManeuver::routeName() const
{
    return rtName;
}

/*!
    Returns the name of the next street.
*/
QString QManeuver::nextStreetName() const
{
    return nxtStreetName;
}

/*!
    Returns a sign post.
*/
QString QManeuver::signPost() const
{
    return sgnPost;
}

/*!
    Returns a value representing the traffic direction.
*/
qint64 QManeuver::trafficDirection() const
{
    return traffDir;
}

/*!
    Returns a value representing an icon.
*/
qint64 QManeuver::icon() const
{
    return icn;
}

/*!
    Returns the geographical coordinates that define this part of the route in form of a 2D-polyline.

    The geo coordinates in the way points are less detailed then those returned by maneuverPoints(),
    and are primarily designed for displaying on screen.

    \sa maneuverPoints()
*/
const QList<QGeoCoordinate> QManeuver::wayPoints() const
{
    return wPoints;
}

/*!
    Returns the geographical coordinates that define this part of the route in form of a 2D-polyline.

    Maneuver points describe the actual maneuver to take with a much higher detail than those returned by wayPoints().

    \sa wayPoints()
*/
const QList<QGeoCoordinate> QManeuver::maneuverPoints() const
{
    return mPoints;
}


QTM_END_NAMESPACE

