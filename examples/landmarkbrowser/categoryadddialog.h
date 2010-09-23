#ifndef CATEGORYADDDIALOG_H
#define CATEGORYADDDIALOG_H

#include <QWidget>

#include <qlandmarkcategory.h>

#include "ui_categoryadddialog.h"

QTM_USE_NAMESPACE

class CategoryAddDialog : public QDialog, public Ui_CategoryAddDialog
{
Q_OBJECT
public:
    CategoryAddDialog(QWidget *parent, Qt::WindowFlags flags =0);
    ~CategoryAddDialog();

    QLandmarkCategory category();
public slots:
    virtual void accept();
    virtual void reject();
private:
    QLandmarkCategory m_category;
};

#endif
