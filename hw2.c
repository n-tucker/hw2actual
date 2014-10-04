/*
   hw2.c

   COMP1917 Computing 1

   Program supplied as a starting point for
   Assignment 2: Message Board

   UNSW Session 2, 2014
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "hw2.h"


int globalMessageNum = 1;


int main( void )
{
  MsgNode *list = NULL;
  MsgNode *node;
  char command[MAX_LINE];
  char c;

  printPrompt();

  // enter a loop, reading and executing commands from the user
  while( fgets(command,MAX_LINE,stdin) != NULL ) {
    char *p;

    // replace newline with end-of-string character
    if(( p = strchr(command,'\n')) != NULL ) {
      *p = '\0';
    }
    p = command;
    while( isspace(*p)) { // skip any initial spaces
      p++;
    }
    c = *p;
    if( isdigit(c)) {

      // INSERT CODE FOR JUMPING TO MESSAGE k
    }
    else switch( c ) {
	int i = 1;
    case 'a': case 'A': // Add item
      // MODIFY THIS CODE, AS APPROPRIATE
      list = add_to_tail( node = getNode(), list ); //creates new node, stores in 'node' and then returns the head of the linked list to 'list'
      printFull( node );
      break;
	case 'l': case 'L':
		node = list;
		if (list != NULL) {
			while (node->messageNum < globalMessageNum-1) {
				printf("  %2d ", node->messageNum);
				printBrief(node);
				node = node->next;
				//printf("test");
				i++;
			}
			if ((node->messageNum == globalMessageNum-1) && globalMessageNum >1) {
				printf("->%2d ",node->messageNum);
				printBrief(node);
			}
		}
		break;
	case 'p': case 'P':
		printFull( node );
		break;	
	
		

      // INSERT CODE HERE

      // TO IMPLEMENT OTHER COMMANDS

    case 'h': case 'H': // Help
		printHelp();
		break;

    case 'q': case 'Q': // Quit
      freeList( list );
      printf("Bye!\n");
      return 0;
      break;
    }

    printPrompt();
  }

  return 0;
}

// INSERT NEW FUNCTIONS, AS APPROPRIATE

/************************************************************
   Print prompt only if output goes to screen
*/
void printPrompt()
{
  if (isatty(fileno(stdin))) {
    printf("Enter command (A,F,B,P,L,D,R,T,S,U,Q, H for Help): ");
  }
}

/************************************************************
   Print the list of commands available to the user,
   and a brief summary of what each command does.
*/
void printHelp()
{
  printf("\n");
  printf(" A - Add\n" );
  printf(" L - List\n" );
  printf(" P - Print\n" );
  printf(" F - Forward\n" );
  printf(" B - Back\n" );
  printf("<k>- jump to message k\n");
  printf(" D - Delete\n");
  printf(" R - Reply\n");
  printf(" T - Threads\n");
  printf(" S - Search\n");
  printf(" U - Undo\n" );
  printf(" Q - Quit\n");
  printf(" H - Help\n");
  printf("\n");
}

/************************************************************
   allocate space for a new message and get the
   name, date, time and text from standard input.
*/
MsgNode * getNode( void )
{
  MsgNode * new_node;
  new_node = (MsgNode *)malloc(sizeof(MsgNode));
  if( new_node == NULL ) {
     printf("Error: could not allocate memory.\n");
     exit( 1 );
  }
  new_node->messageNum= globalMessageNum++;
  new_node->inReplyTo = 0;
  new_node->indent    = 0;
  new_node->deleted   = FALSE;
  new_node->name      = getName();
  getDate( &new_node->date );
  getTime( &new_node->time );
  new_node->text      = getText();
  new_node->next      = NULL;

  return( new_node );
}

/************************************************************
   Read one line of text from standard input,
   store it in a string and return a pointer to the string.
*/
char * getName( void )
{
  char buffer[MAX_LINE];
  char *name;
  int length;
  int ch;
  int i;

  // prompt user for input
  printf( "Name: " );
  // skip any intial newline character
  if(( ch = getchar()) == '\n' ) {
     ch = getchar();
  }
  // read text initially into a buffer
  i=0;
  while( i < MAX_LINE && ch != '\n' && ch != EOF ) {
     buffer[i++] = ch;
     ch = getchar();
  }
  // trim of any trailing whitespace
  while( isspace( buffer[i-1] )) {
    i--;
  }
  // allocate just enough space to store the string
  length = i;
  name = (char *)malloc((length+1)*sizeof(char));
  if( name == NULL ) {
     printf("Error: could not allocate memory.\n");
     exit( 1 );
  }
  // copy text from buffer into new string
  for( i=0; i < length; i++ ) {
     name[i] = buffer[i];
  }
  name[i] = '\0'; // add end-of-string marker

  return( name );
}

/************************************************************
   Read several lines of text from standard input,
   store them in a string and return a pointer to the string.
*/
char * getText( void )
{
	char buffer[MAX_TEXT];
	char *text;
	int length;
	int ch;
	int i;
	printf("Text: ");
	ch = getchar();
	i=0;
	while(( i < MAX_TEXT )&&( ch != EOF )) {
		buffer[i++] = ch;
		ch = getchar();
		// stop when you encounter a dot on a line by itself
		if( i > 1 && ch == '\n' && buffer[i-1] == '.'
								&& buffer[i-2] == '\n' ) {
			ch = EOF;
			i  = i-2; // strip off the dot and newlines
		}
	}
	length = i;
	// allocate just enough space to store the string
	text = (char *)malloc((length+1)*sizeof(char));
	if( text == NULL ) {
		printf("Error: could not allocate memory.\n");
		exit( 1 );
	}
	// copy text from buffer to new string
	for( i=0; i<length; i++ ) {
		text[i] = buffer[i];
	}
	text[i] = '\0'; // add end-of-string marker

	return( text );
}

