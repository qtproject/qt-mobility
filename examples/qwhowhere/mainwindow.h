/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>

// QtMobility API headers
// Location
#include <qgeopositioninfosource.h>
#include <qgeopositioninfo.h>
// Bearer
#include <qnetworkconfigmanager.h>
#include <qnetworksession.h>

// QtMobility namespace
QTM_USE_NAMESPACE

class PictureItem;
class FetchMap;
class ContactsDialog;
class Message;

class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void resizeEvent(QResizeEvent *);

private:
    void showSearchingGpsIcon(bool searching);
    void showMessage(QString msg);
    void createMenu();
    void startGps();
    void openDialog();

public slots:
    void positionUpdated(QGeoPositionInfo gpsPos);
    void mapImageLoaded(QPixmap);
    void askFriendLocation();
    void sendYourLocation();
    void contactSelected(QString phoneNumber);
    void sendingSMSFailed(int);
    void friendLocationSMSReceived(QString latitude, QString longitude);
    void friendAskLocationSMS(QString friendPhoneNumber);
    void updateMap();

private slots:
    void delayedInit();
    void bearerError(QNetworkSession::SessionError error);
    void bearerOpened();

private:
    // Graphics view fw
    QGraphicsScene* m_scene;
    QGraphicsView* m_view;

    // Dialog for selecting contact
    QPointer<ContactsDialog> m_contactDialog;

    // Google Map API
    FetchMap* m_fetchmap;

    // Messaging
    Message* m_message;

    // Map picture
    QPixmap m_yourMapPixmap;
    PictureItem* m_yourMapItem;

    // Animated message
    QPointer<PictureItem> m_messageItem;

    // Gps logo
    PictureItem* m_gps;
    QPointer<QPropertyAnimation> m_gpsAnim;

    // for bearer management
    QNetworkSession* m_session;

    // Location data
    QGeoPositionInfoSource *m_location;
    QGeoPositionInfo m_YougeoPosInfo;
    qreal m_friend_longitude;
    qreal m_friend_latitude;

    bool askLocation;
};

#endif // MAINWINDOW_H
