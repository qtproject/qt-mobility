#include "landmarkbrowser.h"
#include <QDebug>
#include <qlandmarkfilter.h>
#include <qlandmark.h>
#include <QModelIndex>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QProgressDialog>
#include <QProgressBar>

QTM_USE_NAMESPACE

LandmarkBrowser::LandmarkBrowser(QWidget *parent, Qt::WindowFlags flags)
{
    setupUi(this);
    table->insertColumn(3);
    table->hideColumn(3);

    manager = new QLandmarkManager(this);

    landmarkFetch = new QLandmarkFetchRequest(manager, this);
    QObject::connect(landmarkFetch, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)),
                this,SLOT(fetchHandler(QLandmarkAbstractRequest::State)));

    landmarkImport = new QLandmarkImportRequest(manager, this);
    QObject::connect(landmarkImport, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)),
                this,SLOT(fetchHandler(QLandmarkAbstractRequest::State)));

    landmarkRemove = new QLandmarkRemoveRequest(manager, this);
    QObject::connect(landmarkRemove, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)),
                this,SLOT(fetchHandler(QLandmarkAbstractRequest::State)));

     landmarkSave = new QLandmarkSaveRequest(manager, this);
     QObject::connect(landmarkSave, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)),
                this,SLOT(fetchHandler(QLandmarkAbstractRequest::State)));

    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    table->setHorizontalHeaderItem(0, new QTableWidgetItem("Lat"));
    table->setHorizontalHeaderItem(1, new QTableWidgetItem("Long"));
    table->setHorizontalHeaderItem(2, new QTableWidgetItem("Name"));

    progress = new QProgressDialog (tr("Operation in progress..."),tr("Cancel"),0,0, this);
    progress->setCancelButton(0);
    progress->setWindowTitle("Please wait");
}

LandmarkBrowser::~LandmarkBrowser()
{
    delete landmarkFetch;
    delete landmarkImport;
    delete landmarkRemove;
    delete landmarkSave;
    delete progress;
    delete manager;
}

void LandmarkBrowser::on_importLandmarks_clicked()
{
    QFileDialog fileDialog(this, tr("Import File"), ".", tr("Landmark files (*.gpx)"));
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    QStringList selectedFiles;
    if (fileDialog.exec()) {
        selectedFiles = fileDialog.selectedFiles();
        if (selectedFiles.count() ==0 )
            return;
    } else {
        return;
    }

    landmarkImport->setFileName(selectedFiles.at(0));
    landmarkImport->setFormat("GpxV1.1");
    landmarkImport->start();

    progress->show();
}

void LandmarkBrowser::on_fetchLandmarks_clicked()
{
    landmarkFetch->start();
    progress->show();
}

void LandmarkBrowser::on_deleteLandmarks_clicked()
{
    QItemSelectionModel *selection  = table->selectionModel();
    QModelIndexList selectedIndexes = selection->selectedRows();

    QList<QLandmarkId> deleteIds;

    QLandmarkId id;
    QModelIndex index;
    while(selectedIndexes.count() > 0) {
        index = selectedIndexes.takeLast();
        id.setManagerUri(manager->managerUri());
        id.setLocalId(table->item(index.row(),3)->text());
        deleteIds.append(id);
        table->removeRow(index.row());
        selectedIndexes = table->selectionModel()->selectedRows();
    }

    if (deleteIds.count() == 0)
        return;

    manager->removeLandmarks(deleteIds);
}

void LandmarkBrowser::on_addLandmark_clicked()
{
    QLandmark lm;
    double latitude = latitudeLineEdit->text().toDouble();
    double longitude = longitudeLineEdit->text().toDouble();
    lm.setCoordinate(QGeoCoordinate(latitude, longitude));
    lm.setName(nameLineEdit->text());
    manager->saveLandmark(&lm);
    table->insertRow(table->rowCount());
    table->setItem(table->rowCount()-1,0,new QTableWidgetItem(QString::number(lm.coordinate().latitude(),'f',10)));
    table->setItem(table->rowCount()-1,1,new QTableWidgetItem(QString::number(lm.coordinate().longitude(),'f',10)));
    table->setItem(table->rowCount()-1,2,new QTableWidgetItem(lm.name()));
    table->setItem(table->rowCount()-1,3,new QTableWidgetItem(lm.landmarkId().localId()));
}


void LandmarkBrowser::fetchHandler(QLandmarkAbstractRequest::State state)
{
    if (state == QLandmarkAbstractRequest::FinishedState)
    {
        QLandmarkAbstractRequest *request = qobject_cast<QLandmarkAbstractRequest*> (sender());
        if (!request)
            return;
        switch (request->type()) {
            case QLandmarkAbstractRequest::ImportRequest : {
                    for(int i = table->rowCount() -1;  i >= 0; --i) {
                        if (i %20 == 0)
                            qApp->processEvents();
                        table->removeRow(i);
                    }

                    if (request->error() == QLandmarkManager::NoError) {
                        QList<QLandmark> lms = manager->landmarks(QLandmarkFilter());
                        QLandmark lm;
                        for (int i=0;i < lms.count(); ++i){
                            lm = lms.at(i);
                            table->insertRow(table->rowCount());
                            table->setItem(table->rowCount()-1,0,new QTableWidgetItem(QString::number(lm.coordinate().latitude(),'f',10)));
                            table->setItem(table->rowCount()-1,1,new QTableWidgetItem(QString::number(lm.coordinate().longitude(),'f',10)));
                            table->setItem(table->rowCount()-1,2,new QTableWidgetItem(lm.name()));
                            table->setItem(table->rowCount()-1,3,new QTableWidgetItem(lm.landmarkId().localId()));

                            if (i %20 == 0)
                                qApp->processEvents();
                        }
                    } else {
                        QMessageBox::warning(this,"Warning", "Import Failed", QMessageBox::Ok, QMessageBox::NoButton);
                    }
                    progress->hide();
                break;
            }
            case QLandmarkAbstractRequest::LandmarkFetchRequest: {
                    for(int i = table->rowCount() -1;  i >= 0; --i) {
                        if (i %20 == 0)
                            qApp->processEvents();
                        table->removeRow(i);
                    }

                    if (landmarkFetch->error() == QLandmarkManager::NoError) {
                        QList<QLandmark> lms = landmarkFetch->landmarks();
                        if (lms.count() == 0) {
                            QMessageBox::information(this,"Information", "No landmarks found", QMessageBox::Ok, QMessageBox::NoButton);
                            progress->hide();
                            return;
                        }

                        QLandmark lm;
                        for ( int i =0; i < lms.count(); ++i) {
                            lm = lms.at(i);
                            table->insertRow(table->rowCount());
                            table->setItem(table->rowCount()-1,0,new QTableWidgetItem(QString::number(lm.coordinate().latitude(),'f',10)));
                            table->setItem(table->rowCount()-1,1,new QTableWidgetItem(QString::number(lm.coordinate().longitude(),'f',10)));
                            table->setItem(table->rowCount()-1,2,new QTableWidgetItem(lm.name()));
                            table->setItem(table->rowCount()-1,3,new QTableWidgetItem(lm.landmarkId().localId()));

                            if (i %20)
                                qApp->processEvents();
                        }
                    }  else {
                        QMessageBox::warning(this,"Warning", "Fetch Failed", QMessageBox::Ok, QMessageBox::NoButton);
                    }
                    progress->hide();
                break;
            }
        }
    }
}
