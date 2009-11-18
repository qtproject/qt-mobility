TARGET = PublishSubscribeEx
TEMPLATE = app


SOURCES += main.cpp\
        publishsubscribemainwindow.cpp

HEADERS  += publishsubscribemainwindow.h

FORMS    += publishsubscribemainwindow.ui

crml.sources = resources.qcrml profile.qcrml
crml.path = c:/resource/qt/crml
DEPLOYMENT += crml
TARGET.CAPABILITY = ReadUserData

qtAddLibrary(QtPublishSubscribe)
