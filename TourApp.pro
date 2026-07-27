QT       += core gui sql widgets
TARGET    = SkyTickets
TEMPLATE  = app
CONFIG   += c++17

SOURCES += \
    main.cpp \
    person.cpp \
    user.cpp \
    tour.cpp \
    booking.cpp \
    database.cpp \
    loginwindow.cpp \
    clientwindow.cpp \
    adminwindow.cpp \
    tourcard.cpp

HEADERS += \
    person.h \
    user.h \
    tour.h \
    booking.h \
    database.h \
    appstyle.h \
    loginwindow.h \
    clientwindow.h \
    adminwindow.h \
    tourcard.h

QMAKE_CXXFLAGS += -Wall -Wextra
