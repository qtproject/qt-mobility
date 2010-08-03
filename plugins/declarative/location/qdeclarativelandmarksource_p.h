#ifndef QDECLARATIVELANDMARKSOURCE_P_H
#define QDECLARATIVELANDMARKSOURCE_P_H

#include <qdeclarativelandmark_p.h>

#include <qlandmark.h>
#include <qlandmarkmanager.h>
#include <qlandmarkfetchrequest.h>

#include <QtDeclarative/qdeclarative.h>
#include <QAbstractListModel>

QTM_BEGIN_NAMESPACE

class QDeclarativeLandmarkSource: public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)

public:
    explicit QDeclarativeLandmarkSource(QObject* parent = 0);
    ~QDeclarativeLandmarkSource();

    // From QAbstractListModel
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    // Roles for exposing data via model
    enum {
        NameRole = Qt::UserRole + 500, // Check
        PhoneRole,
        DescriptionRole,
        RadiusRole,
        IconUrlRole,
        UrlRole
    };

    QString name();
    QString error();

    void setActive(bool active);
    bool isActive() const;

signals:

    void nameChanged(QString name);
    void errorChanged(QString error);
    void activeChanged(bool active);

public slots:

private slots:

    void update();
    void cancelUpdate();
    void fetchRequestStateChanged(QLandmarkAbstractRequest::State);

private:

    void convertLandmarksToDeclarative();

private:

    QLandmarkManager* m_manager;
    QLandmarkFetchRequest* m_fetchRequest;
    // Landmark list received from platform
    QList<QLandmark> m_landmarks;
    // Same landmark list, but as declarative classes
    QMap<QString, QDeclarativeLandmark*> m_landmarkMap;

    QString m_error;
    bool m_active;
};


QTM_END_NAMESPACE
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLandmarkSource));


#endif // QDECLARATIVELANDMARKSOURCE_P_H
