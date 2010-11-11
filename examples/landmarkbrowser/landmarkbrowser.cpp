/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qlandmarkfilter.h>
#include <qlandmarkcategoryremoverequest.h>
#include <qlandmark.h>

#include <QModelIndex>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QProgressDialog>
#include <QProgressBar>
#include <QDebug>
#include <QTimer>

#include "landmarkbrowser.h"
#include "landmarkadddialog.cpp"
#include "categoryadddialog.h"

QTM_USE_NAMESPACE

LandmarkBrowser::LandmarkBrowser(QWidget *parent, Qt::WindowFlags flags)
    :currentLandmarkOffset(0),
     currentCategoryOffset(0),
     limit(20), filterDialog(0), progress(0), manager(0),
     landmarkFetch(0), landmarkExport(0), landmarkRemove(0),
     landmarkSave(0), categoryFetch(0)


{
    setupUi(this);

    categoryTable->removeColumn(2);
    categoryTable->hideColumn(1);
    categoryTable->setHorizontalHeaderItem(0, new QTableWidgetItem("Name"));

    manager = new QLandmarkManager(this);
    if (manager->error() == QLandmarkManager::InvalidManagerError) {
        QMessageBox::warning(this,"Warning", "Manager is invalid closing application, please view any warnings "
                             "on the the console", QMessageBox::Ok, QMessageBox::NoButton);
        QTimer::singleShot(0, this,SLOT(close()));
        return;
    }


    landmarkFetch = new QLandmarkFetchRequest(manager, this);
    QObject::connect(landmarkFetch, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)),
                this,SLOT(fetchHandler(QLandmarkAbstractRequest::State)));

    landmarkImport = new QLandmarkImportRequest(manager, this);
    connect(landmarkImport, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)),
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

    filterDialog = new LandmarkFilterDialog(landmarkFetch, manager, this);
    connect(filterDialog,SIGNAL(doFetchAll()), this, SLOT(doFetchAll()));

    progress = new QProgressDialog (tr("Please wait..."),tr("Cancel"),0,0, this);
    progress->setWindowTitle(tr("Loading Landmarks"));
    QObject::connect(progress,SIGNAL(canceled()), this, SLOT(cancel()));

    landmarkFetch->setLimit(limit);
    landmarkFetch->setOffset(currentLandmarkOffset);
    landmarkFetch->setSorting(QLandmarkNameSort());
    if (!landmarkFetch->start() && landmarkFetch->error() == QLandmarkManager::InvalidManagerError) {
        QMessageBox::warning(this,"Warning", "Manager is invalid closing application", QMessageBox::Ok, QMessageBox::NoButton);
        QTimer::singleShot(0, this,SLOT(close()));
        return;
    }

    categoryFetch->setLimit(limit);
    categoryFetch->setOffset(currentCategoryOffset);
    categoryFetch->start();

    progress->show();

    #ifdef Q_OS_SYMBIAN
        gpxRadioButton->setEnabled(false);
        gpxRadioButton->setVisible(false);
    #endif

        connect(manager, SIGNAL(landmarksAdded(QList<QLandmarkId>)),this,SLOT(landmarksAdded(QList<QLandmarkId>)));
        connect(manager,SIGNAL(landmarksChanged(QList<QLandmarkId>)), this, SLOT(landmarksChanged()));
        connect(manager, SIGNAL(landmarksRemoved(QList<QLandmarkId>)),this,SLOT(landmarksRemoved()));

        connect(manager, SIGNAL(categoriesAdded(QList<QLandmarkCategoryId>)), this, SLOT(categoriesAdded()));
        connect(manager, SIGNAL(categoriesChanged(QList<QLandmarkCategoryId>)), this,SLOT(categoriesChanged()));
        connect(manager, SIGNAL(categoriesRemoved(QList<QLandmarkCategoryId>)), this,SLOT(categoriesRemoved()));

        connect(manager, SIGNAL(dataChanged()),this, SLOT(dataChanged()));
}

