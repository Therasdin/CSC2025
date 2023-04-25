/*********************************
* Lane Malone
* Assembly code
*revision date 3/29
* Lane0.asm
* This code scans and runs code from an asm file with different commands, and functions
*********************************/
//need to create the functions run the array notation. 
#define _CRT_SECURE_NO_WARNINGS  // lets us use depricated code

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

char ASM_FILE_NAME[ ] = "Lane0.asm";

#define MAX 150			// strlen of simulators memory can be changed
#define COL 7			// number of columns for output
#define LINE_SIZE 20	// For c-strings

//REGISTERS and other
#define AXREG 0
#define BXREG 1
#define CXREG 2
#define DXREG 3
#define ADDRESS 6
//#define MEMLOC 6
#define CONSTANT 7


//commands
#define FUNCTION 4
#define HALT 5
#define GET 6
#define PUT 7
#define JUMP 8
#define CMP 96
#define ADD 160
#define MOVREG 192
#define MOVMEM 224

//Jumps
#define JE 0
#define JNE 1
#define JB 2
#define JBE 3
#define JA 4
#define JAE 5
#define JMP 6

//boolean
#define TRUE 1
#define FALSE 0

//masks
#define COMMANDMASK 224
#define REGMASK 24
#define ENDMASK 7

enum operType { reg, mem, constant, arrayBx, arrayBxPlus, none };  //list of all types of operand types


//types of registers 
struct Registers
{
	int AX;
	int BX;
	int CX;
	int DX;
	int flag;
}regis;

typedef short int Memory;  //sets the type of memory to short int 
Memory memory[ MAX ] = { 0 };   //global variable the memory of the virtual machine
Memory address;     //globla variable the current address in the virtual machine


void runCode( );	// Executes the machine code	****NEEDS WORK***
void splitCommand( char line[ ], char command[ ], char part2[ ], char part3[ ] );	// splits line of asm into it's three parts	****NEEDS WORK***
void convertToMachineCode( FILE* fin );	// Converts a single line of ASM to machine code	***NEEDS WORK***
void assembler( );			// Converts the entire ASM file and stores it in memory
void printMemoryDump( );	// Prints memeory with commands represented as integes
void funConvert( char line[ ] ); // converts function to machine code

// Helper functions.
int convertToNumber( char line[ ], int start );	// converts a sub-string to an int
int whichReg( char regLetter );			// Returns the number of the letter registar
void changeToLowerCase( char line[ ] );	// Changes each character to lower case
void printMemoryDumpHex( );				// Prints memory in hexedecimal
/********************   PutValue   ***********************

Puts value into whichever register it belongs
-----------------------------------------------------------*/
void putValue( int part2, int value )
{
	switch ( part2 )
	{
		case AXREG:
			regis.AX = value;
			break;
		case BXREG:
			regis.BX = value;
			break;
		case CXREG:
			regis.CX = value;
			break;
		case DXREG:
			regis.DX = value;
			break;
		default:
			printf( "%d", part2 );
			printf( "Error in putValue" );
			break;

	}
}
/********************   GetValue   ***********************

Gets register value
Gets a register and returns whats stored at the address 
-----------------------------------------------------------*/
int getValue( int oper )
{
	switch ( oper )
	{
		case AXREG:
			return regis.AX;
		case BXREG:
			return regis.BX;
		case CXREG:
			return regis.CX;
		case DXREG:
			return regis.DX;
		case CONSTANT:
			address++;
			return memory[ address ];
		case ADDRESS:
			address++;
			return memory[ address ];

		default:
			printf( "Error in getValue" );
			break;
	}
}

int main( )
{
	assembler( );
	runCode( );
	printMemoryDump( );  //displays memory with final vaules
	printf( "\n" );
	system( "pause" );
	return 0;
}

/********************   assembler   ***********************

changes the assembly code to machine code and places the
commands into the memory.
-----------------------------------------------------------*/
void assembler( )
{
	address = 0;
	FILE* fin;		// File pointer for reading in the assembly code.

	//recommend changeing so you can type in file name
	fopen_s( &fin, ASM_FILE_NAME, "r" );
	if ( fin == NULL )
	{
		printf( "Error, file didn't open\n\nExiting program...\n\n" );
		system( "pause" );
		exit( 1 );
	}

	for ( int i = 0; i < MAX && !feof( fin ); i++ )
	{
		convertToMachineCode( fin );
	}
}



