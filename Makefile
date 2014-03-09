# -*- mode:Makefile -*-

## Note: setenv WIND_BASE in the shell before 'make':
## setenv WIND_BASE /home/tnagler/proj/frc/WindRiver/vxworks-6.3

SILENT    = @

WIND_DIR  = /home/tnagler/proj/frc/WindRiver
WIND_BASE = $(WIND_DIR)/vxworks-6.3
WPI_LIB_A = $(WIND_BASE)/target/lib/WPILib.a

DEFINES    =
DEFINES   += -DCPU=PPC603
DEFINES   += -DTOOL_FAMILY=gnu
DEFINES   += -DTOOL=gnu
DEFINES   += -D_WRS_KERNEL

TARGET     = FRC_UserProgram.out

SOURCES    =
SOURCES   += util/countup_timer.cpp
SOURCES   += util/negedge_trigger.cpp
SOURCES   += util/jag_interface.cpp
SOURCES   += util/driver_station_interface.cpp
SOURCES   += util/posedge_toggle.cpp
SOURCES   += util/point.cpp
SOURCES   += util/bound_tracker.cpp
SOURCES   += util/interface.cpp
SOURCES   += util/posedge_trigger.cpp
SOURCES   += util/util.cpp
SOURCES   += util/countdown_timer.cpp
SOURCES   += util/perf_tracker.cpp
SOURCES   += input/cyborg_joystick.cpp
#OURCES   += input/joystick_linux.cpp
SOURCES   += input/panel2014.cpp
SOURCES   += input/util.cpp
SOURCES   += control/injector.cpp
SOURCES   += control/wheelrpms.cpp
SOURCES   += control/injector_arms.cpp
SOURCES   += control/shooter_status.cpp
SOURCES   += control/force_interface.cpp
SOURCES   += control/collector_tilt.cpp
SOURCES   += control/bunny_launcher.cpp
SOURCES   += control/bist.cpp
SOURCES   += control/fire_control.cpp
SOURCES   += control/main.cpp
SOURCES   += control/shooter_wheels.cpp
SOURCES   += control/collector.cpp
SOURCES   += control/control_status.cpp
SOURCES   += control/octocanum.cpp
SOURCES   += control/ejector.cpp
#OURCES   += control/speedcontrol.cpp
SOURCES   += control/toplevel.cpp
SOURCES   += control/wheel_rpm_interface.cpp
SOURCES   += control/wheel_sim.cpp
SOURCES   += control/holonomic.cpp
SOURCES   += control/gyro_tracker.cpp
SOURCES   += control/pump.cpp
SOURCES   += control/force.cpp
SOURCES   += jag_control.cpp
SOURCES   += to_crio.cpp
SOURCES   += dio_control.cpp

OBJECTS    = $(SOURCES:.cpp=.o)
DEPENDS    = $(OBJECTS:.o=.deps)

INCLUDES  += -I$(WIND_BASE)/target/h
INCLUDES  += -I$(WIND_BASE)/target/h/WPILib
INCLUDES  += -I$(WIND_BASE)/target/h/wrn/coreip
INCLUDES  += -I.

CCPPC      = wine $(WIND_DIR)/gnu/3.4.4-vxworks-6.3/x86-win32/bin/ccppc.exe
LDPPC      = wine $(WIND_DIR)/gnu/3.4.4-vxworks-6.3/x86-win32/bin/ldppc.exe
PLINK      = wine $(WIND_DIR)/workbench-3.3/x86-win32/bin/plink.exe
NMPPC      = wine $(WIND_DIR)/gnu/3.4.4-vxworks-6.3/x86-win32/bin/nmppc.exe
TCLSH      = wine $(WIND_DIR)/workbench-3.3/foundation/x86-win32/bin/tclsh.exe
#TCLSH     = /usr/bin/tclsh

# munch.tcl utility to munch C++ modules for static constructors:
MUNCH      = $(WIND_BASE)/host/resource/hutils/tcl/munch.tcl

# gnu linker command file for VxWorks .out files:
LINKOUT    = $(WIND_BASE)/target/h/tool/gnu/ldscripts/link.OUT

#XXFLAGS   = -g
#XXFLAGS  += -g
#XXFLAGS  += -ggdb3
#XXFLAGS  += -ansi
CXXFLAGS  += -Wall
#XXFLAGS  += -O -Wuninitialized
#XXFLAGS  += -O3
#XXFLAGS  += -MD -MP
CXXFLAGS  += -mcpu=603
CXXFLAGS  += -mstrict-align
CXXFLAGS  += -mno-implicit-fp
CXXFLAGS  += -mlongcall

LDFLAGS    = -r -nostdlib -X

DELFILES   = $(OBJECTS) $(TARGET) partialImage.o ctdt.c ctdt.o 

%.o: %.cpp Makefile
	@echo '  Compiling' $<
	$(SILENT) $(CCPPC) $(DEFINES) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

%.deps: %.cpp
	@echo '  Parsing' $<
	$(SILENT) $(CCPPC) $(DEFINES) $(CXXFLAGS) $(INCLUDES) -c $< -MM -MF $@

all: $(TARGET)

.PHONY: deps
deps: $(DEPENDS)

.PHONY: deploy
deploy: $(TARGET)
	@echo '  Deploying $(TARGET)'
	$(SILENT) /usr/bin/wput --binary ftp://10.14.25.2/ni-rt/system/$(TARGET)

.PHONY: undeploy
undeploy:
	@echo '  Undeploying $(TARGET)'
	$(SILENT) /usr/bin/wget --delete-after ftp://10.14.25.2/ni-rt/system/$(TARGET)

.PHONY: clean
clean:
	@echo '  Cleaning'
	rm -f $(DELFILES)

# ---------------------------------------------------------------------------
# Special rules:
# ---------------------------------------------------------------------------

partialImage.o: $(OBJECTS)
	@echo '  Linking (partial)'
	$(SILENT) $(LDPPC) $(LDFLAGS) $(OBJECTS) -o partialImage.o

ctdt.c: partialImage.o
	@echo '  Generating ctdt table'
	$(SILENT) $(NMPPC) partialImage.o $(WPI_LIB_A) | $(TCLSH) $(MUNCH) -c ppc > ctdt.c

ctdt.o: ctdt.c
	@echo '  Compiling ctdt table'
	$(SILENT) $(CCPPC) $(DEFINES) $(CXXFLAGS) $(INCLUDES) -c ctdt.c -o ctdt.o

$(TARGET): ctdt.o
	@echo '  Linking (final)'
	$(SILENT) $(LDPPC) $(LDFLAGS) -T $(LINKOUT) ctdt.o partialImage.o $(WPI_LIB_A) -o $@

# ---------------------------------------------------------------------------
# Autogenerated dependencies follow:
# ---------------------------------------------------------------------------