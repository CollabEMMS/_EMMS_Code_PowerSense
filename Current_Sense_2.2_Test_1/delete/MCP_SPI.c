/************************
 MCP-3909 Communication Module
 Read values from MCP using SPI
 */
#include <stdio.h>
#include <stdbool.h>
#include "MCP_SPI.h"
#include "config.h"

#include <xc.h>
#include <p18f25k22.h>


void mcpSPIInit( void );
void mcpSPIStart( void );

void mcpSPIStart( void )
{
    mcp_spi_init( );

    MCP_SPI_CS_SET = 0;

    SSP1CON1bits.SSPEN = 1;

    MCP_MCLR_SET = 0;
    __delay_us( 100 );
    MCP_MCLR_SET = 1;
    __delay_us( 1 );

    MCP_SPI_CS_SET = 1;

    // set multiplier mode output with F2=0, F0=1
    SSP1BUF = 0b10100001;
    while ( SSP1STATbits.BF == 0 )
    {
	NOP( );
	// wait until it is clocked out
    }
    SSP1CON1bits.SSPEN = 0;

    return;
}

void mcpSPIInit( void )
{
    // use SPI 1

    MCP_SPI_MSDO_DIR = 0;
    MCP_SPI_MSDI_DIR = 1;
    MCP_SPI_MSCK_DIR = 0;
    MCP_SPI_CS_DIR = 0;
    MCP_SPI_CS_SET = 0;

    SSP1CON1bits.SSPEN = 0;
    SSP1CON1bits.WCOL = 0;
    SSP1CON1bits.SSPOV = 0;
    SSP1CON1bits.CKP = 0;
    SSP1CON1bits.SSPM = 0b0001;

    SSP1CON2 = 0b00000000;

    SSP1STAT = 0b00000000;
    SSP1STATbits.SMP = 1;
    SSP1STATbits.CKE = 0;

    SSP1CON3 = 0b00000000;
    SSP1MSK = 0b00000000;
    SSP1ADD = 0b00000000;

    return;

}

