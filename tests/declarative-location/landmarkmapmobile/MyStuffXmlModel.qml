import QtQuick 1.0

 XmlListModel {
     source: "mystuff.xml"
     query: "/item"
     XmlRole { name: "itemTitle"; query: "title/string()" }
     XmlRole { name: "itemDate"; query: "pubDate/string()" }
     XmlRole { name: "itemLongitude"; query: "longitude/number()" }
     XmlRole { name: "itemLatitude"; query: "latitude/number()" }
 }