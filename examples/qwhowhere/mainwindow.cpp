/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#include "mainwindow.h"
#include "pictureitem.h"
#include "fetchmap.h"
#include "contactsdialog.h"
#include "message.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        m_session(0),
        m_location(0)
{
    setWindowTitle("QWhoWhere");
    m_friend_longitude = 0.0;
    m_friend_latitude = 0.0;

    // Create QGraphicsScene and QGraphicsView
    m_scene = new QGraphicsScene(this);
    m_view = new QGraphicsView(m_scene, this);
    m_view->setCacheMode(QGraphicsView::CacheBackground);
    m_view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setBackgroundBrush(QBrush(Qt::white));

    // Google Map fetcher
    m_fetchmap = new FetchMap(this);
    QObject::connect(m_fetchmap, SIGNAL(mapImageLoaded(QPixmap)),
                     this, SLOT(mapImageLoaded(QPixmap)));
    m_yourMapPixmap = 0;

    // SMS message sending and receiving
    m_message = new Message(this);
    QObject::connect(m_message, SIGNAL(sendingSMSFailed(int)),
                     this, SLOT(sendingSMSFailed(int)));
    QObject::connect(m_message, SIGNAL(friendLocationSMSReceived(QString, QString)),
                     this, SLOT(friendLocationSMSReceived(QString, QString)));
    QObject::connect(m_message, SIGNAL(friendAskLocationSMS(QString)),
                     this, SLOT(friendAskLocationSMS(QString)));
    QObject::connect(m_message, SIGNAL(raise()),
                     this, SLOT(raise()));

    // Create Satellite icon
    QPixmap pixmap(":/Satellite.png");
    m_gps = new PictureItem(QSize(50, 50), pixmap, this);
    m_gps->setZValue(100);
    m_scene->addItem(m_gps);

    createMenu();
    setCentralWidget(m_view);

    // Remove context menu from the all widgets
#ifdef Q_OS_SYMBIAN
    QWidgetList widgets = QApplication::allWidgets();
    QWidget* w = 0;
    foreach(w, widgets) {
        w->setContextMenuPolicy(Qt::NoContextMenu);
    }
#endif

    QTimer::singleShot(0, this, SLOT(delayedInit()));
}

MainWindow::~MainWindow()
{
    if (m_location)
        m_location->stopUpdates();
    if (m_session)
        m_session->close();
}

void MainWindow::delayedInit()
{
    // Set up the GPS
    m_location = QGeoPositionInfoSource::createDefaultSource(this);

    if (!m_location)
    {
        QMessageBox::warning(this, "QWhoWhere", tr("This example requires GPS support in order to function."));
        QTimer::singleShot(0, qApp, SLOT(quit()));
        return;
    }

    QObject::connect(m_location, SIGNAL(positionUpdated(QGeoPositionInfo)),
                     this, SLOT(positionUpdated(QGeoPositionInfo)));

    // Set up the bearer management
    QNetworkConfigurationManager manager;
    const bool canStartIAP = (manager.capabilities()
                              & QNetworkConfigurationManager::CanStartAndStopInterfaces);

    // Is there default access point, use it
    QNetworkConfiguration cfg = manager.defaultConfiguration();
    if (!cfg.isValid() || (!canStartIAP && cfg.state() != QNetworkConfiguration::Active)) {
        QMessageBox::warning(this, "QWhoWhere", tr("This example requires network access in order to function."));
        QTimer::singleShot(0, qApp, SLOT(quit()));
        return;
    }

    m_session = new QNetworkSession(cfg);
    connect(m_session,
            SIGNAL(error(QNetworkSession::SessionError)),
            this,
            SLOT(bearerError(QNetworkSession::SessionError)));
    connect(m_session,
            SIGNAL(opened()),
            this,
            SLOT(bearerOpened()));

    m_session->open();
}

