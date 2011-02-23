#include "statswidget.h"

#include <QLabel>
#include <QFormLayout>
#include <QVariant>

StatsWidget::StatsWidget(QWidget *parent) :
    QWidget(parent)
{
    m_layout = new QFormLayout;
    m_layout->setMargin(0);
    m_layout->setVerticalSpacing(0);

    setLayout(m_layout);
}

void StatsWidget::stat(const QString & key, const QVariant & value)
{
    QHash<QString,QLabel*>::iterator it = statMap.find(key);
    if (it == statMap.end()) {
        QLabel * label = new QLabel;
        m_layout->addRow(key+":", label);
        it = statMap.insert(key, label);
    }

    it.value()->setText(value.toString());
}
