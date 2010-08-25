#ifndef QDECLARATIVELANDMARKCATEGORYMODEL_P_H
#define QDECLARATIVELANDMARKCATEGORYMODEL_P_H

#include <qdeclarativelandmark_p.h>
#include <qdeclarativelandmarkcategory_p.h>
#include <qdeclarativelandmarkfilters_p.h>

#include <qlandmarkcategoryfetchrequest.h>
#include <qlandmarkcategory.h>
#include <qlandmarkmanager.h>

#include <QtDeclarative/qdeclarative.h>
#include <QAbstractListModel>

QTM_BEGIN_NAMESPACE

class QDeclarativeLandmarkCategoryModel: public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_PROPERTY(bool autoUpdate READ autoUpdate WRITE setAutoUpdate NOTIFY autoUpdateChanged)
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged)
    Q_PROPERTY(int offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    explicit QDeclarativeLandmarkCategoryModel(QObject* parent = 0);
    ~QDeclarativeLandmarkCategoryModel();

    // From QAbstractListModel
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    // Roles for exposing data via model
    enum {
        NameRole = Qt::UserRole + 500, // Check
        DescriptionRole,
        IconSourceRole,
    };
    int count();
    int limit();
    void setLimit(int limit);
    int offset();
    void setOffset(int offset);
    QString error();
    void setAutoUpdate(bool autoUpdate);
    bool autoUpdate() const;

signals:
    void errorChanged(QString error);
    void autoUpdateChanged(bool autoUpdate);
    void limitChanged(int limit);
    void offsetChanged(int offset);
    void countChanged(int count);

private slots:
    void update();
    void cancelUpdate();
    void fetchRequestStateChanged(QLandmarkAbstractRequest::State);

private:
    void convertCategoriesToDeclarative();
    void setFetchHints();

private:
    QLandmarkManager* m_manager;
    QLandmarkCategoryFetchRequest* m_fetchRequest;

    // Landmark list received from platform
    QList<QLandmarkCategory> m_categories;
    // Same category list, but as declarative classes
    QMap<QString, QDeclarativeLandmarkCategory*> m_categoryMap;
    QString m_error;
    bool m_autoUpdate;
    int m_limit;
    int m_offset;
};

QTM_END_NAMESPACE
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLandmarkCategoryModel));

#endif // QDECLARATIVELANDMARKCATEGORYMODEL_P_H
