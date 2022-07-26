
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = project
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11


SOURCES += \
	canvasmanipulationmodel.cpp \
	model/logger.cpp \
	model/model.cpp \
	network/network.cpp \
	painter_main/main.cpp \
	persistence/persistance.cpp \
	selectable.cpp \
	shape_drawing/drawing.cpp \
	shape_drawing/drawing_factory.cpp \
	shape_drawing/gesture.cpp \
	shape_drawing/lastdrawingprovider.cpp \
    qrandomgenerator.cpp \
	userinputdialog.cpp \
	view/painter.cpp \
	view/widget.cpp
	
HEADERS += \
	canvasmanipulationmodel.h \
	common.hpp \
	functions.hpp \
	model/logger.h \
	model/model.h \
	model/model_painter_interface.h \
	network/network.h \
	oldSerialisation.hpp \
	persistence/persistance.h \
	selectable.h \
	shape_drawing/drawing.hpp \
	shape_drawing/drawing_factory.h \
	shape_drawing/gesture.h \
	shape_drawing/lastdrawingprovider.hpp \
	shape_drawing/smoothDrawing.hpp \
    qrandomgenerator.h \
	userinputdialog.h \
	view/painter.h \
	view/widget.h
	
FORMS += \
        userinputdialog.ui \
        widget.ui
		
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# remove possible other optimization flags
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2

## add the desired -O3 if not present
#QMAKE_CXXFLAGS_RELEASE *= -O3

SUBDIRS += \
	trainer.pro
	
