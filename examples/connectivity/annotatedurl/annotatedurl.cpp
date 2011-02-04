/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#include "annotatedurl.h"

#include <qnearfieldtarget.h>
#include <qndefmessage.h>
#include <qndefrecord.h>
#include <qndefnfctextrecord.h>
#include <qndefnfcurirecord.h>

#include <QtCore/QUrl>
#include <QtCore/QLocale>

#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QMouseEvent>
#include <QtGui/QDesktopServices>

AnnotatedUrl::AnnotatedUrl(QWidget *parent)
:   QWidget(parent)
{
    QGridLayout *grid = new QGridLayout;

    m_image = new QLabel;
    grid->addWidget(m_image, 0, 0, 2, 1, Qt::AlignCenter);

    m_title = new QLabel;
    QFont titleFont = m_title->font();
    titleFont.setBold(true);
    m_title->setFont(titleFont);
    grid->addWidget(m_title, 0, 1);

    m_url = new QLabel;
    grid->addWidget(m_url, 1, 1);

    setLayout(grid);
}

AnnotatedUrl::~AnnotatedUrl()
{
}

void AnnotatedUrl::targetDetected(QNearFieldTarget *target)
{
    if (!target->hasNdefMessage())
        return;

    connect(target, SIGNAL(ndefMessageRead(QNdefMessage)),
            this, SLOT(displayNdefMessage(QNdefMessage)));

    target->readNdefMessages();
}

void AnnotatedUrl::targetLost(QNearFieldTarget *target)
{
    target->deleteLater();
}

void AnnotatedUrl::mouseReleaseEvent(QMouseEvent *event)
{
    if (rect().contains(event->pos()))
        QDesktopServices::openUrl(QUrl(m_url->text()));
}

void AnnotatedUrl::displayNdefMessage(const QNdefMessage &message)
{
    enum {
        MatchedNone,
        MatchedFirst,
        MatchedEnglish,
        MatchedLanguage,
        MatchedLanguageAndCountry
    } bestMatch = MatchedNone;

    m_title->clear();
    m_url->clear();
    m_image->clear();

    QLocale defaultLocale;

    foreach (const QNdefRecord &record, message) {
        if (record.isRecordType<QNdefNfcTextRecord>()) {
            QNdefNfcTextRecord textRecord(record);

            QLocale locale(textRecord.locale());

            // already found best match
            if (bestMatch == MatchedLanguageAndCountry) {
                // do nothing
            } else if (bestMatch <= MatchedLanguage && locale == defaultLocale) {
                m_title->setText(textRecord.text());
                bestMatch = MatchedLanguageAndCountry;
            } else if (bestMatch <= MatchedEnglish &&
                       locale.language() == defaultLocale.language()) {
                m_title->setText(textRecord.text());
                bestMatch = MatchedLanguage;
            } else if (bestMatch <= MatchedFirst && locale.language() == QLocale::English) {
                m_title->setText(textRecord.text());
                bestMatch = MatchedEnglish;
            } else if (bestMatch == MatchedNone) {
                m_title->setText(textRecord.text());
                bestMatch = MatchedFirst;
            }
        } else if (record.isRecordType<QNdefNfcUriRecord>()) {
            QNdefNfcUriRecord uriRecord(record);

            m_url->setText(uriRecord.uri().toString());
        } else if (record.typeNameFormat() == QNdefRecord::Mime &&
                   record.type().startsWith("image/")) {
            m_image->setPixmap(QPixmap::fromImage(QImage::fromData(record.payload())));
        }
    }
}
