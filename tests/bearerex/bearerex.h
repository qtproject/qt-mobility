#ifndef ACCESSPOINTMANAGEREX_H
#define ACCESSPOINTMANAGEREX_H

#include <QtGui>

#include "ui_bearerex.h"
#include "ui_detailedinfodialog.h"
#include "ui_sessiondialog.h"

#include "qnetworkconfigmanager.h"
#include "qnetworksession.h"
#include "xqlistwidget.h"

class QHttp;

class BearerEx : public QMainWindow, public Ui::BearerExMainWindow
{
     Q_OBJECT

public:
    BearerEx(QWidget* parent = 0);
    void createMenus();
    void showConfigurations();

private Q_SLOTS:
    void on_updateConfigurationsButton_clicked();
    void on_updateListButton_clicked();
    void on_showDetailsButton_clicked();
    void on_createSessionButton_clicked();
    void on_monitorConfigurationsButton_clicked();

    void configurationsUpdateCompleted();
    void configurationAdded(const QNetworkConfiguration& config);
    void configurationRemoved(const QNetworkConfiguration& config);
    void onlineStateChanged(bool isOnline);
    void configurationChanged(const QNetworkConfiguration & config);     

private:
    QNetworkConfigurationManager m_NetworkConfigurationManager;
    QAction* m_openAction;
    bool m_monitor;
};

class DetailedInfoDialog : public QDialog, public Ui::DetailedInfoDialog
{
    Q_OBJECT

public:
    DetailedInfoDialog(QNetworkConfiguration* apNetworkConfiguration = 0, QWidget* parent = 0);
};


class SessionDialog : public QDialog, public Ui::SessionDialog
{
    Q_OBJECT

public:
    SessionDialog(QNetworkConfiguration* apNetworkConfiguration = 0, QWidget* parent = 0);

private Q_SLOTS:
    void on_createQHttpButton_clicked();
    void on_sendRequestButton_clicked();
    void on_openSessionButton_clicked();
    void on_closeSessionButton_clicked();
    void on_stopConnectionButton_clicked();
    void on_deleteSessionButton_clicked();
    void on_alrButton_clicked();
    void done(bool error);
    
    void newConfigurationActivated();
    void preferredConfigurationChanged(const QNetworkConfiguration& config, bool isSeamless);
    void stateChanged(QNetworkSession::State state);    

private: //data
    QHttp* m_http;
    QNetworkSession* m_NetworkSession;
    QNetworkConfiguration m_config;
    bool m_httpRequestOngoing;
    bool m_alrEnabled;
};

#endif // ACCESSPOINTMANAGEREX_H

// End of file

