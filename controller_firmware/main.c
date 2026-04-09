#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>
#include <avr/interrupt.h> 

#include "utils.h"
#include "joystick.h"
#include "uart.h"
#include "lcd.h"

// Variables globales statiques pour mémoriser l'état précédent (Optimisation LCD)
static int8_t last_x = -101;
static int8_t last_y = -101;
static uint8_t last_b = 2; 

void tx_commandes_wifi(int8_t x, int8_t y, uint8_t bouton) {
    char commande_wifi[40];
    
    // Enregistrement valeurs joystick dans tableau commande wifi (\n = separateur)
    sprintf(commande_wifi, "x=%d y=%d b=%d\n", x, y, bouton);
    
    // Message dans buffer FIFO pour envoi
    uart0_put_string(commande_wifi);
}

void msg_lcd(int8_t x, int8_t y, uint8_t bouton) {
    // Mettre a jour slm si une valeur a changé
    if (x != last_x || y != last_y || bouton != last_b) {
        
        // Retour au début écran
        lcd_set_cursor_position(0, 0);
        
        // %-4d aligne le texte à gauche et prend 4 espaces dans ecran
        printf("X:%-4d Y:%-4d\n", x, y);
        
        // Espaces vides final pour écraser le reste de la ligne
        printf("Btn: %-3d        ", bouton); 
       
        last_x = x;
        last_y = y;
        last_b = bouton;
    }
}

int main(void) {
    
    // Forcer broche RESET (PA6) ESP8266 à 1 pour stable connection
    DDRA = set_bit(DDRA, PA6);
    PORTA = set_bit(PORTA, PA6);

    // Délai de 3 secondes initialisation
    _delay_ms(3000);

    uart0_init();
    joystick_init();
    lcd_init();
    
    // Activation interruptions globales 
    sei();
    
    while(1) {
        // Lecture axes joystick (-100.0 à 100.0) 
        int8_t axe_y = (int8_t)joystick_get_vertical();
        int8_t axe_x = (int8_t)joystick_get_horizontal();

        // Capture clic avec front montant 
        bool clic = joystick_get_click();
        uint8_t etat_bouton = clic ? 1 : 0;

        // Mise à jour de affichage
        mettre_a_jour_lcd(axe_x, axe_y, etat_bouton);
        
        // Envoie commandes au robot
        tx_commandes_wifi(axe_x, axe_y, etat_bouton);

        // Délai essentiel afin de ne pas deborder le buffer de réception (150) du robot 
        _delay_ms(50);
    }
    
    return 0;
}

