#include "placepresenter.h"

PlacePresenter::PlacePresenter(QTreeWidget *treeWidget, const QGeocodingReply* codingReply)
        : GeoPresenter(treeWidget), codingReply(codingReply)
{
}

void PlacePresenter::show()
{
    treeWidget->clear();
    QTreeWidgetItem* top = showPlaces();
    QList<QGeoLocation> places = codingReply->places();

    for (int i = 0; i < places.length(); i++) {
        showPlace(top, places[i]);
    }
}

QTreeWidgetItem* PlacePresenter::showPlaces()
{
    QTreeWidgetItem* top = new QTreeWidgetItem(treeWidget);
    top->setText(0, "places");

    QTreeWidgetItem* prop = new QTreeWidgetItem(top);
    prop->setText(0, "result");
    prop->setText(1, QString().setNum((quint16) codingReply->resultCode()));

    prop = new QTreeWidgetItem(top);
    prop->setText(0, "description");
    prop->setText(1, codingReply->resultDescription());

    prop = new QTreeWidgetItem(top);
    prop->setText(0, "count");
    prop->setText(1, QString().setNum(codingReply->count()));

    return top;
}

void PlacePresenter::showPlace(QTreeWidgetItem* top, const QGeoLocation& place)
{
    QTreeWidgetItem* prop;
    QTreeWidgetItem* placeItem = new QTreeWidgetItem(top);
    placeItem->setText(0, "place");

    QString s = place.title();

    if (!s.isEmpty()) {
        prop = new QTreeWidgetItem(placeItem);
        prop->setText(0, "title");
        prop->setText(1, s);
    }

    s = place.language();

    if (!s.isEmpty()) {
        prop = new QTreeWidgetItem(placeItem);
        prop->setText(0, "language");
        prop->setText(1, s);
    }

    if (!place.boundingBox().isEmpty())
        showBoundingBox(placeItem, place.boundingBox());

    prop = new QTreeWidgetItem(placeItem);
    prop->setText(0, "position");
    prop->setText(1, formatGeoCoordinate(place.position()));

    if (!place.address().isEmpty()) {
        prop = new QTreeWidgetItem(placeItem);
        prop->setText(0, "address");

        showAddress(prop, place.address());
    }

    /*
    if ( !place->alternatives().isEmpty() )
    {
        prop = new QTreeWidgetItem(placeItem);
        prop->setText( 0, "alternatives" );

        showAddress(prop, place->alternatives());
    }
    */
}

void PlacePresenter::showAddress(QTreeWidgetItem* addrItem, const QAddress& address)
{
    QTreeWidgetItem* prop;
    QString s = address.country();

    if (!s.isEmpty()) {
        prop = new QTreeWidgetItem(addrItem);
        prop->setText(0, "country");
        prop->setText(1, s);
    }

    s = address.countryCode();

    if (!s.isEmpty()) {
        prop = new QTreeWidgetItem(addrItem);
        prop->setText(0, "country code");
        prop->setText(1, s);
    }

    s = address.state();

    if (!s.isEmpty()) {
        prop = new QTreeWidgetItem(addrItem);
        prop->setText(0, "state");
        prop->setText(1, s);
    }

    s = address.county();

    if (!s.isEmpty()) {
        prop = new QTreeWidgetItem(addrItem);
        prop->setText(0, "county");
        prop->setText(1, s);
    }

    s = address.city();

    if (!s.isEmpty()) {
        prop = new QTreeWidgetItem(addrItem);
        prop->setText(0, "city");
        prop->setText(1, s);
    }

    s = address.district();

    if (!s.isEmpty()) {
        prop = new QTreeWidgetItem(addrItem);
        prop->setText(0, "district");
        prop->setText(1, s);
    }

    s = address.thoroughfareName();

    if (!s.isEmpty()) {
        prop = new QTreeWidgetItem(addrItem);
        prop->setText(0, "thoroughfare name");
        prop->setText(1, s);
    }

    s = address.thoroughfareNumber();

    if (!s.isEmpty()) {
        prop = new QTreeWidgetItem(addrItem);
        prop->setText(0, "thoroughfare number");
        prop->setText(1, s);
    }

    s = address.postCode();

    if (!s.isEmpty()) {
        prop = new QTreeWidgetItem(addrItem);
        prop->setText(0, "post code");
        prop->setText(1, s);
    }
}
