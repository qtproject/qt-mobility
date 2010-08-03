#ifndef QDECLARATIVELANDMARKSOURCE_P_H
#define QDECLARATIVELANDMARKSOURCE_P_H

#include <qdeclarativelandmark_p.h>
#include <qdeclarativelandmarkfilters_p.h>

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
    Q_PROPERTY(QObject* nameFilter READ nameFilter WRITE setNameFilter NOTIFY nameFilterChanged)
    //Q_PROPERTY(QObject* nameFilter READ nameFilter NOTIFY nameFilterChanged)
    //Q_PROPERTY(QObject* proximityFilter READ proximityFilter NOTIFY proximityFilterChanged)
    Q_PROPERTY(QObject* proximityFilter READ proximityFilter WRITE setProximityFilter NOTIFY proximityFilterChanged)

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
    QObject* nameFilter();
    QObject* proximityFilter();
    void setNameFilter(QObject* correctThisFunction) {}
    void setProximityFilter(QObject* correctThisFunction) {}

signals:
    void nameChanged(QString name);
    void errorChanged(QString error);
    void activeChanged(bool active);
    void nameFilterChanged(QObject* nameFilter);

private slots:
    void update();
    void cancelUpdate();
    void fetchRequestStateChanged(QLandmarkAbstractRequest::State);

private:
    void convertLandmarksToDeclarative();

private:
    QLandmarkManager* m_manager;
    QLandmarkFetchRequest* m_fetchRequest;
    QDeclarativeLandmarkNameFilter m_nameFilter;
    QDeclarativeLandmarkProximityFilter m_proximityFilter;
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