void MainWindow::bearerError(QNetworkSession::SessionError error)
{
    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Retry);

    if (error == QNetworkSession::UnknownSessionError) {
        // No network found or user failed to select a network from a list of options
        msgBox.setText(tr("This examples requires network access in order to function."));
        msgBox.setInformativeText(tr("Press Cancel to quit this example."));
    } else if (error == QNetworkSession::SessionAbortedError) {
        // User went out of range or shutdown network
        msgBox.setText(tr("Out of range of network."));
        msgBox.setInformativeText(tr("Move back into range and press Retry, or press cancel to quit this example."));
    } else {
        msgBox.setText(tr("A networking error has occurred."));
        msgBox.setInformativeText(tr("Press Retry to attempt to connect to the network again, or press Cancel to quit the example."));
    }

    int ret = msgBox.exec();
    if (ret == QMessageBox::Retry) {
        QTimer::singleShot(0, m_session, SLOT(open()));
    } else {
        QTimer::singleShot(0, qApp, SLOT(quit()));
    }
}

void MainWindow::bearerOpened()
{
    startGps();
}

void MainWindow::startGps()
{
    showSearchingGpsIcon(true);
    m_location->startUpdates();
}

void MainWindow::positionUpdated(QGeoPositionInfo gpsPos)
{
    // Take your GPS position only once and search map for that location
    if (gpsPos.isValid() && !m_YougeoPosInfo.isValid()) {
        // Get first position
        showMessage("GPS position get");
        m_YougeoPosInfo = gpsPos;
        showSearchingGpsIcon(false);

        // Check is there messages in the inbox
        m_message->checkMessages();

        // Search Map image
        m_fetchmap->fetchMapImage(size(), m_YougeoPosInfo.coordinate().longitude(),
                                  m_YougeoPosInfo.coordinate().latitude(), m_friend_longitude, m_friend_latitude);
    } else if (gpsPos.isValid() && m_YougeoPosInfo.isValid()) {
        // Stop GPS, we have position
        showSearchingGpsIcon(false);
        m_location->stopUpdates();
    } else {
        showSearchingGpsIcon(true);
    }
}

void MainWindow::updateMap()
{
    // User wants to update his position to the map
    // Invalidate current position
    m_YougeoPosInfo.setCoordinate(QGeoCoordinate());
    // Start GPS
    startGps();
}

void MainWindow::mapImageLoaded(QPixmap p)
{
    // Show loaded map image
    m_yourMapPixmap = p;
    m_yourMapItem = new PictureItem(m_yourMapPixmap.size(), m_yourMapPixmap, this);
    QPointF to = QPointF((rect().width() - m_yourMapItem->boundingRect().width()) / 2,
                         (rect().height() - m_yourMapItem->boundingRect().height()) / 2);
    m_yourMapItem->setZValue(1);
    m_scene->addItem(m_yourMapItem);
    QPointF from = QPointF(to.x(), m_yourMapPixmap.height() * -1);
    m_yourMapItem->setPos(from);
    // Start animation for the image
    m_yourMapItem->animatePosition(from, to, 5000);
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    // Update scene rect
    m_scene->setSceneRect(m_view->rect());

    // GPS item pos to top right corner
    m_gps->setPos(QPointF(m_scene->sceneRect().width() - m_gps->boundingRect().width() - 5, 5));
}

void MainWindow::showSearchingGpsIcon(bool searching)
{
    // Animate GPS logo or stop animation
    if (searching) {
        if (!m_gpsAnim)
            m_gpsAnim = m_gps->animateAngle(0, 360, true);
    } else {
        if (m_gpsAnim) {
            m_gps->stop(m_gpsAnim);
            m_gpsAnim = 0;
        }
    }
}

void MainWindow::createMenu()
{
    this->menuBar()->addAction("Update map", this, SLOT(updateMap()));
    this->menuBar()->addAction("Send your location", this, SLOT(sendYourLocation()));
    this->menuBar()->addAction("Ask friend location", this, SLOT(askFriendLocation()));
}

void MainWindow::askFriendLocation()
{
    askLocation = true;
    openDialog();
}

