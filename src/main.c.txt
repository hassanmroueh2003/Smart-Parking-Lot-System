#include <p18cxxx.h>
#include <delays.h>
#include <EEPROM.h>
#include <LCD4lib.h> 

//#pragma config FOSC = INTIO67  // Internal oscillator block
//#pragma config WDTEN = OFF     // Watchdog Timer disabled
//#pragma config LVP = OFF       // Single-Supply ICSP disabled
//#pragma config BOREN = OFF     // Brown-out Reset disabled

//#define _XTAL_FREQ 500000  // Oscillator frequency for delay functions

#define eeNum 0x00

#define BUZZER PORTCbits.RC6
#define Lights PORTCbits.RC7
#define outsensor PORTAbits.RA0
#define Insensor PORTAbits.RA1
#define LightSensor PORTAbits.RA2
#define motor PORTCbits.RC2



enum{OFF,ON};                    // to define 0 and 1 as OFF and ON
unsigned char Cnb = 0; // to define the number of cars in the garage 
unsigned char j;  //loop index 
unsigned int open_close;
unsigned int ang1;
char digits;

void Setup(void);
void GateControl (void);
void LightsControl(void);
void GateSetup(void);
void setPWMduty(unsigned int duty);
void MoveGate(int open_close);

void main(void){
    Setup();
    while(1){
        //LightsControl();
        GateControl();
    }
}
void Setup(void){
    OSCCON = 0b0110000;
    InitLCD();

    
    Lights = OFF;             //Lights OFF, hence the LEDs are OFF 
    BUZZER =OFF;              //BUZZER is OFF
    
    TRISC &= 0x1F;         //RC6, RC7,as Digital output (for the buzzer and lights) 
    ANSELC = 0x00;
    TRISBbits.RB0 =TRISBbits.RB1=TRISBbits.RB2=TRISBbits.RB3= 0;
   
    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;
    TRISAbits.TRISA2 = 1; //RA 0,1,2 as input (for sensors)
    ANSELAbits.ANSA0 = 0;
    ANSELAbits.ANSA1 = 0;
    ANSELAbits.ANSA2 = 0; //RA 0,1,2 digital (For sensors)

    DispRomStr(Ln1Ch0, (ROM *) "Parking Lot    "); // to write in LCD line 1
    DispRomStr(Ln2Ch0, (ROM *) "    empty spots"); // to write in LCD line 2
    GateSetup();

    ReadEE(eeNum, Cnb);
    Bin2Asc(Cnb, digits);
    
    SendCmd(Ln2Ch1);
    SendChar(digits);
    
}


 
void Beep (unsigned int loopcount){    
//Beep function that will turn the BUZZER ON 
    
    unsigned int k; 
    for(k = 0; k < 2*loopcount; k++){  
        /*multiply the taken value by 2 because 1 cycle consist on 2 edges 
        (rise and fall and Duty cycle is 50%)*/ 
       BUZZER = ~BUZZER;               
        /*toggling the BUZZER value as if we have rising edge and falling 
        edge to make a cycle*/ 
       Delay1KTCYx(1);        
       //Giving each edge 1ms  
    } 
    BUZZER = 0;                  
    //clearing the BUZZER when finished 
}

void GateControl (void){
    
    if (Cnb >= 3){
        DispRomStr(Ln1Ch0, (ROM *) "No empty spots  "); // to write in LCD line 1
        DispRomStr(Ln2Ch0, (ROM *) "Sorry           "); // to write in LCD line 2
        Beep(500);
        MoveGate(0);
    }
    else if((!outsensor == 1) && (!Insensor == 0)){
            Beep(500);
            //Gate = ON;(without delay bas stay on) 
            DispRomStr(Ln1Ch0, (ROM *) "Hey,Enter please   "); // to write in LCD line 1
            DispRomStr(Ln2Ch0, (ROM *) "      empty spots  "); // to write in LCD line 2 //zabet el eeprom
            MoveGate(1);
            while(!Insensor == 0){
                MoveGate(1);
                /*
                for(j = 0; j < 500; j++){
                    MoveGate(1);
                    if (Insensor == 1)
                        break;
                }*/
                DispRomStr(Ln1Ch0, (ROM *) "Move please     "); // to write in LCD line 1
                DispRomStr(Ln2Ch0, (ROM *) "park your car   "); // to write in LCD line 2
            }
            Beep(500);
            Cnb++;
            MoveGate(0);
    }else if((!outsensor == 1) && (!Insensor == 1)){//case of cars coming opposite to each other
                DispRomStr(Ln1Ch0, (ROM *) "inside car     "); // to write in LCD line 1
                DispRomStr(Ln2Ch0, (ROM *) "should go out  "); // to write in LCD line 2
                MoveGate(0);
            }
    if ((!Insensor == 1)&& (!outsensor == 0) /*&& (Gate =OFF )*/){
        Beep(500);
        MoveGate(1);
            while(!outsensor == 0){
                MoveGate(1);
                /*
                for(j = 0; j < 500; j++){
                    MoveGate(1);
                    if (outsensor == 1)
                        break;
                }
                 */
                DispRomStr(Ln1Ch0, (ROM *) "Move please     "); // to write in LCD line 1
                DispRomStr(Ln2Ch0, (ROM *) "park your car   "); // to write in LCD line 2
            }
            Beep(500);
            Cnb--;
    }else if (Cnb < 3 && (!outsensor == 0) && (!Insensor == 0)){
         DispRomStr(Ln1Ch0, (ROM *) "Parking Lot    "); // to write in LCD line 1
         DispRomStr(Ln2Ch0, (ROM *) "    empty spots"); // to write in LCD line 2
        Bin2Asc(Cnb, digits);
        SendCmd(Ln2Ch0);
        SendChar(digits);
    }
    Wrt2EE(Cnb, eeNum);

}

void LightsControl(void){
    if (LightSensor =ON){
        Lights = ON;
        Delay10KTCYx(200);
    }
    else {
        Lights = OFF;
        Delay10KTCYx(200);
    }    
}
void GateSetup(void){
    // Set the PWM frequency and timer configurations for PIC18F45K22
    // Assuming Timer2 with Prescale = 16 and Postscale = 1
    PR2 = 156;
    T2CONbits.T2CKPS = 0b11; // Prescaler 1:16
    T2CONbits.TMR2ON = 1;    // Timer2 ON
    T2CONbits.T2OUTPS = 0;

    // CCP2 as PWM
    CCP2CONbits.CCP2M = 0b1100; // PWM mode
    CCPR2L = 0;

    // Setting TRIS bits for CCP pins
    TRISCbits.TRISC1 = 0;  // CCP2 output


    
    
}

void setPWMduty(unsigned int duty) {
    PR2 = 250; // load period 
    CCPR2L = duty; // load duty cycle 
    T2CONbits.TMR2ON = 1; // enable Timer2 
    T2CONbits.T2CKPS0 = 1; // clock it with (Fosc/4) / 4 
    CCP2CON = 0x0C; // PWM mode 
}

void MoveGate(int open_close){
    if (!open_close)
            setPWMduty(187.5);
        else
            setPWMduty(87.5);
}


