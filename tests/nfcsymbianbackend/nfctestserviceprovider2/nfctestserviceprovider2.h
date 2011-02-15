/*
============================================================================
 Name		: nfctestserviceprovider2.cpp
 Author	  : 
 Copyright   : Your copyright notice
 Description : Declares Class
============================================================================
*/

#ifndef nfctestserviceprovider2_H
#define nfctestserviceprovider2_H

#include <QtGui/QMainWindow>
#include "ui_nfctestserviceprovider2.h"

class nfctestserviceprovider2 : public QMainWindow
{
    Q_OBJECT

public:
	nfctestserviceprovider2(QWidget *parent = 0);
    ~nfctestserviceprovider2();

private:
    Ui::nfctestserviceprovider2 ui;
};

#endif // nfctestserviceprovider2_H
