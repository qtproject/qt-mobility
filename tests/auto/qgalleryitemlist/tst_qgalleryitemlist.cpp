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

#include <QtTest/QtTest>

#include <qgalleryitemlist.h>

QTM_USE_NAMESPACE

class tst_QGalleryItemList : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void metaData();
    void cursorPosition();
};

class QtTestGalleryItemList : public QGalleryItemList
{
public:
    QtTestGalleryItemList() {}

    QStringList propertyNames() const { return m_propertyNames; }
    void setPropertyNames(const QStringList &propertyNames) {
        m_propertyNames = propertyNames; m_metaData.resize(propertyNames.count()); }

    int propertyKey(const QString &propertyName) const {
        return m_propertyNames.indexOf(propertyName); }

    QGalleryProperty::Attributes propertyAttributes(int) const {
        return QGalleryProperty::Attributes(); }

    int count() const { return 1; }

    QVariant id(int) const { return QVariant(); }
    QUrl url(int) const { return QUrl(); }
    QString type(int) const { return QString(); }
    QString parentId(int) const { return QString(); }
    QList<QGalleryResource> resources(int) const { return QList<QGalleryResource>(); }
    ItemStatus status(int) const { return ItemStatus(); }

    QVariant metaData(int index, int key) const {
        return index == 0 ? m_metaData.value(key) : QVariant(); }
    void setMetaData(int index, int key, const QVariant &value) {
        if (index == 0) m_metaData.replace(key, value); }

    using QGalleryItemList::metaData;
    using QGalleryItemList::setMetaData;
private:
    QStringList m_propertyNames;
    QVector<QVariant> m_metaData;
};

void tst_QGalleryItemList::metaData()
{
    const QStringList propertyNames = QStringList()
            << QLatin1String("artist")          // Key 0
            << QLatin1String("album")           // Key 1
            << QLatin1String("genre")           // Key 2
            << QLatin1String("trackNumber");    // Key 3

    enum
    {
        Artist = 0,
        Album = 1,
        Genre = 2,
        TrackNumber = 3
    };

    const QVariant artist1 = QLatin1String("An artist");
    const QVariant artist2 = QLatin1String("Another artist");
    const QVariant album1 = QLatin1String("Self Titled");
    const QVariant album2 = QLatin1String("Greatest Hits");
    const QVariant genre = QLatin1String("Rock");
    const QVariant trackNumber = 7;

    QtTestGalleryItemList list;
    list.setPropertyNames(propertyNames);

    list.setMetaData(0, Artist, artist1);
    list.setMetaData(0, Album, album1);
    list.setMetaData(0, Genre, genre);
    list.setMetaData(0, TrackNumber, trackNumber);

    QCOMPARE(list.metaData(0, Artist), artist1);
    QCOMPARE(list.metaData(0, Album), album1);
    QCOMPARE(list.metaData(0, Genre), genre);
    QCOMPARE(list.metaData(0, TrackNumber), trackNumber);

    QCOMPARE(list.metaData(1, Artist), QVariant());
    QCOMPARE(list.metaData(1, Album), QVariant());
    QCOMPARE(list.metaData(1, Genre), QVariant());
    QCOMPARE(list.metaData(1, TrackNumber), QVariant());

    list.setMetaData(1, Artist, artist2);
    list.setMetaData(1, Album, album2);
    QCOMPARE(list.metaData(0, Artist), artist1);
    QCOMPARE(list.metaData(0, Album), album1);
    QCOMPARE(list.metaData(1, Artist), QVariant());
    QCOMPARE(list.metaData(1, Album), QVariant());

    QCOMPARE(list.metaData(0, QLatin1String("artist")), artist1);
    QCOMPARE(list.metaData(0, QLatin1String("album")), album1);
    QCOMPARE(list.metaData(0, QLatin1String("genre")), genre);
    QCOMPARE(list.metaData(0, QLatin1String("trackNumber")), trackNumber);

    QCOMPARE(list.metaData(1, QLatin1String("artist")), QVariant());
    QCOMPARE(list.metaData(1, QLatin1String("album")), QVariant());
    QCOMPARE(list.metaData(1, QLatin1String("genre")), QVariant());
    QCOMPARE(list.metaData(1, QLatin1String("trackNumber")), QVariant());

    list.setMetaData(1, QLatin1String("artist"), artist2);
    list.setMetaData(1, QLatin1String("album"), album2);
    QCOMPARE(list.metaData(0, QLatin1String("artist")), artist1);
    QCOMPARE(list.metaData(0, QLatin1String("album")), album1);
    QCOMPARE(list.metaData(1, QLatin1String("artist")), QVariant());
    QCOMPARE(list.metaData(1, QLatin1String("album")), QVariant());

    list.setMetaData(0, QLatin1String("artist"), artist2);
    list.setMetaData(0, QLatin1String("album"), album2);

    QCOMPARE(list.metaData(0, Artist), artist2);
    QCOMPARE(list.metaData(0, Album), album2);
    QCOMPARE(list.metaData(0, Genre), genre);
    QCOMPARE(list.metaData(0, TrackNumber), trackNumber);

    QCOMPARE(list.metaData(0, QLatin1String("artist")), artist2);
    QCOMPARE(list.metaData(0, QLatin1String("album")), album2);
    QCOMPARE(list.metaData(0, QLatin1String("genre")), genre);
    QCOMPARE(list.metaData(0, QLatin1String("trackNumber")), trackNumber);
}

void tst_QGalleryItemList::cursorPosition()
{
    QtTestGalleryItemList list;

    QCOMPARE(list.minimumPagedItems(), 0);
    QCOMPARE(list.cursorPosition(), 0);

    list.setCursorPosition(67);
    QCOMPARE(list.cursorPosition(), 67);

    list.setCursorPosition(-54);
    QCOMPARE(list.cursorPosition(), 0);
}

QTEST_MAIN(tst_QGalleryItemList)

#include "tst_qgalleryitemlist.moc"
