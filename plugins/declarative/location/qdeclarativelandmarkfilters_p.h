#ifndef QDECLARATIVELANDMARKFILTERS_P_H
#define QDECLARATIVELANDMARKFILTERS_P_H

#include <qlandmarkintersectionfilter.h>
#include <qlandmarkproximityfilter.h>
#include <qdeclarativecoordinate_p.h>
#include <qlandmarkunionfilter.h>
#include <qlandmarknamefilter.h>
#include <qlandmarkfilter.h>

#include <QtDeclarative/qdeclarative.h>
#include <QDeclarativeListProperty>
#include <QObject>

QTM_BEGIN_NAMESPACE

class QDeclarativeLandmarkFilterBase : public QObject
{
    Q_OBJECT

public:
    explicit QDeclarativeLandmarkFilterBase(QObject* parent = 0)  : QObject(parent) {}
    virtual QLandmarkFilter* filter() = 0;
};

class QDeclarativeLandmarkNameFilter : public QDeclarativeLandmarkFilterBase
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

public:
    explicit QDeclarativeLandmarkNameFilter(QObject* parent = 0);
    ~QDeclarativeLandmarkNameFilter();

    QString name() const;
    void setName(const QString& name);
    // From QDeclarativeLandmarkFilterBase
    virtual QLandmarkFilter* filter();

signals:
    void nameChanged();

private:
    QString m_name;
    QLandmarkNameFilter m_filter;
};

class QDeclarativeLandmarkProximityFilter : public QDeclarativeLandmarkFilterBase
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeCoordinate* coordinate READ coordinate WRITE setCoordinate NOTIFY coordinateChanged)
    Q_PROPERTY(double radius READ radius WRITE setRadius NOTIFY radiusChanged)

public:
    explicit QDeclarativeLandmarkProximityFilter(QObject* parent = 0);
    ~QDeclarativeLandmarkProximityFilter();

    double radius() const;
    void setRadius(const double radius);
    QDeclarativeCoordinate* coordinate() const;
    void setCoordinate(QDeclarativeCoordinate* coordinate);
    // From QDeclarativeLandmarkFilterBase
    virtual QLandmarkFilter* filter();

signals:
    void radiusChanged();
    void coordinateChanged();

private:
    double m_radius;
    QDeclarativeCoordinate* m_coordinate;
    QLandmarkProximityFilter m_filter;
};


class QDeclarativeLandmarkUnionFilter : public QDeclarativeLandmarkFilterBase
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeLandmarkFilterBase> filters READ filters)
    Q_CLASSINFO("DefaultProperty", "filters")

public:
    explicit QDeclarativeLandmarkUnionFilter(QObject* parent = 0);

    // 'READ' accessor for the filters, basically this is also a 'WRITE' accessor
    // as per QDeclarativeListProperty's design.
    QDeclarativeListProperty<QDeclarativeLandmarkFilterBase> filters();
    // Returns a C++ union filter of all set filters
    virtual QLandmarkFilter* filter();

private:
    QList<QDeclarativeLandmarkFilterBase*> m_filters;
    QLandmarkUnionFilter m_filter;
};

class QDeclarativeLandmarkIntersectionFilter : public QDeclarativeLandmarkFilterBase
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeLandmarkFilterBase> filters READ filters)
    Q_CLASSINFO("DefaultProperty", "filters")

public:
    explicit QDeclarativeLandmarkIntersectionFilter(QObject* parent = 0);
    // READ for the filters, basically this is also a WRITE accessor
    // as per QDeclarativeListProperty's design.
    QDeclarativeListProperty<QDeclarativeLandmarkFilterBase> filters();
    // Returns a C++ intersection filter of all set filters
    virtual QLandmarkFilter* filter();

private:
    QList<QDeclarativeLandmarkFilterBase*> m_filters;
    QLandmarkIntersectionFilter m_filter;
};

QTM_END_NAMESPACE
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLandmarkNameFilter));
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLandmarkProximityFilter));
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLandmarkUnionFilter));
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLandmarkIntersectionFilter));

#endif // QDECLARATIVELANDMARKFILTERS_P_H
