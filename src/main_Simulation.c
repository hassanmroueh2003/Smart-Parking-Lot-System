#include <p18cxxx.h>       // Device-specific header
#include <LCD4lib.h>       // LCD 4-bit control library
#include <EEPROM.h>        // EEPROM access library
#include <delays.h>        // Delay routines

#define eeSeconds 0x00     // EEPROM address for storing "Seconds" variable
#define Device PORTCbits.RC7
#define Start PORTCbits.RC0
#define exit  PORTCbits.RC3
#define entrance PORTCbits.RC2
#define motor PORTCbits.RC1

#pragma config FOSC = INTIO67   // Internal oscillator
#pragma config LVP = OFF        // Low-voltage programming disabled
#pragma config BOREN = OFF      // Brown-out reset disabled

#define _XTAL_FREQ 8000000      // Oscillator frequency
#define plus PORTAbits.RA0

unsigned int ang1;
int lmax, lmin;
int i, servo = 1;
char entered = 0, exited = 0;
char Seconds = 0;
char digits[3];

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
        setup_pwm();
        Setup();
        turnOn();
        GateControl();
    }
}

void Setup(void) {
    Seconds = 0;
    TRISC |= 0x0D;             // RC0, RC2, RC3 inputs; RC1 output
    TRISCbits.RC7 = 0;         // RC7 output
    ANSELC &= 0x70;            // Configure RC0-RC4 as digital
    InitLCD();
    DispRomStr(Ln1Ch0,(ROM)"Enter parking ");
    DispRomStr(Ln2Ch0,(ROM)"Remaining:    ");
    ReadEE(eeSeconds, &Seconds);
    Seconds = 0;
    Bin2Asc(Seconds, digits);
    DispVarStr(digits, Ln2Ch11, 3);
    Device = 0;
}

void turnOn(void) {
    while(1) {
        if (Start == 1) {
            Device = 1;
            Seconds = 0;
            set_pwm2_duty(93);
            return;
        }
        Bin2Asc(Seconds, digits);
        DispVarStr(digits, Ln2Ch11, 3);
    }
}

void GateControl(void) {
    while(1) {
        while (Seconds == 3) {
            if (exit) {
                Seconds--;
                while (1) {
                    set_pwm2_duty(140);
                    if (entrance) {
                        Delay10KTCYx(20);
                        break;
                    }
                }
                while (exit);
            }
        }
        if (entrance) {
            while (entrance);
            while (1) {
                set_pwm2_duty(140);
                if (exit) {
                    Delay10KTCYx(20);
                    break;
                }
            }
            Seconds++;
            while (entrance);
        } else if (exit) {
            Seconds--;
            while (1) {
                set_pwm2_duty(140);
                if (entrance) break;
            }
            while (exit);
        } else if (Start) {
            if (Seconds != 0) {
                while (Start);
                break;
            }
        }

        Delay10KTCYx(5);

        if (Seconds > 4 || Seconds < 0)
            Seconds = 0;

        DispRomStr(Ln1Ch0, (ROM)"Enter parking lot");
        Bin2Asc(Seconds, digits);
        DispVarStr(digits, Ln2Ch11, 3);

        if (Seconds == 3)
            DispRomStr(Ln1Ch0, (ROM)" parking is full");

        while (1) {
            set_pwm2_duty(93);
            if (~exit || ~entrance)
                break;
        }
        motor = 0;
    }
}

void setup_pwm(void) {
    PR2 = 155;                     // Set PWM frequency (period register)
    T2CONbits.T2CKPS = 0b11;      // Prescaler = 16
    T2CONbits.TMR2ON = 1;         // Enable Timer2

    CCP2CONbits.CCP2M = 0b1100;   // CCP2 in PWM mode
    TRISCbits.TRISC1 = 0;         // CCP2 pin output
}

void set_pwm2_duty(unsigned int duty) {
    CCPR2L = duty >> 2;
    CCP2CONbits.DC2B = duty & 0x03;
}
