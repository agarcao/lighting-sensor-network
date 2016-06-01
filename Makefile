#
# OMNeT++/OMNEST Makefile for wsn-lighting
#
# This file was generated with the command:
#  opp_makemake -f --deep -O out
#

# Name of target to be created (-o option)
TARGET = wsn-lighting$(EXE_SUFFIX)

# User interface (uncomment one) (-u option)
USERIF_LIBS = $(ALL_ENV_LIBS) # that is, $(TKENV_LIBS) $(CMDENV_LIBS)
#USERIF_LIBS = $(CMDENV_LIBS)
#USERIF_LIBS = $(TKENV_LIBS)

# C++ include paths (with -I)
INCLUDE_PATH = \
    -I. \
    -IhelpStructures \
    -Inode \
    -Inode/application \
    -Inode/application/wsnLogic \
    -Inode/communication \
    -Inode/communication/mac \
    -Inode/communication/mac/bypassMac \
    -Inode/communication/mac/tMac \
    -Inode/communication/mac/tunableMac \
    -Inode/communication/radio \
    -Inode/communication/routing \
    -Inode/communication/routing/bypassRouting \
    -Inode/mobilityManager \
    -Inode/mobilityManager/noMobilityManager \
    -Inode/resourceManager \
    -Inode/sensorManager \
    -Iobstacles \
    -Iobstacles/walls \
    -IpersonNode \
    -IphysicalProcess \
    -IphysicalProcess/personsPhysicalProcess \
    -IwirelessChannel

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS =

# Output directory
PROJECT_OUTPUT_DIR = out
PROJECTRELATIVE_PATH =
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc and .msg files
OBJS = \
    $O/helpStructures/CastaliaModule.o \
    $O/helpStructures/TimerService.o \
    $O/helpStructures/DebugInfoWriter.o \
    $O/node/application/VirtualApplication.o \
    $O/node/application/wsnLogic/WsnLogic.o \
    $O/node/communication/mac/VirtualMac.o \
    $O/node/communication/mac/bypassMac/BypassMAC.o \
    $O/node/communication/mac/tMac/TMAC.o \
    $O/node/communication/mac/tunableMac/TunableMAC.o \
    $O/node/communication/radio/RadioSupportFunctions.o \
    $O/node/communication/radio/Radio.o \
    $O/node/communication/routing/VirtualRouting.o \
    $O/node/communication/routing/bypassRouting/BypassRouting.o \
    $O/node/mobilityManager/VirtualMobilityManager.o \
    $O/node/mobilityManager/noMobilityManager/NoMobilityManager.o \
    $O/node/resourceManager/ResourceManager.o \
    $O/node/sensorManager/SensorManager.o \
    $O/obstacles/walls/Wall.o \
    $O/personNode/PersonNode.o \
    $O/physicalProcess/personsPhysicalProcess/PersonsPhysicalProcess.o \
    $O/wirelessChannel/WirelessChannel.o \
    $O/wirelessChannel/WirelessChannelTemporal.o \
    $O/helpStructures/TimerServiceMessage_m.o \
    $O/node/application/ApplicationPacket_m.o \
    $O/node/application/wsnLogic/WsnLogicPacket_m.o \
    $O/node/communication/mac/MacPacket_m.o \
    $O/node/communication/mac/tMac/TMacPacket_m.o \
    $O/node/communication/mac/tunableMac/TunableMacPacket_m.o \
    $O/node/communication/mac/tunableMac/TunableMacControl_m.o \
    $O/node/communication/radio/RadioControlMessage_m.o \
    $O/node/communication/routing/RoutingPacket_m.o \
    $O/node/communication/routing/bypassRouting/BypassRoutingPacket_m.o \
    $O/node/mobilityManager/MobilityManagerMessage_m.o \
    $O/node/resourceManager/ResourceManagerMessage_m.o \
    $O/node/sensorManager/SensorManagerMessage_m.o \
    $O/physicalProcess/PhysicalProcessMessage_m.o \
    $O/wirelessChannel/WirelessChannelMessages_m.o

