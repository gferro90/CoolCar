#############################################################
#
# Copyright 2011 EFDA | European Fusion Development Agreement
#
# Licensed under the EUPL, Version 1.1 or - as soon they 
# will be approved by the European Commission - subsequent  
# versions of the EUPL (the "Licence"); 
# You may not use this work except in compliance with the 
# Licence. 
# You may obtain a copy of the Licence at: 
#  
# http://ec.europa.eu/idabc/eupl
#
# Unless required by applicable law or agreed to in 
# writing, software distributed under the Licence is 
# distributed on an "AS IS" basis, 
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
# express or implied. 
# See the Licence for the specific language governing 
# permissions and limitations under the Licence. 
#
# $Id: Makefile.inc 3 2012-01-15 16:26:07Z aneto $
#
#############################################################
OBJSX=	CameraUSBDrv.x\
	LineFollower.x\
	PIDController.x\
	RoadSign.x\
	TurnLeftSign.x\
	TurnRightSign.x

 
#CameraUSBDrv.x\
	LineFollower.x\
	PIDController.x\
	RoadSign.x\
	TurnLeftSign.x\
	TurnRightSign.x

MAKEDEFAULTDIR=$(EFDA-MARTe)/MakeDefaults

include $(MAKEDEFAULTDIR)/MakeStdLibDefs.$(TARGET)

CFLAGS+= -I.
CFLAGS+= -I../
CFLAGS+= -I$(EFDA-MARTe)
CFLAGS+= -I$(EFDA-MARTe)/MARTe/MARTeSupportLib
CFLAGS+= -I$(EFDA-MARTe)/BaseLib2/Level0
CFLAGS+= -I$(EFDA-MARTe)/BaseLib2/Level1
CFLAGS+= -I$(EFDA-MARTe)/BaseLib2/Level2
CFLAGS+= -I$(EFDA-MARTe)/BaseLib2/Level3
CFLAGS+= -I$(EFDA-MARTe)/BaseLib2/Level4
CFLAGS+= -I$(EFDA-MARTe)/BaseLib2/Level5
CFLAGS+= -I$(EFDA-MARTe)/BaseLib2/Level6
CFLAGS+= -I$(EFDA-MARTe)/BaseLib2/LoggerService


CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/include/opencv2
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/include/opencv 
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/core
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/cudaarithm
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/cudabgsegm
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/cudacodec
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/cudafeatures2d
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/cudafilters
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/cudaimgproc
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/cudalegacy
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/cudaobjdetect
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/cudaoptflow
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/cudastereo
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/cudawarping
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/cudev
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/features2d
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/flann
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/hal
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/highgui
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/imgcodecs
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/imgproc
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/java
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/ml
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/objdetect
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/photo
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/python
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/shape
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/stitching
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/superres
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/ts
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/video
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/videoio
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/videostab
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/viz
CFLAGS += -I/home/pc/OpenCV/opencv-3.0.0/modules/world
CFLAGS += -I/home/pc/Kinect/kinect/OpenNI-OpenNI-e263e59/Include

all: $(OBJS) \
	$(TARGET)/USBDrv$(GAMEXT)  
	echo  $(OBJS)

include depends.$(TARGET)

include $(MAKEDEFAULTDIR)/MakeStdLibRules.$(TARGET)

