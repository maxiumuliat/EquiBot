#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>
#include <avr/interrupt.h> 

#include "utils.h"
#include "joystick.h"
#include "uart.h"
#include "lcd.h"

//gestion commande d'envoie par wifi au robot
void tx_commandes_wifi(int8_t x, int8_t y, uint8_t bouton) {
    char cmd_wifi[40];
    
    // Enregistrement valeurs joystick dans tableau commande wifi (\n = separateur)
    snprintf(cmd_wifi, sizeof(cmd_wifi), "x=%d y=%d b=%d\n", x, y, bouton);
    
    // Message dans buffer FIFO pour envoi wifi a robot
    uart0_put_string(cmd_wifi);
}



int main(void) {
    
    // Force broche RESET (PA6) ESP8266 à 1 pour stable connection
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

        // Capture click avec front montant 
        bool clic = joystick_get_click();
        uint8_t etat_bouton = clic ? 1 : 0;
        
        // Envoie commandes au robot
        tx_commandes_wifi(axe_x, axe_y, etat_bouton);

        // Délai pour pas deborder buffer réception (150 caractères max) du robot 
        _delay_ms(50);
    }
    
    return 0;
}
