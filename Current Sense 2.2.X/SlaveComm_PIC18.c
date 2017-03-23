// UPDATED 2016-03-19

#include <xc.h>
#include <p18cxxx.h>

#include "Communications.h"
#include <stdbool.h>
//#include <pic18f25k22.h>

#define BUFFER_LENGTH 40  // max size is positive signed character size
#define PORT_COUNT 3 // one based count of the number of ports

#define PARAMETER_MAX_COUNT 5
#define PARAMETER_MAX_LENGTH 10

#define CHAR_NULL '\0'
#define COMMAND_SEND_RECEIVE_PRIMER_CHAR '#' // something to run the SPI clock so data can be received
#define COMMAND_START_CHAR '!'
#define COMMAND_END_CHAR '*'
#define COMMAND_DELIMETER ';'


#define LEDSET LATBbits.LATB4
#define LEDDIR TRISBbits.RB4
#define LEDREAD PORTBbits.RB4

//#define COMMUNICATIONS_WATCHDOG_COUNT_LIMIT 10000
// this watchdog needs to be changed to time based - it is not the number of characters sent, it is the number of times the program has looped.
// depending on the CPU clock, this can be really fast or it can be slow.
// should mimic the delay_ms that is on the PIC 8


//#define SPI_SS PORTBbits.RB0

bool SPI_transmit_wait;

enum receive_status
{
    receive_waiting,
    receive_in_command,
    receive_end_command
};

struct buffer
{
    char data_buffer[ BUFFER_LENGTH];
    unsigned char write_position;
    unsigned char read_position;
};


//#define LEDGreen LATAbits.LATA2

//#define LEDRed LATAbits.LATA3



extern void delayMS( unsigned int );

bool SPI_receive_data( char* data );
void set_current_port( unsigned char * );
enum receive_status receive_data( struct buffer * );
bool process_data( struct buffer *receive_buffer, struct buffer *send_buffer );
void process_data_parameterize( char parameters[PARAMETER_MAX_COUNT][PARAMETER_MAX_LENGTH], struct buffer *buffer_to_parameterize );
bool process_data_parameters( char parameters[PARAMETER_MAX_COUNT][PARAMETER_MAX_LENGTH], struct buffer *send_buffer );

void command_builder1( struct buffer *send_buffer, char* data1 );
void command_builder2( struct buffer *send_buffer, char* data1, char* data2 );
void command_builder3( struct buffer *send_buffer, char* data1, char* data2, char* data3 );
void command_builder4( struct buffer *send_buffer, char* data1, char* data2, char* data3, char* data4 );
void command_builder_add_char( struct buffer *send_buffer, char data );
void command_builder_add_string( struct buffer *send_buffer, char *data );

bool send_data( struct buffer *send_buffer );
bool SPI_send_data( char data );

bool strmatch( char* a, char* b );
int strcmp2( char* a, char* b );
void strcpy2( char* rcv, char* source );

void resetCommunications( struct buffer * receive_buffer );

void send_end_of_transmission( struct buffer *send_buffer );
void com_command_testLED( struct buffer * send_buffer );


// alternative watchdog timer by using a timer
//void SPIWatchdogTimerInit( void );
//bool SPIWatchdogTimerCheck( unsigned char timer );
//void SPIWatchdogTimerReset( void );


/***********************
 main code body
 */

