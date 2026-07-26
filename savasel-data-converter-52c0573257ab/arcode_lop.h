/* 
 * File:   arcode_lop.h
 * Author: kadir
 *
 * Created on 13 December 2021, 12:12
 */

#ifndef ARCODE_LOP_H
#define	ARCODE_LOP_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define MAX_TRANSMIT_BUFFER_SIZE		5
#define MAX_RECEIVE_BUFFER_SIZE			15

#define LOP_HOST_TO_MODULE_PACKET_SIZE	5
#define LOP_HOST_TO_MODULE_PACKET_ID	0x0A

union LOP_Host_To_Module {
	struct {
		uint8_t msgSize;
		uint8_t msgId;
		uint8_t msgData[LOP_HOST_TO_MODULE_PACKET_SIZE - 3];
		uint8_t checksum;
	} b;
	uint8_t bytes[LOP_HOST_TO_MODULE_PACKET_SIZE];
};

struct ModuleRcvFromLopHost {
	uint8_t upCallButton:1;
	uint8_t downCallButton:1;
	uint8_t pi1:1;
	uint8_t pi2:1;
	uint8_t reserved1:4;
	
	uint8_t reserved2;
};

#define MODULE_TO_LOP_HOST_PACKET_SIZE	15
#define MODULE_TO_LOP_HOST_PACKET_ID	0x0B
union Module_To_LOP_Host {
	struct {
		uint8_t msgSize;
		uint8_t msgId;
		uint8_t msgData[MODULE_TO_LOP_HOST_PACKET_SIZE - 3];
		uint8_t checksum;
	} b;
	uint8_t bytes[MODULE_TO_LOP_HOST_PACKET_SIZE];
};

struct ModuleSendToLopHost {
	uint8_t currentFloorNo;
	uint8_t leftCharacter;
	uint8_t rightCharacter;
	
	uint8_t upCallLed:1;
	uint8_t downCallLed:1;
	uint8_t po1:1;
	uint8_t po2:1;
	uint8_t reserved1:1;
	uint8_t earthquake:1;
	uint8_t carLight:1;
	uint8_t sleepModeActive:1;

	uint8_t floorChimeType:3;
	uint8_t enabledLopFloorChime:1;
	uint8_t playLopFloorChime:1;
	uint8_t overLoad:1;
	uint8_t enabledButtonSound:1;
	uint8_t displayScrollDirection:1;
	
	uint8_t displayScrollSpeed:3;
	uint8_t outOfService:1;
	uint8_t travelUpDirection:1;
	uint8_t travelDownDirection:1;
	uint8_t collectionUpDirection:1;
	uint8_t collectionDownDirection:1;
	
	uint8_t configFloorNo;
	uint8_t infoMessageNo;

	uint8_t flgLopLearningMode:1;
	uint8_t doorA_AvailableAtCurrentFloor:1;
	uint8_t doorB_AvailableAtCurrentFloor:1;
	uint8_t doorA_IsOpening:1;
	uint8_t doorB_IsOpening:1;
	uint8_t doorA_IsClosing:1;
	uint8_t doorB_IsClosing:1;
	uint8_t flgCanbusConnectionOK:1;
	
	uint8_t errorCode;

	uint8_t reserved3;
	uint8_t reserved4;
};

#define MODULE_TO_HOST_DISPLAY_INFO_MESSAGES_PACKET_SIZE	10
#define MODULE_TO_HOST_DISPLAY_INFO_MESSAGES_PACKET_ID		0x0E
union Module_To_Host_Display_Info_Msg {
	struct {
		uint8_t msgSize;
		uint8_t msgId;
		uint8_t msgData[MODULE_TO_HOST_DISPLAY_INFO_MESSAGES_PACKET_SIZE - 3];
		uint8_t checksum;
	} b;
	uint8_t bytes[MODULE_TO_HOST_DISPLAY_INFO_MESSAGES_PACKET_SIZE];
};

struct ModuleSendDisplayInfoMsg{
	uint8_t stringId;
	uint8_t stringPart;
	uint8_t stringData[5];
};

//uint8_t INFO_MESSAGES[15][26]; //Info message count is 15 and maximum character size of an info message is 25.
//0 -> Out-of-service message
//1 -> Overloaded message
//2 -> Maintenance message
//3 -> Fire evacuation message
//4 -> Backup power evacuation message
//5 -> Startup message
//6 -> Door open error message
//7 -> Idle message
//8 -> Message to display on LOPs in priority service
//9 -> Message to display on COP in priority service
//10...14 -> Undefined

#define MODULE_TO_HOST_DATE_TIME_MESSAGE_PACKET_SIZE	10
#define MODULE_TO_HOST_DATE_TIME_MESSAGE_PACKET_ID		0x0F
union Module_To_Host_Date_Time_Msg {
	struct {
		uint8_t msgSize;
		uint8_t msgId;
		uint8_t msgData[MODULE_TO_HOST_DATE_TIME_MESSAGE_PACKET_SIZE - 3];
		uint8_t checksum;
	} b;
	uint8_t bytes[MODULE_TO_HOST_DATE_TIME_MESSAGE_PACKET_SIZE];
};

struct ModuleSendDateTimeMsg {
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	
	uint8_t reserved1;
	uint8_t reserved2;
};

//16.09.2014 17:45
//year : 14
//month : 9
//day : 16
//hour : 17
//minute : 45

#ifdef	__cplusplus
}
#endif

#endif	/* ARCODE_LOP_H */

