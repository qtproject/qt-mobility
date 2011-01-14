/*
============================================================================
 Name		: nfctestserviceprovider.cpp
 Author	  : 
 Copyright   : Your copyright notice
 Description : Declares Class
============================================================================
*/

#ifndef nfctestserviceprovider_H
#define nfctestserviceprovider_H

#include <QtGui/QMainWindow>
#include "ui_nfctestserviceprovider.h"

class nfctestserviceprovider : public QMainWindow
{
    Q_OBJECT

public:
	nfctestserviceprovider(QWidget *parent = 0);
    ~nfctestserviceprovider();

private:
    Ui::nfctestserviceprovider ui;
};

#endif // nfctestserviceprovider_H