void communications( bool firstTime )
{
    static struct buffer receive_buffer;
    static struct buffer send_buffer;

    static bool end_of_transmission_received = false;
    bool no_more_to_send; // here to make this more readable

    static enum receive_status receive_current_state;

//    static unsigned long communications_watchdog_counter = 0;
    
    if( firstTime == true )
    {
//	communications_watchdog_counter = 0;
	send_buffer.write_position = 0;
	send_buffer.read_position = 0;
	resetCommunications( &send_buffer );
    }
    else
    {
	receive_current_state = receive_data( &receive_buffer );

	switch( receive_current_state )
	{
	case receive_waiting:
	    // don't need to worry about it too much
	    break;
	case receive_in_command:
	    // don't need to worry about it too much
	    break;
	case receive_end_command:

	    if( process_data( &receive_buffer, &send_buffer ) == true )
	    {
		end_of_transmission_received = true;
	    }
//	    communications_watchdog_counter = 0;
	    break;
	}

	if( end_of_transmission_received == true )
	{
	    // we reset the SPI port here BEFORE starting a new command
	    // if a character is placed in the buffer, it is lost when the reset happens
	    //resetCommunications( &send_buffer );
//	    communications_watchdog_counter = 0;
	    end_of_transmission_received = false;
	}

	no_more_to_send = send_data( &send_buffer );

//	communications_watchdog_counter++;
//
//	if ( SPIWatchdogTimerCheck(5))
//	{
//	    
//	    
//    TMR0H = 0;
//    TMR0L = 0;
//	    if(   LEDGreen == 1 )
//	 {
//	     LEDGreen = 0;
//	 }
//	 else
//	 {
//	     LEDGreen = 1;
//	 }
//	}
//	
//	
//	if( communications_watchdog_counter >= COMMUNICATIONS_WATCHDOG_COUNT_LIMIT )
//	{
//	    resetCommunications( &send_buffer );
//	    communications_watchdog_counter = 0;
//	}
	
	static bool last_state_active = false;
	if( PORTBbits.SS2 == 0b1 )
	{
	    LEDSET = 1;
	    last_state_active = false;

	    if ( last_state_active == true)
	    {
	//	resetCommunications( &send_buffer );
		last_state_active = false;
	    }
	}
	else
	{
	    LEDSET = 0;
	    if( last_state_active == false)
	    {
//		SPISlaveInit();
		resetCommunications( &send_buffer );
		RESET();
	    }

	    last_state_active = true;
	}

    }

    return;
}

void resetCommunications( struct buffer * send_buffer )
{
    //SSP2CON1bits.SSPEN = 0; //disable SPI
    //SSP2CON1bits.SSPEN = 1; //enable SPI
 SPISlaveInit(  );

    SSP2CON1bits.WCOL = 0;
    SPI_transmit_wait = false;

    send_buffer->read_position = 0;
    send_buffer->write_position = 0;

    // we can change this to whatever needs to happen
    com_command_testLED( send_buffer );
           
    return;
}

enum receive_status receive_data( struct buffer * receive_buffer )
{
    char data;

    static enum receive_status my_status = receive_waiting;

    if( my_status == receive_end_command )
    {
	my_status = receive_waiting;
    }

    if( SPI_receive_data( &data ) == true )
    {
	if( ( data == COMMAND_START_CHAR ) && ( my_status != receive_in_command ) )
	{
	    my_status = receive_in_command;
	    receive_buffer->read_position = 0;
	    receive_buffer->write_position = 0;
	}

	if( my_status == receive_in_command )
	{
	    receive_buffer->data_buffer[ receive_buffer->write_position] = data;

	    receive_buffer->write_position++;
	    if( receive_buffer->write_position >= BUFFER_LENGTH )
	    {
		receive_buffer->write_position = ( BUFFER_LENGTH - 1 );
	    }
	}

	if( ( my_status == receive_in_command ) && ( data == COMMAND_END_CHAR ) )
	{
	    my_status = receive_end_command;
	}
    }

    return my_status;
}

bool process_data( struct buffer *receive_buffer, struct buffer * send_buffer )
{
    bool end_of_transmission_received;

    // if we are here then the receive buffer must have good data with start and end command characters
    // the characters are not included as they were not added

    char parameters[PARAMETER_MAX_COUNT][PARAMETER_MAX_LENGTH];

    process_data_parameterize( parameters, receive_buffer );

    end_of_transmission_received = process_data_parameters( parameters, send_buffer );

    return end_of_transmission_received;

}

