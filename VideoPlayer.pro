#-------------------------------------------------
#
# Project created by QtCreator 2016-09-01T16:10:47
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

UI_DIR  = obj/Gui
MOC_DIR = obj/Moc
OBJECTS_DIR = obj/Obj


contains(QT_ARCH, i386) {
    message("32-bit")
    DESTDIR = $${PWD}/bin32
} else {
    message("64-bit")
    DESTDIR = $${PWD}/bin64
}
QMAKE_CXXFLAGS += -std=c++11

TARGET = VideoPlayer
TEMPLATE = app

include(module/VideoPlayer/VideoPlayer.pri)
include(module/DragAbleWidget/DragAbleWidget.pri)

SOURCES += \
    src/Anime4KPlugin.cpp \
    src/ScreenshotThread.cpp \
    src/Waifu2x/src/convertRoutine.cpp \
    src/Waifu2x/src/modelHandler.cpp \
    src/Waifu2x/src/test.cpp \
    src/Waifu2x/src/waifu2x.cpp \
    src/Yx_opencv_ImgChange.cpp \
    src/main.cpp \
    src/AppConfig.cpp \
    src/Base/FunctionTransfer.cpp \
    src/MainWindow.cpp \
    src/Widget/ShowVideoWidget.cpp \
    src/Widget/VideoSlider.cpp

HEADERS  += \
    src/Anime4KPlugin.h \
    src/AppConfig.h \
    src/Base/FunctionTransfer.h \
    src/ImageKernel.hpp \
    src/MainWindow.h \
    src/ScreenshotThread.h \
    src/Waifu2x/src/convertRoutine.hpp \
    src/Waifu2x/src/modelHandler.hpp \
    src/Waifu2x/src/picojson.h \
    src/Waifu2x/src/tclap/Arg.h \
    src/Waifu2x/src/tclap/ArgException.h \
    src/Waifu2x/src/tclap/ArgTraits.h \
    src/Waifu2x/src/tclap/CmdLine.h \
    src/Waifu2x/src/tclap/CmdLineInterface.h \
    src/Waifu2x/src/tclap/CmdLineOutput.h \
    src/Waifu2x/src/tclap/Constraint.h \
    src/Waifu2x/src/tclap/DocBookOutput.h \
    src/Waifu2x/src/tclap/HelpVisitor.h \
    src/Waifu2x/src/tclap/IgnoreRestVisitor.h \
    src/Waifu2x/src/tclap/MultiArg.h \
    src/Waifu2x/src/tclap/MultiSwitchArg.h \
    src/Waifu2x/src/tclap/OptionalUnlabeledTracker.h \
    src/Waifu2x/src/tclap/StandardTraits.h \
    src/Waifu2x/src/tclap/StdOutput.h \
    src/Waifu2x/src/tclap/SwitchArg.h \
    src/Waifu2x/src/tclap/UnlabeledMultiArg.h \
    src/Waifu2x/src/tclap/UnlabeledValueArg.h \
    src/Waifu2x/src/tclap/ValueArg.h \
    src/Waifu2x/src/tclap/ValuesConstraint.h \
    src/Waifu2x/src/tclap/VersionVisitor.h \
    src/Waifu2x/src/tclap/Visitor.h \
    src/Waifu2x/src/tclap/XorHandler.h \
    src/Waifu2x/src/tclap/ZshCompletionOutput.h \
    src/Waifu2x/src/waifu2x.h \
    src/Widget/ShowVideoWidget.h \
    src/Widget/VideoSlider.h \
    src/Yx_opencv_ImgChange.h


FORMS    += \
    src/MainWindow.ui \
    src/Widget/ShowVideoWidget.ui

RESOURCES += \
    resources/resources.qrc

INCLUDEPATH += $$PWD/src \
    $$PWD/src/Waifu2x/src

win32:RC_FILE=$$PWD/resources/main.rc

DISTFILES += \
    src/Waifu2x/models/noise1_model.json \
    src/Waifu2x/models/noise2_model.json \
    src/Waifu2x/models/scale2.0x_model.json

INCLUDEPATH += $$(OPENCV_SDK_DIR)/include

LIBS += -L$$(OPENCV_SDK_DIR)/x64/mingw/lib \
        -lopencv_core349        \
        -lopencv_highgui349     \
        -lopencv_imgcodecs349   \
        -lopencv_imgproc349     \
        -lopencv_features2d349  \
        -lopencv_calib3d349