# Message files
MSGFILES = \
    helpStructures/TimerServiceMessage.msg \
    node/application/ApplicationPacket.msg \
    node/application/wsnLogic/WsnLogicPacket.msg \
    node/communication/mac/MacPacket.msg \
    node/communication/mac/tMac/TMacPacket.msg \
    node/communication/mac/tunableMac/TunableMacPacket.msg \
    node/communication/mac/tunableMac/TunableMacControl.msg \
    node/communication/radio/RadioControlMessage.msg \
    node/communication/routing/RoutingPacket.msg \
    node/communication/routing/bypassRouting/BypassRoutingPacket.msg \
    node/mobilityManager/MobilityManagerMessage.msg \
    node/resourceManager/ResourceManagerMessage.msg \
    node/sensorManager/SensorManagerMessage.msg \
    physicalProcess/PhysicalProcessMessage.msg \
    wirelessChannel/WirelessChannelMessages.msg

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc or configuser.vc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIB_SUBDIR = $(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" -loppmain$D $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)

COPTS = $(CFLAGS)  $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE and have object
# files depend on it (except when "make depend" was called)
COPTS_FILE = $O/.last-copts
ifneq ($(MAKECMDGOALS),depend)
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
$(shell $(MKPATH) "$O" && echo "$(COPTS)" >$(COPTS_FILE))
endif
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# <<<
#------------------------------------------------------------------------------

# Main target
all: $O/$(TARGET)
	$(Q)$(LN) $O/$(TARGET) .

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile
	@$(MKPATH) $O
	@echo Creating executable: $@
	$(Q)$(CXX) $(LDFLAGS) -o $O/$(TARGET)  $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS)

.PHONY: all clean cleanall depend msgheaders

.SUFFIXES: .cc