void MainWindow::sendYourLocation()
{
    askLocation = false;
    openDialog();
}

void MainWindow::openDialog()
{
    // Open dialog for selecting contact
    if (m_YougeoPosInfo.isValid() || askLocation) {
        m_contactDialog = new ContactsDialog(this);

        QObject::connect(m_contactDialog, SIGNAL(contactSelected(QString)),
                         this, SLOT(contactSelected(QString)));
        m_contactDialog->exec();
        QObject::disconnect(m_contactDialog, SIGNAL(contactSelected(QString)),
                            this, SLOT(contactSelected(QString)));

        delete m_contactDialog;
    } else {
        showMessage("Waiting for your GPS location, try later again");
    }
}

void MainWindow::contactSelected(QString phoneNumber)
{
    // Contact selected
    QString type;
    if (askLocation)
        type = "REQ:";
    else
        type = "RES:";

    showMessage("SMS message was sent");

    // Send SMS message
    m_message->sendLocationSMS(type, m_YougeoPosInfo, phoneNumber);
}

void MainWindow::sendingSMSFailed(int /*error*/)
{
    // TODO: Gives some error even SMS message was sent in N97
    //if (error == QMessageService::CanceledState) {
    //  showMessage("Failed to send SMS message");
    //}
}

void MainWindow::friendLocationSMSReceived(QString lat, QString longi)
{
    // Frien location received via SMS
    m_friend_longitude = longi.toDouble();
    m_friend_latitude = lat.toDouble();

    // Show you and friend location on the map
    if (m_YougeoPosInfo.isValid()) {
        showMessage("Friend location received");
        m_fetchmap->fetchMapImage(size(), m_YougeoPosInfo.coordinate().longitude(),
                                  m_YougeoPosInfo.coordinate().latitude(), m_friend_longitude, m_friend_latitude);
    }
}

void MainWindow::friendAskLocationSMS(QString friendPhoneNumber)
{
    // Ask permission to send response SMS
    if (QMessageBox::Ok == QMessageBox::question(this, "QWhoWhere", QString(
                "Your friend %1 ask your location. Do you want to send it?").arg(friendPhoneNumber),
            QMessageBox::Ok | QMessageBox::No)) {

        if (!m_YougeoPosInfo.isValid()) {
            showMessage("You do not have GPS location, can not send it");
            return;
        }

        m_message->sendLocationSMS("RES:", m_YougeoPosInfo, friendPhoneNumber);
        showMessage("SMS message was sent");
    }
}

void MainWindow::showMessage(QString msg)
{
    // Show animated message to the user
    if (m_messageItem) {
        m_scene->removeItem(m_messageItem);
        delete m_messageItem;
        m_messageItem = 0;
    }

    // Create pixmap into message is drawed
    QPixmap pixmap(QSize(size().width() - 100, (size().width() - 100) / 3));
    QSize size = pixmap.size();

    QPainter painter(&pixmap);
    // Draw background
    painter.setBrush(Qt::white);
    painter.drawRect(pixmap.rect());
    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::black);
    // Draw text
    painter.setFont(QFont("Helvetica", 6));
    QFontMetrics fm = painter.fontMetrics();
    QPoint c = QPoint((size.width() - fm.width(msg)) / 2, (size.height() - fm.height()) / 2);
    painter.drawText(pixmap.rect(), Qt::AlignCenter | Qt::TextWordWrap, msg);
    // Draw borders
    painter.drawRect(QRect(QPoint(2, 2), QSize(size.width() - 4, size.height() - 4)));
    painter.end();

    // Animate created pixmap
    m_messageItem = new PictureItem(pixmap.size(), pixmap, this);
    m_scene->addItem(m_messageItem);
    m_messageItem->setZValue(101);
    m_messageItem->setPos(QPointF((width() - size.width()) / 2, (height() - size.height()) / 2));
    m_messageItem->setHideWhenFinished();
    // Start animation
    m_messageItem->animateAngle(-90, 360);
}

