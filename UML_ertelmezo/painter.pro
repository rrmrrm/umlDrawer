
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

CONFIG += c++17

SOURCES += \
	model/CanvasElements/canvaselementbase.cpp \
	model/CanvasElements/cemanager.cpp \
	model/CanvasElements/canvaselement.cpp \
	model/DrawingClassifiers/idrawingclassifier.cpp \
	model/DrawingClassifiers/neuralclassifier.cpp \
	model/DrawingClassifiers/simpleclassifier.cpp \
	model/canvasmanipulation.cpp \
	model/connector.cpp \
	model/logger.cpp \
	model/model.cpp \
	model/modelStateOp.cpp \
	model/modelstatemanager.cpp \
	model/identityDir/connectiondata.cpp \
	model/identityDir/identifiable.cpp \
	model/identityDir/qobjectident.cpp \
	model/myqline.cpp \
	model/plug.cpp \
	model/userinputmodel.cpp \
	mysharedptr.cpp \
	network/network.cpp \
	painter_main/main.cpp \
	persistence/persistence.cpp \
	shape_drawing/drawing_factory.cpp \
	shape_drawing/gesture.cpp \
    qrandomgenerator.cpp \
	view/canvas.cpp \
	view/canvaselementwidgetbase.cpp \
	view/connwidget.cpp \
	view/linewidget.cpp \
	view/savelastdrawingwidget.cpp \
	view/textwidget.cpp \
	view/umlclasswidget.cpp \
	view/umlrelationwidget.cpp \
	view/userinputdialog.cpp \
	view/widget.cpp
	
HEADERS += \
	common.hpp \
	functions.hpp \
	model/CanvasElements/canvaselement.h \
	model/CanvasElements/canvaselementbase.h \
	model/CanvasElements/canvaselementtypes.h \
	model/CanvasElements/cemanager.h \
	model/DrawingClassifiers/idrawingclassifier.h \
	model/DrawingClassifiers/neuralclassifier.h \
	model/DrawingClassifiers/simpleclassifier.h \
	model/canvasmanipulation.h \
	model/connector.h \
	model/dirDim.hpp \
	model/idrawingcontainer.h \
	model/initializer.hpp \
	model/logger.h \
	model/model.h \
	model/modelStateOp.h \
	model/modelStateOp_impl.h \
	model/modelstatemanager.h \
	model/identityDir/connectiondata.h \
	model/identityDir/identifiable.h \
	model/identityDir/qobjectident.h \
	model/myqline.h \
	model/myweakptr.h \
	model/myweakptr_impl.h \
	model/plug.h \
	model/target.h \
	model/userinputmodel.h \
	mysharedptr.h \
	network/network.h \
	persistence/persistence.h \
	persistence/streamoperators.h \
	persistence/streamoperators_impl.h \
	shape_drawing/drawing_factory.h \
	shape_drawing/gesture.h \
	shape_drawing/drawing.hpp \
    qrandomgenerator.h \
	view/canvas.h \
	view/canvaselementwidgetbase.h \
	view/canvaselementwidgetcreator.hpp \
	view/connwidget.h \
	view/linewidget.h \
	view/savelastdrawingwidget.h \
	view/textwidget.h \
	view/umlclasswidget.h \
	view/umlrelationwidget.h \
	view/userinputdialog.h \
	view/widget.h
	
FORMS += \
        view/savelastdrawingwidget.ui \
	view/umlClass.ui \
        view/userinputdialog.ui \
        view/widget.ui \
		
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
	
