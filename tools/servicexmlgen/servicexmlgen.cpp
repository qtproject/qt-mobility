/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "servicexmlgen.h"
#include "servicewidget.h"
#include "errorcollector.h"

#include <servicemetadata_p.h>

#include <QDebug>
#include <QVBoxLayout>
#include <QApplication>
#include <QPushButton>
#include <QLabel>
#include <QXmlStreamWriter>
#include <QFileDialog>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QDesktopWidget>
#include <QBuffer>
#include <QMenuBar>

QTM_USE_NAMESPACE

static const QString serviceMetaDataErrorString(int error)
{
    switch (error) {
        case ServiceMetaData::SFW_ERROR_NO_SERVICE:
            return QObject::tr("XML does not contain <service> node.");
        case ServiceMetaData::SFW_ERROR_NO_SERVICE_NAME:
            return QObject::tr("XML does not specify service name.");
        case ServiceMetaData::SFW_ERROR_NO_SERVICE_FILEPATH:
            return QObject::tr("XML does not specify service resource location.");
        case ServiceMetaData::SFW_ERROR_NO_SERVICE_INTERFACE:
            return QObject::tr("XML does not contain any interfaces.");
        case ServiceMetaData::SFW_ERROR_NO_INTERFACE_VERSION:
            return QObject::tr("XML specifies an interface without a version.");
        case ServiceMetaData::SFW_ERROR_NO_INTERFACE_NAME:
            return QObject::tr("XML specifies an interface without a name.");
        case ServiceMetaData::SFW_ERROR_UNABLE_TO_OPEN_FILE:
            return QObject::tr("Cannot open XML file.");
        case ServiceMetaData::SFW_ERROR_INVALID_XML_FILE:
            return QObject::tr("The file's XML data is not valid.");
        case ServiceMetaData::SFW_ERROR_PARSE_SERVICE:
            return QObject::tr("Unable to parse service details.");
        case ServiceMetaData::SFW_ERROR_PARSE_INTERFACE:
            return QObject::tr("Unable to parse interface details.");
        case ServiceMetaData::SFW_ERROR_DUPLICATED_INTERFACE:
            return QObject::tr("XML contains duplicate interface.");
        case ServiceMetaData::SFW_ERROR_INVALID_VERSION:
            return QObject::tr("XML contains invalid interface version.");
        case ServiceMetaData::SFW_ERROR_DUPLICATED_TAG:
            return QObject::tr("XML contains a duplicate tag.");
        case ServiceMetaData::SFW_ERROR_INVALID_CUSTOM_TAG:
            return QObject::tr("XML contains an invalid custom property.");
        case ServiceMetaData::SFW_ERROR_DUPLICATED_CUSTOM_KEY:
            return QObject::tr("XML contains a duplicate custom property.");
        default:
            return QString();
    }
}

ServiceXmlGenerator::ServiceXmlGenerator(QWidget *parent)
    : QMainWindow(parent),
      m_firstShow(true),
      m_unsavedData(false),
      m_serviceInfo(new ServiceWidget),
      m_buttonPreview(0),
      m_previewEdit(new QPlainTextEdit)
{
    connect(m_serviceInfo, SIGNAL(dataChanged()), SLOT(serviceDataChanged()));

    QPushButton *buttonLoad = new QPushButton(tr("Load from XML..."));
    buttonLoad->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    connect(buttonLoad, SIGNAL(clicked()), this, SLOT(loadFromXml()));

    m_buttonPreview = new QPushButton(tr("Show preview"));
    m_buttonPreview->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    connect(m_buttonPreview, SIGNAL(clicked()), this, SLOT(togglePreview()));

    QPushButton *buttonSave = new QPushButton(tr("Save as XML..."));
    buttonSave->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    connect(buttonSave, SIGNAL(clicked()), this, SLOT(saveToXml()));

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&New"), this, SLOT(clickedNew()), QKeySequence::New);

    QHBoxLayout *buttons = new QHBoxLayout;
    buttons->addWidget(buttonLoad);
    buttons->addStretch();
    buttons->addWidget(m_buttonPreview);
    buttons->addWidget(buttonSave);
    QWidget *buttonsWidget = new QWidget;
    buttonsWidget->setLayout(buttons);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_serviceInfo);
    mainLayout->addWidget(buttonsWidget);
    QWidget *main = new QWidget;
    main->setLayout(mainLayout);
    setCentralWidget(main);

    m_previewEdit->setReadOnly(true);
    m_previewEdit->installEventFilter(this);
    clickedNew();

    m_previewEdit->setWindowTitle(tr("XML Preview"));
    setWindowTitle(tr("Service XML Generator"));
}

ServiceXmlGenerator::~ServiceXmlGenerator()
{
    delete m_previewEdit;
}