/************************************************************
   Get date from standard input;
   if date is invalid, ask the user to re-enter it.
*/
void getDate( Date *d )
{
  printf("Date: ");
  while( !scanDate( d ) || !dateOK( d )) {
     printf("Re-enter date in format dd/mm/yy: ");
  }
}

/************************************************************
   scan date in the format dd/mm/yyyy
*/
int scanDate( Date *d )
{
  char s[MAX_LINE];

  fgets( s, MAX_LINE, stdin );
  if(sscanf(s,"%d/%d/%d",&d->day,&d->month,&d->year)<3){
    return FALSE;
  }
  if( d->year < 100 ) { // turn /12 into /2012, etc.
    d->year = 2000 + d->year;
  }
  return TRUE;
}

/************************************************************
   Get time from standard input;
   if time is invalid, ask the user to re-enter it.
*/
void getTime( Time *t )
{
  printf("Time: ");
  while( !scanTime( t ) || !timeOK( t )) {
     printf("Re-enter time in format hh:mm:ss: ");
  }
}

/************************************************************
   scan time in the format hh:mm:ss
*/
int scanTime( Time *t )
{
  char s[MAX_LINE];

  fgets( s, MAX_LINE, stdin );
  return(
     sscanf(s,"%d:%d:%d",&t->hour,&t->minute,&t->second)==3);
}

/************************************************************
   Return TRUE if date is valid; FALSE otherwise.
*/
int dateOK( Date *d ) {
	
  if (d->month == 2 && (d->year)%4 == 0) {
    return(   d->day   > 0 && d->day   <= 29
           && d->month > 0 && d->month <= 12
           && d->year >= 1582 );
  }
  else if (d->month == 2) {
    return(   d->day   > 0 && d->day   <= 28
           && d->month > 0 && d->month <= 12
           && d->year >= 1582 );
  }
  else if (d->month == 1 || d->month == 3 || d->month == 5 || d->month == 7 || d->month == 8 || d->month == 10 || d->month == 12) {
  return(   d->day   > 0 && d->day   <= 31
         && d->month > 0 && d->month <= 12
         && d->year >= 1582 );
  }
  else if(d->month == 4 || d->month == 6 || d->month == 9 || d->month == 11) {
    return(   d->day   > 0 && d->day   <= 30
           && d->month > 0 && d->month <= 12
           && d->year >= 1582 );
  }
    
  else {
  return FALSE;
  }


}

/************************************************************
   Return TRUE if time is valid; FALSE otherwise.
*/
int timeOK( Time *t )
{
  return(   t->hour   >= 0 && t->hour   < 24
         && t->minute >= 0 && t->minute < 60
         && t->second >= 0 && t->second < 60 );
}

// INSERT NEW FUNCTIONS, AS APPROPRIATE

/************************************************************
  Print the specified integer in two digits
  (prefixed with '0' if necessary)
*/
void printPadded( int n )
{
  if( n < 10 ) {
    putchar('0');
  }
  printf("%d",n );
}

/************************************************************
  Print date in the format dd/mm/yyyy
*/
void printDate( Date d )
{
  printPadded( d.day );
  putchar('/');
  printPadded( d.month );
  putchar('/');
  printf("%d", d.year );
}

/************************************************************
  Print time in the format hh:mm:ss
*/
void printTime( Time t )
{
  printPadded( t.hour );
  putchar(':');
  printPadded( t.minute );
  putchar(':');
  printPadded( t.second );
}

/************************************************************
  Print the Name, followed by the first line of the Text.
*/
void printBrief( MsgNode * msg )
{
  char *text=msg->text;
  int i=0,j=0;
  if( msg->deleted ) {
    printf("[deleted]\n");
  }
  else {
    printf("%s: ", msg->name );
    while( isspace( text[i] )) {
      i++;
    }
    while( j < 40 && text[i+j] != '\0'
                  && text[i+j] != '\n' ) {
      putchar( text[i+j] );
      j++;
    }
    putchar('\n');
  }
}

/************************************************************
  Print message in Full
*/
void printFull( MsgNode * msg )
{
  if( msg != NULL ) {
    printf("Message %d", msg->messageNum );
    if( msg->deleted ) {
      printf(" has been deleted.\n");
    }
    else {
      printf("\nDate: ");
      printDate( msg->date );
      printf("\nTime: ");
      printTime( msg->time );
      printf("\nName: %s\n", msg->name );
      printf("Text: %s\n", msg->text );
    }
  }
}

/************************************************************
  Free all memory occupied by linked list of messages
*/
void freeList( MsgNode *head )
{
  MsgNode *node;
  while( head != NULL ) {
    node = head;
    head = head->next;
    free( node->name );
    free( node->text );
    free( node );
  }
}

MsgNode * add_to_tail( MsgNode *new_node, MsgNode *head)
{
	if(head == NULL) {
		head = new_node;
	}
	else {
		MsgNode *node = head;
		while( node->next != NULL ) {
			node = node->next;
		}
		node->next = new_node;
	}
	return( head );
}
