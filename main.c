/* --------------------------------------------------------------
 * Fichier     :   main.c
 * Auteur(s)   :
 * Description :
 * -------------------------------------------------------------- */

#include <xc.h>
#include <math.h>

// Configuration materielle du PIC :
#pragma config FEXTOSC = OFF           // Pas de source d'horloge externe
#pragma config RSTOSC = HFINTOSC_64MHZ // Horloge interne de 64 MHz
#pragma config WDTE = OFF              // Désactiver le watchdog

#define _XTAL_FREQ 64000000 // Frequence d'horloge - necessaire aux macros de delay (_delay(N) ; __delay_us(N) ; __delay_ms(N)))

// Définition des masques, macros, etc. :
// TODO
// NOTE : Registre A bit 2 = out_env_4
#define OUT_ENV_4_MASK 0b00100000
// NOTE : Registre A bit 3 = out_env_3
#define OUT_ENV_3_MASK 0b00010000
// NOTE : Registre A bit 4 = out_env_2
#define OUT_ENV_2_MASK 0b00001000
// NOTE : Registre A bit 5 = out_env_1
#define OUT_ENV_1_MASK 0b00000100

#define OUT_ENV_MASK (OUT_ENV_4_MASK || OUT_ENV_3_MASK || OUT_ENV_2_MASK || OUT_ENV_1_MASK)

#define BTN1_MASK 0b01000000

// Déclaration de fonctions et variables globales permettant au code C et à l'asm de les partager
// Une même fonction ou variable côté asm est préfixée par un underscore, et ne l'est pas côté C
// Avec ce formalisme, elles sont utilisables de façon intercangeable et transparente :
// | ---- asm ----- | ------------- C ----------------- |
// | _TX_64LEDS  <--|--> void TX_64LEDS(void)           |
// | _pC         <--|--> volatile const char * pC       |
// | _LED_MATRIX <--|--> volatile char LED_MATRIX [256] |

// Définition des fonctions relatives à la matrice de LEDs:
extern void TX_64LEDS(void); // Fonction définie dans tx.asm ; Fonction permettant d'envoyer la commande pour piloter les 64 LEDs, telle que décrite dans LED_MATRIX

// Définition des constantes / variables relatives à la matrice de LEDs :
volatile char LED_MATRIX [256] ; // Definition d'une matrice de 64 x 4 octets contenant les composantes R/G/B/W de chaque LED (1 octet/couleur/LED)
volatile const char * pC = LED_MATRIX; // Pointeur vers LED_MATRIX



int read_adc(int pin_to_read){
    /* Fonction de lecture de l'ADC en fonction du pin d'entr�e � lire
     * Retourne le nombre de leds � allumer en fonction du niveau d'entr�e */
    int resultHigh;
    int resultLow;
    int resultTotal;
    
    ADPCH = pin_to_read; //RA5 (out_env_1) is Analog channel
    ADCON0bits.ADGO = 1; //D�but de conversion
    while (ADCON0bits.ADGO); //Attendre que la conversion soit finie
    resultHigh = ADRESH; //Lire le r�sultat
    resultLow = ADRESL; //Read result
    // On d�cale les bits pour pouvoir additionner
    resultHigh = resultHigh * (int)pow(2,2);
    resultLow = resultLow * (int)pow(2, -6); 
    resultTotal = resultHigh + resultLow;
    return resultTotal % 128;
}


