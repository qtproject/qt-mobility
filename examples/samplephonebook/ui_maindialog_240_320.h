/********************************************************************************
** Form generated from reading ui file 'maindialog_240_320.ui'
**
** Created: Tue Sep 8 11:41:40 2009
**      by: Qt User Interface Compiler version 4.6.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MAINDIALOG_240_320_H
#define UI_MAINDIALOG_240_320_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainDialog240By320
{
public:
    QVBoxLayout *verticalLayout;
    QListWidget *contactListWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *openButton;
    QPushButton *addButton;
    QPushButton *deleteButton;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *findButton;
    QPushButton *importButton;
    QPushButton *exportButton;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_7;
    QComboBox *contactEngineComboBox;
    QHBoxLayout *horizontalLayout_4;
    QLabel *contactStatusLabel;

    void setupUi(QWidget *MainDialog240By320)
    {
        if (MainDialog240By320->objectName().isEmpty())
            MainDialog240By320->setObjectName(QString::fromUtf8("MainDialog240By320"));
        MainDialog240By320->resize(263, 320);
        QPalette palette;
        QBrush brush(QColor(232, 231, 230, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush);
        QBrush brush1(QColor(255, 255, 255, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        QBrush brush2(QColor(145, 144, 144, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush2);
        QBrush brush3(QColor(185, 185, 185, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush3);
        MainDialog240By320->setPalette(palette);
        verticalLayout = new QVBoxLayout(MainDialog240By320);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        contactListWidget = new QListWidget(MainDialog240By320);
        contactListWidget->setObjectName(QString::fromUtf8("contactListWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(contactListWidget->sizePolicy().hasHeightForWidth());
        contactListWidget->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(contactListWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        openButton = new QPushButton(MainDialog240By320);
        openButton->setObjectName(QString::fromUtf8("openButton"));

        horizontalLayout->addWidget(openButton);

        addButton = new QPushButton(MainDialog240By320);
        addButton->setObjectName(QString::fromUtf8("addButton"));

        horizontalLayout->addWidget(addButton);

        deleteButton = new QPushButton(MainDialog240By320);
        deleteButton->setObjectName(QString::fromUtf8("deleteButton"));

        horizontalLayout->addWidget(deleteButton);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        findButton = new QPushButton(MainDialog240By320);
        findButton->setObjectName(QString::fromUtf8("findButton"));

        horizontalLayout_2->addWidget(findButton);

        importButton = new QPushButton(MainDialog240By320);
        importButton->setObjectName(QString::fromUtf8("importButton"));

        horizontalLayout_2->addWidget(importButton);

        exportButton = new QPushButton(MainDialog240By320);
        exportButton->setObjectName(QString::fromUtf8("exportButton"));

        horizontalLayout_2->addWidget(exportButton);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_7 = new QLabel(MainDialog240By320);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        horizontalLayout_3->addWidget(label_7);

        contactEngineComboBox = new QComboBox(MainDialog240By320);
        contactEngineComboBox->setObjectName(QString::fromUtf8("contactEngineComboBox"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(contactEngineComboBox->sizePolicy().hasHeightForWidth());
        contactEngineComboBox->setSizePolicy(sizePolicy1);

        horizontalLayout_3->addWidget(contactEngineComboBox);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        contactStatusLabel = new QLabel(MainDialog240By320);
        contactStatusLabel->setObjectName(QString::fromUtf8("contactStatusLabel"));

        horizontalLayout_4->addWidget(contactStatusLabel);


        verticalLayout->addLayout(horizontalLayout_4);


        retranslateUi(MainDialog240By320);

        QMetaObject::connectSlotsByName(MainDialog240By320);
    } // setupUi

    void retranslateUi(QWidget *MainDialog240By320)
    {
        MainDialog240By320->setWindowTitle(QApplication::translate("MainDialog240By320", "Dialog", 0, QApplication::UnicodeUTF8));
        openButton->setText(QApplication::translate("MainDialog240By320", "Open", 0, QApplication::UnicodeUTF8));
        addButton->setText(QApplication::translate("MainDialog240By320", "Add", 0, QApplication::UnicodeUTF8));
        deleteButton->setText(QApplication::translate("MainDialog240By320", "Delete", 0, QApplication::UnicodeUTF8));
        findButton->setText(QApplication::translate("MainDialog240By320", "Find", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        importButton->setToolTip(QApplication::translate("MainDialog240By320", "Import contact from vCard file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        importButton->setText(QApplication::translate("MainDialog240By320", "Import", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        exportButton->setToolTip(QApplication::translate("MainDialog240By320", "Export contact as vCard file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        exportButton->setText(QApplication::translate("MainDialog240By320", "Export", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("MainDialog240By320", "Current Backend:", 0, QApplication::UnicodeUTF8));
        contactStatusLabel->setText(QApplication::translate("MainDialog240By320", "Contact 0 of 0 (Unsaved)", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(MainDialog240By320);
    } // retranslateUi

};

namespace Ui {
    class MainDialog240By320: public Ui_MainDialog240By320 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINDIALOG_240_320_H
