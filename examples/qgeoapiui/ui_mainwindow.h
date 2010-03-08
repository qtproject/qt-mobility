/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Thu Feb 25 18:10:42 2010
**      by: Qt User Interface Compiler version 4.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>
#include <QtGui/QTreeWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QTextEdit *srcLong;
    QLabel *label;
    QTextEdit *srcLat;
    QTextEdit *dstLat;
    QTextEdit *dstLong;
    QLabel *label_2;
    QTreeWidget *treeWidget;
    QLabel *mapTileLabel;
    QPushButton *btnRequest;
    QTextEdit *locLat;
    QLabel *label_3;
    QTextEdit *locLong;
    QPushButton *btnReverseCoding;
    QTextEdit *obloc;
    QLabel *label_4;
    QTextEdit *country;
    QLabel *label_5;
    QTextEdit *state;
    QLabel *label_6;
    QTextEdit *city;
    QLabel *label_7;
    QTextEdit *zip;
    QLabel *label_8;
    QTextEdit *street;
    QTextEdit *streetNumber;
    QLabel *label_9;
    QPushButton *btnCoding;
    QLabel *label_10;
    QTextEdit *tileLong;
    QTextEdit *tileLat;
    QTextEdit *tileZoomLevel;
    QPushButton *btnRequestTile;

    void setupUi(QMainWindow *MainWindow) {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(652, 519);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        srcLong = new QTextEdit(centralWidget);
        srcLong->setObjectName(QString::fromUtf8("srcLong"));
        srcLong->setGeometry(QRect(50, 10, 71, 16));
        srcLong->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        srcLong->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 10, 46, 13));
        srcLat = new QTextEdit(centralWidget);
        srcLat->setObjectName(QString::fromUtf8("srcLat"));
        srcLat->setGeometry(QRect(130, 10, 71, 16));
        srcLat->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        srcLat->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        dstLat = new QTextEdit(centralWidget);
        dstLat->setObjectName(QString::fromUtf8("dstLat"));
        dstLat->setGeometry(QRect(360, 10, 71, 16));
        dstLat->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        dstLat->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        dstLong = new QTextEdit(centralWidget);
        dstLong->setObjectName(QString::fromUtf8("dstLong"));
        dstLong->setGeometry(QRect(280, 10, 71, 16));
        dstLong->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        dstLong->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(220, 10, 61, 16));
        treeWidget = new QTreeWidget(centralWidget);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
        treeWidget->setHeaderItem(__qtreewidgetitem);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
        treeWidget->setGeometry(QRect(10, 150, 631, 361));
        mapTileLabel = new QLabel(centralWidget);
        mapTileLabel->setObjectName(QString::fromUtf8("mapTileLabel"));
        mapTileLabel->setGeometry(QRect(10, 150, 631, 361));
        btnRequest = new QPushButton(centralWidget);
        btnRequest->setObjectName(QString::fromUtf8("btnRequest"));
        btnRequest->setGeometry(QRect(440, 10, 81, 23));
        locLat = new QTextEdit(centralWidget);
        locLat->setObjectName(QString::fromUtf8("locLat"));
        locLat->setGeometry(QRect(130, 90, 71, 16));
        locLat->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        locLat->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 90, 46, 13));
        locLong = new QTextEdit(centralWidget);
        locLong->setObjectName(QString::fromUtf8("locLong"));
        locLong->setGeometry(QRect(50, 90, 71, 16));
        locLong->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        locLong->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        btnReverseCoding = new QPushButton(centralWidget);
        btnReverseCoding->setObjectName(QString::fromUtf8("btnReverseCoding"));
        btnReverseCoding->setGeometry(QRect(210, 90, 101, 23));
        obloc = new QTextEdit(centralWidget);
        obloc->setObjectName(QString::fromUtf8("obloc"));
        obloc->setGeometry(QRect(10, 40, 191, 16));
        obloc->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        obloc->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(210, 40, 41, 16));
        country = new QTextEdit(centralWidget);
        country->setObjectName(QString::fromUtf8("country"));
        country->setGeometry(QRect(250, 40, 71, 16));
        country->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        country->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(330, 40, 31, 16));
        state = new QTextEdit(centralWidget);
        state->setObjectName(QString::fromUtf8("state"));
        state->setGeometry(QRect(360, 40, 71, 16));
        state->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        state->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(440, 40, 21, 16));
        city = new QTextEdit(centralWidget);
        city->setObjectName(QString::fromUtf8("city"));
        city->setGeometry(QRect(460, 40, 71, 16));
        city->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        city->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(540, 40, 21, 16));
        zip = new QTextEdit(centralWidget);
        zip->setObjectName(QString::fromUtf8("zip"));
        zip->setGeometry(QRect(560, 40, 71, 16));
        zip->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        zip->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        label_8 = new QLabel(centralWidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(210, 60, 41, 16));
        street = new QTextEdit(centralWidget);
        street->setObjectName(QString::fromUtf8("street"));
        street->setGeometry(QRect(250, 60, 71, 16));
        street->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        street->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        streetNumber = new QTextEdit(centralWidget);
        streetNumber->setObjectName(QString::fromUtf8("streetNumber"));
        streetNumber->setGeometry(QRect(360, 60, 71, 16));
        streetNumber->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        streetNumber->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        label_9 = new QLabel(centralWidget);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(330, 60, 31, 16));
        btnCoding = new QPushButton(centralWidget);
        btnCoding->setObjectName(QString::fromUtf8("btnCoding"));
        btnCoding->setGeometry(QRect(440, 60, 81, 23));
        label_10 = new QLabel(centralWidget);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(10, 120, 46, 13));
        tileLong = new QTextEdit(centralWidget);
        tileLong->setObjectName(QString::fromUtf8("tileLong"));
        tileLong->setGeometry(QRect(50, 120, 71, 16));
        tileLong->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tileLong->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tileLat = new QTextEdit(centralWidget);
        tileLat->setObjectName(QString::fromUtf8("tileLat"));
        tileLat->setGeometry(QRect(130, 120, 71, 16));
        tileLat->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tileLat->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tileZoomLevel = new QTextEdit(centralWidget);
        tileZoomLevel->setObjectName(QString::fromUtf8("tileZoomLevel"));
        tileZoomLevel->setGeometry(QRect(210, 120, 31, 16));
        tileZoomLevel->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tileZoomLevel->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        btnRequestTile = new QPushButton(centralWidget);
        btnRequestTile->setObjectName(QString::fromUtf8("btnRequestTile"));
        btnRequestTile->setGeometry(QRect(250, 120, 101, 23));
        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow) {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "source", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "destination", 0, QApplication::UnicodeUTF8));
        mapTileLabel->setText(QApplication::translate("MainWindow", "map tile", 0, QApplication::UnicodeUTF8));
        btnRequest->setText(QApplication::translate("MainWindow", "request route", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "location", 0, QApplication::UnicodeUTF8));
        btnReverseCoding->setText(QApplication::translate("MainWindow", "reverse geocoding", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "country", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "state", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MainWindow", "city", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("MainWindow", "zip", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("MainWindow", "street", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("MainWindow", "#", 0, QApplication::UnicodeUTF8));
        btnCoding->setText(QApplication::translate("MainWindow", "geocoding", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("MainWindow", "coord", 0, QApplication::UnicodeUTF8));
        btnRequestTile->setText(QApplication::translate("MainWindow", "request map tile", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui
{
class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
