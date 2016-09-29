/*
 * ErrorFunction.cpp
 *
 *  Created on: 26/ago/2016
 *      Author: pc
 */

#include "usbd_cdc_if.h"
#include "StreamString.h"
#include "StreamMemoryReference.h"
#include "task.h"

using namespace MARTe;

extern "C" {

}

static char8 buffer[128] = { 0 };

static char8 buffer2[128] = { 0 };

void PrintStack(ThreadIdentifier &tid) {
	while (1) {
		memset(buffer2, 0, 128);

	    StreamMemoryReference buffer2_stream(buffer2, 128);

	    buffer2_stream.Seek(0);
		uint32 stack_remained = uxTaskGetStackHighWaterMark(tid);

		//allErrors.Printf(" File %s Function %s Line %d", errorInfo.fileName, errorInfo.functionName, errorInfo.header.lineNumber);
		buffer2_stream.Printf("Stack remained %d\n\r", stack_remained);
		uint32_t size = buffer2_stream.Size() + 1;
		if (!USBInitialized()) {
			USBOpen();
		}
		USBWrite(buffer2_stream.Buffer(), &size, 0);
		Sleep::Sec(20.);
	}
}

void DebugErrorProcessFunction(
		const MARTe::ErrorManagement::ErrorInformation &errorInfo,
		const char * const errorDescription) {
//	StackType_t *stackTop=(StackType_t*) xTaskGetCurrentTaskHandle();
	memset(buffer, 0, 128);
    StreamMemoryReference buffer_stream(buffer, 128);
    buffer_stream.Seek(0);
	buffer_stream.Printf("%s\n\r", errorDescription);

	uint32_t size = buffer_stream.Size() + 1;
	if (!USBInitialized()) {
		USBOpen();
	}
	USBWrite(buffer, &size, 0);
}
