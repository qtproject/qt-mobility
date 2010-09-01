#ifndef QDECLARATIVELANDMARKFILTERS_P_H
#define QDECLARATIVELANDMARKFILTERS_P_H

#include <qlandmarkintersectionfilter.h>
#include <qlandmarkproximityfilter.h>
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

class QDeclarativeLandmarkFilter : public QDeclarativeLandmarkFilterBase
{
    Q_OBJECT
    Q_PROPERTY(FilterType type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
    Q_ENUMS(FilterType)

public:
    explicit QDeclarativeLandmarkFilter(QObject* parent = 0);
    ~QDeclarativeLandmarkFilter();
    enum FilterType {
        Name,
        Proximity
    };

    FilterType type() const;
    void setType(FilterType type);
    QVariant value() const;
    void setValue(const QVariant &value);
    virtual QLandmarkFilter* filter();

signals:
    void typeChanged();
    void valueChanged();

private:
    FilterType m_type;
    QVariant m_value;
    QLandmarkFilter* m_filter;
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
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLandmarkFilter));
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLandmarkUnionFilter));
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLandmarkIntersectionFilter));

#endif // QDECLARATIVELANDMARKFILTERS_P_H
