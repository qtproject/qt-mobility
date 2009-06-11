class QCoordinate
{
public:
    enum CoordinateType {
        InvalidCoordinate,
        Coordinate2D,
        Coordinate3D
    };

    enum CoordinateFormat {
        DecimalDegrees,
        DecimalDegreesWithHemisphere,
        DegreesMinutes,
        DegreesMinutesWithHemisphere,
        DegreesMinutesSeconds,
        DegreesMinutesSecondsWithHemisphere
    };

    QCoordinate();
    QCoordinate(double latitude, double longitude);
    QCoordinate(double latitude, double longitude, double altitude);
    QCoordinate(const QCoordinate &other);
    ~QCoordinate();

    QCoordinate &operator=(const QCoordinate &other);

    bool isValid() const;
    CoordinateType type() const;

    void setLatitude(double latitude);
    double latitude() const;

    void setLongitude(double longitude);
    double longitude() const;

    void setAltitude(double altitude);
    double altitude() const;

    qreal distanceTo(const QCoordinate &other) const;
    qreal azimuthTo(const QCoordinate &other) const;

    QString toString(CoordinateFormat format = DegreesMinutesSecondsWithHemisphere) const;

private:
    QCoordinatePrivate *d;
};
QDataStream &operator<<(QDataStream &stream, const QCoordinate &coordinate);
QDataStream &operator>>(QDataStream &stream, QCoordinate &coordinate);



class QPositionUpdate
{
public:
    enum Property {
        Heading,
        GroundSpeed,
        VerticalSpeed,
        MagneticVariation,
        HorizontalAccuracy,
        VerticalAccuracy
    };

    QPositionUpdate();
    QPositionUpdate(const QCoordinate &coordinate, const QDateTime &updateTime);
    QPositionUpdate(const QPositionUpdate &other);
    ~QPositionUpdate();

    QPositionUpdate &operator=(const QPositionUpdate &other);

    bool isNull() const;

    void setUpdateTime(const QDateTime &updateTime);
    QDateTime updateTime() const;

    void setCoordinate(const QCoordinate &coordinate);
    QCoordinate coordinate() const;

    void setDoubleProperty(Property property, qreal value);
    qreal doubleProperty(Property property) const;
    void removeProperty(Property property);

    bool hasProperty(Property property) const;

private:
    QPositionUpdatePrivate *d;
};
QDataStream &operator<<(QDataStream &stream, const QPositionUpdate &update);
QDataStream &operator>>(QDataStream &stream, QPositionUpdate &update);



class QSatelliteInfo
{
public:
    enum Property {
        Elevation,
        Azimuth
    };

    QSatelliteInfo();
    ~QSatelliteInfo();

    void setPrnNumber(int prn);
    int prnNumber() const;

    void setSignalStrength(int signalStrength);
    int signalStrength() const;

    void setDoubleProperty(Property property, qreal value);
    qreal doubleProperty(Property property) const;
    void removeProperty(Property property);

    bool hasProperty(Property property) const;
};



class QSatelliteInfoSource : public QObject
{
    Q_OBJECT
public:
    explicit QSatelliteInfoSource(QObject *parent = 0);

    static QSatelliteInfoSource *createSource();

public slots:
    virtual void startUpdates() = 0;
    virtual void stopUpdates() = 0;

    virtual void requestUpdate(int timeout = 5000) = 0;

signals:
    void satellitesInViewUpdated(const QList<QSatelliteInfo> &satellites);
    void satellitesInUseUpdated(const QList<QSatelliteInfo> &satellites);
    void requestTimeout();

private:
    Q_DISABLE_COPY(QSatelliteInfoSource)
    QSatelliteInfoSourcePrivate *d;
};



class QPositionSource : public QObject
{
    Q_OBJECT
public:
    enum IntervalType {
        TimeInterval,
        DistanceInterval
    };

    enum PositioningMethod {
        SatellitePositioningMethods = 0x000000ff,
        NonSatellitePositioningMethods = 0xffffff00,
        AllPositioningMethods = 0xffffffff
    };
    Q_DECLARE_FLAGS(PositioningMethods, PositioningMethod)


    explicit QPositionSource(QObject *parent = 0);

    virtual void setUpdateInterval(IntervalType type, int interval);
    IntervalType updateIntervalType() const;
    int updateIntervalValue() const;

    virtual void setPreferredPositioningMethods(PositioningMethods methods);
    PositioningMethods preferredPositioningMethods() const;

    QPositionUpdate lastKnownPosition(bool fromSatellitePositioningMethodsOnly = false) const = 0;

    virtual PositioningMethods supportedPositioningMethods() const = 0;
    virtual int minimumIntervalForType(IntervalType type) const = 0;

    static QPositionSource *createSource();

public slots:
    virtual void startUpdates() = 0;
    virtual void stopUpdates() = 0;

    virtual void requestUpdate(int timeout = 5000) = 0;

signals:
    void positionUpdated(const QPositionUpdate &update);
    void requestTimeout();

private:
    Q_DISABLE_COPY(QPositionSource)
    QPositionSourcePrivate *d;
};



class QPositionAreaMonitor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool inMonitoredArea READ inMonitoredArea)
public:
    explicit QPositionAreaMonitor(QPositionSource *source, QObject *parent = 0);

    QPositionSource *source() const;

    void setMonitoredArea(const QCoordinate &coordinate, int radius);
    QCoordinate coordinate() const;
    int radius() const;

    bool inMonitoredArea() const;

signals:
    void areaEntered(const QPositionUpdate &update);
    void areaExited(const QPositionUpdate &update);

private:
    Q_DISABLE_COPY(QPositionAreaMonitor)
    QPositionAreaMonitorPrivate *d;
}; 

