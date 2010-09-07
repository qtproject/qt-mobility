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
   void on_deleteLandmarks_clicked();
   void on_nextButton_clicked();
   void on_prevButton_clicked();
   void on_addLandmark_clicked();
   void fetchHandler(QLandmarkAbstractRequest::State state);
   void cancel();

private:
    void updateRowLabels();
    void updateTable(const QList<QLandmark> &lms);

    QLandmarkFetchRequest *landmarkFetch;
    QLandmarkImportRequest *landmarkImport;
    QLandmarkExportRequest *landmarkExport;
    QLandmarkRemoveRequest *landmarkRemove;
    QLandmarkSaveRequest *landmarkSave;
    QProgressDialog *progress;
    QLandmarkManager *manager;
    int currentOffset;
    const int limit;
};