bool ServiceXmlGenerator::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_previewEdit && event->type() == QEvent::Hide ) {
        m_buttonPreview->setText(tr("Show preview"));
    }
    return false;
}

void ServiceXmlGenerator::showEvent(QShowEvent *event)
{
    if (m_firstShow) {
        m_firstShow = false;

        resize(width(), minimumHeight());
        m_previewEdit->resize(width() * 0.75, height() / 2);

        int space = 30;
        int totalWidth = width() + space + m_previewEdit->width();

        QDesktopWidget desktop;
        move((desktop.width() - totalWidth) / 2, (desktop.height() - height()) / 2);
        m_previewEdit->move(x() + width() + space, (desktop.height() - m_previewEdit->height()) / 2);

        togglePreview();
    }

    QMainWindow::showEvent(event);
}

void ServiceXmlGenerator::closeEvent(QCloseEvent *event)
{
    if (shouldClearData()) {
        m_previewEdit->close();
        event->accept();
    } else {
        event->ignore();
    }
}

void ServiceXmlGenerator::serviceDataChanged()
{
    m_unsavedData = true;
    refreshPreview();
}

void ServiceXmlGenerator::loadFromXml()
{
    loadFromXml(QString());
}

void ServiceXmlGenerator::loadFromXml(const QString& f)
{
    if (!shouldClearData())
        return;

    QString fileName = f;
    if (fileName.isEmpty())
        fileName = QFileDialog::getOpenFileName(0, tr("Open XML file"), QString(), "*.xml");
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    ServiceMetaData data(0);
    if (file.open(QIODevice::ReadOnly)) {
        data.setDevice(&file);
        if (data.extractMetadata()) {
            m_serviceInfo->load(data.parseResults());
            m_unsavedData = false;
            return;
        }
    }

    QString msg = tr("Cannot read contents of %1.", "file name").arg(fileName);
    QString details = serviceMetaDataErrorString(data.getLatestError());
    if (!details.isEmpty())
        msg += "\n\n" + tr("Error: ") + details;
    QMessageBox::warning(0, tr("Error loading XML file"), msg);
}

void ServiceXmlGenerator::togglePreview()
{
    if (m_previewEdit->isVisible()) {
        m_buttonPreview->setText(tr("Show preview"));
        m_previewEdit->hide();
    } else {
        refreshPreview();
        m_buttonPreview->setText(tr("Hide preview"));
        m_previewEdit->show();
    }
}

bool ServiceXmlGenerator::saveToXml()
{
    ErrorCollector errors;
    m_serviceInfo->validate(&errors);
    if (errors.errorCount() > 0) {
        QMessageBox::information(0, tr("Invalid data"), errors.errorMessage());
        return false;
    }

    refreshPreview();

    QString fileName = QFileDialog::getSaveFileName(0, tr("Save to file"));
    if (fileName.isEmpty())
        return false;

    QFile out(fileName);
    if (!out.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(0, tr("Write error"),
                tr("Cannot open %1 for writing", "file name").arg(fileName));
        return false;
    }

    qint64 len = out.write(m_previewEdit->toPlainText().toLatin1());
    out.close();
    if (len < 0) {
        QMessageBox::warning(0, tr("Write error"),
                tr("Cannot write data to %1", "file name").arg(fileName));
        return false;
    }

    m_unsavedData = false;
    QMessageBox::information(0, tr("File saved"),
            tr("Saved successfully to %1.", "file name").arg(fileName));
    return true;
}

void ServiceXmlGenerator::clickedNew()
{
    if (!shouldClearData())
        return;

    m_serviceInfo->load(ServiceMetaDataResults());
    m_unsavedData = false;
}

bool ServiceXmlGenerator::shouldClearData()
{
    if (m_unsavedData) {
        QMessageBox msg;
        msg.setWindowTitle(tr("Save changes?"));
        msg.setText("The service has been modified.");
        msg.setInformativeText("Do you want to save your changes?");
        msg.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msg.setDefaultButton(QMessageBox::Save);
        switch (msg.exec()) {
            case QMessageBox::Save:
                if (!saveToXml())
                    return false;
                return true;
            case QMessageBox::Cancel:
                return false;
            default:    // Discard
                break;
        }
    }
    return true;
}

void ServiceXmlGenerator::refreshPreview()
{
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    getServiceXml(&buffer);
    buffer.close();
    m_previewEdit->setPlainText(buffer.data());
}

void ServiceXmlGenerator::getServiceXml(QIODevice *device)
{
    QXmlStreamWriter writer(device);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement(QLatin1String("service"));
    m_serviceInfo->writeXml(&writer);
    writer.writeEndElement();
    writer.writeEndDocument();
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ServiceXmlGenerator win;
    if (argc == 2)
        win.loadFromXml(app.arguments().at(1));
    win.show();

    return app.exec();
}

