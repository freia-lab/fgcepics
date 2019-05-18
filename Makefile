# Makefile for Asyn fgcudp support
#
# Created by jlhenriq on Fri Mar  8 14:50:54 2019
# Based on the Asyn devGpib template

TOP = .
include $(TOP)/configure/CONFIG

DIRS := configure
DIRS += $(wildcard *[Ss]up)
DIRS += $(wildcard *[Aa]pp)
DIRS += $(wildcard ioc[Bb]oot)

include $(TOP)/configure/RULES_TOP
