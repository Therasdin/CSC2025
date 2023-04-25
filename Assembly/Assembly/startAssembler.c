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

//REGISTERS
#define AXREG 0
#define BXREG 1
#define CXREG 2
#define DXREG 3

//commands
#define HALT 5
#define MOVREG 192

//boolean
#define TRUE 1
#define FALSE 0

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
Memory memory[MAX] = { 0 };   //global variable the memory of the virtual machine
Memory address;     //globla variable the current address in the virtual machine


void runCode( );	// Executes the machine code	****NEEDS WORK***
void splitCommand( char line[ ], char command[ ], char oper1[ ], char oper2[ ] );	// splits line of asm into it's three parts	****NEEDS WORK***
void convertToMachineCode( FILE *fin );	// Converts a single line of ASM to machine code	***NEEDS WORK***
void assembler( );			// Converts the entire ASM file and stores it in memory
void printMemoryDump( );	// Prints memeory with commands represented as integes

// Helper functions.
void convertToNumber( char line[ ], int *start, int *value );	// converts a sub-string to an int
int whichReg( char regLetter );			// Returns the number of the letter registar
void changeToLowerCase( char line[ ] );	// Changes each character to lower case
void printMemoryDumpHex( );				// Prints memory in hexedecimal


int main( )
{
	printMemoryDump( );  //displays the starting memory
	assembler( );
	//runCode( );
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
	FILE *fin;		// File pointer for reading in the assembly code.

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

	//to be coded
}

/********************   splitCommand   ***********************
splits line of asm into it's three parts

Needs work
-----------------------------------------------------------*/
void splitCommand( char line[ ], char command[ ], char oper1[ ], char oper2[ ] )
{
	char *spacePtr = strchr( line, ' ' );  //the pointer where the first space occures
	int index;
	if ( spacePtr == NULL )  //no space 
	{
		strcpy( command, line);
		oper1 = "";
		oper2 = ""; 
	}
	else
	{
		index = (int)(spacePtr - line);  //the differnce between the start point and the space

		printf( "\nIndex is: %d", index );
		if ( index < 1 || index > 3 )
		{
			printf( "\a\a\tnumber not in the range\n" );
			system( "pause" );
			exit( 1 );	// This is temporary. You must find a way to deal with index out of bounds.
		}

		strncpy( command, line, index);	// move the command into the command variable
		command[index] = '\0';				// add the string stopper
		line += index;
		spacePtr = strchr(line, ' ');
		index = (int)(spacePtr - line);
		strncpy(oper1, line, index);
		oper1[index] = '\0';
		line += index;
		strcpy(oper2, line);


	}
	printf( "\nCommand = %s %s %s", command, oper1, oper2 );

}

/********************   convertToMachineCode   ***********************
Converts a single line of ASM to machine code

Needs work
---------------------------------------------------------------------*/
void convertToMachineCode( FILE *fin )
{
	char line[LINE_SIZE];		// full command
	char command[LINE_SIZE];	// the asm commmand
	char oper1[LINE_SIZE], oper2[LINE_SIZE];	// the two operands, could be empty
	int machineCode = 0;			// One line of converted asm code from the file

	fgets( line, LINE_SIZE, fin );		// Takes one line from the asm file
	changeToLowerCase( line );

	splitCommand( line, command, oper1, oper2 );
	if ( command[0] == 'h' )  //halt
	{
		memory[address] = HALT;
		address++;
	}
	else if ( command[0] == 'm' )  //move into a register
	{
		machineCode = MOVREG;
		printf(\n % d, machineCode);
		machineCode = machineCode + (whichReg(oper1[0]<<3));
		printf(\n%d , machineCode);
	}
	printf( "\n" );
	printMemoryDump( );
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
			for ( int column = 0; location < MAX&&column < COL; column++ )
		{
			if ( !(numRows - 1 == row&&carryOver - 1 < column) )
			{
				printf( "%5d.%5d", location, memory[location] );
				location += (numRows - (carryOver - 1 < column));
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
void convertToNumber( char line[ ], int *start, int *value )
{
	char number[16];
	int negative = FALSE;
	int i = 0;

	if ( line[*start] == '-' )
	{
		++*start;
		negative = TRUE;
	}
	while ( i < 16 && strlen( line ) > *start && isdigit( line[*start] ) )
	{
		number[i] = line[*start];
		i++;
		++*start;
	}
	number[i] = '\0';
	*value = atoi( number );
	if ( negative == TRUE )
	{
		*value = -*value;// *-1;
	}
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
		line[index] = tolower( line[index] );
		index++;
	}
}

/*********************   changeToLowerCase   ********************
Prints memory in hexedecimal
----------------------------------------------------------------*/
void printMemoryDumpHex( )
{
	int numRows = MAX / COL + 1;	//number of rows that will print
	int carryOver = MAX % COL;		//number of columns on the bottom row
	int location;   //the current location being called
	for (int row = 0; row < numRows; row++)
	{
		location = row;
		//print(setw( 3 ) << location << "." << setw( 5 ) << memory[location];
		for (int column = 0; location < MAX && column < COL; column++)
		{
			if (!(numRows - 1 == row && carryOver - 1 < column))
			{
				printf( "%5d.%3x", location, memory[location] );
				location += (numRows - (carryOver - 1 < column));
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
