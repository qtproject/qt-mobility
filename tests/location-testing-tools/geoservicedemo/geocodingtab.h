/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef GEOCODINGTAB_H_
#define GEOCODINGTAB_H_

#include <QWidget>
#include <QDialog>

#include <qgeosearchmanager.h>

class QTreeWidget;
class QLineEdit;
class QPushButton;
class QComboBox;
class QGroupBox;

QTM_USE_NAMESPACE

class GeoCodingInputDialog: public QDialog
{
    Q_OBJECT
public:
    enum GeocodingType {
        GeocodingOneBox,
        GeocodingLandmark,
        GeoCodingStructured
    };

    GeoCodingInputDialog(QString &obloc, QGeoAddress &address, GeocodingType &type,QWidget *parent = 0);

private slots:
    void accept();
    void oneBoxSearchToogled(bool on);
    void addressSeachToogled(bool on);

private:
    QString &m_oblocStr;
    QGeoAddress &m_address;
    GeocodingType &m_type;
    QLineEdit *m_obloc;
    QComboBox *m_oneBoxType;
    QLineEdit *m_country;
    QLineEdit *m_state;
    QLineEdit *m_city;
    QLineEdit *m_zip;
    QLineEdit *m_street;
    QGroupBox *m_gbOneBox;
    QGroupBox *m_gbAddress;
};

class GeocodingTab: public QWidget
{
    Q_OBJECT

public:
    GeocodingTab(QWidget *parent = 0);
    ~GeocodingTab();

public slots:
    void initialize(QGeoSearchManager *searchManager);

private slots:
    void on_btnRequest_clicked();
    void replyFinished(QGeoSearchReply* reply);
    void resultsError(QGeoSearchReply* reply, QGeoSearchReply::Error errorCode, QString errorString);

private:
    QGeoSearchManager *m_searchManager;
    QString m_oblocStr;
    QGeoAddress m_address;
    GeoCodingInputDialog::GeocodingType m_type;
    QTreeWidget *m_resultTree;
    QPushButton *m_requestBtn;
};

#endif // GEOCODINGTAB_H_
