/*!
    \class QCoordinate
    \brief The QCoordinate class defines a geographical position on the surface of the Earth.

    A QCoordinate is defined by latitude, longitude, and optionally, altitude.

    Use type() to determine whether a coordinate is a 2D coordinate (has
    latitude and longitude only) or 3D coordinate (has latitude, longitude
    and altitude). Use distanceTo() and azimuthTo() to calculate the distance
    and bearing between coordinates.

    The coordinate values should be specified using the WGS84 datum.
*/

/*!
    \enum QCoordinate::CoordinateType
    Defines the types of a coordinate.

    \value InvalidCoordinate An invalid coordinate. A coordinate is invalid if its latitude or longitude values are invalid.
    \value Coordinate2D A coordinate with valid latitude and longitude values.
    \value Coordinate3D A coordinate with valid latitude and longitude values, and also an altitude value.
*/

/*!
    \enum QCoordinate::CoordinateFormat
    Defines the possible formatting options for toString().

    \value DecimalDegrees Returns a string representation of the coordinates in decimal degrees format.
    \value DecimalDegreesWithHemisphere Returns a string representation of the coordinates in decimal degrees format, using 'N', 'S', 'E' or 'W' to indicate the hemispheres of the coordinates.
    \value DegreesMinutes Returns a string representation of the coordinates in degrees-minutes format.
    \value DegreesMinutesWithHemisphere Returns a string representation of the coordinates in degrees-minutes format, using 'N', 'S', 'E' or 'W' to indicate the hemispheres of the coordinates.
    \value DegreesMinutesSeconds Returns a string representation of the coordinates in degrees-minutes-seconds format.
    \value DegreesMinutesSecondsWithHemisphere Returns a string representation of the coordinates in degrees-minutes-seconds format, using 'N', 'S', 'E' or 'W' to indicate the hemispheres of the coordinates.

    \sa toString()
*/

/*!
    \fn QCoordinate::QCoordinate();

    Creates an invalid coordinate.
*/

/*!
    \fn QCoordinate::QCoordinate(double latitude, double longitude);

    Creates a coordinate with the given \a latitude and \a longitude.
*/

/*!
    \fn QCoordinate::QCoordinate(double latitude, double longitude, double altitude);

    Creates a coordinate with the given \a latitude, \a longitude and \a altitude.
*/

/*!
    \fn QCoordinate::QCoordinate(const QCoordinate &other);

    Creates a coordinate with the values of \a other.
*/

/*!
    \fn QCoordinate::~QCoordinate();

    Destroys a coordinate.
*/

/*!
    \fn QCoordinate &QCoordinate::operator=(const QCoordinate &other);

    Assigns the values from \a other to this coordinate.
*/


/*!
    \fn bool QCoordinate::operator==(const QCoordinate &other) const;

    Returns true if the latitude, longitude and altitude of this
    coordinate are the same as those of \a other.
*/

/*!
    \fn bool QCoordinate::operator!=(const QCoordinate &other) const;

    Returns true if the latitude, longitude or altitude of this
    coordinate are not the same as those of \a other.
*/

/*!
    \fn bool QCoordinate::isValid() const;

    Returns true if the type() is Coordinate2D or Coordinate3D.
*/

/*!
    \fn CoordinateType QCoordinate::type() const;

    Returns the type of the coordinate.
*/

/*!
    \fn void QCoordinate::setLatitude(double latitude);

    Sets the latitude (in decimal degrees) to \a latitude. The value should
    be in the WGS84 datum.

    To be valid, the latitude must be between -90 to 90 inclusive.
*/

/*!
    \fn double QCoordinate::latitude() const;

    Returns the coordinate's latitude.

    The return value is undefined if the latitude has not been set.

    \sa isValid(), type()
*/

/*!
    \fn void QCoordinate::setLongitude(double longitude);

    Sets the longitude (in decimal degrees) to \a longitude. The value should
    be in the WGS84 datum.

    To be valid, the longitude must be between -180 to 180 inclusive.
*/

/*!
    \fn double QCoordinate::longitude() const;

    Returns the coordinate's longitude.

    The return value is undefined if the longitude has not been set.

    \sa isValid(), type()
*/

/*!
    \fn void QCoordinate::setAltitude(double altitude);

    Sets the altitude (meters above sea level) to \a altitude.
*/

/*!
    \fn double QCoordinate::altitude() const;

    Returns the coordinate's altitude.

    The return value is undefined if the altitude has not been set.

    \sa type()
*/

