// 0.Documentation Section 
// Lab7_SOS, main.c

// Runs on LM4F120 or TM4C123 LaunchPad
//TODO: describe program
// Authors: Mateo Randulfe, Konstantin Dragostinov
// Date: November/December 2017

//1. Pre-processor Directives
//#include "TExaS.h"
#include <stdio.h>   // standard C library 
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "uart.h"    // functions to implment input/output
#include "TExaS.h" 

// 2. Declarations Section
//keypad functions
#define PASSWORD 2958
#define PI 3.14
#define E 2.718 //Euler's number


//3. Variable declarations
float firsthirdNumbermber=0,secondNumber=0,thirdNumber=0,fourthNumber=0,fifthNumber=0,sixthNumber=0,answer=0; //variables for six operands and the answer.

int passwordCh=0; //if 1 the password has been guessed if 0 it has not (1 or 0)
int counter=0; //counter for knowing the position of each operator in the final operation (+,-,*/)
int cursoraddress=0; //stores the current cursor address
int flagmult=0, shiftflag=0, operation=0; //flag declarations for state whether multiplication, , . (shifting), or an operation (if shifting and mult flag are set) have been performed
int pos[20]; // string that stores the operators for each position in the operation (each point of the string) in values from 1 (sum) to 4 (division)
int a=0,x=0,y=0,q=0,w=0; //variables that help to nest operations
int pw=0; //password variable used for comparison of the input password with PASSWORD 

char passQuery[20], passEntry[20], pwchar[20]; //char arrays used for the password
char string, pad, padNew, input; //char arrays for manipulation of keypad inputs
char buffer[20]; //used for printing the final answerwer.
char numone[20], numtwo[20],numthree[20],numfour[20], numfive[20],numsix[20]; //Strings for storing the numbers for each operand
char numonefinal[20], numtwofinal[20],numthreefinal[20],numfourfinal[20], numfivefinal[20],numsixfinal[20]; //Final string that stores the operand

//4. Function prototypes
void lcdcommandInit(void); //Command aiding lcdInit(), which takes care of some LCD settings (display lines, character font, cursor)
void lcdInit(void); //Initialises the LCD
void PortInit(void); //Initialise Port A & Port B, used by the LCD
void lcdWriteCommand(unsigned char c);
void lcdClearScreen(void); //Wipes the LCD of all characters
void lcdGoto(unsigned char address);
void lcdWriteData (char c); //Writes a single character to the LCD
void lcdWriteRamString(char *string); //Writes a string to the LCD

void Init(void); //System initialisation function containing other (LCD & Keypad) initialisation functions

void keypadInit(void); //Initialise Port D & Port E, used by the keypad
char readKeypad(); //return input from the keypad

float performCalculation(char input); //main calculations function
char returnNumber(char a, char b, char c); //return the number from the string coming from readKeypad()
int password(char input); //main password function

void SysTick_Wait(unsigned long delay); void SysTick_Init(void); void PLL_Init(void); //Prototypes for functions taking care of time management
void longDelay(); //A few seconds delay


