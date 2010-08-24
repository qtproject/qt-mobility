#ifndef QDECLARATIVELANDMARKMODEL_P_H
#define QDECLARATIVELANDMARKMODEL_P_H

#include <qdeclarativelandmark_p.h>
#include <qdeclarativelandmarkfilters_p.h>

#include <qlandmark.h>
#include <qlandmarkmanager.h>
#include <qlandmarkfetchrequest.h>

#include <QtDeclarative/qdeclarative.h>
#include <QAbstractListModel>

QTM_BEGIN_NAMESPACE

class QDeclarativeLandmarkModel: public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_PROPERTY(bool autoUpdate READ autoUpdate WRITE setAutoUpdate NOTIFY autoUpdateChanged)
    Q_PROPERTY(QObject* nameFilter READ nameFilter WRITE setFilter NOTIFY nameFilterChanged)
    Q_PROPERTY(QObject* proximityFilter READ proximityFilter WRITE setFilter NOTIFY proximityFilterChanged)
    Q_PROPERTY(int landmarksPerUpdate READ landmarksPerUpdate WRITE setLandmarksPerUpdate NOTIFY landmarksPerUpdateChanged)
    Q_PROPERTY(int landmarksOffset READ landmarksOffset WRITE setLandmarksOffset NOTIFY landmarksOffsetChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    explicit QDeclarativeLandmarkModel(QObject* parent = 0);
    ~QDeclarativeLandmarkModel();

    // From QAbstractListModel
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    // Roles for exposing data via model
    enum {
        NameRole = Qt::UserRole + 500, // Check
        PhoneNumberRole,
        DescriptionRole,
        RadiusRole,
        IconSourceRole,
        UrlRole,
        LatitudeRole,
        LongitudeRole
    };

    int count();
    int landmarksPerUpdate();
    void setLandmarksPerUpdate(int landmarksPerUpdate);
    int landmarksOffset();
    void setLandmarksOffset(int landmarksOffset);
    QString error();
    void setAutoUpdate(bool autoUpdate);
    bool autoUpdate() const;
    QObject* nameFilter();
    QObject* proximityFilter();
    void setFilter(QObject* filter);

signals:
    void errorChanged(QString error);
    void autoUpdateChanged(bool autoUpdate);
    void nameFilterChanged(QObject* nameFilter);
    void proximityFilterChanged(QObject* nameFilter);
    void landmarksPerUpdateChanged(int landmarksPerUpdate);
    void landmarksOffsetChanged(int landmarksOffset);
    void countChanged(int count);

private slots:
    void update();
    void cancelUpdate();
    void fetchRequestStateChanged(QLandmarkAbstractRequest::State);

private:
    void convertLandmarksToDeclarative();
    void setFetchHints();

private:
    QLandmarkManager* m_manager;
    QDeclarativeLandmarkNameFilter* m_nameFilter;
    QDeclarativeLandmarkProximityFilter* m_proximityFilter;
    QLandmarkFetchRequest* m_fetchRequest;
    // Landmark list received from platform
    QList<QLandmark> m_landmarks;
    // Same landmark list, but as declarative classes
    QMap<QString, QDeclarativeLandmark*> m_landmarkMap;
    QString m_error;
    bool m_autoUpdate;
    int m_landmarksPerUpdate;
    int m_landmarksOffset;
};

QTM_END_NAMESPACE
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLandmarkModel));

#endif // QDECLARATIVELANDMARKMODEL_P_H