void process_data_parameterize( char parameters[PARAMETER_MAX_COUNT][PARAMETER_MAX_LENGTH], struct buffer * buffer_to_parameterize )
{
    unsigned char parameter_position = 0;
    unsigned char parameter_index = 0;

    // only one command is expected due to the way we read
    // go through buffer until we hit end char or end of buffer

    // this is super important - we must initialize the entire array
    // if we do not we risk passing junk into some functions that assume strings and check for NULL
    // without NULL a string function could run forever until we die from old age
    // even then it would keep running
    for( int inx = 0; inx < PARAMETER_MAX_COUNT; inx++ )
    {
	parameters[inx][0] = CHAR_NULL;
    }

    while( ( buffer_to_parameterize->data_buffer[buffer_to_parameterize->read_position ] != COMMAND_END_CHAR ) && ( buffer_to_parameterize->read_position < BUFFER_LENGTH ) && ( buffer_to_parameterize->read_position != buffer_to_parameterize->write_position ) )
    {
	switch( buffer_to_parameterize->data_buffer[buffer_to_parameterize->read_position] )
	{
	case COMMAND_START_CHAR:
	    // this character should never appear
	    break;
	case COMMAND_DELIMETER:
	    // move to next parameter
	    parameter_position = 0;
	    parameter_index++;

	    if( parameter_index >= PARAMETER_MAX_COUNT )
	    {
		// if we run out of parameters just overwrite the last one
		// we should never have this case, but this keeps us from crashing and burning
		parameter_index = ( PARAMETER_MAX_COUNT - 1 );
	    }

	    break;
	default:
	    // add the character to the current parameter
	    parameters[parameter_index][parameter_position] = buffer_to_parameterize->data_buffer[buffer_to_parameterize->read_position];
	    parameter_position++;
	    if( parameter_position >= PARAMETER_MAX_LENGTH )
	    {
		// if our parameter is too long, just overwrite the last character
		// we should never have this case, but this keeps us from crashing and burning
		parameter_position = ( PARAMETER_MAX_LENGTH - 1 );
	    }

	    // always make the last character a null
	    parameters[parameter_index][parameter_position] = CHAR_NULL;
	    break;
	}

	buffer_to_parameterize->read_position++;
    }

    return;
}

bool process_data_parameters( char parameters[PARAMETER_MAX_COUNT][PARAMETER_MAX_LENGTH], struct buffer * send_buffer )
{
    bool end_of_transmission_received = false;

    // the 'commands' shown here are for example only
    // make them whatever is needed

    // ideally, any new commands are set in a separate function called from one of these tests
    // it's not very clean to call the command builder functions from here
    // especially if there is some processing to do, like setting a clock or something


    if( strmatch( parameters[0], "END" ) == true )
    {
	end_of_transmission_received = true;
    }
    else if( strmatch( parameters[0], "Set" ) == true )
    {
	if( strmatch( parameters[1], "Power" ) == true )
	{
	    //set_power( parameters[3]);
	    // send reply?
	}
	else if( strmatch( parameters[1], "Volts" ) == true )
	{
	    //set_volts( parameters[2]);
	    // send reply?
	}
	else if( strmatch( parameters[1], "Current" ) == true )
	{
	    //set_current( parameters[3]);
	    // send reply?
	}

    }
    else if( strmatch( parameters[0], "Read" ) == true )
    {

	if( strmatch( parameters[1], "Power" ) == true )
	{
	    // send command power
	}
	else if( strmatch( parameters[1], "Volts" ) == true )
	{
	    // send command volts
	}
	else if( strmatch( parameters[1], "Current" ) == true )
	{
	    // send command current
	}

    }
    else if( strmatch( parameters[0], "Data" ) == true )
    {
	if( strmatch( parameters[1], "LEDB" ) == true )
	{
	    if( strmatch( parameters[2], "On" ) == true )
	    {
		command_builder3( send_buffer, "Set", "LEDB", "Off" );
	    }
	    else if( strmatch( parameters[2], "Off" ) == true )
	    {
		command_builder3( send_buffer, "Set", "LEDB", "On" );
	    }
	}
    }
    else if( strmatch( parameters[0], "Conf" ) == true )
    {
	if( strmatch( parameters[1], "LEDB" ) == true )
	{
	    send_end_of_transmission( send_buffer );
	}
    }

    // add new parameters as necessary
    // NEVER check for a higher parameter number than we allocated for.
    // see earlier comments about NULLS and dying from old age

    return end_of_transmission_received;
}

