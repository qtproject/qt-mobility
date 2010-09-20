#ifndef QDECLARATIVELANDMARKCATEGORY_P_H
#define QDECLARATIVELANDMARKCATEGORY_P_H

#include <qlandmark.h>
#include <qlandmarkcategory.h>
#include <QtDeclarative/qdeclarative.h>
#include <QAbstractListModel>
#include <QtCore>

QTM_BEGIN_NAMESPACE

class QDeclarativeLandmarkCategory : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QUrl iconSource READ iconSource WRITE setIconSource NOTIFY iconSourceChanged)

public:
    explicit QDeclarativeLandmarkCategory(QObject* parent = 0);

    QString name();
    void setName(const QString& name);

    QUrl iconSource();
    void setIconSource(const QUrl& iconSource);

    QLandmarkCategory category() const;

signals:
    void nameChanged();
    void iconSourceChanged();

private:

    QLandmarkCategory m_category;
    friend class QDeclarativeLandmarkCategoryModel;
    void setCategory(const QLandmarkCategory& category);
};

QTM_END_NAMESPACE
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLandmarkCategory));

#endif // QDECLARATIVELANDMARKCATEGORY_P_H
