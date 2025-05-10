#include <p18cxxx.h>
#include <LCD4lib.h>
#include <EEPROM.h>
#include <delays.h>

// Pin definitions
#define eeSeconds 0x00
#define Device    PORTCbits.RC7
#define Start     PORTCbits.RC0
#define exit      PORTCbits.RC3
#define entrance  PORTCbits.RC2
#define motor     PORTCbits.RC1
#define plus      PORTAbits.RA0

// Configuration bits
#pragma config FOSC = INTIO67
#pragma config LVP = OFF
#pragma config BOREN = OFF

#define _XTAL_FREQ 8000000  // Clock frequency for delays

// Variables
unsigned int ang1;
int lmax, lmin;
int i, servo = 1;
char entered = 0, exited = 0;
char Seconds = 0;       // Number of cars currently parked
char digits[3];         // Buffer to hold ASCII digits for display

// Function prototypes
void Setup(void);
void GateControl(void);
void turnOn(void);
void setup_pwm(void);
void set_pwm2_duty(unsigned int duty);

void main(void) {
    lmin = 30;
    lmax = 65;
    ang1 = lmin;

    while(1) {
        setup_pwm();     // Initialize PWM for motor control
        Setup();         // Initialize system
        turnOn();        // Wait for Start button press
        GateControl();   // Handle parking gate logic
    }
}

void Setup(void) {
    Seconds = 0;
    TRISC |= 0x0D;         // Set RC0, RC2, RC3 as input
    TRISCbits.RC7 = 0;     // Set RC7 as output
    ANSELC &= 0x70;        // Make RC0-RC4 digital
    InitLCD();             // Initialize LCD display

    DispRomStr(Ln1Ch0, (ROM)"Enter parking ");
    DispRomStr(Ln2Ch0, (ROM)"Remaining cars are :");

    ReadEE(eeSeconds, &Seconds);  // Retrieve stored count from EEPROM (if needed)
    Seconds = 0;

    Bin2Asc(Seconds, digits);     // Convert number of cars to ASCII for LCD
    DispVarStr(digits, Ln2Ch11, 3);

    Device = 0;  // Turn off main device initially
}

void turnOn(void) {
    while(1) {
        if (Start == 1) {          // Start button pressed
            Device = 1;
            Seconds = 0;
            set_pwm2_duty(93);    // Initial PWM position
            return;
        }
        Bin2Asc(Seconds, digits);
        DispVarStr(digits, Ln2Ch11, 3);
    }
}

void GateControl(void) {
    while(1) {
        while(Seconds == 3) {  // Wait if parking is full
            if (~exit) {
                Seconds--;
                while(1) {
                    set_pwm2_duty(140);
                    if(~entrance) {
                        Delay10KTCYx(20);
                        break;
                    }
                }
                while(exit);
            }
        }

        if (~entrance) {   // A car is entering
            while(~entrance);
            while(1) {
                set_pwm2_duty(140);
                if(~exit) {
                    Delay10KTCYx(20);
                    break;
                }
            }
            Seconds++;
            while(~entrance);
        } else if (~exit) {   // A car is exiting
            Seconds--;
            while(1) {
                set_pwm2_duty(140);
                if(~entrance)
                    break;
            }
            while(~exit);
        } else if (Start) {
            if(Seconds != 0) {
                while(Start);
                break;
            }
        }

        Delay10KTCYx(5);  // Debounce delay

        if(Seconds > 4 || Seconds < 0)
            Seconds = 0;

        DispRomStr(Ln1Ch0, (ROM)"Enter parking lot");
        Bin2Asc(Seconds, digits);
        DispVarStr(digits, Ln2Ch11, 3);

        if(Seconds == 3)
            DispRomStr(Ln1Ch0, (ROM)" parking is full");

        while(1) {
            set_pwm2_duty(93);
            if(~exit || ~entrance)
                break;
        }
        motor = 0;  // Stop the motor
    }
}

void setup_pwm(void) {
    PR2 = 155;                      // Set PWM period
    T2CONbits.T2CKPS = 0b11;       // Prescaler = 16
    T2CONbits.TMR2ON = 1;          // Turn on Timer2
    CCP2CONbits.CCP2M = 0b1100;    // Enable PWM mode on CCP2
    TRISCbits.TRISC1 = 0;          // Set CCP2 pin as output
}

void set_pwm2_duty(unsigned int duty) {
    CCPR2L = duty >> 2;            // Set upper 8 bits of duty
    CCP2CONbits.DC2B = duty & 0x03; // Set lower 2 bits of duty
}