LandmarkBrowser::~LandmarkBrowser()
{
    delete filterDialog;
     filterDialog =0;

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

    QString fileName;
#if defined(Q_WS_MAEMO_6) || defined (Q_WS_MAEMO_5)
    fileName = QFileDialog::getOpenFileName(this,tr("Import File"),"/home/user",fileFilterString);
#else
    fileName = QFileDialog::getOpenFileName(this,tr("Import File"), ".",fileFilterString);
#endif
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

    QString fileName;
#if defined(Q_WS_MAEMO_6) || defined (Q_WS_MAEMO_5)
    fileName = QFileDialog::getSaveFileName(this,tr("Export File"),"/home/user",fileFilterString);
#else
    fileName = QFileDialog::getSaveFileName(this,tr("Export File"),".",fileFilterString);
#endif

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
        deleteIds.append(landmarks.at(index.row()).landmarkId());
        table->removeRow(index.row());
        landmarks.removeAt(index.row());
        selectedIndexes = table->selectionModel()->selectedRows();
    }

    if (deleteIds.count() == 0)
        return;

    manager->removeLandmarks(deleteIds);
}

void LandmarkBrowser::on_setFilterButton_clicked()
{
    if (!filterDialog->exec()) {
        return;
    }

    currentLandmarkOffset =0;
    landmarkFetch->setOffset(currentLandmarkOffset);
    landmarkFetch->start();
    progress->setWindowTitle(tr("Loading Landmarks"));
    progress->show();
}

void LandmarkBrowser::on_deleteCategoriesButton_clicked()
{
    QItemSelectionModel *selection  = categoryTable->selectionModel();
    QModelIndexList selectedIndexes = selection->selectedRows();

    QList<QLandmarkCategoryId> deleteIds;

    QLandmarkCategoryId id;
    QModelIndex index;
    bool alreadyWarned = false;
    while(selectedIndexes.count() > 0) {
        index = selectedIndexes.takeLast();
        id.setManagerUri(manager->managerUri());
        id.setLocalId(categoryTable->item(index.row(),1)->text());
        if (manager->isReadOnly(id)) {
            if (!alreadyWarned) {
                QMessageBox::warning(this,"Warning", "Cannot delete a global category", QMessageBox::Ok, QMessageBox::NoButton);
                alreadyWarned = true;
            }

            selection->setCurrentIndex(index, QItemSelectionModel::Deselect);
            categoryTable->setSelectionModel(selection);

        } else {
            deleteIds.append(id);
            categoryTable->removeRow(index.row());
        }

        selectedIndexes = categoryTable->selectionModel()->selectedRows();
    }

    if (deleteIds.count() == 0)
        return;

    QLandmarkCategoryRemoveRequest catRemove(manager, this);
    catRemove.setCategoryIds(deleteIds);
    catRemove.start();
#ifdef Q_OS_SYMBIAN
    catRemove.waitForFinished(30);
#endif
}

void LandmarkBrowser::on_addLandmark_clicked()
{
    LandmarkAddDialog addDialog(this);
#ifndef Q_OS_SYMBIAN
    addDialog.resize(this->width(), this->height());
#endif
    if (!addDialog.exec()) {
        return;
    }

    QLandmark lm = addDialog.landmark();
    manager->saveLandmark(&lm);
}

void LandmarkBrowser::on_editLandmarkButton_clicked()
{
    QItemSelectionModel *selection = table->selectionModel();
    QModelIndexList selectedIndexes = selection->selectedRows();
    if (selectedIndexes.count()  > 0) {
        LandmarkAddDialog addDialog(this, 0, landmarks.at(selectedIndexes.at(0).row()));

        if (!addDialog.exec()) {
            return;
        }

        QLandmark lm = addDialog.landmark();
        manager->saveLandmark(&lm);
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
}

void LandmarkBrowser::fetchHandler(QLandmarkAbstractRequest::State state)
{
    qDebug() << "AMOS fetch handler called state =" << state;
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

                        landmarks = landmarkFetch->landmarks();
                        updateTable(landmarks);

                        table->setUpdatesEnabled(true);
                    }  else {
                        QMessageBox::warning(this,"Warning", "Fetch Failed", QMessageBox::Ok, QMessageBox::NoButton);
                    }
                    updateRowLabels();
                    delete progress;
                    progress = new QProgressDialog (tr("Please wait..."),tr("Cancel"),0,0, this);
                    QObject::connect(progress,SIGNAL(canceled()), this, SLOT(cancel()));
                break;
            }
        case QLandmarkAbstractRequest::CategoryFetchRequest: {
                qDebug() << "AMOS CategoryFetchRequest finished";
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

                    qDebug() << "AMOS CategoryFetchRequest finished1";

                    QList<QLandmarkCategory> cats;
                    cats = categoryFetch->categories();
                    updateCategoryTable(cats);

                    categoryTable->setUpdatesEnabled(true);
                    qDebug() << "AMOS CategoryFetchRequest finished2";
                } else {
                    QMessageBox::warning(this, "Warning", "Category Fetch Failed", QMessageBox::Ok, QMessageBox::NoButton);
                }
                updateCategoryRowLabels();
                if (tabWidget->currentIndex() == 1) {
                    //progress->hide();
                    delete progress;
                    progress = new QProgressDialog (tr("Please wait..."),tr("Cancel"),0,0, this);
                    QObject::connect(progress,SIGNAL(canceled()), this, SLOT(cancel()));
                }
            }
        }
    }
}

