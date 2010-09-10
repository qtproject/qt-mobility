#ifndef QDECLARATIVELANDMARKCATEGORYMODEL_P_H
#define QDECLARATIVELANDMARKCATEGORYMODEL_P_H

#include <qdeclarativelandmark_p.h>
#include <qdeclarativelandmarkmodel_p.h>
#include <qdeclarativelandmarkcategory_p.h>
#include <qdeclarativelandmarkfilters_p.h>

#include <qlandmarkcategory.h>
#include <qlandmarkmanager.h>
#include <qlandmarkabstractrequest.h>

#include <QtDeclarative/qdeclarative.h>
#include <QAbstractListModel>

QTM_BEGIN_NAMESPACE

class QDeclarativeLandmarkCategoryModel: public QDeclarativeLandmarkAbstractModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QDeclarativeLandmark* landmark READ landmark WRITE setLandmark NOTIFY landmarkChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeLandmarkCategory> categories READ categories NOTIFY categoriesChanged)

public:
    explicit QDeclarativeLandmarkCategoryModel(QObject* parent = 0);
    virtual ~QDeclarativeLandmarkCategoryModel();

    // From QAbstractListModel
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    // Roles for exposing data via model
    enum {
        CategoryRole = Qt::UserRole + 500
    };
    QDeclarativeLandmark* landmark() const;
    void setLandmark(QDeclarativeLandmark* landmark);
    int count() const;

    // For testing purpose to access ordered data
    QList<QLandmarkCategory> categoryList();
    QDeclarativeListProperty<QDeclarativeLandmarkCategory> categories();
    static void categories_append(QDeclarativeListProperty<QDeclarativeLandmarkCategory>* prop, QDeclarativeLandmarkCategory* category);
    static int categories_count(QDeclarativeListProperty<QDeclarativeLandmarkCategory>* prop);
    static QDeclarativeLandmarkCategory* categories_at(QDeclarativeListProperty<QDeclarativeLandmarkCategory>* prop, int index);
    static void categories_clear(QDeclarativeListProperty<QDeclarativeLandmarkCategory>* prop);

signals:
    void countChanged();
    void landmarkChanged();
    void categoriesChanged();

private slots:
    void cancelUpdate();
    void fetchRequestStateChanged(QLandmarkAbstractRequest::State state);

private:
    Q_INVOKABLE void startUpdate();
    void convertCategoriesToDeclarative();
    void setFetchRange();
    void setFetchFilters();
    void setFetchOrder();

private:
    QLandmarkAbstractRequest* m_fetchRequest;
    // Landmark list received from platform
    QList<QLandmarkCategory> m_categories;
    // Same category list, but as declarative classes
    QMap<QString, QDeclarativeLandmarkCategory*> m_categoryMap;
    QDeclarativeLandmark* m_landmark;
};

QTM_END_NAMESPACE
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLandmarkCategoryModel));

#endif // QDECLARATIVELANDMARKCATEGORYMODEL_P_H