/*!
    \fn qreal QCoordinate::distanceTo(const QCoordinate &other) const;

    Returns the distance (in meters) from this coordinate to the coordinate
    specified by \a other. Altitude is not used in the calculation.

    This calculation returns the great-circle distance between the two
    coordinates, with an assumption that the Earth is spherical for the
    purposes of the calculation.

    Returns 0 if the type of this coordinate or the type of \a other is
    QCoordinate::InvalidCoordinate.
*/

/*!
    \fn qreal QCoordinate::azimuthTo(const QCoordinate &other) const;

    Returns the azimuth (or bearing) in degrees from this coordinate to the
    coordinate specified by \a other. Altitude is not used in the calculation.

    There is an assumption that the Earth is spherical for the purposes of
    the calculation.

    Returns 0 if the type of this coordinate or the type of \a other is
    QCoordinate::InvalidCoordinate.
*/

/*!
    \fn QString QCoordinate::toString(CoordinateFormat format = DegreesMinutesSecondsWithHemisphere) const;

    Returns this coordinate as a string in the specified \a format.

    For example, if this coordinate latitude-longitude coordinates of
    (-27.46758\unicode{0xB0}, 153.027892\unicode{0xB0}), these are the
    strings that are returned depending on \a format:

    \table
    \header
        \o \a format value
        \o Returned string
    \row
        \o \l DecimalDegrees
        \o -27.46758\unicode{0xB0}, 153.02789\unicode{0xB0}
    \row
        \o \l DecimalDegreesWithHemisphere
        \o 27.46758\unicode{0xB0} S, 153.02789\unicode{0xB0} E
    \row
        \o \l DegreesMinutes
        \o -27\unicode{0xB0} 28.054', 153\unicode{0xB0} 1.673'
    \row
        \o \l DegreesMinutesWithHemisphere
        \o 27\unicode{0xB0} 28.054 S', 153\unicode{0xB0} 1.673' E
    \row
        \o \l DegreesMinutesSeconds
        \o -27\unicode{0xB0} 28' 3.2", 153\unicode{0xB0} 1' 40.4"
    \row
        \o \l DegreesMinutesSecondsWithHemisphere
        \o 27\unicode{0xB0} 28' 3.2" S, 153\unicode{0xB0} 1' 40.4" E
    \endtable
*/



/*!
    \class QPositionUpdate
    \brief The QPositionUpdate class contains information gathered on one's global position, direction and velocity at a particular point in time.

    A QPositionUpdate contains, at a minimum, a geographical coordinate and
    a timestamp. It may also have heading and speed measurements as well as
    estimates of the accuracy of the provided data.

    \sa QPositionSource
*/

/*!
    \enum QPositionUpdate::Property
    Defines the properties for a position update.

    \value Heading The bearing to true north, in degrees.
    \value GroundSpeed The ground speed, in metres/sec.
    \value VerticalSpeed The vertical speed, in metres/sec.
    \value MagneticVariation The angle between the horizontal component of the magnetic field and true north, in degrees. Also known as magnetic declination.
    \value HorizontalAccuracy The accuracy of the provided latitude-longitude value, in metres.
    \value VerticalAccuracy The accuracy of the provided altitude value, in metres.
*/

/*!
    \fn QPositionUpdate::QPositionUpdate();

    Creates an invalid update.

    \sa isValid()
*/

/*!
    \fn QPositionUpdate::QPositionUpdate(const QCoordinate &coordinate, const QDateTime &updateTime);

    Creates an update with the given \a coordinate and \a updateTime.
*/

/*!
    \fn QPositionUpdate::QPositionUpdate(const QPositionUpdate &other);

    Creates an update with the values of \a other.
*/

/*!
    \fn QPositionUpdate::~QPositionUpdate();

    Destroys an update.
*/

/*!
    \fn QPositionUpdate &QPositionUpdate::operator=(const QPositionUpdate &other);

    Assigns the values from \a other to this update.
*/

/*!
    \fn bool QPositionUpdate::operator==(const QPositionUpdate &other) const

    Returns true if all of this update's values are the same as those of
    \a other.
*/

/*!
    \fn bool QPositionUpdate::operator!=(const QPositionUpdate &other) const

    Returns true if any of this update's values are not the same as those of
    \a other.
*/

/*!
    \fn bool QPositionUpdate::isValid() const;

    Returns true if the update's updateTime() and coordinate() values are both valid.

    \sa QCoordinate::isValid(), QDateTime::isValid()
*/

/*!
    \fn void QPositionUpdate::setUpdateTime(const QDateTime &updateTime);

    Sets the date and time at which this update was received to \a updateTime.

    The \a updateTime must be in UTC time.
*/

