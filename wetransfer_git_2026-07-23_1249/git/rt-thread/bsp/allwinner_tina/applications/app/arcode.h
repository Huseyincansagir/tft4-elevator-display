
#include <stdint.h>


struct ArcodeM2H {
	uint8_t currentFloorNo;
	uint8_t leftCharacter;
	uint8_t rightCharacter;
	
	uint8_t upCallLed:1;
	uint8_t downCallLed:1;
	uint8_t po1:1;
	uint8_t po2:1;
	uint8_t vatmanKey:1;
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

	uint8_t fireMode:2;
	uint8_t reserved1:6;

	uint8_t reserved2;
};