/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in Technology Preview License Agreement accompanying
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "servicewidget.h"

#include <servicemetadata_p.h>

#include <QDebug>
#include <QMainWindow>
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


class ServiceXmlGenerator : public QMainWindow
{
    Q_OBJECT
public:
    ServiceXmlGenerator(QWidget *parent = 0);
    ~ServiceXmlGenerator();

protected:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);

private slots:
    void serviceDataChanged();
    void loadFromXml();
    void togglePreview();
    bool saveToXml();
    void clickedNew();

private:
    bool shouldClearData();
    void refreshPreview();
    void getServiceXml(QIODevice *device);

    bool m_firstShow;
    bool m_unsavedData;
    ServiceWidget *m_serviceInfo;

    QPushButton *m_buttonPreview;
    QPlainTextEdit *m_previewEdit;
};

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
    clickedNew();

    m_previewEdit->setWindowTitle(tr("XML Preview"));
    setWindowTitle(tr("Service XML Generator"));
}

ServiceXmlGenerator::~ServiceXmlGenerator()
{
    delete m_previewEdit;
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
    QString fileName = QFileDialog::getOpenFileName(0, tr("Choose XML file"), QString(), "*.xml");
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        ServiceMetaData data(&file);
        if (data.extractMetadata()) {
            m_serviceInfo->load(data);
            return;
        }
    }

    QMessageBox::warning(0, tr("Error loading XML file"),
            tr("Cannot read contents of %1", "file name").arg(fileName));
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
    bool serviceInfoOk = m_serviceInfo->validate();
    if (!serviceInfoOk) {
        QMessageBox::information(0, tr("Service incomplete"),
                tr("One or more mandatory fields are incomplete."));
        return false;
    }

    QString fileName = QFileDialog::getSaveFileName(0, tr("Save to file"));
    if (fileName.isEmpty())
        return false;

    refreshPreview();
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

    ServiceMetaData data(0);
    m_serviceInfo->load(data);
    m_unsavedData = false;
}

bool ServiceXmlGenerator::shouldClearData()
{
    if (m_unsavedData) {
        QMessageBox msg;
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
    win.show();

    return app.exec();
}

#include "servicexmlgen.moc"
