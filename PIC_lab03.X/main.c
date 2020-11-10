/*
 * File:   main.c
 * Author: Advanced Digital Systems Laboratory
 *
 * Created on August 12, 2020, 8:30 AM
 */

// <editor-fold defaultstate="collapsed" desc="Configuration bits">

// CONFIG1
// External Oscillator Selection (Oscillator not enabled)
#pragma config FEXTOSC = OFF    

// Reset Oscillator Selection (HFINTOSC with HFFRQ = 64 MHz and CDIV = 1:1)
#pragma config RSTOSC = HFINTOSC_64MHZ

// CONFIG2
// Clock out Enable bit (CLKOUT function is disabled)
#pragma config CLKOUTEN = OFF
// PRLOCKED One-Way Set Enable bit (PRLOCKED bit can be cleared and
// set only once)
#pragma config PR1WAY = ON
// Clock Switch Enable bit (The NOSC and NDIV bits cannot be changed
// by user software)
#pragma config CSWEN = OFF
// Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config FCMEN = OFF

// CONFIG3
// MCLR Enable bit
// (If LVP = 0, MCLR pin is MCLR;
// If LVP = 1, RE3 pin function is MCLR)
#pragma config MCLRE = EXTMCLR
// Power-up timer selection bits (PWRT is disabled)
#pragma config PWRTS = PWRT_OFF
// Multi-vector enable bit (Interrupt contoller does not use
// vector table to prioritze interrupts)
#pragma config MVECEN = OFF
// IVTLOCK bit One-way set enable bit (IVTLOCKED bit can be cleared
// and set only once)
#pragma config IVT1WAY = ON
// Low Power BOR Enable bit (Low-Power BOR disabled)
#pragma config LPBOREN = OFF
// Brown-out Reset Enable bits (Brown-out Reset disabled)
#pragma config BOREN = OFF

// CONFIG4
// Brown-out Reset Voltage Selection bits (Brown-out Reset Voltage (VBOR)
// set to 1.9V)
#pragma config BORV = VBOR_1P9
// ZCD Disable bit (ZCD module is disabled.
// ZCD can be enabled by setting the ZCDSEN bit of ZCDCON)
#pragma config ZCD = OFF
// PPSLOCK bit One-Way Set Enable bit (PPSLOCKED bit can be cleared
// and set only once; PPS registers remain locked after one clear/set cycle)
#pragma config PPS1WAY = ON
// Stack Full/Underflow Reset Enable bit (Stack full/underflow
// will cause Reset)
#pragma config STVREN = ON
// Low Voltage Programming Enable bit (HV on MCLR/VPP must be used
// for programming)
#pragma config LVP = OFF
// Extended Instruction Set Enable bit (Extended Instruction Set and
// Indexed Addressing Mode disabled)
#pragma config XINST = OFF

// CONFIG5
// WDT Period selection bits (Divider ratio 1:65536;
// software control of WDTPS)
#pragma config WDTCPS = WDTCPS_31
// WDT operating mode (WDT Disabled; SWDTEN is ignored)
#pragma config WDTE = OFF

// CONFIG6
// WDT Window Select bits (window always open (100%);
// software control; keyed access not required)
#pragma config WDTCWS = WDTCWS_7
// WDT input clock selector (Software Control)
#pragma config WDTCCS = SC

// CONFIG7
// Boot Block Size selection bits (Boot Block size is 512 words)
#pragma config BBSIZE = BBSIZE_512
// Boot Block enable bit (Boot block disabled)
#pragma config BBEN = OFF
// Storage Area Flash enable bit (SAF disabled)
#pragma config SAFEN = OFF
// Background Debugger (Background Debugger disabled)
#pragma config DEBUG = OFF

// CONFIG8
// Boot Block Write Protection bit (Boot Block not Write protected)
#pragma config WRTB = OFF
// Configuration Register Write Protection bit (Configuration registers
// not Write protected)
#pragma config WRTC = OFF
// Data EEPROM Write Protection bit (Data EEPROM not Write protected)
#pragma config WRTD = OFF
// SAF Write protection bit (SAF not Write Protected)
#pragma config WRTSAF = OFF
// Application Block write protection bit (Application Block not write
// protected)
#pragma config WRTAPP = OFF

// CONFIG10
// PFM and Data EEPROM Code Protection bit (PFM and Data EEPROM code
// protection disabled)
#pragma config CP = OFF

// </editor-fold>

#define USED_UART                   1

// Χρησιμοποιείται για την printf
#include <stdio.h>

// Χρησιμοποιείται για την strcmp()
#include <string.h>

// Χρησιμοποιείται για τις δηλώσεις μεταβλητών uint8_t, int8_t κλπ.
#include <stdint.h>

#include <xc.h>

// Υλοποίηση της συνάρτησης putch() για τη χρήση εντολών
// printf στη σειριακή θύρα UART2
void putch(unsigned char data)
{
#if USED_UART == 1
    // Αναμονή για όσo ο transmit buffer είναι γεμάτος
    while(U1FIFObits.TXBF == 1);
    // Φόρτωση δεδομένων από την παράμετρο data στον transmit buffer
    U1TXB = data;
#elif USED_UART == 2
    // Αναμονή για όσo ο transmit buffer είναι γεμάτος
    while(U2FIFObits.TXBF == 1);
    // Φόρτωση δεδομένων από την παράμετρο data στον transmit buffer
    U2TXB = data;
#endif
}