void command_builder1( struct buffer *send_buffer, char* data1 )
{
    command_builder_add_char( send_buffer,COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    command_builder_add_char( send_buffer,COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    command_builder_add_char( send_buffer, COMMAND_START_CHAR );
    command_builder_add_string( send_buffer, data1 );
    command_builder_add_char( send_buffer, COMMAND_END_CHAR );

    return;
}

void command_builder2( struct buffer *send_buffer, char* data1, char* data2 )
{
    command_builder_add_char( send_buffer,COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    command_builder_add_char( send_buffer,COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    command_builder_add_char( send_buffer,COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    command_builder_add_char( send_buffer,COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    command_builder_add_char( send_buffer,COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    command_builder_add_char( send_buffer,COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    command_builder_add_char( send_buffer, COMMAND_START_CHAR );
    command_builder_add_string( send_buffer, data1 );
    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    command_builder_add_string( send_buffer, data2 );
    command_builder_add_char( send_buffer, COMMAND_END_CHAR );

    return;
}

void command_builder3( struct buffer *send_buffer, char* data1, char* data2, char* data3 )
{
    command_builder_add_char( send_buffer,COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    command_builder_add_char( send_buffer,COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    command_builder_add_char( send_buffer, COMMAND_START_CHAR );
    command_builder_add_string( send_buffer, data1 );
    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    command_builder_add_string( send_buffer, data2 );
    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    command_builder_add_string( send_buffer, data3 );
    command_builder_add_char( send_buffer, COMMAND_END_CHAR );

    return;
}

void command_builder4( struct buffer *send_buffer, char* data1, char* data2, char* data3, char* data4 )
{
    command_builder_add_char( send_buffer,COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    command_builder_add_char( send_buffer,COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    command_builder_add_char( send_buffer, COMMAND_START_CHAR );
    command_builder_add_string( send_buffer, data1 );
    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    command_builder_add_string( send_buffer, data2 );
    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    command_builder_add_string( send_buffer, data3 );
    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    command_builder_add_string( send_buffer, data4 );
    command_builder_add_char( send_buffer, COMMAND_END_CHAR );

    return;
}

void command_builder_add_char( struct buffer *send_buffer, char data )
{
    send_buffer->data_buffer[send_buffer->write_position] = data;

    send_buffer->write_position++;
    if( send_buffer->write_position >= BUFFER_LENGTH )
    {
	send_buffer->write_position = 0;
    }

    return;
}

void command_builder_add_string( struct buffer *send_buffer, char *data_string )
{
    for( int inx = 0; data_string[inx] != CHAR_NULL; inx++ )
    {
	command_builder_add_char( send_buffer, data_string[inx] );
    }

    return;
}

bool send_data( struct buffer * send_buffer )
{
    bool send_end;

    if( send_buffer->read_position == send_buffer->write_position )
    {
	send_end = true;
	SPI_send_data( '\0' );
    }
    else
    {
	send_end = false;


	if( SPI_send_data( send_buffer->data_buffer[send_buffer->read_position] ) == true )
	{

	    send_buffer->read_position++;
	    if( send_buffer->read_position >= BUFFER_LENGTH )
	    {
		send_buffer->read_position = 0;
	    }
	}
    }

    return send_end;
}

bool strmatch( char* a, char* b )
{
    int result;
    bool match;

    result = strcmp2( a, b );

    match = ( result == 0 ) ? true : false;

    return match;
}

int strcmp2( char* a, char* b )
{
    int inx = 0;
    int match = 0;

    while( ( a[inx] != CHAR_NULL ) && ( b[inx] != CHAR_NULL ) && ( match == 0 ) )
    {
	if( a[inx] > b[inx] )
	{
	    match = 1;
	}
	else if( a[inx] < b[inx] )
	{
	    match = -1;
	}
	else if( a[inx] == b[inx] )
	{
	    //do nothing = never reset to zero
	}

	inx++;
    }
  
    
    if (( a[inx] == CHAR_NULL) && (b[inx] != CHAR_NULL ))
    {
            match = -1;
    }
    else if (( a[inx] != CHAR_NULL) && (b[inx] == CHAR_NULL ))
    {
            match = 1;
    }
    
    return match;
    
}

bool SPI_receive_data( char *data )
{

    bool recvGood = false;

    if( SSP2STATbits.BF == 1 )
    {
	*data = SSP2BUF;
	recvGood = true;
	SSP2CON1bits.WCOL = 0;
	SPI_transmit_wait = false;
    }
    else
    {
	recvGood = false;
    }

    return recvGood;

}

bool SPI_send_data( char data )
{
    bool sendGood = false;
   
    if( SPI_transmit_wait == false )
    {
	SSP2BUF = data;
	SPI_transmit_wait = true;
	    sendGood = true;
    }
    else
    {
	sendGood = false;
    }

    return sendGood;
}

/************************/
// RESPONSES

void send_end_of_transmission( struct buffer * send_buffer )
{
    command_builder1( send_buffer, "END" );

    return;
}

void com_command_testLED( struct buffer * send_buffer )
{
    command_builder2( send_buffer, "Read", "LEDB" );

    // we need to change this into a comm primer function
    
    // how do we drive the communications from here
    // send data
    //  voltage
    //  current
    //  power
    // do we need to ask for any new calibration parameters?
    
    return;
}

void SPISlaveInit( void )
{

    TRISAbits.TRISA0 = 0; // pin 2 connected as an output for pulse
    TRISAbits.TRISA1 = 1; // pin 3 connected as an input for pulse
    LEDDIR = 0; // pin 25 connected as an output for LED
    TRISCbits.TRISC3 = 0; // pin 14 connected as an output for pulse freq.
    TRISCbits.TRISC5 = 0; // pin 16 connected as an output for pulse freq.
    TRISCbits.TRISC6 = 0; // set pin 17 as an output for MCLR
    TRISCbits.TRISC7 = 0; // set pin 18 as an output for pulse freq.
    ANSELAbits.ANSA1 = 0b0; // turn off analog to digital conversion

    LATCbits.LATC6 = 1; // set the MCLR of the MCP high
    LATCbits.LATC3 = 1; // set pin 14 to a 1 to set freq. control F2 for pulse
    LATCbits.LATC5 = 1; // set pin 16 to a 1 to set freq. control F1 for pulse
    LATCbits.LATC7 = 1; // set pin 18 to a 1 to set freq. control F0 for pulse
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    SSP2CON1bits.SSPEN = 0; //Synchronous Serial Port Enable bit

    ANSELBbits.ANSB0 = 0b0;
    ANSELBbits.ANSB1 = 0b0;
    ANSELBbits.ANSB2 = 0b0;
    ANSELBbits.ANSB3 = 0b0;

    TRISBbits.RB0 = 0b1;
    TRISBbits.RB1 = 0b1;
    TRISBbits.RB2 = 0b1;
    TRISBbits.RB3 = 0b0;

    SSP2STATbits.SMP = 0;
    SSP2STATbits.CKE = 1;

    SSP2CON1bits.WCOL = 0; //Write Collision Detect bit
    SSP2CON1bits.SSPOV = 0; //Receive Overflow Indicator bit
    SSP2CON1bits.SSPEN = 0; //Synchronous Serial Port Enable bit
    SSP2CON1bits.CKP = 1; //Clock Polarity Select bit
    SSP2CON1bits.SSPM = 0b0100; //Synchronous Serial Port Mode Select bits


    SSP2CON3 = 0x00;
    SSP2CON3bits.BOEN = 0b0; //Buffer Overwrite Enable bit

    SSP2CON1bits.SSPEN = 1; //Synchronous Serial Port Enable bit

//    SPIWatchdogTimerInit(); 
    
    return;
}

//void SPIWatchdogTimerInit( )
//{
//    T0CONbits.T08BIT = 0;;
//    T0CONbits.T0CS = 0;
//    T0CONbits.T0SE = 0;
//    T0CONbits.PSA = 0;
//    T0CONbits.T0PS = 0b111;
//    
//    TMR0H = 0;
//    TMR0L = 0;
//
//    T0CONbits.TMR0ON = 1;
//    
//    return;
//}
//
//bool SPIWatchdogTimerCheck( unsigned char timer )
//{
//    bool time_past;
//    
//    // must read low byte first to cause high byte to be loaded
//    if ((TMR0L >= 0 )  && (TMR0H >= timer))
//    {
//	time_past = true;
//    }
//    else
//    {
//	time_past = false;
//    }
//
//    return time_past;
//    
//}
//
//void SPIWatchdogTimerReset()
//{
//    // the order is important - setting the Low byte causes the high byte to be stored as well
//    // just storing the high byte does nothing
//    TMR0H = 0;
//    TMR0L = 0;
//    
//}