/* LCD File
LCD able to write up to two lines of eight characteres
2.7-5.5V
80 characters maximum on the RAM
*/
#define lcdRS (*((volatile unsigned long *)0x40004020)) //RS - PA3
#define lcdEN (*((volatile unsigned long *)0x40004010)) //EN - PA2
#define lcdDB (*((volatile unsigned long *)0x400050F0)) //DB4-7 are mapped to PB2-5
//Defines for PortA
#define GPIO_PORTA_DIR_R        (*((volatile unsigned long *)0x40004400))
#define GPIO_PORTA_AFSEL_R      (*((volatile unsigned long *)0x40004420))
#define GPIO_PORTA_DEN_R        (*((volatile unsigned long *)0x4000451C))
#define GPIO_PORTA_LOCK_R       (*((volatile unsigned long *)0x40004520))
#define GPIO_PORTA_CR_R         (*((volatile unsigned long *)0x40004524))
#define GPIO_PORTA_AMSEL_R      (*((volatile unsigned long *)0x40004528))
#define GPIO_PORTA_PCTL_R       (*((volatile unsigned long *)0x4000452C))
//Defines for PortB
#define GPIO_PORTB_DATA_R       (*((volatile unsigned long *)0x400053FC))
#define GPIO_PORTB_DIR_R        (*((volatile unsigned long *)0x40005400))
#define GPIO_PORTB_AFSEL_R      (*((volatile unsigned long *)0x40005420))
#define GPIO_PORTB_DEN_R        (*((volatile unsigned long *)0x4000551C))
#define GPIO_PORTB_LOCK_R      (*((volatile unsigned long *)0x40005520))
#define GPIO_PORTB_CR_R         (*((volatile unsigned long *)0x40005524))
#define GPIO_PORTB_AMSEL_R      (*((volatile unsigned long *)0x40005528))
#define GPIO_PORTB_PCTL_R       (*((volatile unsigned long *)0x4000552C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
//Define for delays
#define delay4500ns 360 
#define delay400us 3200
#define delay50ms 400000
#define delay150ms 1200000
#define delay2000us 16000
#define delay500ms 4000000

void SysTick_Wait(unsigned long delay);
void SysTick_Init(void);
void PLL_Init(void); //Prototypes for functions taking care of time management (such as delays)


void PortInit(void){
	unsigned long delay;
	//Init for PortB
	SYSCTL_RCGC2_R |= 0x00000002;     // 1) b clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTB_CR_R |= 0x3C;           // allow changes to PB2-PB5       
  GPIO_PORTB_AMSEL_R &= 0x00;        // 3) disable analog function
  GPIO_PORTB_PCTL_R &= 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTB_DIR_R |= 0x3C;          // 5.2) PB2-PB5 as OUTPUTS
  GPIO_PORTB_AFSEL_R &= 0x00;        // 6) no alternate function   
  GPIO_PORTB_DEN_R |= 0x3C;          // 7) enable digital pins to PB2-PB5   
	
	//Init for PortA2-3
  SYSCTL_RCGC2_R |= 0x00000001;     // 1) A clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTA_CR_R |= 0x0C;           // allow changes to PA2-3       
  GPIO_PORTA_AMSEL_R &= 0x00;        // 3) disable analog function
  GPIO_PORTA_PCTL_R &= 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTA_DIR_R |= 0x0C;          // 5.1) PA2-3 OUTPUTS
  GPIO_PORTA_AFSEL_R &= 0x00;        // 6) no alternate function 
  GPIO_PORTA_DEN_R |= 0x0C;          // 7) enable digital pins PA2-3    
}
void lcdENPulse(unsigned long time){ //set EN high for x time
	lcdEN |= 0x04;
	SysTick_Wait(time);
	lcdEN &=~ 0x04; //return en to 0 
}
void lcdInit(void) {
	PLL_Init(); SysTick_Init(); //init time functions

	lcdEN =0x00<<2; //set enable to 0
	lcdDB =0x00<<2; //set data to 0

	SysTick_Wait(delay50ms); //50 ms wait
	
	lcdRS=0x00<<2; //RS to 0
	
	lcdDB =0x3<<2; //waking up instructions
	lcdENPulse(delay4500ns); //latch enable line for 450 ns
	
	SysTick_Wait(delay50ms); 
	lcdDB =0x3<<2;
	lcdENPulse(delay4500ns);
	
	SysTick_Wait(delay2000us); //designated times to wait from datasheet
	lcdENPulse(delay4500ns);
	
	SysTick_Wait(delay2000us);
	
	lcdDB =0x2<<2;	             
  lcdENPulse(delay4500ns);
	
	SysTick_Wait(delay2000us);	
}


void lcdWriteCommand(unsigned char c){     // function for setting commands

	lcdRS =0x00;                 //line low
	SysTick_Wait(delay400us);
	
	lcdDB=(c&0xF0)>>2;  //send first high nible to PB2-5
	lcdENPulse(delay4500ns);

	SysTick_Wait(delay50ms); //50ms delay
	
	lcdDB=(c&0x0F)<<2;//set low niblle 
	lcdENPulse(delay4500ns);

	SysTick_Wait(delay50ms); //50ms delay
}
void lcdWriteData (char c){   //function for setting data
	lcdRS =0x08;                   //RS on for sending data
	SysTick_Wait(delay400us);
	
	lcdDB=(c&0xF0)>>2; //high nibble	
	lcdENPulse(delay4500ns);

	SysTick_Wait(delay2000us); //50ms delay

	lcdDB=(c&0x0F)<<2; //low nibble	
	lcdENPulse(delay4500ns);

	SysTick_Wait(delay2000us); //50ms delay
	lcdRS =0x00;
}
void lcdGoto(unsigned char address){
	//lcdWriteCommand(0XFE); SysTick_Wait(delay50ms); //lcdWriteCommand(0x00);//prepare LCD to move cursor
	lcdWriteCommand(address); SysTick_Wait(delay50ms);// lcdWriteCommand(0x00);//move cursor right
}
void lcdcommandInit(void){
	//Data display lines are N = 0/1 <-> 1/2 lines, Data length DL = 0/1 <-> 4/8 bits, Character Font F = 0/1 <-> 5x8/5x10
	//We need 4-bit data, 2 lines, 5x8 font (0,1,0)
	//Cursor I/D to increment and display don't shift   
	lcdWriteCommand(0x28); SysTick_Wait(delay50ms); lcdWriteCommand(0x00); //sets lcd to 4 bit data, 2 lines, 5x8 font
	lcdWriteCommand(0x06); SysTick_Wait(delay50ms); lcdWriteCommand(0x00); //entry mode: cursor to increment and display not to shift	
	lcdWriteCommand(0x0F); SysTick_Wait(delay50ms); lcdWriteCommand(0x00); //sets display pn, cursor on, blink on	
	lcdWriteCommand(0x04); SysTick_Wait(delay50ms); lcdWriteCommand(0x00); //entry mode: cursor to increment and display not to shift
}
void lcdClearScreen(void){
	lcdWriteCommand(0x01); SysTick_Wait(delay50ms); lcdWriteCommand(0x00); //clear lcd
	lcdWriteCommand(0x02); SysTick_Wait(delay50ms); lcdWriteCommand(0x00); //"Home" LCD
}
void lcdWriteRamString(char *string) {//function for writing strings
while(*string) {
	lcdWriteData(*string);
	string++;
	}
}            