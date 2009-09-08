/********************************************************************************
** Form generated from reading ui file 'contactdetails_240_320.ui'
**
** Created: Tue Sep 8 11:41:40 2009
**      by: Qt User Interface Compiler version 4.6.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_CONTACTDETAILS_240_320_H
#define UI_CONTACTDETAILS_240_320_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ContactDetails240By320
{
public:
    QVBoxLayout *verticalLayout;
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
    QHBoxLayout *horizontalLayout_6;
    QSpacerItem *horizontalSpacer;
    QPushButton *cancelButton;
    QPushButton *saveButton;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QDialog *ContactDetails240By320)
    {
        if (ContactDetails240By320->objectName().isEmpty())
            ContactDetails240By320->setObjectName(QString::fromUtf8("ContactDetails240By320"));
        ContactDetails240By320->resize(240, 320);
        verticalLayout = new QVBoxLayout(ContactDetails240By320);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(ContactDetails240By320);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        nameEdit = new QLineEdit(ContactDetails240By320);
        nameEdit->setObjectName(QString::fromUtf8("nameEdit"));

        horizontalLayout->addWidget(nameEdit);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_2 = new QLabel(ContactDetails240By320);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy);

        horizontalLayout_2->addWidget(label_2);

        avatarButton = new QPushButton(ContactDetails240By320);
        avatarButton->setObjectName(QString::fromUtf8("avatarButton"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(avatarButton->sizePolicy().hasHeightForWidth());
        avatarButton->setSizePolicy(sizePolicy1);
        avatarButton->setMinimumSize(QSize(0, 30));

        horizontalLayout_2->addWidget(avatarButton);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_3 = new QLabel(ContactDetails240By320);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_3->addWidget(label_3);

        emailEdit = new QLineEdit(ContactDetails240By320);
        emailEdit->setObjectName(QString::fromUtf8("emailEdit"));

        horizontalLayout_3->addWidget(emailEdit);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_4 = new QLabel(ContactDetails240By320);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout_4->addWidget(label_4);

        homePhoneEdit = new QLineEdit(ContactDetails240By320);
        homePhoneEdit->setObjectName(QString::fromUtf8("homePhoneEdit"));

        horizontalLayout_4->addWidget(homePhoneEdit);


        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_5 = new QLabel(ContactDetails240By320);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout_5->addWidget(label_5);

        workPhoneEdit = new QLineEdit(ContactDetails240By320);
        workPhoneEdit->setObjectName(QString::fromUtf8("workPhoneEdit"));

        horizontalLayout_5->addWidget(workPhoneEdit);


        verticalLayout->addLayout(horizontalLayout_5);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        label_8 = new QLabel(ContactDetails240By320);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        horizontalLayout_8->addWidget(label_8);

        mobilePhoneEdit = new QLineEdit(ContactDetails240By320);
        mobilePhoneEdit->setObjectName(QString::fromUtf8("mobilePhoneEdit"));

        horizontalLayout_8->addWidget(mobilePhoneEdit);


        verticalLayout->addLayout(horizontalLayout_8);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        label_6 = new QLabel(ContactDetails240By320);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        horizontalLayout_7->addWidget(label_6);

        addressEdit = new QPlainTextEdit(ContactDetails240By320);
        addressEdit->setObjectName(QString::fromUtf8("addressEdit"));

        horizontalLayout_7->addWidget(addressEdit);


        verticalLayout->addLayout(horizontalLayout_7);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer);

        cancelButton = new QPushButton(ContactDetails240By320);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

        horizontalLayout_6->addWidget(cancelButton);

        saveButton = new QPushButton(ContactDetails240By320);
        saveButton->setObjectName(QString::fromUtf8("saveButton"));

        horizontalLayout_6->addWidget(saveButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout_6);


        retranslateUi(ContactDetails240By320);

        QMetaObject::connectSlotsByName(ContactDetails240By320);
    } // setupUi

    void retranslateUi(QDialog *ContactDetails240By320)
    {
        ContactDetails240By320->setWindowTitle(QApplication::translate("ContactDetails240By320", "Contact details", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ContactDetails240By320", "Name:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("ContactDetails240By320", "Avatar:", 0, QApplication::UnicodeUTF8));
        avatarButton->setText(QApplication::translate("ContactDetails240By320", "No image selected", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("ContactDetails240By320", "Email:", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("ContactDetails240By320", "Home Phone:", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("ContactDetails240By320", "Work Phone:", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("ContactDetails240By320", "Mobile Phone:", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("ContactDetails240By320", "Address", 0, QApplication::UnicodeUTF8));
        cancelButton->setText(QApplication::translate("ContactDetails240By320", "Cancel", 0, QApplication::UnicodeUTF8));
        saveButton->setText(QApplication::translate("ContactDetails240By320", "Save", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(ContactDetails240By320);
    } // retranslateUi

};

namespace Ui {
    class ContactDetails240By320: public Ui_ContactDetails240By320 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTACTDETAILS_240_320_H
