// COL1     COL2    COL3    COL4
//  [1]     [2]     [3]     [A]
//  [4]     [5]     [6]     [B]
//  [7]     [8]     [9]     [C]
//  [*]     [0]     [#]   	[D]
//PortD defines for Outputs (columns)
#define GPIO_PORTD_DATA_R       (*((volatile unsigned long *)0x400073FC))
#define GPIO_PORTD_DIR_R        (*((volatile unsigned long *)0x40007400))
#define GPIO_PORTD_AFSEL_R      (*((volatile unsigned long *)0x40007420))
#define GPIO_PORTD_DEN_R        (*((volatile unsigned long *)0x4000751C))
#define GPIO_PORTD_LOCK_R       (*((volatile unsigned long *)0x40007520))
#define GPIO_PORTD_CR_R         (*((volatile unsigned long *)0x40007524))
#define GPIO_PORTD_AMSEL_R      (*((volatile unsigned long *)0x40007528))
#define GPIO_PORTD_PCTL_R       (*((volatile unsigned long *)0x4000752C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
//PortE defines for Inputs (rows)
#define GPIO_PORTE_DATA_R       (*((volatile unsigned long *)0x400243FC))
#define GPIO_PORTE_DIR_R        (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_PDR_R        (*((volatile unsigned long *)0x40024514)) //Pull-Down resistor
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_LOCK_R       (*((volatile unsigned long *)0x40024520))
#define GPIO_PORTE_CR_R         (*((volatile unsigned long *)0x40024524))
#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *)0x4002452C))
//Define all the PINS in PortD individually
#define PORTD0      (*((volatile unsigned long *)0x40007004))
#define PORTD1      (*((volatile unsigned long *)0x40007008))
#define PORTD2      (*((volatile unsigned long *)0x40007010))
#define PORTD3      (*((volatile unsigned long *)0x40007020))
//Define all the PINS in PortE individually
#define PORTE0      (*((volatile unsigned long *)0x40024004))
#define PORTE1      (*((volatile unsigned long *)0x40024008))
#define PORTE2      (*((volatile unsigned long *)0x40024010))
#define PORTE3      (*((volatile unsigned long *)0x40024020))
void keypadInit(void){
	unsigned long delay;
	//Init for port D (output,columns) 0-3
	SYSCTL_RCGC2_R |= 0x00000008;     // 1) D clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTD_LOCK_R = 0x4C4F434B;   // 2) unlock PortD 
  GPIO_PORTD_CR_R |= 0x0F;           // allow changes to PD3-0       
  GPIO_PORTD_AMSEL_R &= 0x00;        // 3) disable analog function
  GPIO_PORTD_PCTL_R &= 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTD_DIR_R |= 0x0F;          // 5.2) PD3-0 Outputs
  GPIO_PORTD_AFSEL_R &= 0x00;        // 6) no alternate function   
  GPIO_PORTD_DEN_R |= 0x0F;          // 7) enable digital pins PD3-0   
	//Init for port E (input,rows) 0-3
  SYSCTL_RCGC2_R |= 0x00000010;     // 1) E clock; 
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTE_LOCK_R = 0x4C4F434B;   // 2) unlock PortE
  GPIO_PORTE_CR_R |= 0x0F;           // allow changes to PE3-0       
  GPIO_PORTE_AMSEL_R &= 0x00;        // 3) disable analog function
  GPIO_PORTE_PCTL_R &= 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTE_DIR_R &= ~0x0F;          // 5.1) PE3-0 Inputs	
  GPIO_PORTE_AFSEL_R &= 0x00;        // 6) no alternate function
  GPIO_PORTE_PDR_R |= 0x0F;          // enable pulldown resistors on PFE3-0     
  GPIO_PORTE_DEN_R |= 0x0F;          // 7) enable digital pins PE3-0     
}  
char readKeypad (){
	 PORTD0=0x00;PORTD1=0x02;PORTD2=0x00;PORTD3=0x00; //Second coulumn on , check state of rows for this columns
	 if(PORTE0==0x01){return '2';} //Read 1st row
	 else if(PORTE1==0x02){return '5';} //Read 2nd row
	 else if(PORTE2==0x04){return '8';} //Read 3rd row
	 else if (PORTE3==0x08){return '0';} //Read 4th row
	 PORTD0=0x01;PORTD1=0x00;PORTD2=0x00;PORTD3=0x00; // 1st column on, check rows
	 if(PORTE0==0x01){return '1';} if(PORTE1==0x02){return '4';}
   else if(PORTE2==0x04){return '7';} else if (PORTE3==0x08){return '*';}
	 PORTD0=0x00;PORTD1=0x00;PORTD2=0x04;PORTD3=0x00; // 3rd column on, check rows
	 if(PORTE0==0x01){return '3';}	if(PORTE1==0x02){return '6';}
	 else if(PORTE2==0x04){return '9';} else if(PORTE3==0x08) {return 'o';} //=
   PORTD0=0x00;PORTD1=0x00;PORTD2=0x00;PORTD3=0x08; // 4th columns on, check rows
	 if(PORTE0==0x01){return '+';} else if(PORTE1==0x02){return '-';}
	 else if(PORTE2==0x04){return '/';} else if(PORTE3==0x08){return '.';}
	 else{return 'H';}	// return a default value if there is nothing pressed		 
}