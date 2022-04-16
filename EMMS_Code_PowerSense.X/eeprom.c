/****************
 INCLUDES
 only include the header files that are required
 ****************/

#include "common.h"


// set initial calibration to:
//	c1 =				22124	(221.24 mWh/pulse)
//	c2 = 22124/16 =		1382	(13.82 mWh/pulse)

//__EEPROM_DATA( 0x00, 0x00, 0x56, 0x6c, 0x00, 0x00, 0x05, 0x66 );
/****************
 MACROS
 ****************/

//#define EEPROM_ADDR_CALIBRATION_1_BYTE_0	0x00
//#define EEPROM_ADDR_CALIBRATION_1_BYTE_1	0x01
//#define EEPROM_ADDR_CALIBRATION_1_BYTE_2	0x02
//#define EEPROM_ADDR_CALIBRATION_1_BYTE_3	0x03
//
//#define EEPROM_ADDR_CALIBRATION_2_BYTE_0	0x04
//#define EEPROM_ADDR_CALIBRATION_2_BYTE_1	0x05
//#define EEPROM_ADDR_CALIBRATION_2_BYTE_2	0x06
//#define EEPROM_ADDR_CALIBRATION_2_BYTE_3	0x07


/****************
 VARIABLES
 these are the globals required by only this c file
 there should be as few of these as possible to help keep things clean
 variables required by other c functions should be here and also in the header .h file
 as external
 ****************/

// external

// internal only


/****************
 FUNCTION PROTOTYPES
 only include functions called from within this code
 external functions should be in the header
 ideally these are in the same order as in the code listing
 any functions used internally and externally (prototype here and in the .h file)
	 should be marked
 *****************/

unsigned char eepromRead( unsigned char badd );
void eepromWrite( unsigned char badd, unsigned char bdat );

/****************
 CODE
 ****************/


unsigned char eepromRead( unsigned char badd )
{
	unsigned char readVal;

	EEADR = badd;

	EECON1bits.CFGS = 0;
	EECON1bits.EEPGD = 0;
	EECON1bits.RD = 1;

	readVal = EEDATA;

	return readVal; // return with read byte
}

void eepromWrite( unsigned char badd, unsigned char bdat )
{
	unsigned char GIE_BIT_VAL;

	EEADR = badd;
	EEDATA = bdat;

	EECON1bits.EEPGD = 0;
	EECON1bits.CFGS = 0;

	EECON1bits.WREN = 1;
	GIE_BIT_VAL = INTCONbits.GIE;
	INTCONbits.GIE = 0;

	EECON2 = 0x55; // critical unlock sequence
	EECON2 = 0xAA;
	EECON1bits.WR = 1; // end critical sequence
	while( EECON1bits.WR )
	{
	} //Wait till the write completion

	INTCONbits.GIE = GIE_BIT_VAL;
	EECON1bits.WREN = 0;

	return;
}

//unsigned long eepromCalibrate1Read( void )
//{
//	unsigned long value;
//	unsigned char valueByte0;
//	unsigned char valueByte1;
//	unsigned char valueByte2;
//	unsigned char valueByte3;
//
//	valueByte0 = eepromRead( EEPROM_ADDR_CALIBRATION_1_BYTE_0 );
//	valueByte1 = eepromRead( EEPROM_ADDR_CALIBRATION_1_BYTE_1 );
//	valueByte2 = eepromRead( EEPROM_ADDR_CALIBRATION_1_BYTE_2 );
//	valueByte3 = eepromRead( EEPROM_ADDR_CALIBRATION_1_BYTE_3 );
//
//	value = (unsigned long)( (unsigned long)valueByte0 << 24 ) | ( (unsigned long)valueByte1 << 16 ) | ( (unsigned long)valueByte2 << 8 ) | ( (unsigned long)valueByte3 );
//
//	return value;
//}

//void eepromCalibrate1Write( unsigned long value )
//{
//	unsigned char valueByte0;
//	unsigned char valueByte1;
//	unsigned char valueByte2;
//	unsigned char valueByte3;
//
//	valueByte0 = ( value & 0xFF000000 ) >> 24;
//	valueByte1 = ( value & 0x00FF0000 ) >> 16;
//	valueByte2 = ( value & 0x0000FF00 ) >> 8;
//	valueByte3 = ( value & 0x000000FF );
//
//	eepromWrite( EEPROM_ADDR_CALIBRATION_1_BYTE_0, valueByte0 );
//	eepromWrite( EEPROM_ADDR_CALIBRATION_1_BYTE_1, valueByte1 );
//	eepromWrite( EEPROM_ADDR_CALIBRATION_1_BYTE_2, valueByte2 );
//	eepromWrite( EEPROM_ADDR_CALIBRATION_1_BYTE_3, valueByte3 );
//
//	return;
//}

//unsigned long eepromCalibrate2Read( void )
//{
//	unsigned long value;
//	unsigned char valueByte0;
//	unsigned char valueByte1;
//	unsigned char valueByte2;
//	unsigned char valueByte3;
//
//	valueByte0 = eepromRead( EEPROM_ADDR_CALIBRATION_2_BYTE_0 );
//	valueByte1 = eepromRead( EEPROM_ADDR_CALIBRATION_2_BYTE_1 );
//	valueByte2 = eepromRead( EEPROM_ADDR_CALIBRATION_2_BYTE_2 );
//	valueByte3 = eepromRead( EEPROM_ADDR_CALIBRATION_2_BYTE_3 );
//
//	value = (unsigned long)( (unsigned long)valueByte0 << 24 ) | ( (unsigned long)valueByte1 << 16 ) | ( (unsigned long)valueByte2 << 8 ) | ( (unsigned long)valueByte3 );
//
//	return value;
//}

//void eepromCalibrate2Write( unsigned long value )
//{
//	unsigned char valueByte0;
//	unsigned char valueByte1;
//	unsigned char valueByte2;
//	unsigned char valueByte3;
//
//	valueByte0 = ( value & 0xFF000000 ) >> 24;
//	valueByte1 = ( value & 0x00FF0000 ) >> 16;
//	valueByte2 = ( value & 0x0000FF00 ) >> 8;
//	valueByte3 = ( value & 0x000000FF );
//
//	eepromWrite( EEPROM_ADDR_CALIBRATION_2_BYTE_0, valueByte0 );
//	eepromWrite( EEPROM_ADDR_CALIBRATION_2_BYTE_1, valueByte1 );
//	eepromWrite( EEPROM_ADDR_CALIBRATION_2_BYTE_2, valueByte2 );
//	eepromWrite( EEPROM_ADDR_CALIBRATION_2_BYTE_3, valueByte3 );
//
//	return;
//}

