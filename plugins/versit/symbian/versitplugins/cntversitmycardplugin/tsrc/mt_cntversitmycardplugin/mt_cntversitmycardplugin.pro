TEMPLATE = app
TARGET = 

QT += testlib sql xml
CONFIG  += qtestlib

DEPENDPATH += .
INCLUDEPATH += .

symbian:
{
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
	
	# Input
	HEADERS += mt_cntversitmycardplugin.h
	
	SOURCES += mt_cntversitmycardplugin.cpp
	
	TARGET.CAPABILITY = ALL -TCB
  
  LIBS += -lQtContacts -lQtVersit
  
  CONFIG += symbian_test
}

