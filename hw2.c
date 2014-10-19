/*
   hw2.c

   COMP1917 Computing 1

   Program supplied as a starting point for
   Assignment 2: Message Board

   UNSW Session 2, 2014
*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "hw2.h"


int globalMessageNum = 0;
int currentMessageNum = 0;
int main( void )
{
  MsgNode *list = NULL;
  MsgNode *node;
  char command[MAX_LINE];
  char c;
  int printCase = 0;
  int listCase = 0;
  printPrompt();

  // enter a loop, reading and executing commands from the user
  while( fgets(command,MAX_LINE,stdin) != NULL ) {
	Undo storage1;
	Undo storage2;
	
	storage2 = storage1;
	storage1.list = list;
	storage1.printCase = printCase;	//Storage for useful variables
	storage1.listCase = listCase;
	storage1.currentMessageNum = currentMessageNum;
	storage1.globalMessageNum = globalMessageNum;

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
		int i = c - '0';
		if ((node != NULL) && (i <= globalMessageNum) && (0 < i)) {
			node = list;
			while (node->messageNum != i) {	//Scans through until message with message number <k> is found 
			node = node->next;
			}
			currentMessageNum = node->messageNum;
			overallPrintArgument( printCase, list );
		}
    }
    else switch( c ) {
    case 'a': case 'A': // Add item
		list = add_to_tail( node = getNode(), list ); //creates new node, stores in 'node' and then returns the head of the linked list to 'list'
		overallPrintArgument(printCase, list);
		break;
	case 'l': case 'L':
		printCase = 1;
		listCase = 1;
		overallPrintArgument(printCase,list);
		break;
		
	case 'p': case 'P':
		if (list != NULL){
		printCase = 0;
		overallPrintArgument(printCase, currentNode(list));
		}
		break;
		
	case 'd': case 'D':
		node = currentNode(list);	//Gets the current node and sets msg->deleted to be true
		node->deleted = TRUE;
		overallPrintArgument(printCase, list);
		break;
		
	case 'f': case 'F':
		
		if (currentNode(list)->next != NULL) {
			if (((listCase == 0)||(listCase == 1)) && (currentMessageNum < globalMessageNum)) {
				currentMessageNum++;
			}		//Different display cases, depending on whether 'L' of 'P' was last selected
			else if (listCase == 2) {
				//if (currentNode(list)->next != NULL) {
				node = currentNode( list )->next;
				currentMessageNum = node->messageNum;	
			}
			overallPrintArgument(printCase, list);
		}
		break;
		
	case 'b': case 'B':
		if (currentNode (list) != list) {
			if ((listCase == 1) || (listCase == 0)) {
				currentMessageNum--;
			}		//Different display cases, depending on whether 'L' of 'P' was last selected
			else if (listCase == 2) {
				MsgNode *temp = list;
				while (temp-> next != currentNode(list) ){
				temp = temp->next;
				}
				currentMessageNum = temp-> messageNum;
			}
			overallPrintArgument(printCase, list);
		}
		break;
			
	case 'r': case 'R':
		if (list != NULL) {//&& (currentNode(list)->next != NULL)){
		storage1.isReply = 1;
		storage1.replynode = currentNode(list)->next;	//Sets new item to be next to replied item in linked list
		insertNode( list );
		overallPrintArgument(printCase, list);
		}
		break;
	
	case 't': case 'T':
		printCase = 2;
		listCase = 2;
		overallPrintArgument(printCase, list );
		break;
		
	
	case 's': case 'S':
		search(list);
		break;
	
	
	case 'u': case 'U':
		list = storage2.list;
		printCase = storage2.printCase;	//Checks to see if 'L' or 'P' was last used
		listCase = storage2.listCase;
		currentMessageNum = storage2.currentMessageNum;	//Gets variables from storage2 structure
		globalMessageNum = storage2.globalMessageNum;
		if (storage2.isReply == 1) {	//Checks to see if 'R' was last used
			currentNode(list)-> next = storage2.replynode;
		}
		overallPrintArgument(printCase, list);
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
  globalMessageNum++;
  new_node->found 	  = 0;
  new_node->messageNum= globalMessageNum;
  currentMessageNum   = globalMessageNum;
  new_node->inReplyTo = 0;
  new_node->indent    = 0;
  new_node->deleted   = FALSE;
  new_node->name      = getName();	//All the elements of a node
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
  if(sscanf(s,"%d/%d/%d",&d->day,&d->month,&d->year)<3){	//Checks for 3 imputs
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
  while( !scanTime( t ) || !timeOK( t )) {	//Checks for 3 imputs
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
	
  if (d->month == 2 && (d->year)%4 == 0) {	//Checks leap year case
    return(   d->day   > 0 && d->day   <= 29
           && d->month > 0 && d->month <= 12
           && d->year >= 1582 );
  }
  else if (d->month == 2) {	//Checks February case
    return(   d->day   > 0 && d->day   <= 28
           && d->month > 0 && d->month <= 12
           && d->year >= 1582 );
  }
  else if (d->month == 1 || d->month == 3 || d->month == 5 || d->month == 7 || d->month == 8 || d->month == 10 || d->month == 12) {
  return(   d->day   > 0 && d->day   <= 31	//All months with 31 days
         && d->month > 0 && d->month <= 12
         && d->year >= 1582 );
  }
  else if(d->month == 4 || d->month == 6 || d->month == 9 || d->month == 11) {
    return(   d->day   > 0 && d->day   <= 30	//All months with 30 days
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
         && t->minute >= 0 && t->minute < 60	//Checks time is valid
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
  if( msg->deleted ) {	//Checks to see is msg->deleted has been set to true
    printf("[deleted]\n");
  }
  else {
    printf("%s: ", msg->name );
    while( isspace( text[i] )) {
      i++;
    }
    while( j < 40 && text[i+j] != '\0'	//Else prints out name and text of message
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
    printf("Message %d", msg->messageNum );	//Checks to see if msg->deleted has been set to true
    if( msg->deleted ) {
      printf(" has been deleted.\n");
    }
    else {
      printf("\nDate: ");
      printDate( msg->date );
      printf("\nTime: ");	//Else prints out date, time, name and test of message
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
    head = head->next;	//Frees space malloc'd for nodes in linked list
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
		while( node->next != NULL ) {	//finds last node in linked list
			node = node->next;
		}
		node->next = new_node;	//Sets next node to be the new node
	}
	return( head );
}

void actualPrintBrief( MsgNode* list)
{
	int i = 1;
	MsgNode *node = list;
	MsgNode* array[globalMessageNum+1];
	while (node != NULL) {
		array[node->messageNum] = node;
		node = node->next;
	}
	while (i < globalMessageNum+1) {
		if (array[i]->messageNum == currentMessageNum){
			printf("->%2d ",array[i]->messageNum);
		}
		else {
			printf("  %2d ", array[i]->messageNum);
		}
		printBrief(array[i]);
		i++;
	}
}

void overallPrintArgument (int printCase, MsgNode *list) {
	if (list != NULL) {
		MsgNode* node = currentNode(list);
		printf("\n");
		if (printCase == 0) {
			printFull(node);
		}
		else if (printCase == 1) {
			actualPrintBrief( list );
		}
		else if (printCase == 2) {
			printThread(list);
		}
		printf("\n");
	}
}


void printSpace(MsgNode *node) 
{
	int i = 0;
	while ( i < node->indent) {
		printf("   ");
		i++;
	}
}

MsgNode *currentNode( MsgNode *head ) {
	MsgNode *node = head;
	while (node->messageNum != currentMessageNum) {
		node = node->next;
	}
	return( node );
}

MsgNode *getReply( MsgNode *list , MsgNode *node)
{
  MsgNode *new_node;
  MsgNode *inreplyto;
  inreplyto = currentNode(list);
  new_node = (MsgNode *)malloc(sizeof(MsgNode));
  if( new_node == NULL ) {
     printf("Error: could not allocate memory.\n");
     exit( 1 );
  }
  globalMessageNum++;
  new_node->found =0;
  new_node->messageNum = globalMessageNum;
  currentMessageNum = globalMessageNum;
  new_node->inReplyTo = inreplyto->messageNum;
  new_node->indent    = inreplyto->indent + 1;
  new_node->deleted   = FALSE;
  new_node->name      = getName();
  getDate( &new_node->date );
  getTime( &new_node->time );
  new_node->text      = getText();
  new_node->next = node->next;
  node->next = new_node;
  //printf("test1");
  return( new_node );
}

void insertNode( MsgNode *list ){
	MsgNode *node = currentNode( list );
	MsgNode *tempPointer = currentNode( list );
	while ((tempPointer->next != NULL) && (tempPointer->next->indent > node->indent)) {
		tempPointer = tempPointer->next;
	}
	node= tempPointer;
	node->next = getReply( list , node );
}

void printThread( MsgNode *list ){
	MsgNode *node = list;
	while(node != NULL){
		if (node->messageNum == currentMessageNum) {
			printf("->");
		}
		else {
			printf("  ");
		}
		printf("%2d ",node->messageNum);
		printSpace(node);
		printBrief(node);
		node = node->next;
	}
}
		
void search(MsgNode * list) {
	MsgNode* node;
	node = list;
	char search[256] = {0};
	getSearch(search);
	while (node != NULL) {
		MsgNode* new_node; 
		new_node = getSearchNode(node);
		char *text = NULL;
		text = node->text;
		int i,j,k,l;
		j=0;
		int text_str = strlen(text);
		int search_str = strlen(search);
		while (j < text_str - search_str +1) {
			char string[256] = {0};
			for (k = 0; k < search_str; k++) {
				l = j;
				string[k] = text[l+k];
			}
			char* temp = strcasestr(string, search);
			i = 0;
			int string_str = strlen(string);
			if (temp != NULL) {
				while( i< string_str) {
					new_node->text[j] = toupper(string[i]);
					new_node->found = 1;
					i++;
					j++;
				}
			}
			else {
				if ( j == text_str - search_str) {
					while( i< string_str) {
						new_node->text[j] = string[i];
						i++;
						j++;
					}
				}
				else {
				new_node->text[j] = string[i];
				}
				j++;
			}
		}
		if (new_node->found == 1) {
			printf("\n");
			printFull(new_node);
			printf("\n");
			//free(new_node->text);
			free(new_node);
		}
		node = node->next;
	}
	//free(node);
	
}
	
MsgNode * getSearchNode( MsgNode *node )
{
	MsgNode * new_node;
	new_node = (MsgNode *)malloc(sizeof(MsgNode));
	if( new_node == NULL ) {
		printf("Error: could not allocate memory.\n");
		exit( 1 );
	}
	new_node->found 	  = 0;
	new_node->messageNum= node->messageNum;
	new_node->inReplyTo = 0;
	new_node->indent    = 0;
	new_node->deleted   = FALSE;
	new_node->name      = node->name;
	new_node->date	    = node->date;
	new_node->time      = node->time;
	new_node->text 	    = (char *)malloc(sizeof(node->text));
	new_node->next      = NULL;
	
	return( new_node );
}	
		
char* getSearch(char searchText[256]) {
	printf("Search text: ");
	fflush(stdout);
	fgets(searchText, 256, stdin);
	int str_len = strlen(searchText);
	if ((str_len > 0) && (searchText[str_len -1] == '\n')) {
		searchText[str_len -1] = '\0';
	}

	return (searchText);
}	
