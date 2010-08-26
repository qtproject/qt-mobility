#ifndef QDECLARATIVELANDMARKMODEL_P_H
#define QDECLARATIVELANDMARKMODEL_P_H

#include <qdeclarativelandmark_p.h>
#include <qdeclarativelandmarkfilters_p.h>

#include <qlandmark.h>
#include <qlandmarkmanager.h>
#include <qlandmarksortorder.h>
#include <qlandmarkfetchrequest.h>

#include <QtDeclarative/qdeclarative.h>
#include <QDeclarativeParserStatus>
#include <QAbstractListModel>

QTM_BEGIN_NAMESPACE

class QDeclarativeLandmarkModel: public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT

    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_PROPERTY(bool autoUpdate READ autoUpdate WRITE setAutoUpdate NOTIFY autoUpdateChanged)
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged)
    Q_PROPERTY(int offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QDeclarativeLandmarkFilterBase* filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(SortKey sortBy READ sortBy WRITE setSortBy NOTIFY sortByChanged)
    Q_PROPERTY(SortOrder sortOrder READ sortOrder WRITE setSortOrder NOTIFY sortOrderChanged)
    // dbFileName is _not_ official public property, but used in testing
    Q_PROPERTY(QString dbFileName READ dbFileName WRITE setDbFileName NOTIFY dbFileNameChanged)
    Q_ENUMS(SortOrder)
    Q_ENUMS(SortKey)
    Q_INTERFACES(QDeclarativeParserStatus)

public:
    explicit QDeclarativeLandmarkModel(QObject* parent = 0);
    ~QDeclarativeLandmarkModel();

    // From QAbstractListModel
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    // From QDeclarativeParserStatus
    void classBegin() {}
    void componentComplete();

    // Roles for exposing data via model
    enum Roles {
        NameRole = Qt::UserRole + 500, // Check
        PhoneNumberRole,
        DescriptionRole,
        RadiusRole,
        IconSourceRole,
        UrlRole,
        LatitudeRole,
        LongitudeRole
    };
    enum SortOrder {
        AscendingOrder = Qt::AscendingOrder,
        DescendingOrder = Qt::DescendingOrder
    };
    enum SortKey {
        DefaultSort = QLandmarkSortOrder::DefaultSort,
        NameSort = QLandmarkSortOrder::NameSort
    };

    SortKey sortBy() const;
    void setSortBy(SortKey key);
    SortOrder sortOrder() const;
    void setSortOrder(SortOrder order);
    int count();
    int limit();
    void setLimit(int limit);
    int offset();
    void setOffset(int offset);
    QString error() const;
    QString dbFileName() const;
    void setDbFileName(QString fileName);
    void setAutoUpdate(bool autoUpdate);
    bool autoUpdate() const;
    QDeclarativeLandmarkFilterBase* filter();
    void setFilter(QDeclarativeLandmarkFilterBase* filter);

signals:
    void errorChanged(QString error);
    void autoUpdateChanged();
    void limitChanged();
    void offsetChanged();
    void countChanged();
    void filterChanged();
    void sortByChanged();
    void sortOrderChanged();
    void dbFileNameChanged();

private slots:
    void update();
    void cancelUpdate();
    void scheduleUpdate();
    void fetchRequestStateChanged(QLandmarkAbstractRequest::State);

private:
    Q_INVOKABLE void startUpdate();
    void convertLandmarksToDeclarative();
    void setFetchRange();
    void setFetchOrder();

private:
    QLandmarkManager* m_manager;
    QDeclarativeLandmarkFilterBase* m_filter;
    QLandmarkFetchRequest* m_fetchRequest;
    QLandmarkSortOrder* m_sortingOrder;
    // Landmark list received from platform
    QList<QLandmark> m_landmarks;
    // Same landmark list, but as declarative classes
    QMap<QString, QDeclarativeLandmark*> m_landmarkMap;
    SortOrder m_sortOrder;
    SortKey m_sortKey;
    QString m_error;
    QString m_dbFileName;
bool m_componentCompleted : 1;
bool m_updatePending : 1;
bool m_autoUpdate : 1;
    int m_limit;
    int m_offset;
};

QTM_END_NAMESPACE
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLandmarkModel));

#endif // QDECLARATIVELANDMARKMODEL_P_H
