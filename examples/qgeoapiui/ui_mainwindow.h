/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Tue 30. Mar 11:11:08 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QVBoxLayout *qgeoapiLayout;
    QHBoxLayout *routeLayout;
    QLabel *label;
    QTextEdit *srcLong;
    QTextEdit *srcLat;
    QLabel *label_2;
    QTextEdit *dstLong;
    QTextEdit *dstLat;
    QPushButton *btnRequest;
    QHBoxLayout *geocoding_1_Layout;
    QTextEdit *obloc;
    QLabel *label_4;
    QTextEdit *country;
    QLabel *label_5;
    QTextEdit *state;
    QLabel *label_6;
    QTextEdit *city;
    QHBoxLayout *geocodind_2_Layout;
    QLabel *label_7;
    QTextEdit *zip;
    QLabel *label_8;
    QTextEdit *street;
    QLabel *label_9;
    QTextEdit *streetNumber;
    QPushButton *btnCoding;
    QHBoxLayout *revGeocodingLayout;
    QLabel *label_3;
    QTextEdit *locLong;
    QTextEdit *locLat;
    QPushButton *btnReverseCoding;
    QHBoxLayout *mapTileLayout;
    QLabel *label_10;
    QTextEdit *tileLong;
    QTextEdit *tileLat;
    QTextEdit *tileZoomLevel;
    QPushButton *btnRequestTile;
    QHBoxLayout *resultsLayout;
    QTreeWidget *treeWidget;
    QLabel *mapTileLabel;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(454, 414);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetNoConstraint);
        qgeoapiLayout = new QVBoxLayout();
        qgeoapiLayout->setSpacing(6);
        qgeoapiLayout->setObjectName(QString::fromUtf8("qgeoapiLayout"));
        qgeoapiLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        routeLayout = new QHBoxLayout();
        routeLayout->setSpacing(6);
        routeLayout->setObjectName(QString::fromUtf8("routeLayout"));
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy1);

        routeLayout->addWidget(label);

        srcLong = new QTextEdit(centralWidget);
        srcLong->setObjectName(QString::fromUtf8("srcLong"));
        sizePolicy1.setHeightForWidth(srcLong->sizePolicy().hasHeightForWidth());
        srcLong->setSizePolicy(sizePolicy1);
        srcLong->setMaximumSize(QSize(55, 25));
        srcLong->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        srcLong->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        routeLayout->addWidget(srcLong);

        srcLat = new QTextEdit(centralWidget);
        srcLat->setObjectName(QString::fromUtf8("srcLat"));
        sizePolicy1.setHeightForWidth(srcLat->sizePolicy().hasHeightForWidth());
        srcLat->setSizePolicy(sizePolicy1);
        srcLat->setMaximumSize(QSize(55, 25));
        srcLat->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        srcLat->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        routeLayout->addWidget(srcLat);

        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        sizePolicy1.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy1);

        routeLayout->addWidget(label_2);

        dstLong = new QTextEdit(centralWidget);
        dstLong->setObjectName(QString::fromUtf8("dstLong"));
        sizePolicy1.setHeightForWidth(dstLong->sizePolicy().hasHeightForWidth());
        dstLong->setSizePolicy(sizePolicy1);
        dstLong->setMaximumSize(QSize(55, 25));
        dstLong->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        dstLong->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        routeLayout->addWidget(dstLong);

        dstLat = new QTextEdit(centralWidget);
        dstLat->setObjectName(QString::fromUtf8("dstLat"));
        sizePolicy1.setHeightForWidth(dstLat->sizePolicy().hasHeightForWidth());
        dstLat->setSizePolicy(sizePolicy1);
        dstLat->setMaximumSize(QSize(55, 25));
        dstLat->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        dstLat->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        routeLayout->addWidget(dstLat);

        btnRequest = new QPushButton(centralWidget);
        btnRequest->setObjectName(QString::fromUtf8("btnRequest"));
        sizePolicy1.setHeightForWidth(btnRequest->sizePolicy().hasHeightForWidth());
        btnRequest->setSizePolicy(sizePolicy1);

        routeLayout->addWidget(btnRequest);


        qgeoapiLayout->addLayout(routeLayout);

        geocoding_1_Layout = new QHBoxLayout();
        geocoding_1_Layout->setSpacing(6);
        geocoding_1_Layout->setObjectName(QString::fromUtf8("geocoding_1_Layout"));
        geocoding_1_Layout->setSizeConstraint(QLayout::SetDefaultConstraint);
        obloc = new QTextEdit(centralWidget);
        obloc->setObjectName(QString::fromUtf8("obloc"));
        sizePolicy1.setHeightForWidth(obloc->sizePolicy().hasHeightForWidth());
        obloc->setSizePolicy(sizePolicy1);
        obloc->setMinimumSize(QSize(0, 0));
        obloc->setMaximumSize(QSize(150, 25));
        obloc->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        obloc->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        geocoding_1_Layout->addWidget(obloc);

        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        sizePolicy1.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy1);

        geocoding_1_Layout->addWidget(label_4);

        country = new QTextEdit(centralWidget);
        country->setObjectName(QString::fromUtf8("country"));
        sizePolicy1.setHeightForWidth(country->sizePolicy().hasHeightForWidth());
        country->setSizePolicy(sizePolicy1);
        country->setMaximumSize(QSize(50, 25));
        country->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        country->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        geocoding_1_Layout->addWidget(country);

        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        sizePolicy1.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
        label_5->setSizePolicy(sizePolicy1);

        geocoding_1_Layout->addWidget(label_5);

        state = new QTextEdit(centralWidget);
        state->setObjectName(QString::fromUtf8("state"));
        sizePolicy1.setHeightForWidth(state->sizePolicy().hasHeightForWidth());
        state->setSizePolicy(sizePolicy1);
        state->setMaximumSize(QSize(50, 25));
        state->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        state->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        geocoding_1_Layout->addWidget(state);

        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        sizePolicy1.setHeightForWidth(label_6->sizePolicy().hasHeightForWidth());
        label_6->setSizePolicy(sizePolicy1);

        geocoding_1_Layout->addWidget(label_6);

        city = new QTextEdit(centralWidget);
        city->setObjectName(QString::fromUtf8("city"));
        sizePolicy1.setHeightForWidth(city->sizePolicy().hasHeightForWidth());
        city->setSizePolicy(sizePolicy1);
        city->setMaximumSize(QSize(50, 25));
        city->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        city->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        geocoding_1_Layout->addWidget(city);


        qgeoapiLayout->addLayout(geocoding_1_Layout);

        geocodind_2_Layout = new QHBoxLayout();
        geocodind_2_Layout->setSpacing(6);
        geocodind_2_Layout->setObjectName(QString::fromUtf8("geocodind_2_Layout"));
        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        sizePolicy1.setHeightForWidth(label_7->sizePolicy().hasHeightForWidth());
        label_7->setSizePolicy(sizePolicy1);

        geocodind_2_Layout->addWidget(label_7);

        zip = new QTextEdit(centralWidget);
        zip->setObjectName(QString::fromUtf8("zip"));
        sizePolicy1.setHeightForWidth(zip->sizePolicy().hasHeightForWidth());
        zip->setSizePolicy(sizePolicy1);
        zip->setMaximumSize(QSize(75, 25));
        zip->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        zip->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        geocodind_2_Layout->addWidget(zip);

        label_8 = new QLabel(centralWidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        sizePolicy1.setHeightForWidth(label_8->sizePolicy().hasHeightForWidth());
        label_8->setSizePolicy(sizePolicy1);

        geocodind_2_Layout->addWidget(label_8);

        street = new QTextEdit(centralWidget);
        street->setObjectName(QString::fromUtf8("street"));
        sizePolicy1.setHeightForWidth(street->sizePolicy().hasHeightForWidth());
        street->setSizePolicy(sizePolicy1);
        street->setMaximumSize(QSize(75, 25));
        street->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        street->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        geocodind_2_Layout->addWidget(street);

        label_9 = new QLabel(centralWidget);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        sizePolicy1.setHeightForWidth(label_9->sizePolicy().hasHeightForWidth());
        label_9->setSizePolicy(sizePolicy1);

        geocodind_2_Layout->addWidget(label_9);

        streetNumber = new QTextEdit(centralWidget);
        streetNumber->setObjectName(QString::fromUtf8("streetNumber"));
        sizePolicy1.setHeightForWidth(streetNumber->sizePolicy().hasHeightForWidth());
        streetNumber->setSizePolicy(sizePolicy1);
        streetNumber->setMaximumSize(QSize(75, 25));
        streetNumber->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        streetNumber->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        geocodind_2_Layout->addWidget(streetNumber);

        btnCoding = new QPushButton(centralWidget);
        btnCoding->setObjectName(QString::fromUtf8("btnCoding"));
        sizePolicy1.setHeightForWidth(btnCoding->sizePolicy().hasHeightForWidth());
        btnCoding->setSizePolicy(sizePolicy1);

        geocodind_2_Layout->addWidget(btnCoding);


        qgeoapiLayout->addLayout(geocodind_2_Layout);

        revGeocodingLayout = new QHBoxLayout();
        revGeocodingLayout->setSpacing(6);
        revGeocodingLayout->setObjectName(QString::fromUtf8("revGeocodingLayout"));
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        sizePolicy1.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy1);

        revGeocodingLayout->addWidget(label_3);

        locLong = new QTextEdit(centralWidget);
        locLong->setObjectName(QString::fromUtf8("locLong"));
        sizePolicy1.setHeightForWidth(locLong->sizePolicy().hasHeightForWidth());
        locLong->setSizePolicy(sizePolicy1);
        locLong->setMaximumSize(QSize(100, 25));
        locLong->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        locLong->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        revGeocodingLayout->addWidget(locLong);

        locLat = new QTextEdit(centralWidget);
        locLat->setObjectName(QString::fromUtf8("locLat"));
        sizePolicy1.setHeightForWidth(locLat->sizePolicy().hasHeightForWidth());
        locLat->setSizePolicy(sizePolicy1);
        locLat->setMaximumSize(QSize(100, 25));
        locLat->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        locLat->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        revGeocodingLayout->addWidget(locLat);

        btnReverseCoding = new QPushButton(centralWidget);
        btnReverseCoding->setObjectName(QString::fromUtf8("btnReverseCoding"));
        sizePolicy1.setHeightForWidth(btnReverseCoding->sizePolicy().hasHeightForWidth());
        btnReverseCoding->setSizePolicy(sizePolicy1);

        revGeocodingLayout->addWidget(btnReverseCoding);


        qgeoapiLayout->addLayout(revGeocodingLayout);

        mapTileLayout = new QHBoxLayout();
        mapTileLayout->setSpacing(6);
        mapTileLayout->setObjectName(QString::fromUtf8("mapTileLayout"));
        label_10 = new QLabel(centralWidget);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        sizePolicy1.setHeightForWidth(label_10->sizePolicy().hasHeightForWidth());
        label_10->setSizePolicy(sizePolicy1);

        mapTileLayout->addWidget(label_10);

        tileLong = new QTextEdit(centralWidget);
        tileLong->setObjectName(QString::fromUtf8("tileLong"));
        sizePolicy1.setHeightForWidth(tileLong->sizePolicy().hasHeightForWidth());
        tileLong->setSizePolicy(sizePolicy1);
        tileLong->setMaximumSize(QSize(75, 25));
        tileLong->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tileLong->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        mapTileLayout->addWidget(tileLong);

        tileLat = new QTextEdit(centralWidget);
        tileLat->setObjectName(QString::fromUtf8("tileLat"));
        sizePolicy1.setHeightForWidth(tileLat->sizePolicy().hasHeightForWidth());
        tileLat->setSizePolicy(sizePolicy1);
        tileLat->setMaximumSize(QSize(75, 25));
        tileLat->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tileLat->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        mapTileLayout->addWidget(tileLat);

        tileZoomLevel = new QTextEdit(centralWidget);
        tileZoomLevel->setObjectName(QString::fromUtf8("tileZoomLevel"));
        sizePolicy1.setHeightForWidth(tileZoomLevel->sizePolicy().hasHeightForWidth());
        tileZoomLevel->setSizePolicy(sizePolicy1);
        tileZoomLevel->setMaximumSize(QSize(75, 25));
        tileZoomLevel->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tileZoomLevel->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        mapTileLayout->addWidget(tileZoomLevel);

        btnRequestTile = new QPushButton(centralWidget);
        btnRequestTile->setObjectName(QString::fromUtf8("btnRequestTile"));
        sizePolicy1.setHeightForWidth(btnRequestTile->sizePolicy().hasHeightForWidth());
        btnRequestTile->setSizePolicy(sizePolicy1);

        mapTileLayout->addWidget(btnRequestTile);


        qgeoapiLayout->addLayout(mapTileLayout);


        verticalLayout->addLayout(qgeoapiLayout);

        resultsLayout = new QHBoxLayout();
        resultsLayout->setSpacing(6);
        resultsLayout->setObjectName(QString::fromUtf8("resultsLayout"));
        treeWidget = new QTreeWidget(centralWidget);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
        treeWidget->setHeaderItem(__qtreewidgetitem);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(treeWidget->sizePolicy().hasHeightForWidth());
        treeWidget->setSizePolicy(sizePolicy2);
        treeWidget->header()->setDefaultSectionSize(150);
        treeWidget->header()->setMinimumSectionSize(150);

        resultsLayout->addWidget(treeWidget);

        mapTileLabel = new QLabel(centralWidget);
        mapTileLabel->setObjectName(QString::fromUtf8("mapTileLabel"));
        sizePolicy.setHeightForWidth(mapTileLabel->sizePolicy().hasHeightForWidth());
        mapTileLabel->setSizePolicy(sizePolicy);

        resultsLayout->addWidget(mapTileLabel);


        verticalLayout->addLayout(resultsLayout);

        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "source", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "destination", 0, QApplication::UnicodeUTF8));
        btnRequest->setText(QApplication::translate("MainWindow", "request route", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "country", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "state", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MainWindow", "city", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("MainWindow", "zip", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("MainWindow", "street", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("MainWindow", "#", 0, QApplication::UnicodeUTF8));
        btnCoding->setText(QApplication::translate("MainWindow", "geocoding", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "location", 0, QApplication::UnicodeUTF8));
        btnReverseCoding->setText(QApplication::translate("MainWindow", "reverse geocoding", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("MainWindow", "coord", 0, QApplication::UnicodeUTF8));
        btnRequestTile->setText(QApplication::translate("MainWindow", "request map tile", 0, QApplication::UnicodeUTF8));
        mapTileLabel->setText(QApplication::translate("MainWindow", "map tile", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
