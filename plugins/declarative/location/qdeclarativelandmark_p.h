#ifndef QDECLARATIVELANDMARK_P_H
#define QDECLARATIVELANDMARK_P_H

#include <QtCore>
#include <QAbstractListModel>
#include <qlandmark.h>
#include <QtDeclarative/qdeclarative.h>

QTM_BEGIN_NAMESPACE

class QDeclarativeLandmark : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString phone READ phone WRITE setPhone NOTIFY phoneChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(double radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(QUrl iconUrl READ iconUrl WRITE setIconUrl NOTIFY iconUrlChanged)
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)

public:
    explicit QDeclarativeLandmark(QObject* parent = 0);

    QString name();
    void setName(const QString& name);

    QString phone();
    void setPhone(const QString& phone);

    QString description();
    void setDescription(const QString& description);

    double radius();
    void setRadius(const double& radius);

    QUrl iconUrl();
    void setIconUrl(const QUrl& iconUrl);

    QUrl url();
    void setUrl(const QUrl& url);

signals:

    void nameChanged(QString name);
    void phoneChanged(QString phone);
    void descriptionChanged(QString descriptionChanged);
    void radiusChanged(double radius);
    void iconUrlChanged(QUrl iconUrl);
    void urlChanged(QUrl url);

public slots:

private:

    QLandmark m_landmark;
};

QTM_END_NAMESPACE
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLandmark));

#endif // QDECLARATIVELANDMARK_P_H
