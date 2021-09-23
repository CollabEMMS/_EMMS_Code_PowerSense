
#ifndef MCPSPI_H
#define	MCPSPI_H


/****************
 MACROS
 these are the macros that are required by external c files
 do not include macros that are only used internally within this module
 ****************/

//// MCP SPI MSDO
//// set pin 16 as an output for SPI
//#define MCP_SPI_MSDO_DIR TRISCbits.TRISC5
//
//// MCP SPI MSDI
//// set pin 15 as an input for SPI    
//#define MCP_SPI_MSDI_DIR TRISCbits.TRISC4
//
//// MCP SPI MSLK
//// set pin 14 as the SPI clock SCK    
//#define MCP_SPI_MSCK_DIR TRISCbits.TRISC3
//
//// MCP SPI CS
//// set pin 18 as the SPI CS for MCP
//#define MCP_SPI_CS_DIR TRISCbits.TRISC7
//#define MCP_SPI_CS_SET LATCbits.LATC7



/****************
 VARIABLES
 these are the globals required by external c files
 there should be as few of these as possible to help keep things clean
 these are all 'extern' and require that the variable is declared in the c file
  ****************/


/****************
 FUNCTION PROTOTYPES
 only include functions required by external c files
 ideally these are in the same order as in the code listing
 any functions used internally and externally must have the prototype in both the c and h files and should be marked
 
 *****************/


// external and internal

// external only






#endif	/* MCPSPI_H */

