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
#include "categoryadddialog.h"

QTM_USE_NAMESPACE

LandmarkBrowser::LandmarkBrowser(QWidget *parent, Qt::WindowFlags flags)
    :currentLandmarkOffset(0),
     currentCategoryOffset(0),
     limit(20)
{
    setupUi(this);
    table->insertColumn(3);
    table->hideColumn(3);

    categoryTable->removeColumn(2);
    categoryTable->hideColumn(1);
    categoryTable->setHorizontalHeaderItem(0, new QTableWidgetItem("Name"));

    manager = new QLandmarkManager(this);

    landmarkFetch = new QLandmarkFetchRequest(manager, this);
    QObject::connect(landmarkFetch, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)),
                this,SLOT(fetchHandler(QLandmarkAbstractRequest::State)));

    landmarkImport = new QLandmarkImportRequest(manager, this);
    QObject::connect(landmarkImport, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)),
                this,SLOT(fetchHandler(QLandmarkAbstractRequest::State)));

    landmarkExport = new QLandmarkExportRequest(manager, this);
    QObject::connect(landmarkExport, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)),
                this,SLOT(fetchHandler(QLandmarkAbstractRequest::State)));

    landmarkRemove = new QLandmarkRemoveRequest(manager, this);
    QObject::connect(landmarkRemove, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)),
                this,SLOT(fetchHandler(QLandmarkAbstractRequest::State)));

     landmarkSave = new QLandmarkSaveRequest(manager, this);
     QObject::connect(landmarkSave, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)),
                this,SLOT(fetchHandler(QLandmarkAbstractRequest::State)));

     categoryFetch = new QLandmarkCategoryFetchRequest(manager, this);
     QObject::connect(categoryFetch, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)),
                      this,SLOT(fetchHandler(QLandmarkAbstractRequest::State)));

    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    categoryTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    table->setHorizontalHeaderItem(0, new QTableWidgetItem("Lat"));
    table->setHorizontalHeaderItem(1, new QTableWidgetItem("Long"));
    table->setHorizontalHeaderItem(2, new QTableWidgetItem("Name"));

    prevLandmarkButton->setEnabled(false);
    progress = new QProgressDialog (tr("Please wait..."),tr("Cancel"),0,0, this);
    progress->setWindowTitle(tr("Loading Landmarks"));
    QObject::connect(progress,SIGNAL(canceled()), this, SLOT(cancel()));
    landmarkFetch->setLimit(limit);
    landmarkFetch->setOffset(currentLandmarkOffset);
    landmarkFetch->start();

    categoryFetch->setLimit(limit);
    categoryFetch->setOffset(currentCategoryOffset);
    categoryFetch->start();

    progress->show();

    #ifdef Q_OS_SYMBIAN
        gpxRadioButton->setEnabled(false);
        gpxRadioButton->setVisible(false);
    #endif
}

LandmarkBrowser::~LandmarkBrowser()
{
    delete landmarkFetch;
    landmarkFetch =0;
    delete landmarkImport;
    landmarkImport =0;
    delete landmarkExport;
    landmarkExport =0;
    delete landmarkRemove;
    landmarkRemove =0;
    delete landmarkSave;
    landmarkSave =0;
    delete categoryFetch;
    categoryFetch = 0;
    delete progress;
    progress =0;
    delete manager;
    manager=0;
}

void LandmarkBrowser::on_importLandmarks_clicked()
{
    QString fileFilterString;
    #ifdef Q_OS_SYMBIAN
        fileFilterString = tr("Landmark files (*.lmx *)");
    #else
        fileFilterString = tr("Landmark files (*.gpx *.lmx *)");
    #endif

    QString fileName = QFileDialog::getOpenFileName(this,tr("Import File"),".",fileFilterString);
    if (!fileName.isEmpty()) {
        landmarkImport->setFileName(fileName);
        landmarkImport->start();
        progress->setWindowTitle(tr("Importing Landmarks"));
        progress->show();
    }
}

