#ifndef QDECLARATIVELANDMARKCATEGORYSOURCE_P_H
#define QDECLARATIVELANDMARKCATEGORYSOURCE_P_H

#include <qdeclarativelandmark_p.h>
#include <qdeclarativelandmarkcategory_p.h>
#include <qdeclarativelandmarkfilters_p.h>

#include <qlandmarkcategoryfetchrequest.h>
#include <qlandmarkcategory.h>
#include <qlandmarkmanager.h>

#include <QtDeclarative/qdeclarative.h>
#include <QAbstractListModel>

QTM_BEGIN_NAMESPACE

class QDeclarativeLandmarkCategorySource: public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)

public:
    explicit QDeclarativeLandmarkCategorySource(QObject* parent = 0);
    ~QDeclarativeLandmarkCategorySource();

    // From QAbstractListModel
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    // Roles for exposing data via model
    enum {
        NameRole = Qt::UserRole + 500, // Check
        DescriptionRole,
        IconUrlRole,
    };
    QString name();
    QString error();
    void setActive(bool active);
    bool isActive() const;

signals:
    void nameChanged(QString name);
    void errorChanged(QString error);
    void activeChanged(bool active);

private slots:
    void update();
    void cancelUpdate();
    void fetchRequestStateChanged(QLandmarkAbstractRequest::State);

private:
    void convertCategoriesToDeclarative();

private:
    QLandmarkManager* m_manager;
    QLandmarkCategoryFetchRequest* m_fetchRequest;

    // Landmark list received from platform
    QList<QLandmarkCategory> m_categories;
    // Same category list, but as declarative classes
    QMap<QString, QDeclarativeLandmarkCategory*> m_categoryMap;
    QString m_error;
    bool m_active;
};

QTM_END_NAMESPACE
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLandmarkCategorySource));

#endif // QDECLARATIVELANDMARKCATEGORYSOURCE_P_H
