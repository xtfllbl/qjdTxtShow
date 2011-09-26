TARGET = show
TEMPLATE = app
CONFIG += qt debug
QMAKE_LFAGS+= -static

DEPENDPATH += . debug

MOC_DIR += ./GeneratedFiles/moc
OBJECTS_DIR += ./GeneratedFiles/ofile
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles

INCLUDEPATH += .
SOURCES += main.cpp \
    qjd_show_mainwindow.cpp \
    qjd_show_help.cpp \
    qjd_show_segy.cpp\
    qjd_show_widget2.cpp \
    qjdsegybh.cpp \
    qjdsegyth.cpp \
    qjdtraceheader.cpp \
    qjd.cpp \
    qjddatastream.cpp \
    qjd_show_text.cpp \
    changecolorrange.cpp

HEADERS += qjd_show_mainwindow.h \
    qjd_show_help.h \
    qjd_show_segy.h \
    qjd_show_widget2.h \
    qjdsegybh.h \
    qjdsegyth.h \
    qjdtraceheader.h \
    qjd.h \
    qjddatastream.h \
    qjd_show_text.h \
    changecolorrange.h

FORMS += qjd_show_mainwindow.ui \
    qjd_show_help.ui \
    changecolorrange.ui
RESOURCES += image.qrc \
    styles.qrc

OTHER_FILES += \
    styles/stylesheet.qss


