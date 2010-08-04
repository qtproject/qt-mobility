#ifndef QDECLARATIVELANDMARKFILTERS_P_H
#define QDECLARATIVELANDMARKFILTERS_P_H

#include <qlandmarkproximityfilter.h>
#include <qlandmarknamefilter.h>
#include <qlandmarkfilter.h>

#include <QtDeclarative/qdeclarative.h>
#include <QObject>

QTM_BEGIN_NAMESPACE

// File consists of misc smaller filters that can be
// used when filtering landmarks and categories

class QDeclarativeLandmarkNameFilter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
public:
    explicit QDeclarativeLandmarkNameFilter(QObject *parent = 0);
    QString name();
    void setName(const QString& name);
signals:
    void nameChanged(QString name);
    void filterChanged();
private:
    QLandmarkNameFilter m_filter;
    friend class QDeclarativeLandmarkSource;
    QLandmarkNameFilter* filter();
};

class QDeclarativeLandmarkProximityFilter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double latitude READ latitude WRITE setLatitude NOTIFY latitudeChanged)
    Q_PROPERTY(double longitude READ longitude WRITE setlongitude NOTIFY longitudeChanged)
    Q_PROPERTY(double radius READ radius WRITE setRadius NOTIFY radiusChanged)

public:
    //explicit QDeclarativeLandmarkProximityFilter(QObject *parent = 0);
    QDeclarativeLandmarkProximityFilter(QObject *parent = 0);
    double latitude() const;
    void setLatitude(double latitude);
    double longitude() const;
    void setlongitude(double longitude);
    double radius() const;
    void setRadius(double radius);

signals:
    void latitudeChanged(double latitude);
    void longitudeChanged(double longitude);
    void radiusChanged(double radius);
    void filterChanged();
private:
    QLandmarkProximityFilter m_filter;
    friend class QDeclarativeLandmarkSource;
    QLandmarkProximityFilter* filter();
    QGeoCoordinate m_coordinate;
};

QTM_END_NAMESPACE
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLandmarkNameFilter));
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLandmarkProximityFilter));

#endif // QDECLARATIVELANDMARKFILTERS_P_H
