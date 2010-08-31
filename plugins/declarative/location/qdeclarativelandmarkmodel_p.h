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


class QDeclarativeLandmarkAbstractModel : public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged)
    Q_PROPERTY(int offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(bool autoUpdate READ autoUpdate WRITE setAutoUpdate NOTIFY autoUpdateChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_INTERFACES(QDeclarativeParserStatus)

public:
    explicit QDeclarativeLandmarkAbstractModel(QObject* parent = 0);
    ~QDeclarativeLandmarkAbstractModel();

    // From QDeclarativeParserStatus
    void classBegin() {}
    void componentComplete();

    // From QAbstractListModel
    virtual int rowCount(const QModelIndex &parent) const = 0;
    virtual QVariant data(const QModelIndex &index, int role) const = 0;

    bool autoUpdate() const;
    void setAutoUpdate(bool autoUpdate);
    QString error() const;
    int limit();
    void setLimit(int limit);
    int offset();
    void setOffset(int offset);

    void scheduleUpdate();
    void setDbFileName(QString fileName); //  testing purposes only

signals:
    void errorChanged();
    void autoUpdateChanged();
    void limitChanged();
    void offsetChanged();

public slots:
    void update();

protected:
    QLandmarkManager* m_manager;
    bool m_componentCompleted;
    bool m_updatePending;
    bool m_autoUpdate;
    QString m_error;
    QString m_dbFileName;
    int m_limit;
    int m_offset;

};

class QDeclarativeLandmarkModel: public QDeclarativeLandmarkAbstractModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QDeclarativeLandmarkFilterBase* filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(SortKey sortBy READ sortBy WRITE setSortBy NOTIFY sortByChanged)
    Q_PROPERTY(SortOrder sortOrder READ sortOrder WRITE setSortOrder NOTIFY sortOrderChanged)
    Q_ENUMS(SortOrder)
    Q_ENUMS(SortKey)

public:
    explicit QDeclarativeLandmarkModel(QObject* parent = 0);
    ~QDeclarativeLandmarkModel();

    // From QAbstractListModel
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

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
    QDeclarativeLandmarkFilterBase* filter();
    void setFilter(QDeclarativeLandmarkFilterBase* filter);
    Q_INVOKABLE QList<QDeclarativeLandmark*> landmarks() const;

signals:
    void countChanged();
    void filterChanged();
    void sortByChanged();
    void sortOrderChanged();

private slots:
    void cancelUpdate();
    void fetchRequestStateChanged(QLandmarkAbstractRequest::State);

private:
    Q_INVOKABLE void startUpdate();
    void convertLandmarksToDeclarative();
    void setFetchRange();
    void setFetchOrder();

private:
    QDeclarativeLandmarkFilterBase* m_filter;
    QLandmarkFetchRequest* m_fetchRequest;
    QLandmarkSortOrder* m_sortingOrder;
    // Landmark list received from platform
    QList<QLandmark> m_landmarks;
    // Same landmark list, but as declarative classes
    QMap<QString, QDeclarativeLandmark*> m_landmarkMap;
    SortOrder m_sortOrder;
    SortKey m_sortKey;
};

QTM_END_NAMESPACE
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLandmarkModel));

#endif // QDECLARATIVELANDMARKMODEL_P_H
