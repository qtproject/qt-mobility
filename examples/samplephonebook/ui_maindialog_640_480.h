/********************************************************************************
** Form generated from reading ui file 'maindialog_640_480.ui'
**
** Created: Tue Sep 8 11:41:40 2009
**      by: Qt User Interface Compiler version 4.6.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MAINDIALOG_640_480_H
#define UI_MAINDIALOG_640_480_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainDialog640By480
{
public:
    QGridLayout *gridLayout;
    QListWidget *contactListWidget;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *nameEdit;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QPushButton *avatarButton;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QLineEdit *emailEdit;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_4;
    QLineEdit *homePhoneEdit;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_5;
    QLineEdit *workPhoneEdit;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_8;
    QLineEdit *mobilePhoneEdit;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_6;
    QPlainTextEdit *addressEdit;
    QSpacerItem *verticalSpacer;
    QVBoxLayout *verticalLayout;
    QPushButton *addButton;
    QPushButton *saveButton;
    QPushButton *deleteButton;
    QPushButton *findButton;
    QPushButton *importButton;
    QPushButton *exportButton;
    QPushButton *quitButton;
    QSpacerItem *verticalSpacer_2;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_7;
    QComboBox *contactEngineComboBox;
    QLabel *contactStatusLabel;

    void setupUi(QWidget *MainDialog640By480)
    {
        if (MainDialog640By480->objectName().isEmpty())
            MainDialog640By480->setObjectName(QString::fromUtf8("MainDialog640By480"));
        MainDialog640By480->resize(640, 480);
        QPalette palette;
        QBrush brush(QColor(232, 231, 230, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush);
        QBrush brush1(QColor(255, 255, 255, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        QBrush brush2(QColor(130, 129, 129, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush2);
        QBrush brush3(QColor(170, 170, 170, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush3);
        MainDialog640By480->setPalette(palette);
        gridLayout = new QGridLayout(MainDialog640By480);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        contactListWidget = new QListWidget(MainDialog640By480);
        contactListWidget->setObjectName(QString::fromUtf8("contactListWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(contactListWidget->sizePolicy().hasHeightForWidth());
        contactListWidget->setSizePolicy(sizePolicy);

        gridLayout->addWidget(contactListWidget, 0, 0, 4, 1);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(MainDialog640By480);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        nameEdit = new QLineEdit(MainDialog640By480);
        nameEdit->setObjectName(QString::fromUtf8("nameEdit"));

        horizontalLayout->addWidget(nameEdit);


        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_2 = new QLabel(MainDialog640By480);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        QSizePolicy sizePolicy1(QSizePolicy::Ignored, QSizePolicy::Ignored);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy1);

        horizontalLayout_2->addWidget(label_2);

        avatarButton = new QPushButton(MainDialog640By480);
        avatarButton->setObjectName(QString::fromUtf8("avatarButton"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(avatarButton->sizePolicy().hasHeightForWidth());
        avatarButton->setSizePolicy(sizePolicy2);
        avatarButton->setMinimumSize(QSize(0, 50));

        horizontalLayout_2->addWidget(avatarButton);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_3 = new QLabel(MainDialog640By480);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_3->addWidget(label_3);

        emailEdit = new QLineEdit(MainDialog640By480);
        emailEdit->setObjectName(QString::fromUtf8("emailEdit"));

        horizontalLayout_3->addWidget(emailEdit);


        verticalLayout_2->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_4 = new QLabel(MainDialog640By480);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout_4->addWidget(label_4);

        homePhoneEdit = new QLineEdit(MainDialog640By480);
        homePhoneEdit->setObjectName(QString::fromUtf8("homePhoneEdit"));

        horizontalLayout_4->addWidget(homePhoneEdit);


        verticalLayout_2->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_5 = new QLabel(MainDialog640By480);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout_5->addWidget(label_5);

        workPhoneEdit = new QLineEdit(MainDialog640By480);
        workPhoneEdit->setObjectName(QString::fromUtf8("workPhoneEdit"));

        horizontalLayout_5->addWidget(workPhoneEdit);


        verticalLayout_2->addLayout(horizontalLayout_5);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        label_8 = new QLabel(MainDialog640By480);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        horizontalLayout_8->addWidget(label_8);

        mobilePhoneEdit = new QLineEdit(MainDialog640By480);
        mobilePhoneEdit->setObjectName(QString::fromUtf8("mobilePhoneEdit"));

        horizontalLayout_8->addWidget(mobilePhoneEdit);


        verticalLayout_2->addLayout(horizontalLayout_8);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        label_6 = new QLabel(MainDialog640By480);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        horizontalLayout_7->addWidget(label_6);

        addressEdit = new QPlainTextEdit(MainDialog640By480);
        addressEdit->setObjectName(QString::fromUtf8("addressEdit"));

        horizontalLayout_7->addWidget(addressEdit);


        verticalLayout_2->addLayout(horizontalLayout_7);


        gridLayout->addLayout(verticalLayout_2, 0, 1, 3, 1);

        verticalSpacer = new QSpacerItem(20, 52, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 0, 2, 1, 1);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        addButton = new QPushButton(MainDialog640By480);
        addButton->setObjectName(QString::fromUtf8("addButton"));

        verticalLayout->addWidget(addButton);

        saveButton = new QPushButton(MainDialog640By480);
        saveButton->setObjectName(QString::fromUtf8("saveButton"));

        verticalLayout->addWidget(saveButton);

        deleteButton = new QPushButton(MainDialog640By480);
        deleteButton->setObjectName(QString::fromUtf8("deleteButton"));

        verticalLayout->addWidget(deleteButton);

        findButton = new QPushButton(MainDialog640By480);
        findButton->setObjectName(QString::fromUtf8("findButton"));

        verticalLayout->addWidget(findButton);

        importButton = new QPushButton(MainDialog640By480);
        importButton->setObjectName(QString::fromUtf8("importButton"));

        verticalLayout->addWidget(importButton);

        exportButton = new QPushButton(MainDialog640By480);
        exportButton->setObjectName(QString::fromUtf8("exportButton"));

        verticalLayout->addWidget(exportButton);

        quitButton = new QPushButton(MainDialog640By480);
        quitButton->setObjectName(QString::fromUtf8("quitButton"));

        verticalLayout->addWidget(quitButton);


        gridLayout->addLayout(verticalLayout, 1, 2, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 112, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_2, 2, 2, 1, 1);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        label_7 = new QLabel(MainDialog640By480);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        horizontalLayout_6->addWidget(label_7);

        contactEngineComboBox = new QComboBox(MainDialog640By480);
        contactEngineComboBox->setObjectName(QString::fromUtf8("contactEngineComboBox"));

        horizontalLayout_6->addWidget(contactEngineComboBox);

        contactStatusLabel = new QLabel(MainDialog640By480);
        contactStatusLabel->setObjectName(QString::fromUtf8("contactStatusLabel"));

        horizontalLayout_6->addWidget(contactStatusLabel);


        gridLayout->addLayout(horizontalLayout_6, 3, 1, 1, 2);


        retranslateUi(MainDialog640By480);

        QMetaObject::connectSlotsByName(MainDialog640By480);
    } // setupUi

    void retranslateUi(QWidget *MainDialog640By480)
    {
        MainDialog640By480->setWindowTitle(QApplication::translate("MainDialog640By480", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainDialog640By480", "Name:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainDialog640By480", "Avatar:", 0, QApplication::UnicodeUTF8));
        avatarButton->setText(QApplication::translate("MainDialog640By480", "No image selected", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainDialog640By480", "Email:", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainDialog640By480", "Home Phone:", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainDialog640By480", "Work Phone:", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("MainDialog640By480", "Mobile Phone:", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MainDialog640By480", "Address", 0, QApplication::UnicodeUTF8));
        addButton->setText(QApplication::translate("MainDialog640By480", "Add", 0, QApplication::UnicodeUTF8));
        saveButton->setText(QApplication::translate("MainDialog640By480", "Save", 0, QApplication::UnicodeUTF8));
        deleteButton->setText(QApplication::translate("MainDialog640By480", "Delete", 0, QApplication::UnicodeUTF8));
        findButton->setText(QApplication::translate("MainDialog640By480", "Find", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        importButton->setToolTip(QApplication::translate("MainDialog640By480", "Import contact from vCard file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        importButton->setText(QApplication::translate("MainDialog640By480", "Import", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        exportButton->setToolTip(QApplication::translate("MainDialog640By480", "Export contact as vCard file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        exportButton->setText(QApplication::translate("MainDialog640By480", "Export", 0, QApplication::UnicodeUTF8));
        quitButton->setText(QApplication::translate("MainDialog640By480", "Quit", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("MainDialog640By480", "Current Backend:", 0, QApplication::UnicodeUTF8));
        contactStatusLabel->setText(QApplication::translate("MainDialog640By480", "Contact 0 of 0 (Unsaved)", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(MainDialog640By480);
    } // retranslateUi

};

namespace Ui {
    class MainDialog640By480: public Ui_MainDialog640By480 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINDIALOG_640_480_H