/********************   runCode   ***********************
Executes the machine code

Needs to be written
-----------------------------------------------------------*/
void runCode( )
{
	address = 0;
	int tempAddress = 0;
	while ( memory[ address ] != HALT )
	{
		int part1 = memory[ address ] & COMMANDMASK;
		int part2 = ( memory[ address ] & REGMASK ) >> 3;
		int part3 = memory[ address ] & ENDMASK;
		if ( part1 > 0 )
		{
			int value = getValue( part3 );
			if ( part1 == MOVREG )
			{
				putValue( part2, value );
			}
			else if ( part1 == ADD )
			{
				putValue( part2, value + getValue( part2 ) );
			}
			else if ( part1 == MOVMEM )
			{
				tempAddress = getValue( part3 );
				value = getValue( part2 );
				memory[ tempAddress ] = value;
			}
			else if ( part1 == CMP )
			{
				int value1 = getValue( part2 );
				int value2 = getValue( part3 );
				if ( value1 > value2 )
				{
					regis.flag = 1;
				}
				else if ( value1 < value2 )
				{
					regis.flag = -1;
				}
				else
				{
					regis.flag = 0;
				}
			}
		}

		else if ( part2 > 0 )
		{
			if ( part2 == JUMP )
			{

				if ( part3 == JE && regis.flag == 0 ) //jump Equal
				{
					address = memory[ address + 1 ] - 1;
				}
				else if ( part3 == JNE && regis.flag != 0 ) //jump Not Equal
				{
					address = memory[ address + 1 ] - 1;
				}
				else if ( part3 == JB && regis.flag == -1 ) //jump Below
				{
					address = memory[ address + 1 ] - 1;
				}
				else if ( part3 == JBE && regis.flag <= -1 ) //jump Below Equal
				{
					address = memory[ address + 1 ] - 1;
				}
				else if ( part3 == JA && regis.flag == 1 ) //jump Above
				{
					address = memory[ address + 1 ] - 1;
				}
				else if ( part3 == JAE && regis.flag >= 0 ) //jump Above Equal
				{
					address = memory[ address + 1 ] - 1;
				}
				else if ( part3 == JMP ) //jump
				{
					address = memory[ address + 1 ] - 1;
				}
				else if ( part3 == 7 && regis.flag == 0 ) //error
				{
					printf( "Error \n" );
				}
			}
		}
		else if ( part3 == PUT )
		{
			printf( "\n Output: %d\n", regis.AX );
		}
		else if ( part3 == GET )
		{
			printf( "Enter A value" );
			scanf("%d", &regis.AX );
		}
		address++;
		printMemoryDump( );
		//add an if statement
	}
	system( "pause" );
}

/********************   splitCommand   ***********************
splits line of asm into it's three parts

works
-----------------------------------------------------------*/
void splitCommand( char line[ ], char command[ ], char part2[ ], char part3[ ] )
{
	char* spacePtr = strchr( line, ' ' );  //the pointer where the first space occures
	int index;
	if ( spacePtr == NULL )
	{ // no space
		strcpy( command, line );
		strcpy( part2, "" );
		strcpy( part3, "" );
	}
	else
	{
		index = ( int ) ( spacePtr - line ); // the difference between the start point and the space
		printf( "\nIndex is: %d", index );
		if ( index < 1 || index > 3 )
		{
			printf( "\a\a\tnumber not in the range\n" );
			system( "pause" );
			exit( 1 ); // This is temporary. You must find a way to deal with index out of bounds.
		}
		sscanf( line, "%[^ ] %[^ ] %s", command, part2, part3 );
	}
	printf( "\nCommand = %s %s %s", command, part2, part3 );

}