void LandmarkBrowser::on_exportLandmarks_clicked()
{
    QString fileFilterString;
    #ifdef Q_OS_SYMBIAN
        fileFilterString = tr("Landmark files (*.lmx *)");
    #else
        fileFilterString = tr("Landmark files (*.gpx *.lmx *)");
    #endif

    QString fileName = QFileDialog::getSaveFileName(this,tr("Export File"),".",fileFilterString);
    if (!fileName.isEmpty()) {
        landmarkExport->setFileName(fileName);
        if (lmxRadioButton->isChecked())
            landmarkExport->setFormat(QLandmarkManager::Lmx);
        else
            landmarkExport->setFormat(QLandmarkManager::Gpx);
        landmarkExport->start();
        progress->setWindowTitle(tr("Exporting Landmarks"));
        progress->show();
    }
}

void LandmarkBrowser::on_deleteLandmarksButton_clicked()
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
    updateTable(manager->landmarks(QLandmarkFilter(), deleteIds.count(), currentLandmarkOffset+table->rowCount()));
    updateRowLabels();
    if (table->rowCount() < limit)
        nextLandmarkButton->setEnabled(false);
}

void LandmarkBrowser::on_deleteCategoriesButton_clicked()
{
    QItemSelectionModel *selection  = categoryTable->selectionModel();
    QModelIndexList selectedIndexes = selection->selectedRows();

    QList<QLandmarkCategoryId> deleteIds;

    QLandmarkCategoryId id;
    QModelIndex index;
    while(selectedIndexes.count() > 0) {
        index = selectedIndexes.takeLast();
        id.setManagerUri(manager->managerUri());
        id.setLocalId(categoryTable->item(index.row(),1)->text());

        deleteIds.append(id);
        categoryTable->removeRow(index.row());
        selectedIndexes = categoryTable->selectionModel()->selectedRows();
    }

    if (deleteIds.count() == 0)
        return;

    for (int i =0;i < deleteIds.count(); ++i)
        manager->removeCategory(deleteIds.at(i));
    updateCategoryTable(manager->categories(deleteIds.count(), currentCategoryOffset+categoryTable->rowCount()));
    updateCategoryRowLabels();
    if (categoryTable->rowCount() < limit)
        nextCategoryButton->setEnabled(false);
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
        nextLandmarkButton->setEnabled(true);

    if (table->rowCount() < limit) {
        table->insertRow(table->rowCount());
        table->setItem(table->rowCount()-1,0,new QTableWidgetItem(QString::number(lm.coordinate().latitude(),'f',2)));
        table->setItem(table->rowCount()-1,1,new QTableWidgetItem(QString::number(lm.coordinate().longitude(),'f',2)));
        table->setItem(table->rowCount()-1,2,new QTableWidgetItem(lm.name()));
        table->setItem(table->rowCount()-1,3,new QTableWidgetItem(lm.landmarkId().localId()));
        updateRowLabels();
    }
}

void LandmarkBrowser::on_addCategoryButton_clicked()
{
    CategoryAddDialog categoryAddDialog(this);
    if (!categoryAddDialog.exec()) {
        return;
    }
    QLandmarkCategory category = categoryAddDialog.category();
    manager->saveCategory(&category);
    if (manager->error() == QLandmarkManager::AlreadyExistsError) {
        QMessageBox::warning(this,"Warning", "Add Category Failed: category already Exists", QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    categoryFetch->setOffset(currentCategoryOffset);
    categoryFetch->start();
    progress->setWindowTitle(tr("Loading Categories"));
    progress->show();
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
                        landmarkFetch->setOffset(currentLandmarkOffset);
                        landmarkFetch->start();
                    } else if (request->error() == QLandmarkManager::CancelError) {
                            // do nothing
                    } else {
                        QMessageBox::warning(this,"Warning", "Import Failed", QMessageBox::Ok, QMessageBox::NoButton);
                        progress->hide();
                    }
                break;
            }
            case QLandmarkAbstractRequest::ExportRequest : {
                if (request->error() == QLandmarkManager::NoError) {
                    progress->hide();                   
                    QMessageBox::information(this,"Finished", "Export Successful", QMessageBox::Ok, QMessageBox::NoButton);

                } else if (request->error() == QLandmarkManager::CancelError) {
                    // do nothing
                } else {
                    QMessageBox::warning(this,"Warning", "Export Failed", QMessageBox::Ok, QMessageBox::NoButton);
                    progress->hide();
                }
                break;
            }
            case QLandmarkAbstractRequest::LandmarkFetchRequest: {
                    if (landmarkFetch->error() == QLandmarkManager::NoError) {
                        if (currentLandmarkOffset < limit)
                            prevLandmarkButton->setEnabled(false);
                        else
                            prevLandmarkButton->setEnabled(true);

                        if (landmarkFetch->landmarks().count() < limit)
                            nextLandmarkButton->setEnabled(false);
                        else
                            nextLandmarkButton->setEnabled(true);

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
        case QLandmarkAbstractRequest::CategoryFetchRequest: {
                if (categoryFetch->error() == QLandmarkManager::NoError) {
                    if (currentCategoryOffset < limit)
                        prevCategoryButton->setEnabled(false);
                    else
                        prevCategoryButton->setEnabled(true);

                    if (categoryFetch->categories().count() < limit)
                        nextCategoryButton->setEnabled(false);
                    else
                        nextCategoryButton->setEnabled(true);

                    categoryTable->setUpdatesEnabled(false);
                    for (int i = categoryTable->rowCount() -1; i>=0; --i) {
                        if (i %20 ==0)
                            qApp->processEvents();
                        categoryTable->removeRow(i);
                    }

                    QList<QLandmarkCategory> cats;
                    cats = categoryFetch->categories();
                    updateCategoryTable(cats);

                    categoryTable->setUpdatesEnabled(true);
                } else {
                    QMessageBox::warning(this, "Warning", "Category Fetch Failed", QMessageBox::Ok, QMessageBox::NoButton);
                }
                updateCategoryRowLabels();
                if (tabWidget->currentIndex() == 1)
                    progress->hide();
            }
        }
    }
}