int main(void){
	
	Init(); //Initialise LCD & Keypad
	
	lcdWriteRamString("ENTER PASSWORD"); //Show prompt "Enter password"
	longDelay(); //Wait for some time after the prompt
	lcdClearScreen(); //Clear screen
	
while(passwordCh==0){ //While the correct password hasn't been entered
	
		pad=readKeypad(); //get first input
		SysTick_Wait(500000000); //delay of so when you hold a key, it doesn't continously write to the LCD
		padNew=readKeypad(); //get second input
	
		 if (readKeypad()!='H' && (pad!=padNew)) { //if the last two inputs are different to each other and different to 'H' (default return value of readKeypad() when nothing is pressed		 
			 if(readKeypad()!='o'){ //aif input different to equals
				 
					password(readKeypad()); //returns password inputs as int
					lcdWriteData('*');//print the keypad input to the LCD as *		 
    }
			 else{ //If equals is pressed
				 
			input='='; //...then the input must be '='
			sprintf(pwchar,"%d",password(readKeypad())); //convert the int to char 
			lcdClearScreen(); //Wipe the screen of all numbers
				 
			if(pw!=PASSWORD){ //If the input password doesn't match the actual one
			lcdWriteRamString("WRONG PASSWORD"); longDelay();lcdClearScreen();pw=0; //Prints message, delays, cleanswer screen, resets pw and sends the user back to try a new password
 }
	  }
		 }
		 if(pw==PASSWORD){ //If the input password matches the defined password
			 passwordCh=1;break; //then set the variable checking whether the passwords match to 1 and leave the "while" loop
		 }  
}

while(passwordCh==1){ //If we have broken out of the previous while loop
	
		pad=readKeypad(); //first input
		SysTick_Wait(500000000); //delay of so when you hold a key, it doesn't continously write to the LCD
		padNew=readKeypad(); //second input
	
		 if (readKeypad()!='H' && (pad!=padNew)) { //if the last two inputs are different to each other and a button pressed
			 
			 cursoraddress++; //increment the address of the cursor
			 if(cursoraddress>16){ //if we have exceed the first row of the LCD (which is 16x2, 16 blocks on each row)...
			 lcdGoto(cursoraddress+151); //...move the cursor to the first position of the second row
			 }
			 
			 if(readKeypad()!='o'){ //if the keypadinput is different to 'o' (equals sign)	
				 
					lcdWriteData(readKeypad());//print the keypad input to the LCD
				 performCalculation(readKeypad()); //perform the calculation made from the keypad inputs
				 
				if(operation==1){ //if an operation (such as * or +) has been done, shift mode
					sprintf(buffer,"%0.2f",performCalculation(readKeypad())); //convert the float from performCalculation with 2 decimal places of precision to a string
					lcdClearScreen();	//clear the screen			
				lcdWriteRamString(buffer); //print the converted number
				} 
				
			operation=0; //reset the operation flag
		} 
			 
		else{ //if we press * and =	or just =
			input='='; //otherwise set input to equals sign	
			sprintf(buffer,"%0.2f",performCalculation(readKeypad()));
			cursoraddress=strlen(buffer); //look for the new position of the cursor after one calculation is performed
			lcdClearScreen();	
			if(answer!=0){
				lcdWriteRamString(buffer); //print computation answerwer to LCD
	  }
			else{lcdClearScreen(); } 
	 }
	}
 }
}



char returnNumber(char a, char b, char c){ //takes a reading from the keypad in a, concatenates the reading to b and copies it to c
    if(a == '0') {strcat(&b,"0");strcpy(&c,&b);}
    if(a == '1') {strcat(&b,"1");strcpy(&c,&b);}
    if(a == '2') {strcat(&b,"2");strcpy(&c,&b);}
    if(a == '3') {strcat(&b,"3");strcpy(&c,&b);}
    if(a == '4') {strcat(&b,"4");strcpy(&c,&b);}
    if(a == '5') {strcat(&b,"5");strcpy(&c,&b);}
    if(a == '6') {strcat(&b,"6");strcpy(&c,&b);}
    if(a == '7') {strcat(&b,"7");strcpy(&c,&b);}
    if(a == '8') {strcat(&b,"8");strcpy(&c,&b);}
    if(a == '9') {strcat(&b,"9");strcpy(&c,&b);}
		if(a == '.') {strcat(&b,".");strcpy(&c,&b);}
		return b;
}