// - Fonction main ----------------------------------------------------------------------
void main(void) {
    /* Configuration des entrées / sorties */
    // TODO
   // D�finir registre A bits 2 � 5 en entr�e
    LATA = 0x00;
    TRISA = 0b11111111;
    ANSELA &= 0xFF;// � 0 mais entr�e analogique = 1
    
    TRISB &= 0xEF; // LED_MASTER : OUTPUT
    TRISC &= 0x00; // LED0-7     : OUTPUT
    
    
    LATC  = 0x00; // Eteindre LED0-7
     /*
    // Allumage des leds
    if(TRISA & OUT_ENV_1_MASK){
        LATC |= 0x01;
    }
    if(TRISA & OUT_ENV_2_MASK){
        LATC |= 0x02;
    }
    if(TRISA & OUT_ENV_3_MASK){
        LATC |= 0x04;
    }
    if(TRISA & OUT_ENV_4_MASK){
        LATC |= 0x08;
    }
     __delay_ms(500);
    
     
     // V�rification des input � l'aide du bouton
    while(1) {
        if(PORTA & BTN1_MASK) {
            LATC |= 0x40;
        } else {
            LATC &= ~0x40;
        }
    }
     
    // En th�orie, les entr�es analogiques seront les suivantes : 
    // Pr�paration des entr�es analogiques
    // D�finir registre A bits 2 � 5 en entr�e
    LATA = 0x00;
    TRISA = 0b11111111;
    ANSELA &= 0xFF;// Tout est � 1 - les entr�es RA2, RA3, RA4 et RA5 sont les entr�es correspondant � la sortie des filtres
    
    
    // Tant que les entr�es sont 1, alors ...pas du tout il faut aller dans adresh / adcon, voir lire les niveaux de chaque pinouille
    // stocker les niveaux pinouille par pinouille et r�agir en fonction 
    // Donc �tape 1 : lire adresh pour chaque 
    // Proc�dure conversion adc : 
        // Configuration du port
            // Disable trisx
            // Configure ansel as analog 
        // Configurer l'adc : 
            // S�lectionner adc conversion clock 
            // Configurer la r�f�rence voltage
            // D�lectionner le channel d'input de l'adc
            // Allumer l'adc
        // If ADACQ = 0, software must wait the required acquisition time
        // Set ADGO bit
        // Attendre l'adc conversion en v�rifiant le bit ADGO
        // Lire les r�sultats de l'adc 
        
    // Exemple pour AN0
    //System Initialize (� remonter en haut du fichier dans l'initialisation)
    initializeSystem();
    //Setup ADC
    ADCON0bits.FM = 1; //right justify
    ADCON0bits.CS = 1; //FRC Clock
    ADPCH = 0x00; //RA0 is Analog channel
    TRISAbits.TRISA0 = 1; //Set RA0 to input
    ANSELAbits.ANSELA0 = 1; //Set RA0 to analog
    ADCON0bits.ON = 1; //Turn ADC On
    int resultHigh;
    int resultLow;
    while (1) {
        ADCON0bits.GO = 1; //D�but de conversion
        while (ADCON0bits.GO); //Attendre que la conversion soit finie
        resultHigh = ADRESH; //Lire le r�sultat
        resultLow = ADRESL; //Read result
    }
       
    */
    
    // Maintenant qu'on a l'exemple : on sait qu'on veut lire le niveau sur une entr�e (mettons an2) et allumer en fonction
    //initializeSystem();
    //Setup ADC
    ADCON0bits.ADFM = 1; //right justify // FM
    ADCON0bits.ADCS = 1; //FRC Clock
    
    // TRISAbits.TRISA0 = 1; //Set RA0 to input
    // ANSELAbits.ANSELA0 = 1; //Set RA0 to analog
    ADCON0bits.ADON = 1; //Turn ADC On
    int out_env_1 = 0x05;
    int out_env_2 = 0x04;
    int out_env_3 = 0x03;
    int out_env_4 = 0x02;
    int nb_leds_1; 
    int nb_leds_2;
    int nb_leds_3;
    int nb_leds_4;
    
    while (1) {
        nb_leds_1 = read_adc(out_env_1);
        nb_leds_2 = read_adc(out_env_2);
        nb_leds_3 = read_adc(out_env_3);
        nb_leds_4 = read_adc(out_env_4);
        
        
        LATC = 0x00; // Eteindre leds 0-7
        for(int i = 0; i < nb_leds_1; i++) {
            LATC |= (int)pow(2,(i-1));
            __delay_ms(500);
        }
        
        
    }
    
    
    // Allumer led en fonction du bouton
    /*if(TRISA && OUT_ENV_3_MASK){
         LATC = 0x02;
    } else {
        LATC = 0x04;
    }*/
    
    // r�cup�ration des entr�es out_env
   
    
    /* Corps du programme */
    // TODO

    /* Code pour vérification du bon fonctionnement de la partir uC (à retirer par la suite) : === DEMO CODE 

    // Initialisation des LEDs =================================================================== DEMO CODE
    TRISB &= 0xEF; // LED_MASTER : OUTPUT -------------------------------------------------------- DEMO CODE
    TRISC &= 0x00; // LED0-7     : OUTPUT -------------------------------------------------------- DEMO CODE

    LATB &= 0xEF; // Eteindre LEDM   ------------------------------------------------------------- DEMO CODE
    LATC  = 0x00; // Eteindre LED0-7 ------------------------------------------------------------- DEMO CODE

    // Blink sur LEDM : ========================================================================== DEMO CODE
    LATB |= 0x10;    // Allumer LEDM   ----------------------------------------------------------- DEMO CODE
    __delay_ms(500); // Macro de délai ----------------------------------------------------------- DEMO CODE
    LATB &= 0xEF;    // Eteindre LEDM  ----------------------------------------------------------- DEMO CODE
    __delay_ms(500); // Macro de délai ----------------------------------------------------------- DEMO CODE
    LATB |= 0x10;    // Allumer LEDM   ----------------------------------------------------------- DEMO CODE

    // Chenillard : ============================================================================== DEMO CODE
    while(1){ //---------------------------------------------------------------------------------- DEMO CODE
        for (int i=0; i<8; i++){ // -------------------------------------------------------------- DEMO CODE
            LATC = 0x01 << i;    // Commander les LEDs de test sur le PORTC ---------------------- DEMO CODE
            __delay_ms(125);     // Macro de délai ----------------------------------------------- DEMO CODE
        } // ------------------------------------------------------------------------------------- DEMO CODE
    } // ----------------------------------------------------------------------------------------- DEMO CODE */

    return;
}
