# Copyright (C) 2022  FREIA Laboratory

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.


# The following lines are required
where_am_I := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
include $(E3_REQUIRE_TOOLS)/driver.makefile

# Most modules only need to be built for x86_64
ARCH_FILTER += linux-x86_64


# Since this file (fgcepics.Makefile) is copied into
# the module directory at build-time, these paths have to be relative
# to that path
APP := .
APPDB := $(APP)/Db
APPSRC := $(APP)

# If you have files to include, you will generally want to include these, e.g.
#
#     SOURCES += $(APPSRC)/fgcepicsMain.cpp
#     SOURCES += $(APPSRC)/library.c
#     HEADERS += $(APPSRC)/library.h
#     USR_INCLUDES += -I$(where_am_I)$(APPSRC)

SOURCES += $(APP)/fgccmdSup/src/asubroutine.c
DBDS += $(wildcard $(APP)/fgccmdSup/src/*.dbd)

SOURCES += $(APP)/fgcudpSup/src/devfgcudp.cpp
SOURCES += $(APP)/fgcudpSup/src/common/class_base_params.cpp
SOURCES += $(APP)/fgcudpSup/src/common/param_utils.cpp
SOURCES += $(APP)/fgcudpSup/src/classes/class_63_params.cpp
SOURCES += $(APP)/fgcudpSup/src/classes/class_62_params.cpp
HEADERS += $(wildcard $(APP)/fgcudpSup/src/common/inc/*.h)
DBDS += $(wildcard $(APP)/fgcudpSup/src/*.dbd)

TEMPLATES += $(wildcard $(APPDB)/*.db)
TEMPLATES += $(wildcard $(APP)/fgccmdSup/protocols/*.proto)
#TEMPLATES += $(wildcard $(APPDB)/*.template)

SCRIPTS += $(wildcard ../iocsh/*.iocsh)

# Same as with any source or header files, you can also use $SUBS and $TMPS to define
# database files to be inflated (using MSI), e.g.
#
#     SUBS = $(wildcard $(APPDB)/*.substitutions)
#     TMPS = $(wildcard $(APPDB)/*.template)

SUBS = $(wildcard $(APPDB)/*.substitutions)

USR_DBFLAGS += -I . -I ..
USR_DBFLAGS += -I $(EPICS_BASE)/db
USR_DBFLAGS += -I $(APPDB)

.PHONY: vlibs
vlibs:
