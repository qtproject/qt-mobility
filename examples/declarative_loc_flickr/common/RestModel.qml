/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

import Qt 4.7

XmlListModel {
    property double latitude: 0
    property double longtitude: 0
    // The flickr rest API with position data
    source: "http://api.flickr.com/services/rest/?min_taken_date=2000-01-01+0:00:00&extras=date_taken&method=flickr.photos.search&per_page=30&sort=date-taken-desc&api_key=e36784df8a03fea04c22ed93318b291c&lat="+latitude+"&lon="+longtitude;
    query: "/rsp/photos/photo"
    XmlRole { name: "title"; query: "@title/string()" }
    XmlRole { name: "datetaken"; query: "@datetaken/string()" }
    XmlRole { name: "farm"; query: "@farm/string()" }
    XmlRole { name: "server"; query: "@server/string()" }
    XmlRole { name: "id"; query: "@id/string()" }
    XmlRole { name: "secret"; query: "@secret/string()" }
}

/*

QString url = "http://farm";
            url.append(child.attribute("farm"));
            url.append(".static.flickr.com/");
            url.append(child.attribute("server"));
            url.append("/");
            url.append(child.attribute("id"));
            url.append("_");
            url.append(child.attribute("secret"));


XmlListModel {
    property string tags : ""

    function commasep(x)
    {
        return x.replace(' ',',');
    }

    //m_latitude = 61.4500;
    //m_longitude = 23.8502;
    //urlstring.append("&lat=");
    //urlstring.append(QString::number(m_latitude));
    //urlstring.append("&lon=");

    source: "http://api.flickr.com/services/rest/photos_public.gne?"+(tags ? "tags="+commasep(tags)+"&" : "")+"format=rss2"
    // +"&lat=61"+"&lon=23"
    // original source: "http://api.flickr.com/services/feeds/photos_public.gne?"+(tags ? "tags="+commasep(tags)+"&" : "")+"format=rss2"
    query: "/rss/channel/item"
    namespaceDeclarations: "declare namespace media=\"http://search.yahoo.com/mrss/\";"

    XmlRole { name: "title"; query: "title/string()" }
    XmlRole { name: "imagePath"; query: "media:thumbnail/@url/string()" }
    XmlRole { name: "url"; query: "media:content/@url/string()" }
    XmlRole { name: "description"; query: "description/string()" }
    XmlRole { name: "tags"; query: "media:category/string()" }
    XmlRole { name: "photoWidth"; query: "media:content/@width/string()" }
    XmlRole { name: "photoHeight"; query: "media:content/@height/string()" }
    XmlRole { name: "photoType"; query: "media:content/@type/string()" }
    XmlRole { name: "photoAuthor"; query: "author/string()" }
    XmlRole { name: "photoDate"; query: "pubDate/string()" }
}
*/
