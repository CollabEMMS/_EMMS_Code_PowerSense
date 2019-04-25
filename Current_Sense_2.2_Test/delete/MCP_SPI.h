/* 
 * File:   MCP_SPI.h
 * Author: austin
 *
 * Created on April 16, 2017, 4:43 PM
 */

#ifndef MCP_SPI_H
#define	MCP_SPI_H


extern void mcpSPIInit( void );
extern void mcpSPIStart( void );

// MCP SPI MSDO
// set pin 16 as an output for SPI
#define MCP_SPI_MSDO_DIR TRISCbits.TRISC5

// MCP SPI MSDI
// set pin 15 as an input for SPI    
#define MCP_SPI_MSDI_DIR TRISCbits.TRISC4

// MCP SPI MSLK
// set pin 14 as the SPI clock SCK    
#define MCP_SPI_MSCK_DIR TRISCbits.TRISC3

// MCP SPI CS
// set pin 18 as the SPI CS for MCP
#define MCP_SPI_CS_DIR TRISCbits.TRISC7
#define MCP_SPI_CS_SET LATCbits.LATC7


#endif	/* MCP_SPI_H */
