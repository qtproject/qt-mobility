include(../config.pri)
        
mac {
    #due to different debug/release library names we have to comply with 
    #whatever Qt does
    !contains(QT_CONFIG,debug)|!contains(QT_CONFIG,release) {
        CONFIG -= debug_and_release debug release
        contains(QT_CONFIG,debug): CONFIG+=debug
        contains(QT_CONFIG,release): CONFIG+=release
    }
}
!contains(build_examples, yes):error(Please use the -examples configure switch to enable building of examples)

OUTPUT_DIR = $$PWD/..
SUBDIRPART = objects
DESTDIR=$$OUTPUT_DIR/bin/examples
MOC_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET/moc
RCC_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET/rcc
UI_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET/ui
OBJECTS_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET
mac:LIBS+= -F$$OUTPUT_DIR/lib
LIBS+= -L$$OUTPUT_DIR/lib