float performCalculation(char input) {	
	if (counter==0 ){                                             //if none operator has been writen means that we are storing first operand
		char w = returnNumber(input, *numone,*numonefinal);
		strcat(numonefinal,&w);
	}
	 if (counter==1){ //store the returnNumber output in w
		char w = returnNumber(input, *numtwo,*numtwofinal); //when returnNumber gives a second number, like 1, it is concatenated  to numtwofinal
		strcat(numtwofinal,&w);
	}
	 if (counter==2){                                           //each counter means that one operator has been set therefore each one represents one number to be stored
		char w = returnNumber(input, *numthree,*numthreefinal);  //the process is then repeated for up to 6 numbers
		strcat(numthreefinal,&w);
	}
	 if (counter==3){
		char w = returnNumber(input, *numfour,*numfourfinal);
		strcat(numfourfinal,&w);
	}
	 if (counter==4){
		char w = returnNumber(input, *numfive,*numfivefinal);
		strcat(numfivefinal,&w);
	}	 
	 if (counter==5){
		char w = returnNumber(input, *numsix,*numsixfinal);
		strcat(numsixfinal,&w);
	}
	 
    if (input == '+') {pos[counter]=1;counter++;} //+; every time you give an operator, the counter is incremented and goes...
    if (input == '-') {pos[counter]=2;counter++;} //-;  the pos array stores a value for each operator position that tells which operator it was
    if (input == '*') {pos[counter]=3;counter++;flagmult=1;} //fires the flag
    if (input == '/') {pos[counter]=4;counter++;} 
		if (input=='.')   {shiftflag=1;} //fires the second flag
		
    if (flagmult==1) { //The flags raised by multiplication and dot keys on the keypad... 				
			if(shiftflag==1){ //...will serve to call alternative functions, because the keypad can only accomodate 6 funcitons & 10 numbers
				
				
			if(input=='3'){  //If you have pressed *, then ., then 3, perform square root operation
				
				operation=1;flagmult=0;shiftflag=0; //set the operation flags & reset the * and . one
				firsthirdNumbermber=atof(numonefinal); //convert the string to float
				
				memset(numonefinal,0,sizeof(numonefinal));memset(numone,0,sizeof(numone)); //Empties the arrays by writing zeroes to each of their indices
				
				answer=sqrt(firsthirdNumbermber);
				firsthirdNumbermber=answer;sprintf(numonefinal,"%0.2f",answer); //converts number into string
			return answer;	//output alternative function calculation result
				
			}				
				if(input=='4'){ //If you have pressed *, then ., then 4, perform cosine operation
					
				operation=1;flagmult=0;shiftflag=0;
				firsthirdNumbermber=atof(numonefinal);
					
				memset(numonefinal,0,sizeof(numonefinal));memset(numone,0,sizeof(numone));
					
				answer=cos(firsthirdNumbermber);
				firsthirdNumbermber=answer;sprintf(numonefinal,"%0.2f",answer);
			return answer;	
			}
				if(input=='5'){ //If you have pressed *, then ., then 5, evaluate the sin of the input
					
				operation=1;flagmult=0;shiftflag=0;
				firsthirdNumbermber=atof(numonefinal);
					
				memset(numonefinal,0,sizeof(numonefinal));memset(numone,0,sizeof(numone));
					
				answer=sin(firsthirdNumbermber);
				firsthirdNumbermber=answer;sprintf(numonefinal,"%0.2f",answer);
			return answer;	
			}			
				if(input=='6'){ //If you have pressed *, then ., then 6, evaluate the tangent of the input
					
				operation=1;flagmult=0;shiftflag=0;
				firsthirdNumbermber=atof(numonefinal);
					
				memset(numonefinal,0,sizeof(numonefinal));memset(numone,0,sizeof(numone));
					
				answer=tan(firsthirdNumbermber);
				firsthirdNumbermber=answer;sprintf(numonefinal,"%0.2f",answer);
			return answer;	
			}		
				if(input=='7'){ //If you have pressed *, then ., then 7, evaluate the inverse cosine of the input
					
				operation=1;flagmult=0;shiftflag=0;
				firsthirdNumbermber=atof(numonefinal);
					
				memset(numonefinal,0,sizeof(numonefinal));memset(numone,0,sizeof(numone));
					
				answer=acos(firsthirdNumbermber);
				firsthirdNumbermber=answer;sprintf(numonefinal,"%0.2f",answer);
			return answer;	
			}
				if(input=='8'){ //If you have pressed *, then ., then 8, evaluate the inverse sin of the input
					
				operation=1;flagmult=0;shiftflag=0;
				firsthirdNumbermber=atof(numonefinal);
					
				memset(numonefinal,0,sizeof(numonefinal));memset(numone,0,sizeof(numone));
					
				answer=asin(firsthirdNumbermber);
				firsthirdNumbermber=answer;sprintf(numonefinal,"%0.2f",answer);
			return answer;	
			}			
				if(input=='9'){ //If you have pressed *, then ., then 9, evaluate the inverse tangent of the input
					
				operation=1;flagmult=0;shiftflag=0;
				firsthirdNumbermber=atof(numonefinal);
					
				memset(numonefinal,0,sizeof(numonefinal));memset(numone,0,sizeof(numone));
					
				answer=atan(firsthirdNumbermber);
				firsthirdNumbermber=answer;sprintf(numonefinal,"%0.2f",answer);
			return answer;	
			}		
				if(input=='-'){ //If you have pressed *, then ., then -, calculate the logarithm with base 10 of the input
					
				operation=1;flagmult=0;shiftflag=0;
				firsthirdNumbermber=atof(numonefinal);
					
				memset(numonefinal,0,sizeof(numonefinal));memset(numone,0,sizeof(numone));
					
				answer=log10(firsthirdNumbermber);
				firsthirdNumbermber=answer;sprintf(numonefinal,"%0.2f",answer);
			return answer;	
			}			
				if(input=='/'){ //If you have pressed *, then ., then /, calculate the natural logarithm with base 10 of the input
					
				operation=1;flagmult=0;shiftflag=0;
				firsthirdNumbermber=atof(numonefinal);
					
				memset(numonefinal,0,sizeof(numonefinal));memset(numone,0,sizeof(numone));
					
				answer=log(firsthirdNumbermber);
				firsthirdNumbermber=answer;sprintf(numonefinal,"%0.2f",answer);
			return answer;	
			}					
				if(input=='0'){ //If you have pressed *, then ., then 0, return pi
					
				operation=1;flagmult=0;shiftflag=0;
				answer=PI;
				firsthirdNumbermber=answer;sprintf(numonefinal,"%0.2f",answer);
			return answer;	
			}
		}
		if(input=='='){ //clear screen & reset the values in the arrays of the intermediary and final values , reset and clear everything
			
				memset(numonefinal,0,sizeof(numonefinal));memset(numtwofinal,0,sizeof(numtwofinal));
				memset(numthreefinal,0,sizeof(numthreefinal));memset(numfourfinal,0,sizeof(numfourfinal));
				memset(numfivefinal,0,sizeof(numfivefinal));memset(numsixfinal,0,sizeof(numsixfinal));
				memset(numone,0,sizeof(numone));memset(numtwo,0,sizeof(numtwo));memset(numthree,0,sizeof(numthree));
				memset(numfour,0,sizeof(numfour));memset(numfive,0,sizeof(numfive));memset(numsix,0,sizeof(numsix));					
			
				secondNumber=0;thirdNumber=0;fourthNumber=0;fifthNumber=0;sixthNumber=0;counter=0;pos[0]=0;pos[1]=0;;pos[2]=0;pos[3]=0;pos[4]=0;answer=0;			
		}
}
		


    if ((input == 'o' | input=='=')) { //If '=' is pressed
			if(pos[0]!=0 && pos[1]==0){  //if there is only one operator
				
				firsthirdNumbermber=atof(numonefinal); secondNumber=atof(numtwofinal); //convert addequate numbers
				
				memset(numonefinal,0,sizeof(numonefinal));memset(numtwofinal,0,sizeof(numtwofinal)); //clear the strings for the next operations
				memset(numone,0,sizeof(numone));memset(numtwo,0,sizeof(numtwo));
				//depending on the operator value perform one or another type of calculation
				if(pos[0]==1){answer=firsthirdNumbermber+secondNumber;} if(pos[0]==2){answer=firsthirdNumbermber-secondNumber;}	if(pos[0]==3){answer=firsthirdNumbermber*secondNumber;} if(pos[0]==4){answer=firsthirdNumbermber/secondNumber;}				
				//set the first number string as the result so Ans function is allowed
					sprintf(numonefinal,"%0.2f",answer);
					//reset paramenters for next calculation
				secondNumber=0;firsthirdNumbermber=answer;counter=0;pos[0]=0;pos[1]=0;		
			}	
			
			if(pos[1]!=0 && pos[2]==0){ //same for the rest
				
				firsthirdNumbermber=atof(numonefinal); secondNumber=atof(numtwofinal);thirdNumber=atof(numthreefinal);
				
				memset(numonefinal,0,sizeof(numonefinal));memset(numtwofinal,0,sizeof(numtwofinal));memset(numthreefinal,0,sizeof(numthreefinal));
				memset(numone,0,sizeof(numone));memset(numtwo,0,sizeof(numtwo));memset(numthree,0,sizeof(numthree));			
				
		 if(pos[0]==1){y=firsthirdNumbermber+secondNumber;} if(pos[0]==2){y=firsthirdNumbermber-secondNumber;}	if(pos[0]==3){y=firsthirdNumbermber*secondNumber;} if(pos[0]==4){y=firsthirdNumbermber/secondNumber;}	
		 if(pos[1]==1){answer=y+thirdNumber;} if(pos[1]==2){answer=y-thirdNumber;}	if(pos[1]==3){answer=y*thirdNumber;} if(pos[1]==4){answer=y/thirdNumber;}
				
			 sprintf(numonefinal,"%0.2f",answer);
				
			 secondNumber=0;thirdNumber=0;firsthirdNumbermber=answer;counter=0;pos[0]=0;pos[1]=0;	
			}
			if(pos[2]!=0 && pos[3]==0){
				
				firsthirdNumbermber=atof(numonefinal); secondNumber=atof(numtwofinal);thirdNumber=atof(numthreefinal);fourthNumber=atof(numfourfinal);
				
				memset(numonefinal,0,sizeof(numonefinal));memset(numtwofinal,0,sizeof(numtwofinal));memset(numthreefinal,0,sizeof(numthreefinal));memset(numfourfinal,0,sizeof(numfourfinal));
				
				memset(numone,0,sizeof(numone));memset(numtwo,0,sizeof(numtwo));memset(numthree,0,sizeof(numthree));memset(numfour,0,sizeof(numfour));					
		 if(pos[0]==1){y=firsthirdNumbermber+secondNumber;} if(pos[0]==2){y=firsthirdNumbermber-secondNumber;}	if(pos[0]==3){y=firsthirdNumbermber*secondNumber;} if(pos[0]==4){y=firsthirdNumbermber/secondNumber;}	
		 if(pos[1]==1){x=y+thirdNumber;} if(pos[1]==2){x=y-thirdNumber;}	if(pos[1]==3){x=y*thirdNumber;} if(pos[1]==4){x=y/thirdNumber;}
			 if(pos[2]==1){answer=x+fourthNumber;} if(pos[2]==2){answer=x-fourthNumber;}	if(pos[2]==3){answer=x*fourthNumber;} if(pos[2]==4){answer=x/fourthNumber;}
				
				 sprintf(numonefinal,"%0.2f",answer);
				
				 secondNumber=0;thirdNumber=0;fourthNumber=0;firsthirdNumbermber=answer;counter=0;pos[0]=0;pos[1]=0;pos[2]=0;	
			}			
			if(pos[3]!=0 && pos[4]==0){
				
				firsthirdNumbermber=atof(numonefinal); secondNumber=atof(numtwofinal);thirdNumber=atof(numthreefinal);fourthNumber=atof(numfourfinal);fifthNumber=atof(numfivefinal);
				
				memset(numonefinal,0,sizeof(numonefinal));memset(numtwofinal,0,sizeof(numtwofinal));
				memset(numthreefinal,0,sizeof(numthreefinal));memset(numfourfinal,0,sizeof(numfourfinal));memset(numfivefinal,0,sizeof(numfivefinal));
				memset(numone,0,sizeof(numone));memset(numtwo,0,sizeof(numtwo));memset(numthree,0,sizeof(numthree));
				
				memset(numfour,0,sizeof(numfour));memset(numfive,0,sizeof(numfive));					
		 if(pos[0]==1){y=firsthirdNumbermber+secondNumber;} if(pos[0]==2){y=firsthirdNumbermber-secondNumber;}	if(pos[0]==3){y=firsthirdNumbermber*secondNumber;} if(pos[0]==4){y=firsthirdNumbermber/secondNumber;}	
		 if(pos[1]==1){x=y+thirdNumber;} if(pos[1]==2){x=y-thirdNumber;}	if(pos[1]==3){x=y*thirdNumber;} if(pos[1]==4){x=y/thirdNumber;}
		 if(pos[2]==1){q=x+fourthNumber;} if(pos[2]==2){q=x-fourthNumber;}	if(pos[2]==3){q=x*fourthNumber;} if(pos[2]==4){q=x/fourthNumber;}
		 if(pos[3]==1){answer=q+fifthNumber;} if(pos[3]==2){answer=q-fifthNumber;}	if(pos[3]==3){answer=q*fifthNumber;} if(pos[3]==4){answer=q/fifthNumber;}	 
				
			 sprintf(numonefinal,"%0.2f",answer);
				
			 secondNumber=0;thirdNumber=0;fifthNumber=0;fourthNumber=0;firsthirdNumbermber=answer;counter=0;pos[0]=0;pos[1]=0;pos[2]=0;pos[3]=0;
			}	
			if(pos[4]!=0){
				
				firsthirdNumbermber=atof(numonefinal); secondNumber=atof(numtwofinal);thirdNumber=atof(numthreefinal);fourthNumber=atof(numfourfinal);fifthNumber=atof(numfivefinal);
				sixthNumber=atof(numsixfinal);
				
				memset(numonefinal,0,sizeof(numonefinal));memset(numtwofinal,0,sizeof(numtwofinal));
				memset(numthreefinal,0,sizeof(numthreefinal));memset(numfourfinal,0,sizeof(numfourfinal));
				memset(numfivefinal,0,sizeof(numfivefinal));memset(numsixfinal,0,sizeof(numsixfinal));
				memset(numone,0,sizeof(numone));memset(numtwo,0,sizeof(numtwo));memset(numthree,0,sizeof(numthree));
				memset(numfour,0,sizeof(numfour));memset(numfive,0,sizeof(numfive));memset(numsix,0,sizeof(numsix));								
		 
		 if(pos[0]==1){y=firsthirdNumbermber+secondNumber;} if(pos[0]==2){y=firsthirdNumbermber-secondNumber;}	if(pos[0]==3){y=firsthirdNumbermber*secondNumber;} if(pos[0]==4){y=firsthirdNumbermber/secondNumber;}	
		 if(pos[1]==1){x=y+thirdNumber;} if(pos[1]==2){x=y-thirdNumber;}	if(pos[1]==3){x=y*thirdNumber;} if(pos[1]==4){x=y/thirdNumber;}
		 if(pos[2]==1){q=x+fourthNumber;} if(pos[2]==2){q=x-fourthNumber;}	if(pos[2]==3){q=x*fourthNumber;} if(pos[2]==4){q=x/fourthNumber;}
		 if(pos[3]==1){w=q+fifthNumber;} if(pos[3]==2){w=q-fifthNumber;}	if(pos[3]==3){w=q*fifthNumber;} if(pos[3]==4){w=q/fifthNumber;}
		 if(pos[4]==1){answer=w+sixthNumber;} if(pos[4]==2){answer=w-sixthNumber;}	if(pos[4]==3){answer=w*sixthNumber;} if(pos[4]==4){answer=w/sixthNumber;}			 
			
			 sprintf(numonefinal,"%0.2f",answer);
				
			 secondNumber=0;thirdNumber=0;fifthNumber=0;sixthNumber=0;fourthNumber=0;firsthirdNumbermber=answer;counter=0;pos[0]=0;pos[1]=0;pos[2]=0;pos[3]=0;
			}				
			
			return answer;
		}
       return answer; answer=0; //Return & Reset answerwer
}		



