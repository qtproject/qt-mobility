/********************************************************************************
** Form generated from reading UI file 'dialog.ui'
**
** Created: Mon 30. Aug 12:54:42 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_H
#define UI_DIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QPushButton *pushButton3;
    QPushButton *pushButton4;
    QPushButton *pushButton5;
    QPushButton *pushButton6;
    QPushButton *pushButton7;
    QPushButton *pushButton8;
    QPushButton *pushButton9;
    QPushButton *pushButton0;
    QPushButton *pushButtonDial;
    QPushButton *pushButton2;
    QPushButton *pushButton1;
    QPushButton *pushButtonStatus;
    QLineEdit *lineEditPhoneNumber;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->resize(265, 272);
        verticalLayout = new QVBoxLayout(Dialog);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        pushButton3 = new QPushButton(Dialog);
        pushButton3->setObjectName(QString::fromUtf8("pushButton3"));

        gridLayout->addWidget(pushButton3, 0, 2, 1, 1);

        pushButton4 = new QPushButton(Dialog);
        pushButton4->setObjectName(QString::fromUtf8("pushButton4"));

        gridLayout->addWidget(pushButton4, 1, 0, 1, 1);

        pushButton5 = new QPushButton(Dialog);
        pushButton5->setObjectName(QString::fromUtf8("pushButton5"));

        gridLayout->addWidget(pushButton5, 1, 1, 1, 1);

        pushButton6 = new QPushButton(Dialog);
        pushButton6->setObjectName(QString::fromUtf8("pushButton6"));

        gridLayout->addWidget(pushButton6, 1, 2, 1, 1);

        pushButton7 = new QPushButton(Dialog);
        pushButton7->setObjectName(QString::fromUtf8("pushButton7"));

        gridLayout->addWidget(pushButton7, 2, 0, 1, 1);

        pushButton8 = new QPushButton(Dialog);
        pushButton8->setObjectName(QString::fromUtf8("pushButton8"));

        gridLayout->addWidget(pushButton8, 2, 1, 1, 1);

        pushButton9 = new QPushButton(Dialog);
        pushButton9->setObjectName(QString::fromUtf8("pushButton9"));

        gridLayout->addWidget(pushButton9, 2, 2, 1, 1);

        pushButton0 = new QPushButton(Dialog);
        pushButton0->setObjectName(QString::fromUtf8("pushButton0"));
        pushButton0->setFlat(false);

        gridLayout->addWidget(pushButton0, 3, 0, 1, 1);

        pushButtonDial = new QPushButton(Dialog);
        pushButtonDial->setObjectName(QString::fromUtf8("pushButtonDial"));

        gridLayout->addWidget(pushButtonDial, 3, 1, 1, 1);

        pushButton2 = new QPushButton(Dialog);
        pushButton2->setObjectName(QString::fromUtf8("pushButton2"));

        gridLayout->addWidget(pushButton2, 0, 1, 1, 1);

        pushButton1 = new QPushButton(Dialog);
        pushButton1->setObjectName(QString::fromUtf8("pushButton1"));

        gridLayout->addWidget(pushButton1, 0, 0, 1, 1);

        pushButtonStatus = new QPushButton(Dialog);
        pushButtonStatus->setObjectName(QString::fromUtf8("pushButtonStatus"));

        gridLayout->addWidget(pushButtonStatus, 3, 2, 1, 1);


        verticalLayout->addLayout(gridLayout);

        lineEditPhoneNumber = new QLineEdit(Dialog);
        lineEditPhoneNumber->setObjectName(QString::fromUtf8("lineEditPhoneNumber"));

        verticalLayout->addWidget(lineEditPhoneNumber);


        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialer", 0, QApplication::UnicodeUTF8));
        pushButton3->setText(QApplication::translate("Dialog", "3", 0, QApplication::UnicodeUTF8));
        pushButton4->setText(QApplication::translate("Dialog", "4", 0, QApplication::UnicodeUTF8));
        pushButton5->setText(QApplication::translate("Dialog", "5", 0, QApplication::UnicodeUTF8));
        pushButton6->setText(QApplication::translate("Dialog", "6", 0, QApplication::UnicodeUTF8));
        pushButton7->setText(QApplication::translate("Dialog", "7", 0, QApplication::UnicodeUTF8));
        pushButton8->setText(QApplication::translate("Dialog", "8", 0, QApplication::UnicodeUTF8));
        pushButton9->setText(QApplication::translate("Dialog", "9", 0, QApplication::UnicodeUTF8));
        pushButton0->setText(QApplication::translate("Dialog", "0", 0, QApplication::UnicodeUTF8));
        pushButtonDial->setText(QApplication::translate("Dialog", "dial", 0, QApplication::UnicodeUTF8));
        pushButton2->setText(QApplication::translate("Dialog", "2", 0, QApplication::UnicodeUTF8));
        pushButton1->setText(QApplication::translate("Dialog", "1", 0, QApplication::UnicodeUTF8));
        pushButtonStatus->setText(QApplication::translate("Dialog", "Status", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_H
