
QT += core gui network widgets

CONFIG += c++11

TARGET = GraphicNetCat
TEMPLATE = app

SOURCES += \
	main.cpp \
	tabwidget.cpp \
	mainwindow.cpp \
	connectionwindow.cpp

HEADERS += \
	mainwindow.hpp \
	tabwidget.hpp \
	connectionwindow.hpp