/********************   convertToMachineCode   ***********************
Converts a single line of ASM to machine code

Needs work
---------------------------------------------------------------------*/
void convertToMachineCode( FILE* fin )
{
	int value = 0;
	char temp[LINE_SIZE];
	char line[ LINE_SIZE ];		// full command
	char command[ LINE_SIZE ];	// the asm commmand
	char part2[ LINE_SIZE ], part3[ LINE_SIZE ];	// the two operands, could be empty
	int machineCode = 0;			// One line of converted asm code from the file

	fgets( line, LINE_SIZE, fin );		// Takes one line from the asm file
	if (  line[0] == '\n' )
	{
		address++;
		return;
	}
	changeToLowerCase( line );

	if ( line[0] == 'f' )
	{
		funConvert(line);
		return;
	}

	splitCommand( line, command, part2, part3 );
	if ( command[ 0 ] == 'h' )  //halt
	{
		memory[ address ] = HALT;
		address++;
		return;
	}
	else if ( command[ 0 ] == 'p' )
	{
		machineCode = PUT;
		memory[ address ] = machineCode;
		address++;
		return;
	}
	else if ( command[ 0 ] == 'g' )
	{
		memory[ address ] = GET;
		address++;
		return;
	}
	else if ( command[ 0 ] == 'j' )
	{
		machineCode = JUMP;
		if ( strcmp( command, "je" ) == 0 ) // X = Y
		{
			machineCode += JE;
			
		}
		else if ( strcmp( command, "jne" ) == 0 ) // X != y
		{
			machineCode += JNE;
		}
		else if ( strcmp( command, "jb" ) == 0 ) // X < Y
		{
			machineCode += JB;
		}
		else if ( strcmp( command, "jbe" ) == 0 ) // X <= Y
		{
			machineCode += JBE;
		}
		else if ( strcmp( command, "ja" ) == 0 ) // X > Y
		{
			machineCode += JA;
		}
		else if ( strcmp( command, "jae" ) == 0 ) // X >=Y
		{
			machineCode += JAE;
		}
		else if ( strcmp( command, "jmp" ) == 0 )
		{
			machineCode += JMP;
		}
		else // not valid (problem)
		{
			machineCode += 7;
		}
		memory[ address ] = machineCode;
		address++;
		memory[ address ] = convertToNumber( part2, 1 );
		address++;
	}
	else
	{
	if ( command[ 0 ] == 'm' )  //move into a register
	{
		if ( part2[ 0 ] == '[' )
		{
			machineCode = MOVMEM;
			machineCode = machineCode + ( valueFrom( part2 ) );
			machineCode = machineCode + ( valueFrom( part3 ) << 3 );
			memory[ address ] = machineCode;
			address++;
		}
		else
		{
			machineCode = MOVREG;
			machineCode = machineCode + ( whichReg( part2[ 0 ] ) << 3 );
			machineCode += ( valueFrom( part3 ) );
			memory[ address ] = machineCode;
			address++;
		}
	}
	else if ( command[ 0 ] == 'a' )
	{
		machineCode = ADD;
		machineCode = machineCode + ( whichReg( part2[ 0 ] ) << 3 );
		machineCode += ( valueFrom( part3 ) );
		memory[ address ] = machineCode;
		address++;
	}
	else if ( command[ 0 ] == 'c' )
	{
		machineCode = CMP;
		machineCode = machineCode + ( whichReg( part2[ 0 ] ) << 3 );
		machineCode += ( valueFrom( part3 ) );
		memory[ address ] = machineCode;
		address++;
	}
	if ( valueFrom( part3 ) == CONSTANT )
	{
		memory[ address ] = convertToNumber( part3, 0 );
		address++;
	}
	else if ( valueFrom( part3 ) == ADDRESS )
	{
		memory[ address ] = convertToNumber( part3, 1 );
		address++;
	}
	else if ( valueFrom( part2 ) == ADDRESS )
	{
		memory[ address ] = convertToNumber( part2, 1 );
		address++;
	}
	
	}
	printf( "\n" );
	printMemoryDump( );
}
void funConvert( char line[ ] )
{
	memory[ address ] = FUNCTION;
	address++;
	memory[ address ] = convertToNumber( line, 5 );
	address++;
	int index = 6;
	int num;
	while ( line[ index ] != ' ' )
	{
		index++;
	}
	index++;
	memory[ address ] = convertToNumber( line, index );
	num = memory[ address ];
	address++;
	int count = 0;
	while ( num > count )
	{
		while ( line[ index ] != ' ' )
		{
			index++;
		}
		index++;
		if ( line[ index ] == '[' )
		{
			index++;
		}
		memory[ address ] = convertToNumber( line, index );
		address++;
		count++;
	}
	printf( "\nCommand = %s ", line );
	address++;
}

int valueFrom( char thisOper[ ] )
{
	int newOper; //holds the value

	if ( thisOper[ 0 ] == 'a' )
	{
		newOper = AXREG;
	}
	else if ( thisOper[ 0 ] == 'b' )
	{
		newOper = BXREG;
	}
	else if ( thisOper[ 0 ] == 'c' )
	{
		newOper = CXREG;
	}
	else if ( thisOper[ 0 ] == 'd' )
	{
		newOper = DXREG;
	}
	else if ( thisOper[ 0 ] == '[' )
	{
		newOper = ADDRESS;
	}
	else
	{
		newOper = CONSTANT;
	}

	return newOper;
}