void main(void)
{
    // Δήλωση μεταβλητών
    // c: βοηθητική μεταβλητή τύπου character (χαρακτήρας))
    // cmd: πίνακας χαρακτήρων για την αποθήκευση της εντολής
    char c, cmd[20];
    uint8_t i = 0;
    
    // Καθορισμός RD0 και RD1 ως ψηφιακές έξοδοι
    // και καθορισμός στο λογικό '0'
    ANSELD = ANSELD & 0xFC;
    TRISD = TRISD & 0xFC;
    LATD = LATD & 0xFC;
    
#if USED_UART == 1
    // Καθορισμός ακροδέκτη RF0 ως ψηφιακή έξοδος και
    // ορισμός του ως σειριακή έξοδος της UART1
    ANSELFbits.ANSELF0 = 0;
    TRISFbits.TRISF0 = 0;
    // U1TX -> RF0
    RF0PPS = 0x20;
    
    // Καθορισμός ακροδέκτη RF1 ως ψηφιακή είσοδος και
    // ορισμός του ως σειριακή είσοδος της UART1
    ANSELFbits.ANSELF1 = 0;
    TRISFbits.TRISF1 = 1;
    // U1RX <- RF0
    U1RXPPS = 0b00101001;
    
    // Καθορισμός baud rate στα 115200
    U1BRGH = 0;  U1BRGL = 138;
    
    // Ρυθμίσεις σειριακής θύρας UART1
    U1CON0 = 0;
    U1CON0bits.BRGS = 1;
    U1CON0bits.TXEN = 1;
    U1CON0bits.RXEN = 1;
    U1CON2 = 0;
    U1CON1 = 0;
    
    // Ενεργοποίηση σειριακής θύρας UART1
    U1CON1bits.ON = 1;
    
#elif USED_UART == 2
    // Καθορισμός ακροδέκτη RB0 ως ψηφιακή έξοδος και
    // ορισμός του ως σειριακή έξοδος της UART2
    ANSELBbits.ANSELB0 = 0;
    TRISBbits.TRISB0 = 0;
    // U2TX -> RB0
    RB0PPS = 0x23;
    
    // Καθορισμός ακροδέκτη RB1 ως ψηφιακή είσοδος και
    // ορισμός του ως σειριακή είσοδος της UART2
    ANSELBbits.ANSELB1 = 0;
    TRISBbits.TRISB1 = 1;
    // U2RX <- RB1
    U2RXPPS = 0b00001001;
    // Καθορισμός baud rate στα 115200
    U2BRGH = 0;  U2BRGL = 139;
    
    // Ρυθμίσεις σειριακής θύρας UART2
    U2CON0 = 0;
    U2CON0bits.BRGS = 1;
    U2CON0bits.TXEN = 1;
    U2CON0bits.RXEN = 1;
    U2CON2 = 0;
    U2CON1 = 0;
    
    // Ενεργοποίηση σειριακής θύρας UART2
    U2CON1bits.ON = 1;
#endif
    
    // Εκτύπωση ενημερωτικού αλφαριθμητικού
    puts("UART is up and running\r");
    
    // Εκκαθάριση buffer εντολής και μετρητή i
    for(i = 0; i < 20; i++) cmd[i] = 0;
    i = 0;
    
    for(;;)
    {
#if USED_UART == 1
        // Αναμονή μέχρι τη λήψη ενός byte στη σειριακή θύρα UART1
        while(U1FIFObits.RXBE == 1);
        asm("nop");
        
        // Ανάγνωση εισερχόμενου byte από τον receive buffer
        c = U1RXB;
        
#elif USED_UART == 2
        // Αναμονή μέχρι τη λήψη ενός byte στη σειριακή θύρα UART2
        while(U2FIFObits.RXBE == 1);
        asm("nop");
        
        // Ανάγνωση εισερχόμενου byte από τον receive buffer
        c = U2RXB;
#endif
        // Αν το εισερχόμενο byte είναι το Carriage Return (CR=0x0D)
        // τότε εξετάζεται αν αναγνωρίζεται η εντολή
        if (c == 0x0D)
        {
            // Αν η εντολή είναι e0, τότε RD0=1
            if (strcmp(cmd, "e0") == 0)
            {
                puts("\n\rActions for e0 command\r");
                LATDbits.LATD0 = 1;
            }
            // Αν η εντολή είναι e1, τότε RD1=1
            else if (strcmp(cmd, "e1") == 0)
            {
                puts("\n\rActions for e1 command\r");
                LATDbits.LATD1 = 1;
            }
            // Αν η εντολή είναι d0, τότε RD0=0
            else if (strcmp(cmd, "d0") == 0)
            {
                puts("\n\rActions for d0 command\r");
                LATDbits.LATD0 = 0;
            }
            // Αν η εντολή είναι d1, τότε RD1=0
            else if (strcmp(cmd, "d1") == 0)
            {
                puts("\n\rActions for d1 command\r");
                LATDbits.LATD1 = 0;
            }
            else if (strcmp(cmd, "e0e1") == 0)
            {
                puts("\n\rActions for d1 command\r");
                LATDbits.LATD0 = 1;
                LATDbits.LATD1 = 1;
            }
            else if (strcmp(cmd, "d0d1") == 0)
            {
                puts("\n\rActions for d1 command\r");
                LATDbits.LATD0 = 0;
                LATDbits.LATD1 = 0;
            }
            // Διαφορετικά η εντολή δεν αναγνωρίζεται
            else puts("\n\rBad command\r");
            
            // Εκκαθάριση buffer χαρακτήρων εντολής και μετρητή i
            for(i = 0; i < 20; i++) cmd[i] = 0;
            i = 0;
        }
        // Αν το εισερχόμενο byte δεν είναι το Line Feed (LF = 0x0A)
        // τότε αποθηκεύεται ο χαρακτήρας στον πίνακα χαρακτήρων εντολής
        else if (c != 0x0A)
        {
            // Εκτύπωση χαρακτήρα (echo)
            printf("%c", c);
            cmd[i++] = c;
        }
    }
    return;
}