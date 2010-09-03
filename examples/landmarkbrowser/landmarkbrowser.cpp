#include <QDebug>
#include <qlandmarkfilter.h>
#include <qlandmark.h>
#include <QModelIndex>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QProgressDialog>
#include <QProgressBar>

#include "landmarkbrowser.h"
#include "landmarkadddialog.cpp"

QTM_USE_NAMESPACE

LandmarkBrowser::LandmarkBrowser(QWidget *parent, Qt::WindowFlags flags)
    :currentOffset(0),
     limit(5)
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

    prevButton->setEnabled(false);
    progress = new QProgressDialog (tr("Please wait..."),tr("Cancel"),0,0, this);
    progress->setWindowTitle(tr("Loading Landmarks"));
    QObject::connect(progress,SIGNAL(canceled()), this, SLOT(cancel()));
    landmarkFetch->setLimit(limit);
    landmarkFetch->setOffset(currentOffset);
    landmarkFetch->start();
    progress->show();
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
    QString fileName = QFileDialog::getOpenFileName(this,tr("Import File"),".",tr("Landmark files (*.gpx *.lmx)"));
    if (!fileName.isEmpty()) {
        landmarkImport->setFileName(fileName);
        landmarkImport->start();
        progress->setWindowTitle(tr("Importing Landmarks"));
        progress->show();
    }
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
    updateTable(manager->landmarks(QLandmarkFilter(), deleteIds.count(), currentOffset+table->rowCount()));
    updateRowLabels();
    if (table->rowCount() < limit)
        nextButton->setEnabled(false);
}

void LandmarkBrowser::on_addLandmark_clicked()
{
    LandmarkAddDialog addDialog(this);
    if (!addDialog.exec()) {
        return;
    }

    QLandmark lm = addDialog.landmark();
    manager->saveLandmark(&lm);

    if (table->rowCount() == limit)
        nextButton->setEnabled(true);

    if (table->rowCount() < limit) {
        table->insertRow(table->rowCount());
        table->setItem(table->rowCount()-1,0,new QTableWidgetItem(QString::number(lm.coordinate().latitude(),'f',2)));
        table->setItem(table->rowCount()-1,1,new QTableWidgetItem(QString::number(lm.coordinate().longitude(),'f',2)));
        table->setItem(table->rowCount()-1,2,new QTableWidgetItem(lm.name()));
        table->setItem(table->rowCount()-1,3,new QTableWidgetItem(lm.landmarkId().localId()));
        updateRowLabels();
    }
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
                    if (request->error() == QLandmarkManager::NoError) {
                        landmarkFetch->setOffset(currentOffset);
                        landmarkFetch->start();
                    } else if (request->error() == QLandmarkManager::CancelError) {
                            // do nothing
                    } else {
                        QMessageBox::warning(this,"Warning", "Import Failed", QMessageBox::Ok, QMessageBox::NoButton);
                        progress->hide();
                    }
                break;
            }
            case QLandmarkAbstractRequest::LandmarkFetchRequest: {
                    if (landmarkFetch->error() == QLandmarkManager::NoError) {
                        if (currentOffset < limit)
                            prevButton->setEnabled(false);
                        else
                            prevButton->setEnabled(true);

                        if (landmarkFetch->landmarks().count() < limit)
                            nextButton->setEnabled(false);
                        else
                            nextButton->setEnabled(true);

                        table->setUpdatesEnabled(false);
                        for(int i = table->rowCount() -1;  i >= 0; --i) {
                            if (i %20 == 0)
                                qApp->processEvents();
                            table->removeRow(i);
                        }

                        QList<QLandmark> lms;
                        lms = landmarkFetch->landmarks();
                        updateTable(lms);

                        table->setUpdatesEnabled(true);
                    }  else {
                        QMessageBox::warning(this,"Warning", "Fetch Failed", QMessageBox::Ok, QMessageBox::NoButton);
                    }
                    updateRowLabels();
                    progress->hide();
                break;
            }
        }
    }
}

void LandmarkBrowser::cancel()
{
    landmarkImport->cancel();
}

void LandmarkBrowser::updateRowLabels()
{
    QStringList labels;
    for (int i=0; i < table->rowCount(); ++i)
        labels << QString::number(currentOffset + i);
    table->setVerticalHeaderLabels(labels);
}

void LandmarkBrowser::on_nextButton_clicked()
{
    currentOffset = currentOffset + limit;
    landmarkFetch->setOffset(currentOffset);
    landmarkFetch->start();
    progress->show();
}

void LandmarkBrowser::on_prevButton_clicked()
{
    currentOffset = currentOffset - limit;
    if (currentOffset < 0)
        currentOffset = 0;
    landmarkFetch->setOffset(currentOffset);
    landmarkFetch->start();
    progress->show();
}

void LandmarkBrowser::updateTable(const QList<QLandmark> &lms)
{
    QLandmark lm;
    for ( int i =0; i < lms.count(); ++i) {
        lm = lms.at(i);
        table->insertRow(table->rowCount());
        table->setItem(table->rowCount()-1,0,new QTableWidgetItem(QString::number(lm.coordinate().latitude(),'f',2)));
        table->setItem(table->rowCount()-1,1,new QTableWidgetItem(QString::number(lm.coordinate().longitude(),'f',2)));
        table->setItem(table->rowCount()-1,2,new QTableWidgetItem(lm.name()));
        table->setItem(table->rowCount()-1,3,new QTableWidgetItem(lm.landmarkId().localId()));

        if (i %20)
            qApp->processEvents();
    }
}