/*!
    \fn QDateTime QPositionUpdate::updateTime() const;

    Returns the date and time at which this update was received, in UTC time.

    Returns an invalid QDateTime if no date/time value has been set.
*/

/*!
    \fn void QPositionUpdate::setCoordinate(const QCoordinate &coordinate);

    Sets the coordinate for this update to \a coordinate.
*/

/*!
    \fn QCoordinate QPositionUpdate::coordinate() const;

    Returns the coordinate for this update.

    Returns an invalid coordinate if no coordinate has been set.
*/

/*!
    \fn void QPositionUpdate::setDoubleProperty(Property property, qreal value);

    Sets the value for \a property to \a value.
*/

/*!
    \fn qreal QPositionUpdate::doubleProperty(Property property) const;

    Returns the value of the specified \a property as a qreal value.

    Returns -1 if the value has not been set.

    \sa hasProperty(), setDoubleProperty()
*/

/*!
    \fn void QPositionUpdate::removeProperty(Property property);

    Removes the specified \a property and its value.
*/

/*!
    \fn bool QPositionUpdate::hasProperty(Property property) const;

    Returns true if the specified \a property is present in this update.
*/



/*!
    \class QSatelliteInfo
    \brief The QSatelliteInfo class contains basic information about a satellite.
*/

/*!
    \enum QSatelliteInfo::Property
    Defines the properties for the satellite information.

    \value Elevation The elevation of the satellite, in degrees.
    \value Azimuth The azimuth to true north, in degrees.
*/

/*!
    \fn QSatelliteInfo::QSatelliteInfo()

    Creates a satellite information object.
*/

/*!
    \fn QSatelliteInfo::~QSatelliteInfo()

    Destroys a QSatelliteInfo object.
*/

/*!
    \fn QSatelliteInfo::setPrnNumber(int prn);

    Sets the PRN (Pseudo-random noise) number to \a prn.

    The PRN number can be used to identify a satellite.
*/

/*!
    \fn int QSatelliteInfo::prnNumber() const;

    Returns the PRN (Pseudo-random noise) number, or -1 if the value has not been set.
*/

/*!
    \fn void QSatelliteInfo::setSignalStrength(int signalStrength);

    Sets the signal strength to \a signalStrength, in decibels.
*/

/*!
    \fn int QSatelliteInfo::signalStrength() const;

    Returns the signal strength, or -1 if the value has not been set.
*/

/*!
    \fn void QSatelliteInfo::setDoubleProperty(Property property, qreal value);

    Sets the value for \a property to \a value.
*/

/*!
    \fn qreal QSatelliteInfo::doubleProperty(Property property) const;

    Returns the value of the specified \a property as a qreal value.

    Returns -1 if the value has not been set.

    \sa hasProperty(), setDoubleProperty()
*/

/*!
    \fn void QSatelliteInfo::removeProperty(Property property);

    Removes the specified \a property and its value.
*/

/*!
    \fn bool QSatelliteInfo::hasProperty(Property property) const;

    Returns true if the specified \a property is present in this update.
*/



/*!
    \class QSatelliteInfoSource
    \brief The QSatelliteInfoSource class is an abstract base class for the distribution of satellite information updates.

    The static function QSatelliteInfoSource::createSource() creates a default
    satellite data source that is appropriate for the platform, if one is 
    available. Otherwise, QSatelliteInfoSource can be subclassed to create an 
    appropriate custom source of satellite data.

    Call startUpdates() and stopUpdates() to start and stop regular updates,
    or requestUpdate() to request a single update.
    When an update is available, satellitesInViewUpdated() and/or
    satellitesInUseUpdated() will be emitted.
*/

/*!
    \fn QSatelliteInfoSource::QSatelliteInfoSource(QObject *parent = 0);

    Creates a source with the specified \a parent.
*/

/*!
    \fn static QSatelliteInfoSource *QSatelliteInfoSource::createSource(QObject *parent = 0);

    Creates and returns a source that reads from the system's default
    source of satellite update information.

    Returns 0 if the system has no default source.
*/

/*!
    \fn virtual void QSatelliteInfoSource::startUpdates() = 0;

    Starts emitting updates at regular intervals. The updates will be 
    provided whenever new satellite information becomes available.

    \sa satellitesInViewUpdated(), satellitesInUseUpdated()
*/

/*!
    \fn virtual void QSatelliteInfoSource::stopUpdates() = 0;

    Stops emitting updates at regular intervals.
*/