/****************************   printMemoryDump   ********************************
prints memory by number
MAX is the amount of elements in the memory array (Vicki used 100)
COL is the number of columns that are to be displayed (Vicki used 7; was originally called COLUMNS)
---------------------------------------------------------------------------------*/
void printMemoryDump( )
{
	int numRows = MAX / COL + 1;	//number of rows that will print
	int carryOver = MAX % COL;		//number of columns on the bottom row
	int location;   //the current location being called
	for ( int row = 0; row < numRows; row++ )
	{
		location = row;
		for ( int column = 0; location < MAX && column < COL; column++ )
		{
			if ( !( numRows - 1 == row && carryOver - 1 < column ) )
			{
				printf( "%5d.%5d", location, memory[ location ] );
				location += ( numRows - ( carryOver - 1 < column ) );
			}
		}
		printf( "\n" );
	}

	printf( "\n" );
	printf( "AX:%d\t", regis.AX );
	printf( "BX:%d\t", regis.BX );
	printf( "CX:%d\t", regis.CX );
	printf( "DX:%d\t", regis.DX );
	printf( "\n\n" );
	printf( "Instruction: %d\n", address );
	printf( "Flag: %d", regis.flag );

	printf( "\n\n" );
}


//-----------------------------------------------------------------------------
//**************   Helper functions   *****************************************
// may be deleted
//-----------------------------------------------------------------------------


/*****************************   ConvertToNumber   ******************************
takes in a line (string) and converts the characters that they represent.
*	line - is the string of assembly code to convert
*	start - is the location where the line is being coverted,
		it starts at the beginning of number and it passed
		back at the next location
*	value - is the integer value of the digits in the code
---------------------------------------------------------------------------------*/
int convertToNumber( char line[ ], int start )
{
	char number[ 16 ];
	int negative = FALSE;
	int value = 0;
	int i = 0;

	if ( line[ start ] == '-' )
	{
		++start;
		negative = TRUE;
	}
	while ( i < 16 && strlen( line ) > start && isdigit( line[ start ] ) )
	{
		number[ i ] = line[ start ];
		i++;
		++start;
	}
	number[ i ] = '\0';
	value = atoi( number );
	if ( negative == TRUE )
	{
		value = -value;// *-1;
	}
	return value;
}

/*********************   whichReg   *************************
changes the letter of the registar to a number.
------------------------------------------------------------*/
int whichReg( char regLetter )
{
	if ( regLetter == 'a' )
	{
		return AXREG;
	}
	else if ( regLetter == 'b' )
	{
		return BXREG;
	}
	else if ( regLetter == 'c' )
	{
		return CXREG;
	}
	else if ( regLetter == 'd' )
	{
		return DXREG;
	}
	return -1;  //something went wrong if -1 is returned
}

/*********************   changeToLowerCase   ********************
Changes each character to lower case
----------------------------------------------------------------*/
void changeToLowerCase( char line[ ] )
{
	int index = 0;
	while ( index < strlen( line ) )
	{
		line[ index ] = tolower( line[ index ] );
		index++;
	}
}

/*********************   printMemoryDumpHex   ********************
Prints memory in hexedecimal
----------------------------------------------------------------*/
void printMemoryDumpHex( )
{
	int numRows = MAX / COL + 1;	//number of rows that will print 
	int carryOver = MAX % COL;		//number of columns on the bottom row
	int location;   //the current location being called
	for ( int row = 0; row < numRows; row++ )
	{
		location = row;
		//print(setw( 3 ) << location << "." << setw( 5 ) << memory[location];
		for ( int column = 0; location < MAX && column < COL; column++ )
		{
			if ( !( numRows - 1 == row && carryOver - 1 < column ) )
			{
				printf( "%5d.%3x", location, memory[ location ] );
				location += ( numRows - ( carryOver - 1 < column ) );
			}
		}
		printf( "\n" );
	}

	printf( "\n" );
	printf( "AX:%d\t", regis.AX );
	printf( "BX:%d\t", regis.BX );
	printf( "CX:%d\t", regis.CX );
	printf( "DX:%d\t", regis.DX );
	printf( "\n\n" );
	printf( "Instruction: %d\n", address );
	printf( "Flag: %d", regis.flag );

	printf( "\n\n" );
}
/**********************************************\
*Problems: Infinite loop with PUT Now, wasn't accounting for blank lines?
* 
**********************************************/