void LandmarkBrowser::cancel()
{
    if (landmarkImport->isActive())
        landmarkImport->cancel();
    else if (landmarkExport->isActive())
        landmarkExport->cancel();
}

void LandmarkBrowser::updateRowLabels()
{
    QStringList labels;
    for (int i=0; i < table->rowCount(); ++i)
        labels << QString::number(currentLandmarkOffset + i);
    table->setVerticalHeaderLabels(labels);
}

void LandmarkBrowser::updateCategoryRowLabels()
{
    QStringList labels;
    for (int i=0; i < categoryTable->rowCount(); ++i)
        labels << QString::number(currentCategoryOffset + i);
    categoryTable->setVerticalHeaderLabels(labels);
}

void LandmarkBrowser::on_nextLandmarkButton_clicked()
{
    currentLandmarkOffset = currentLandmarkOffset + limit;
    landmarkFetch->setOffset(currentLandmarkOffset);
    landmarkFetch->start();
    progress->setWindowTitle(tr("Loading Landmarks"));
    progress->show();
}

void LandmarkBrowser::on_prevLandmarkButton_clicked()
{
    currentLandmarkOffset = currentLandmarkOffset - limit;
    if (currentLandmarkOffset < 0)
        currentLandmarkOffset = 0;
    landmarkFetch->setOffset(currentLandmarkOffset);
    landmarkFetch->start();
    progress->setWindowTitle(tr("Loading Landmarks"));
    progress->show();
}

void LandmarkBrowser::on_nextCategoryButton_clicked()
{
    currentCategoryOffset = currentCategoryOffset + limit;
    categoryFetch->setOffset(currentCategoryOffset);
    categoryFetch->start();
    progress->setWindowTitle(tr("Loading Categories"));
    progress->show();
}

void LandmarkBrowser::on_prevCategoryButton_clicked()
{
    currentCategoryOffset = currentCategoryOffset - limit;
    if (currentCategoryOffset < 0)
        currentCategoryOffset = 0;
    categoryFetch->setOffset(currentLandmarkOffset);
    categoryFetch->start();
    progress->setWindowTitle(tr("Loading Categories"));
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

void LandmarkBrowser::updateCategoryTable(const QList<QLandmarkCategory> &cats)
{
    QLandmarkCategory cat;
    for ( int i =0; i < cats.count(); ++i) {
        cat = cats.at(i);
        categoryTable->insertRow(categoryTable->rowCount());
        categoryTable->setItem(categoryTable->rowCount()-1,0,new QTableWidgetItem(cat.name()));
        categoryTable->setItem(categoryTable->rowCount()-1,1,new QTableWidgetItem(cat.categoryId().localId()));

        if (i %20)
            qApp->processEvents();
    }
}