$O/%.o: %.cc $(COPTS_FILE)
	@$(MKPATH) $(dir $@)
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(qecho) MSGC: $<
	$(Q)$(MSGC) -s _m.cc $(MSGCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

clean:
	$(qecho) Cleaning...
	$(Q)-rm -rf $O
	$(Q)-rm -f wsn-lighting wsn-lighting.exe libwsn-lighting.so libwsn-lighting.a libwsn-lighting.dll libwsn-lighting.dylib
	$(Q)-rm -f ./*_m.cc ./*_m.h
	$(Q)-rm -f helpStructures/*_m.cc helpStructures/*_m.h
	$(Q)-rm -f node/*_m.cc node/*_m.h
	$(Q)-rm -f node/application/*_m.cc node/application/*_m.h
	$(Q)-rm -f node/application/wsnLogic/*_m.cc node/application/wsnLogic/*_m.h
	$(Q)-rm -f node/communication/*_m.cc node/communication/*_m.h
	$(Q)-rm -f node/communication/mac/*_m.cc node/communication/mac/*_m.h
	$(Q)-rm -f node/communication/mac/bypassMac/*_m.cc node/communication/mac/bypassMac/*_m.h
	$(Q)-rm -f node/communication/mac/tMac/*_m.cc node/communication/mac/tMac/*_m.h
	$(Q)-rm -f node/communication/mac/tunableMac/*_m.cc node/communication/mac/tunableMac/*_m.h
	$(Q)-rm -f node/communication/radio/*_m.cc node/communication/radio/*_m.h
	$(Q)-rm -f node/communication/routing/*_m.cc node/communication/routing/*_m.h
	$(Q)-rm -f node/communication/routing/bypassRouting/*_m.cc node/communication/routing/bypassRouting/*_m.h
	$(Q)-rm -f node/mobilityManager/*_m.cc node/mobilityManager/*_m.h
	$(Q)-rm -f node/mobilityManager/noMobilityManager/*_m.cc node/mobilityManager/noMobilityManager/*_m.h
	$(Q)-rm -f node/resourceManager/*_m.cc node/resourceManager/*_m.h
	$(Q)-rm -f node/sensorManager/*_m.cc node/sensorManager/*_m.h
	$(Q)-rm -f obstacles/*_m.cc obstacles/*_m.h
	$(Q)-rm -f obstacles/walls/*_m.cc obstacles/walls/*_m.h
	$(Q)-rm -f personNode/*_m.cc personNode/*_m.h
	$(Q)-rm -f physicalProcess/*_m.cc physicalProcess/*_m.h
	$(Q)-rm -f physicalProcess/personsPhysicalProcess/*_m.cc physicalProcess/personsPhysicalProcess/*_m.h
	$(Q)-rm -f wirelessChannel/*_m.cc wirelessChannel/*_m.h

cleanall: clean
	$(Q)-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(qecho) Creating dependencies...
	$(Q)$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES)  ./*.cc helpStructures/*.cc node/*.cc node/application/*.cc node/application/wsnLogic/*.cc node/communication/*.cc node/communication/mac/*.cc node/communication/mac/bypassMac/*.cc node/communication/mac/tMac/*.cc node/communication/mac/tunableMac/*.cc node/communication/radio/*.cc node/communication/routing/*.cc node/communication/routing/bypassRouting/*.cc node/mobilityManager/*.cc node/mobilityManager/noMobilityManager/*.cc node/resourceManager/*.cc node/sensorManager/*.cc obstacles/*.cc obstacles/walls/*.cc personNode/*.cc physicalProcess/*.cc physicalProcess/personsPhysicalProcess/*.cc wirelessChannel/*.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/helpStructures/CastaliaModule.o: helpStructures/CastaliaModule.cc \
	WSNMessages.h \
	helpStructures/CastaliaModule.h \
	helpStructures/DebugInfoWriter.h \
	node/resourceManager/ResourceManagerMessage_m.h
$O/helpStructures/DebugInfoWriter.o: helpStructures/DebugInfoWriter.cc \
	helpStructures/DebugInfoWriter.h
$O/helpStructures/TimerService.o: helpStructures/TimerService.cc \
	WSNMessages.h \
	helpStructures/TimerService.h \
	helpStructures/TimerServiceMessage_m.h
$O/helpStructures/TimerServiceMessage_m.o: helpStructures/TimerServiceMessage_m.cc \
	helpStructures/TimerServiceMessage_m.h
$O/node/application/ApplicationPacket_m.o: node/application/ApplicationPacket_m.cc \
	node/application/ApplicationPacket_m.h
$O/node/application/VirtualApplication.o: node/application/VirtualApplication.cc \
	WSNMessages.h \
	helpStructures/CastaliaModule.h \
	helpStructures/DebugInfoWriter.h \
	helpStructures/TimerService.h \
	helpStructures/TimerServiceMessage_m.h \
	node/application/ApplicationPacket_m.h \
	node/application/VirtualApplication.h \
	node/communication/mac/MacPacket_m.h \
	node/communication/radio/Radio.h \
	node/communication/radio/RadioControlMessage_m.h \
	node/communication/radio/RadioSupportFunctions.h \
	node/mobilityManager/VirtualMobilityManager.h \
	node/resourceManager/ResourceManager.h \
	node/resourceManager/ResourceManagerMessage_m.h \
	node/sensorManager/SensorManagerMessage_m.h \
	obstacles/walls/Wall.h \
	wirelessChannel/WirelessChannelMessages_m.h
$O/node/application/wsnLogic/WsnLogic.o: node/application/wsnLogic/WsnLogic.cc \
	WSNMessages.h \
	helpStructures/CastaliaModule.h \
	helpStructures/DebugInfoWriter.h \
	helpStructures/TimerService.h \
	helpStructures/TimerServiceMessage_m.h \
	node/application/ApplicationPacket_m.h \
	node/application/VirtualApplication.h \
	node/application/wsnLogic/WsnLogic.h \
	node/application/wsnLogic/WsnLogicPacket_m.h \
	node/communication/mac/MacPacket_m.h \
	node/communication/radio/Radio.h \
	node/communication/radio/RadioControlMessage_m.h \
	node/communication/radio/RadioSupportFunctions.h \
	node/mobilityManager/VirtualMobilityManager.h \
	node/resourceManager/ResourceManager.h \
	node/resourceManager/ResourceManagerMessage_m.h \
	node/sensorManager/SensorManagerMessage_m.h \
	obstacles/walls/Wall.h \
	wirelessChannel/WirelessChannelMessages_m.h
$O/node/application/wsnLogic/WsnLogicPacket_m.o: node/application/wsnLogic/WsnLogicPacket_m.cc \
	node/application/ApplicationPacket_m.h \
	node/application/wsnLogic/WsnLogicPacket_m.h
$O/node/communication/mac/MacPacket_m.o: node/communication/mac/MacPacket_m.cc \
	node/communication/mac/MacPacket_m.h
$O/node/communication/mac/VirtualMac.o: node/communication/mac/VirtualMac.cc \
	WSNMessages.h \
	helpStructures/CastaliaModule.h \
	helpStructures/DebugInfoWriter.h \
	helpStructures/TimerService.h \
	helpStructures/TimerServiceMessage_m.h \
	node/communication/mac/MacPacket_m.h \
	node/communication/mac/VirtualMac.h \
	node/communication/radio/Radio.h \
	node/communication/radio/RadioControlMessage_m.h \
	node/communication/radio/RadioSupportFunctions.h \
	node/communication/routing/RoutingPacket_m.h \
	node/resourceManager/ResourceManager.h \
	node/resourceManager/ResourceManagerMessage_m.h \
	obstacles/walls/Wall.h \
	wirelessChannel/WirelessChannelMessages_m.h
$O/node/communication/mac/bypassMac/BypassMAC.o: node/communication/mac/bypassMac/BypassMAC.cc \
	WSNMessages.h \
	helpStructures/CastaliaModule.h \
	helpStructures/DebugInfoWriter.h \
	helpStructures/TimerService.h \
	helpStructures/TimerServiceMessage_m.h \
	node/communication/mac/MacPacket_m.h \
	node/communication/mac/VirtualMac.h \
	node/communication/mac/bypassMac/BypassMAC.h \
	node/communication/radio/Radio.h \
	node/communication/radio/RadioControlMessage_m.h \
	node/communication/radio/RadioSupportFunctions.h \
	node/communication/routing/RoutingPacket_m.h \
	node/resourceManager/ResourceManager.h \
	node/resourceManager/ResourceManagerMessage_m.h \
	obstacles/walls/Wall.h \
	wirelessChannel/WirelessChannelMessages_m.h
$O/node/communication/mac/tMac/TMAC.o: node/communication/mac/tMac/TMAC.cc \
	WSNMessages.h \
	helpStructures/CastaliaModule.h \
	helpStructures/DebugInfoWriter.h \
	helpStructures/TimerService.h \
	helpStructures/TimerServiceMessage_m.h \
	node/communication/mac/MacPacket_m.h \
	node/communication/mac/VirtualMac.h \
	node/communication/mac/tMac/TMAC.h \
	node/communication/mac/tMac/TMacPacket_m.h \
	node/communication/radio/Radio.h \
	node/communication/radio/RadioControlMessage_m.h \
	node/communication/radio/RadioSupportFunctions.h \
	node/communication/routing/RoutingPacket_m.h \
	node/resourceManager/ResourceManager.h \
	node/resourceManager/ResourceManagerMessage_m.h \
	obstacles/walls/Wall.h \
	wirelessChannel/WirelessChannelMessages_m.h
$O/node/communication/mac/tMac/TMacPacket_m.o: node/communication/mac/tMac/TMacPacket_m.cc \
	node/communication/mac/MacPacket_m.h \
	node/communication/mac/tMac/TMacPacket_m.h
$O/node/communication/mac/tunableMac/TunableMAC.o: node/communication/mac/tunableMac/TunableMAC.cc \
	WSNMessages.h \
	helpStructures/CastaliaModule.h \
	helpStructures/DebugInfoWriter.h \
	helpStructures/TimerService.h \
	helpStructures/TimerServiceMessage_m.h \
	node/communication/mac/MacPacket_m.h \
	node/communication/mac/VirtualMac.h \
	node/communication/mac/tunableMac/TunableMAC.h \
	node/communication/mac/tunableMac/TunableMacControl_m.h \
	node/communication/mac/tunableMac/TunableMacPacket_m.h \
	node/communication/radio/Radio.h \
	node/communication/radio/RadioControlMessage_m.h \
	node/communication/radio/RadioSupportFunctions.h \
	node/communication/routing/RoutingPacket_m.h \
	node/resourceManager/ResourceManager.h \
	node/resourceManager/ResourceManagerMessage_m.h \
	obstacles/walls/Wall.h \
	wirelessChannel/WirelessChannelMessages_m.h
$O/node/communication/mac/tunableMac/TunableMacControl_m.o: node/communication/mac/tunableMac/TunableMacControl_m.cc \
	node/communication/mac/tunableMac/TunableMacControl_m.h
$O/node/communication/mac/tunableMac/TunableMacPacket_m.o: node/communication/mac/tunableMac/TunableMacPacket_m.cc \
	node/communication/mac/MacPacket_m.h \
	node/communication/mac/tunableMac/TunableMacPacket_m.h
$O/node/communication/radio/Radio.o: node/communication/radio/Radio.cc \
	WSNMessages.h \
	helpStructures/CastaliaModule.h \
	helpStructures/DebugInfoWriter.h \
	node/communication/mac/MacPacket_m.h \
	node/communication/radio/Radio.h \
	node/communication/radio/RadioControlMessage_m.h \
	node/communication/radio/RadioSupportFunctions.h \
	node/resourceManager/ResourceManager.h \
	node/resourceManager/ResourceManagerMessage_m.h \
	obstacles/walls/Wall.h \
	wirelessChannel/WirelessChannelMessages_m.h
$O/node/communication/radio/RadioControlMessage_m.o: node/communication/radio/RadioControlMessage_m.cc \
	node/communication/radio/RadioControlMessage_m.h
$O/node/communication/radio/RadioSupportFunctions.o: node/communication/radio/RadioSupportFunctions.cc \
	WSNMessages.h \
	node/communication/radio/RadioControlMessage_m.h \
	node/communication/radio/RadioSupportFunctions.h
$O/node/communication/routing/RoutingPacket_m.o: node/communication/routing/RoutingPacket_m.cc \
	node/communication/routing/RoutingPacket_m.h
$O/node/communication/routing/VirtualRouting.o: node/communication/routing/VirtualRouting.cc \
	WSNMessages.h \
	helpStructures/CastaliaModule.h \
	helpStructures/DebugInfoWriter.h \
	helpStructures/TimerService.h \
	helpStructures/TimerServiceMessage_m.h \
	node/application/ApplicationPacket_m.h \
	node/communication/mac/MacPacket_m.h \
	node/communication/radio/Radio.h \
	node/communication/radio/RadioControlMessage_m.h \
	node/communication/radio/RadioSupportFunctions.h \
	node/communication/routing/RoutingPacket_m.h \
	node/communication/routing/VirtualRouting.h \
	node/resourceManager/ResourceManager.h \
	node/resourceManager/ResourceManagerMessage_m.h \
	obstacles/walls/Wall.h \
	wirelessChannel/WirelessChannelMessages_m.h
$O/node/communication/routing/bypassRouting/BypassRouting.o: node/communication/routing/bypassRouting/BypassRouting.cc \
	WSNMessages.h \
	helpStructures/CastaliaModule.h \
	helpStructures/DebugInfoWriter.h \
	helpStructures/TimerService.h \
	helpStructures/TimerServiceMessage_m.h \
	node/application/ApplicationPacket_m.h \
	node/communication/mac/MacPacket_m.h \
	node/communication/radio/Radio.h \
	node/communication/radio/RadioControlMessage_m.h \
	node/communication/radio/RadioSupportFunctions.h \
	node/communication/routing/RoutingPacket_m.h \
	node/communication/routing/VirtualRouting.h \
	node/communication/routing/bypassRouting/BypassRouting.h \
	node/communication/routing/bypassRouting/BypassRoutingPacket_m.h \
	node/resourceManager/ResourceManager.h \
	node/resourceManager/ResourceManagerMessage_m.h \
	obstacles/walls/Wall.h \
	wirelessChannel/WirelessChannelMessages_m.h
$O/node/communication/routing/bypassRouting/BypassRoutingPacket_m.o: node/communication/routing/bypassRouting/BypassRoutingPacket_m.cc \
	node/communication/routing/RoutingPacket_m.h \
	node/communication/routing/bypassRouting/BypassRoutingPacket_m.h
$O/node/mobilityManager/MobilityManagerMessage_m.o: node/mobilityManager/MobilityManagerMessage_m.cc \
	node/mobilityManager/MobilityManagerMessage_m.h
$O/node/mobilityManager/VirtualMobilityManager.o: node/mobilityManager/VirtualMobilityManager.cc \
	WSNMessages.h \
	helpStructures/CastaliaModule.h \
	helpStructures/DebugInfoWriter.h \
	node/mobilityManager/VirtualMobilityManager.h \
	node/resourceManager/ResourceManagerMessage_m.h \
	wirelessChannel/WirelessChannelMessages_m.h
$O/node/mobilityManager/noMobilityManager/NoMobilityManager.o: node/mobilityManager/noMobilityManager/NoMobilityManager.cc \
	WSNMessages.h \
	helpStructures/CastaliaModule.h \
	helpStructures/DebugInfoWriter.h \
	node/mobilityManager/VirtualMobilityManager.h \
	node/mobilityManager/noMobilityManager/NoMobilityManager.h \
	node/resourceManager/ResourceManager.h \
	node/resourceManager/ResourceManagerMessage_m.h \
	node/sensorManager/SensorManager.h \
	node/sensorManager/SensorManagerMessage_m.h \
	obstacles/walls/Wall.h \
	personNode/PersonNode.h \
	physicalProcess/PhysicalProcessMessage_m.h \
	physicalProcess/personsPhysicalProcess/PersonsPhysicalProcess.h \
	wirelessChannel/WirelessChannelMessages_m.h
$O/node/resourceManager/ResourceManager.o: node/resourceManager/ResourceManager.cc \
	WSNMessages.h \
	helpStructures/CastaliaModule.h \
	helpStructures/DebugInfoWriter.h \
	node/mobilityManager/VirtualMobilityManager.h \
	node/resourceManager/ResourceManager.h \
	node/resourceManager/ResourceManagerMessage_m.h \
	node/sensorManager/SensorManager.h \
	node/sensorManager/SensorManagerMessage_m.h \
	obstacles/walls/Wall.h \
	personNode/PersonNode.h \
	physicalProcess/PhysicalProcessMessage_m.h \
	physicalProcess/personsPhysicalProcess/PersonsPhysicalProcess.h \
	wirelessChannel/WirelessChannelMessages_m.h
$O/node/resourceManager/ResourceManagerMessage_m.o: node/resourceManager/ResourceManagerMessage_m.cc \
	node/resourceManager/ResourceManagerMessage_m.h
$O/node/sensorManager/SensorManager.o: node/sensorManager/SensorManager.cc \
	WSNMessages.h \
	helpStructures/CastaliaModule.h \
	helpStructures/DebugInfoWriter.h \
	node/mobilityManager/VirtualMobilityManager.h \
	node/resourceManager/ResourceManagerMessage_m.h \
	node/sensorManager/SensorManager.h \
	node/sensorManager/SensorManagerMessage_m.h \
	physicalProcess/PhysicalProcessMessage_m.h \
	wirelessChannel/WirelessChannelMessages_m.h
$O/node/sensorManager/SensorManagerMessage_m.o: node/sensorManager/SensorManagerMessage_m.cc \
	node/sensorManager/SensorManagerMessage_m.h
$O/obstacles/walls/Wall.o: obstacles/walls/Wall.cc \
	WSNMessages.h \
	helpStructures/CastaliaModule.h \
	helpStructures/DebugInfoWriter.h \
	node/resourceManager/ResourceManagerMessage_m.h \
	obstacles/walls/Wall.h
$O/personNode/PersonNode.o: personNode/PersonNode.cc \
	WSNMessages.h \
	helpStructures/CastaliaModule.h \
	helpStructures/DebugInfoWriter.h \
	node/mobilityManager/VirtualMobilityManager.h \
	node/resourceManager/ResourceManager.h \
	node/resourceManager/ResourceManagerMessage_m.h \
	node/sensorManager/SensorManager.h \
	node/sensorManager/SensorManagerMessage_m.h \
	obstacles/walls/Wall.h \
	personNode/PersonNode.h \
	physicalProcess/PhysicalProcessMessage_m.h \
	physicalProcess/personsPhysicalProcess/PersonsPhysicalProcess.h \
	wirelessChannel/WirelessChannelMessages_m.h
$O/physicalProcess/PhysicalProcessMessage_m.o: physicalProcess/PhysicalProcessMessage_m.cc \
	physicalProcess/PhysicalProcessMessage_m.h
$O/physicalProcess/personsPhysicalProcess/PersonsPhysicalProcess.o: physicalProcess/personsPhysicalProcess/PersonsPhysicalProcess.cc \
	WSNMessages.h \
	helpStructures/CastaliaModule.h \
	helpStructures/DebugInfoWriter.h \
	node/mobilityManager/VirtualMobilityManager.h \
	node/resourceManager/ResourceManager.h \
	node/resourceManager/ResourceManagerMessage_m.h \
	node/sensorManager/SensorManager.h \
	node/sensorManager/SensorManagerMessage_m.h \
	obstacles/walls/Wall.h \
	personNode/PersonNode.h \
	physicalProcess/PhysicalProcessMessage_m.h \
	physicalProcess/personsPhysicalProcess/PersonsPhysicalProcess.h \
	wirelessChannel/WirelessChannelMessages_m.h
$O/wirelessChannel/WirelessChannel.o: wirelessChannel/WirelessChannel.cc \
	WSNMessages.h \
	helpStructures/CastaliaModule.h \
	helpStructures/DebugInfoWriter.h \
	helpStructures/TimerService.h \
	helpStructures/TimerServiceMessage_m.h \
	node/application/ApplicationPacket_m.h \
	node/application/VirtualApplication.h \
	node/application/wsnLogic/WsnLogic.h \
	node/application/wsnLogic/WsnLogicPacket_m.h \
	node/communication/mac/MacPacket_m.h \
	node/communication/radio/Radio.h \
	node/communication/radio/RadioControlMessage_m.h \
	node/communication/radio/RadioSupportFunctions.h \
	node/mobilityManager/VirtualMobilityManager.h \
	node/resourceManager/ResourceManager.h \
	node/resourceManager/ResourceManagerMessage_m.h \
	node/sensorManager/SensorManager.h \
	node/sensorManager/SensorManagerMessage_m.h \
	obstacles/walls/Wall.h \
	personNode/PersonNode.h \
	physicalProcess/PhysicalProcessMessage_m.h \
	physicalProcess/personsPhysicalProcess/PersonsPhysicalProcess.h \
	wirelessChannel/WirelessChannel.h \
	wirelessChannel/WirelessChannelMessages_m.h \
	wirelessChannel/WirelessChannelTemporal.h
$O/wirelessChannel/WirelessChannelMessages_m.o: wirelessChannel/WirelessChannelMessages_m.cc \
	wirelessChannel/WirelessChannelMessages_m.h
$O/wirelessChannel/WirelessChannelTemporal.o: wirelessChannel/WirelessChannelTemporal.cc \
	wirelessChannel/WirelessChannelTemporal.h

