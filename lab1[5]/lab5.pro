QT += core gui widgets

TEMPLATE = app
CONFIG += c++11
CONFIG += sdk_no_version_check


TARGET = lab5
SOURCES += lab5.cpp


bench.target = bench
bench.commands = g++ -std=c++17 -O3 -o bench bench.cpp
bench.depends = bench.cpp
QMAKE_EXTRA_TARGETS += bench

all.depends += bench
QMAKE_EXTRA_TARGETS += all
