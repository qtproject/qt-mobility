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
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QUrl iconUrl READ iconUrl WRITE setIconUrl NOTIFY iconUrlChanged)

public:
    explicit QDeclarativeLandmarkCategory(QObject* parent = 0);

    QString name();
    void setName(const QString& name);

    QString description();
    void setDescription(const QString& description);

    QUrl iconUrl();
    void setIconUrl(const QUrl& iconUrl);

signals:

    void nameChanged(QString name);
    void descriptionChanged(QString description);
    void iconUrlChanged(QUrl iconUrl);

public slots:

private:

    QLandmarkCategory m_category;
    friend class QDeclarativeLandmarkCategorySource;
    void setCategory(const QLandmarkCategory& category);

};

QTM_END_NAMESPACE
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLandmarkCategory));

#endif // QDECLARATIVELANDMARKCATEGORY_P_H
