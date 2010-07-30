#ifndef LANDMARKADDDIALOG_H
#define LANDMARKADDDIALOG_H

#include <QWidget>

#include <qlandmark.h>

#include "ui_landmarkadddialog.h"

QTM_USE_NAMESPACE

class LandmarkAddDialog : public QDialog, public Ui_LandmarkAddDialog
{
Q_OBJECT
public:
    LandmarkAddDialog(QWidget *parent, Qt::WindowFlags flags =0);
    ~LandmarkAddDialog();

    QLandmark landmark();
public slots:
    virtual void accept();
    virtual void reject();
private:
    QLandmark lm;
};

#endif // LANDMARKADDDIALOG_H
