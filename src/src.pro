TEMPLATE = lib
TARGET = sailfishapp-preload
# This is to prevent including version numbers in resulting file name
CONFIG += plugin

QT += quick
CONFIG += c++11 link_pkgconfig
PKGCONFIG += qmllive

# XXX: Do not hardcode this; use PKGCONFIG += like above (but we cannot do this
# at this point, because of the "-pie" linker flag which would be added by
# qdeclarative5-boostable.pc when available)
INCLUDEPATH += $$PREFIX/include/sailfishapp
LIBS += -lsailfishapp

QMAKE_CXXFLAGS += -fvisibility=hidden -fvisibility-inlines-hidden

SOURCES = libsailfishapp-preload.cpp

target.path = $$[QT_INSTALL_LIBS]/qmllive-sailfish
INSTALLS += target

wrapper.path = $$PREFIX/bin
wrapper.files = $$PWD/qmlliveruntime-sailfish
INSTALLS += wrapper