//this is invoked when we delete category and we are filtering by that category
//we need to do fetch all from scratch
void LandmarkBrowser::doFetchAll()
{
    currentLandmarkOffset =0;
    landmarkFetch->setOffset(currentLandmarkOffset);
    landmarkFetch->start();
    progress->setWindowTitle(tr("Loading Landmarks"));
    progress->show();
}

void LandmarkBrowser::cancel()
{
    if (landmarkImport->isActive())
        landmarkImport->cancel();
    else if (landmarkExport->isActive())
        landmarkExport->cancel();
}

void LandmarkBrowser::landmarksAdded(QList<QLandmarkId> ids)
{
    QMessageBox::information(this, "Information", "Landmark(s) have been added", QMessageBox::Ok, QMessageBox::NoButton);
    reloadingLandmarks();
}

void LandmarkBrowser::landmarksChanged()
{
    QMessageBox::information(this, "Information", "Landmark(s) have been edited", QMessageBox::Ok, QMessageBox::NoButton);
    reloadingLandmarks();
}

void LandmarkBrowser::landmarksRemoved()
{
    QMessageBox::information(this, "Information", "Landmark(s) have been removed", QMessageBox::Ok, QMessageBox::NoButton);
    reloadingLandmarks();
}

void LandmarkBrowser::categoriesAdded()
{
    QMessageBox::information(this, "Information", "Category(/ies) have been added", QMessageBox::Ok, QMessageBox::NoButton);
    reloadingCategories();
}

void LandmarkBrowser::categoriesChanged()
{
    QMessageBox::information(this, "Information", "Category(/ies) have been changed", QMessageBox::Ok, QMessageBox::NoButton);
    reloadingCategories();
}

void LandmarkBrowser::categoriesRemoved()
{
    QMessageBox::information(this, "Information", "Category(/ies) have been removed", QMessageBox::Ok, QMessageBox::NoButton);
    reloadingCategories();
}

void LandmarkBrowser::dataChanged()
{
    QMessageBox::information(this, "Information", "Database has undergone major modifications, most likely an import operation", QMessageBox::Ok, QMessageBox::NoButton);
    reloadingLandmarks();
    reloadingCategories();
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
        if(manager->isReadOnly(cat.categoryId())) {
            categoryTable->setItem(categoryTable->rowCount()-1,0,new QTableWidgetItem(cat.name() + "(global)"));
        } else {
            categoryTable->setItem(categoryTable->rowCount()-1,0,new QTableWidgetItem(cat.name()));
        }
        categoryTable->setItem(categoryTable->rowCount()-1,1,new QTableWidgetItem(cat.categoryId().localId()));

        if (i %20)
            qApp->processEvents();
    }
}

void LandmarkBrowser::reloadingLandmarks()
{
    landmarkFetch->setOffset(currentLandmarkOffset);
    landmarkFetch->start();
    if (tabWidget->currentIndex() == 0) {
        progress->setWindowTitle(tr("Loading Landmarks"));
        progress->show();
    }
}

void LandmarkBrowser::reloadingCategories() {
    qDebug() << "AMOS reloading categories called";
    categoryFetch->setOffset(currentLandmarkOffset);
    categoryFetch->start();
    if (tabWidget->currentIndex() == 1) {
        progress->setWindowTitle(tr("Loading    Categories"));
        progress->show();
    }
}
