#ifndef QDECLARATIVELANDMARK_P_H
#define QDECLARATIVELANDMARK_P_H

#include <QtCore>
#include <QAbstractListModel>
#include <qlandmark.h>
#include <QtDeclarative/qdeclarative.h>


// Define this to get qDebug messages
#define QDECLARATIVE_LANDMARK_DEBUG

#ifdef QDECLARATIVE_LANDMARK_DEBUG
#include <QDebug>
#endif

QTM_BEGIN_NAMESPACE

class QDeclarativeLandmark : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString phoneNumber READ phoneNumber WRITE setPhoneNumber NOTIFY phoneNumberChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(double radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(QUrl iconSource READ iconSource WRITE setIconSource NOTIFY iconSourceChanged)
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)

public:
    explicit QDeclarativeLandmark(QObject* parent = 0);

    QString name();
    void setName(const QString& name);

    QString phoneNumber();
    void setPhoneNumber(const QString& phoneNumber);

    QString description();
    void setDescription(const QString& description);

    double radius();
    void setRadius(const double& radius);

    QUrl iconSource();
    void setIconSource(const QUrl& iconSource);

    QUrl url();
    void setUrl(const QUrl& url);

signals:
    void nameChanged();
    void phoneNumberChanged();
    void descriptionChanged();
    void radiusChanged();
    void iconSourceChanged();
    void urlChanged();

private:
    friend class QDeclarativeLandmarkModel;
    void setLandmark(const QLandmark& landmark);

private:
    QLandmark m_landmark;
};

QTM_END_NAMESPACE
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLandmark));

#endif // QDECLARATIVELANDMARK_P_H