int password(char input) { //concatenates the inputs of readKeypad() into a string, which is converted to int before it's returned
	
    if(input == '0') {strcat(passEntry,"0");strcpy(passQuery,passEntry);} //strcpy (char *dest, const char *src)
    if(input == '1') {strcat(passEntry,"1");strcpy(passQuery,passEntry);} //a '1' is concatenated to passEntry, passEntry is then copied to passQuery
    if(input == '2') {strcat(passEntry,"2");strcpy(passQuery,passEntry);} //For example, we put a '2' in passEntry, then passQuery = passEntry
    if(input == '3') {strcat(passEntry,"3");strcpy(passQuery,passEntry);} //Then we concatenate a '3' to passEntry, which was '2', now it's '23'. passQuery = passEntry = 23
    if(input == '4') {strcat(passEntry,"4");strcpy(passQuery,passEntry);} //Concatenate '4' to passEntry, such that passEntry = '234', then passQuery=passEntry
    if(input == '5') {strcat(passEntry,"5");strcpy(passQuery,passEntry);}
    if(input == '6') {strcat(passEntry,"6");strcpy(passQuery,passEntry);}
    if(input == '7') {strcat(passEntry,"7");strcpy(passQuery,passEntry);}
    if(input == '8') {strcat(passEntry,"8");strcpy(passQuery,passEntry);}
    if(input == '9') {strcat(passEntry,"9");strcpy(passQuery,passEntry);}
		if(input == '.') {strcat(passEntry,".");strcpy(passQuery,passEntry);}
		
if (input == 'o' | input=='=') {
		pw=atoi(passQuery);
	  memset(passQuery,0,sizeof(passQuery));memset(passEntry,0,sizeof(passEntry)); //flush the whole arrays, which when a '=' is pressed should be equal...
}
	return pw;
}
void longDelay() { //A set of SysTick delays; Sorry for the ugly function
	SysTick_Wait(999999999); SysTick_Wait(999999999); SysTick_Wait(999999999); SysTick_Wait(999999999); 
}
void Init(void){ //Bring together all initialisations, for the Ports, the keypad and the LCD
	 PortInit();
	 lcdInit(); lcdcommandInit();
	 keypadInit();
}