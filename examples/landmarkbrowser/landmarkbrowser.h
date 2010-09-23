#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProgressDialog>

#include <qlandmarkfetchrequest.h>
#include <qlandmarkmanager.h>
#include <qlandmarkimportrequest.h>
#include <qlandmarkexportrequest.h>
#include <qlandmarkremoverequest.h>
#include <qlandmarksaverequest.h>
#include <qlandmarkcategoryfetchrequest.h>

#include "ui_landmarkbrowser.h"
QTM_USE_NAMESPACE

class LandmarkBrowser : public QWidget, public Ui_LandmarkBrowser
{
Q_OBJECT
public:
    LandmarkBrowser(QWidget *parent =0, Qt::WindowFlags flags =0);
    ~LandmarkBrowser();

private slots:
   void on_importLandmarks_clicked();
   void on_exportLandmarks_clicked();

   void on_addLandmark_clicked();
   void on_nextLandmarkButton_clicked();
   void on_prevLandmarkButton_clicked();
   void on_deleteLandmarksButton_clicked();

   void on_addCategoryButton_clicked();
   void on_nextCategoryButton_clicked();
   void on_prevCategoryButton_clicked();
   void on_deleteCategoriesButton_clicked();

   void fetchHandler(QLandmarkAbstractRequest::State state);
   void cancel();

private:
    void updateRowLabels();
    void updateCategoryRowLabels();
    void updateTable(const QList<QLandmark> &lms);
    void updateCategoryTable(const QList<QLandmarkCategory> &cats);

    QLandmarkFetchRequest *landmarkFetch;
    QLandmarkImportRequest *landmarkImport;
    QLandmarkExportRequest *landmarkExport;
    QLandmarkRemoveRequest *landmarkRemove;
    QLandmarkSaveRequest *landmarkSave;
    QLandmarkCategoryFetchRequest *categoryFetch;
    QProgressDialog *progress;
    QLandmarkManager *manager;
    int currentLandmarkOffset;
    int currentCategoryOffset;
    const int limit;
};
