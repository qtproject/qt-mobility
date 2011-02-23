#ifndef STATSWIDGET_H
#define STATSWIDGET_H

#include <QWidget>
#include <QHash>

class QLabel;
class QFormLayout;

class StatsWidget : public QWidget
{
    Q_OBJECT
private:
    QFormLayout * m_layout;

public:
    explicit StatsWidget(QWidget *parent = 0);

    QHash<QString, QLabel*> statMap;
    void stat(const QString & key, const QVariant & value);

};

#endif // STATSWIDGET_H
