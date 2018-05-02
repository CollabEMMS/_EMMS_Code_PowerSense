/* 
 * File:   config.h
 * Author: austin
 *
 * Created on April 16, 2017, 2:42 AM
 */

#ifndef CONFIG_H
#define	CONFIG_H 

#define _XTAL_FREQ 16000000  //16 Mhz

// indicator LED 3
#define LED1_DIR TRISCbits.TRISC0
#define LED1_SET LATCbits.LATC0
#define LED1_READ PORTCbits.RC0
// indicator LED 3
#define LED2_DIR TRISCbits.TRISC1
#define LED2_SET LATCbits.LATC1
#define LED2_READ PORTCbits.RC1
// indicator LED 3
#define LED3_DIR TRISBbits.TRISB4
#define LED3_SET LATBbits.LATB4
#define LED3_READ PORTBbits.RB4
//
//// pulse input pin 3
//#define MCP_LFOUT_DIR TRISAbits.TRISA1
//#define MCP_LFOUT_READ PORTAbits.LATA1
//
//// pulse output (pass thru) pin 2
//#define MCP_LFOUT_PASS_DIR TRISAbits.TRISA0
//#define MCP_LFOUT_PASS_SET LATAbits.LATA0
//
//// MCP MCLR
//// set pin 17 as an output for MCLR
//#define MCP_MCLR_DIR TRISCbits.TRISC6
//#define MCP_MCLR_SET LATCbits.LATC6
//
//
//// MCP F0 Frequency Config
//#define MCP_FREQ_F0_DIR TRISCbits.TRISC7
//#define MCP_FREQ_F0_SET LATCbits.LATC7
//
//// MCP F1 Frequency Config
//#define MCP_FREQ_F1_DIR TRISCbits.TRISC5
//#define MCP_FREQ_F1_SET LATCbits.LATC5
//
//// MCP F2 Frequency Config
//#define MCP_FREQ_F2_DIR TRSICbits.TRISC3
//#define MCP_FREQ_F2_SET LATCbits.LATC3
//


// MCP SPI MSDO
// set pin 16 as an output for SPI
#define MCP_SPI_SDO_DIR TRISCbits.TRISC5
#define MCP_SPI_SDO_SET LATCbits.LATC5

// MCP SPI MSDI
// set pin 15 as an input for SPI    
#define MCP_SPI_SDI_DIR TRISCbits.TRISC4
#define MCP_SPI_SDI_READ PORTCbits.RC4

// MCP SPI MSLK
// set pin 14 as the SPI clock SCK    
#define MCP_SPI_CLK_DIR TRISCbits.TRISC3
#define MCP_SPI_CLK_SET LATCbits.LATC3

// MCP SPI CS
// set pin 18 as the SPI CS for MCP
#define MCP_SPI_CS_DIR TRISCbits.TRISC7
#define MCP_SPI_CS_SET LATCbits.LATC7


#endif