/*!
    \fn virtual void QSatelliteInfoSource::requestUpdate(int timeout = 5000);

    Attempts to get the current satellite information and emit
    satellitesInViewUpdated() and satellitesInUseUpdated() with this
    information. This is useful if you do not need the regular updates
    provided by startUpdates(). If the current position cannot be found
    within the given \a timeout (in milliseconds), requestTimeout() is
    emitted.

    This does nothing if another update request is in progress. However
    it can be called even if startUpdates() has already been called and
    regular updates are in progress.
*/

/*!
    \fn void QSatelliteInfoSource::requestTimeout();

    Emitted if requestUpdate() was called and the current satellite
    information could not be retrieved within the specified timeout.
*/

/*!
    \fn void QSatelliteInfoSource::satellitesInViewUpdated(const QList<QSatelliteInfo> &satellites);

    If startUpdates() or requestUpdate() is called, this signal is emitted
    when an update is available on the satellites that are
    currently in view.

    The \a satellites parameter holds the satellites currently in view.
*/

/*!
    \fn void QSatelliteInfoSource::satellitesInUseUpdated(const QList<QSatelliteInfo> &satellites);

    If startUpdates() or requestUpdate() is called, this signal is emitted
    when an update is available on the number of satellites that are
    currently in use.

    These are the satellites that are used to get a "fix" - that
    is, those used to determine the current position.

    The \a satellites parameter holds the satellites currently in use.
*/



/*!
    \class QPositionSource
    \brief The QPositionSource class is an abstract base class for the distribution of positional updates.

    The static function QPositionSource::createSource() creates a default
    position source that is appropriate for the platform, if one is available.
    Otherwise, QPositionSource can be subclassed to create an appropriate
    custom source of position data.

    Users of a QPositionSource subclass can request the current position using
    requestUpdate(), or start and stop regular position updates using
    startUpdates() and stopUpdates(). When an update is available,
    positionUpdate() is emitted. The last known position can be accessed with
    lastUpdate().

    If regular position updates are required, setUpdateInterval() can be used
    to specify how often these updates should be emitted. (If no interval is
    specified, updates are simply provided whenever they are available.)
    For example:

    \code
        // Emit updates every 10 seconds if available
        QPositionSource *source = QPositionSource::createSource();
        source->setUpdateInterval(10000);
    \endcode

    To remove an update interval that was previously set, call
    setUpdateInterval() with a value of 0.

    Note that the position source may have a minimum value requirement for
    update intervals, as returned by minimumIntervalForType().
*/

/*!
    \enum QPositionSource::PositioningMethod
    Defines the types of positioning methods.

    \value SatellitePositioningMethods Satellite-based positioning methods such as GPS.
    \value NonSatellitePositioningMethods Other positioning methods.
    \value AllPositioningMethods A flag that matches all positioning methods.
*/

/*!
    \fn QPositionSource::QPositionSource(QObject *parent = 0);

    Creates a position source with the specified \a parent.
*/

/*!
    \fn virtual void QPositionSource::setUpdateInterval(int msec);

    Sets the source to emit updates every \a msec milliseconds.
    If \a msec is 0, interval-based updates are disabled and the source
    provides updates whenever they are available.

    If \a msec is not 0 and is less than the value returned by
    minimumUpdateInterval(), the interval will be set to the minimum interval.

    Note that implementations may not provide updates at the exact
    interval specified. For example, a given millisecond interval value may be
    rounded to the nearest second if the implementation does not support
    intervals specified to millisecond precision.

    \bold {Note:} When reimplementing this method, subclasses must call the
    base method implementation to ensure updateInterval() returns the correct
    value.
*/

/*!
    \fn int QPositionSource::updateInterval() const;

    Returns the interval value set by setUpdateInterval().

    Returns 0 if no interval has been set.
*/

/*!
    \fn virtual void QPositionSource::setPreferredPositioningMethods(PositioningMethods methods);

    Sets the preferred positioning methods for this source to \a methods.

    \bold {Note:} When reimplementing this method, subclasses must call the
    base method implementation to ensure preferredPositioningMethods() returns the correct value.

    \sa supportedPositioningMethods()
*/

/*!
    \fn PositioningMethods QPositionSource::preferredPositioningMethods() const;

    Returns the positioning methods set by setPreferredPositioningMethods().
*/

/*!
    \fn QPositionUpdate QPositionSource::lastKnownPosition(bool fromSatellitePositioningMethodsOnly = false) const = 0;

    Returns an update containing the last known position, or a null update
    if none is available.

    If \a fromSatellitePositioningMethodsOnly is true, this returns the last
    known position received from a satellite positioning method, and if none
    is available, a null update is returned.
*/

/*!
    \fn virtual PositioningMethods QPositionSource::supportedPositioningMethods() const = 0;

    Returns the positioning methods available to this source.

    \sa setPreferredPositioningMethods()
*/

/*!
    \fn virtual int QPositionSource::minimumUpdateInterval() const = 0;

    Returns the minimum value accepted by setUpdateInterval().
*/

/*!
    \fn static QPositionSource *QPositionSource::createSource(QObject *parent = 0);

    Creates and returns a position source with the given \a parent that
    reads from the system's default sources of location data.

    Returns 0 if the system has no default position source.
*/

/*!
    \fn virtual void QPositionSource::startUpdates() = 0;

    Starts emitting updates at regular intervals as specified by setUpdateInterval().

    If setUpdateInterval() has not been called, the source will emit updates
    as soon as they become available.
*/

/*!
    \fn virtual void QPositionSource::stopUpdates() = 0;

    Stops emitting updates at regular intervals.
*/

/*!
    \fn virtual void QPositionSource::requestUpdate(int timeout = 5000);

    Attempts to get the current position and emit positionUpdated() with
    this information. This is useful if you do not need the regular updates
    provided by startUpdates(). If the current position cannot be found
    within the given \a timeout (in milliseconds), requestTimeout() is
    emitted.

    This does nothing if another update request is in progress. However
    it can be called even if startUpdates() has already been called and
    regular updates are in progress.

    If the source uses multiple positioning methods, it tries to gets the
    current position from the most accurate positioning method within the
    given timeout.
*/

/*!
    \fn void QPositionSource::positionUpdated(const QPositionUpdate &update);

    If startUpdates() or requestUpdate() is called, this signal is emitted
    when an update becomes available.

    The \a update value holds the value of the new update.
*/

/*!
    \fn void QPositionSource::requestTimeout();

    Emitted if requestUpdate() was called and the current position could
    not be retrieved within the specified timeout.
*/

/*!
    \class QPositionAreaMonitor
    \brief The QPositionAreaMonitor class enables the detection of proximity changes for a specified set of coordinates.

    A QPositionAreaMonitor emits signals when the current position is in
    range, or has moved out of range, of a specified area.

    For example:

    \code
        public:
            MyClass::MyClass()
            {
                QPositionAreaMonitor *monitor = QPositionAreaMonitor::createMonitor();
                connect(monitor, SIGNAL(areaEntered(QPositionUpdate)),
                        this, SLOT(areaEntered(QPositionUpdate)));
                connect(monitor, SIGNAL(areaExited(QPositionUpdate)),
                        this, SLOT(areaExited(QPositionUpdate)));

                QCoordinate bigBenLocation(51.50104, -0.124632);
                monitor->setMonitoredArea(bigBenLocation, 100);
            }

        public slots:
            void areaEntered(const QPositionUpdate &update)
            {
                qDebug() << "Now within 100 meters, current position is" << update.coordinate();
            }

            void areaExited(const QPositionUpdate &update)
            {
                qDebug() << "No longer within 100 meters, current position is" << update.coordinate();
            }
    \endcode
*/

/*!
    \fn explicit QPositionAreaMonitor::QPositionAreaMonitor(QObject *parent = 0)

    Creates a monitor with the given \a parent.
*/

/*!
    \fn QPositionAreaMonitor::~QPositionAreaMonitor()

    Destroys the monitor.
*/

/*!
    \fn void QPositionAreaMonitor::setMonitoredArea(const QCoordinate &coordinate, int radius);

    Sets the area to be monitored to the area specified by \a coordinate
    with a radius of \a radius.

    If the current position is within the specified area, areaEntered()
    is emitted immediately.
*/

/*!
    \fn QCoordinate QPositionAreaMonitor::coordinate() const;

    Returns the coordinate set with setMonitoredArea(), or an invalid
    coordinate if no area has been set.
*/

/*!
    \fn int QPositionAreaMonitor::radius() const;

    Returns the radius set with setMonitoredArea(), or 0 if no area
    has been set.
*/

/*!
    \fn static QPositionAreaMonitor *QPositionAreaMonitor::createMonitor(QObject *parent = 0);

    Creates and returns a monitor with the given \a parent that
    monitors areas using resources on the underlying system.

    Returns 0 if the system has no support for position monitoring.
*/

/*!
    \fn void QPositionAreaMonitor::areaEntered(const QPositionUpdate &update);

    Emitted when the current position has moved from a position outside the
    monitored area to a position within the monitored area.

    The \a update holds the new position.
*/

/*!
    \fn void QPositionAreaMonitor::areaExited(const QPositionUpdate &update);

    Emitted when the current position has moved from a position within the
    monitored area to a position outside the monitored area.

    The \a update holds the new position.
*/
