// Inclusions _________________________________________________________________

#include "motor.h"
#include <avr/io.h>

// Définition de fonctions publiques __________________________________________

void pwm0_init(void){

	// Configuration des broches de la modulation de largeur d'impulsion en sortie
	DDRB = set_bits(DDRB, 0b00011000); // Active PB3 et PB4

	// Choix du mode de comparaison pour le comparateur A:
	// "Clear on Compare Match when up-counting. Set on Compare Match when down-counting."
	TCCR0A = set_bit(TCCR0A, COM0A1);
	TCCR0A = clear_bit(TCCR0A, COM0A0);
	
	// Choix du mode de comparaison pour le comparateur B:
	// "Clear on Compare Match when up-counting. Set on Compare Match when down-counting."
	TCCR0A = set_bit(TCCR0A, COM0B1);
	TCCR0A = clear_bit(TCCR0A, COM0B0);
	
	// Choix du mode du compteur :  "PWM phase correct (avec valeur TOP égale à 255)"
	TCCR0A = set_bit(TCCR0A, WGM00);
	TCCR0A = clear_bit(TCCR0A, WGM01);
	TCCR0B = clear_bit(TCCR0B, WGM02);
	
	// Choix de l'horloge interne sans facteur de division de fréquence
	TCCR0B = set_bit(TCCR0B, CS00);
	TCCR0B = clear_bit(TCCR0B, CS01);
	TCCR0B = clear_bit(TCCR0B, CS02);
}

void pwm0_ctrl_PB3(uint8_t compare_value){
	// Choix du rapport cyclique en fixant la valeur de comparaison (Moteur Droit)
	OCR0A = compare_value;
}

void pwm0_ctrl_PB4(uint8_t compare_value){
	// Choix du rapport cyclique en fixant la valeur de comparaison (Moteur Gauche)
	OCR0B = compare_value;
}

void motor_init(void){
	
	// Configuration des broches de direction (PH du H-bridge) en sortie
	DDRB = set_bit(DDRB, PB2); // Direction Gauche
	DDRB = set_bit(DDRB, PB1); // Direction Droite

	PORTB = clear_bit(PORTB, PB2);
	PORTB = clear_bit(PORTB, PB1);
	
	// Initialisation du PWM 0 pour le contrôle des moteurs
	pwm0_init();
	
	// Mise à 0 de la vitesse initiale
	pwm0_ctrl_PB4(0);
	pwm0_ctrl_PB3(0);
}

void motor_ctrl_right(float speed){
	
	int rapport_cyclique = 0;
	float seuil_friction = 40.0; // Le PWM minimum pour que le moteur bouge
	float decalage = 4.0;        // Ta zone morte pour le joystick
	
	// VITESSE MAXIMALE POSITIVE
	if (speed >= 100.0) {
		PORTB = set_bit(PORTB, PB1); // INVERSÉ : set_bit au lieu de clear_bit
		rapport_cyclique = 255;
	}
	// VITESSE PROPORTIONNELLE POSITIVE
	else if (speed > decalage) {
		PORTB = set_bit(PORTB, PB1); // INVERSÉ
		rapport_cyclique = (int)(seuil_friction + ((speed - decalage) / (100.0 - decalage)) * (255.0 - seuil_friction));
	}
	// VITESSE MAXIMALE NÉGATIVE
	else if (speed <= -100.0) {
		PORTB = clear_bit(PORTB, PB1); // INVERSÉ : clear_bit au lieu de set_bit
		rapport_cyclique = 255;
	}
	// VITESSE PROPORTIONNELLE NÉGATIVE
	else if (speed < -decalage) {
		PORTB = clear_bit(PORTB, PB1); // INVERSÉ
		float abs_speed = -speed;
		rapport_cyclique = (int)(seuil_friction + ((abs_speed - decalage) / (100.0 - decalage)) * (255.0 - seuil_friction));
	}
	// ARRÊT TOTAL (Zone morte)
	else {
		rapport_cyclique = 0;
	}
	
	// Envoi du rapport cyclique au moteur de droite
	pwm0_ctrl_PB3((uint8_t)rapport_cyclique);
}

void motor_ctrl_left(float speed){
	
	int rapport_cyclique = 0;
	float seuil_friction = 40.0;
	float decalage = 4.0;
	
	// VITESSE MAXIMALE POSITIVE
	if (speed >= 100.0) {
		PORTB = set_bit(PORTB, PB2); // INVERSÉ : set_bit au lieu de clear_bit
		rapport_cyclique = 255;
	}
	// VITESSE PROPORTIONNELLE POSITIVE
	else if (speed > decalage) {
		PORTB = set_bit(PORTB, PB2); // INVERSÉ
		rapport_cyclique = (int)(seuil_friction + ((speed - decalage) / (100.0 - decalage)) * (255.0 - seuil_friction));
	}
	// VITESSE MAXIMALE NÉGATIVE
	else if (speed <= -100.0) {
		PORTB = clear_bit(PORTB, PB2); // INVERSÉ : clear_bit au lieu de set_bit
		rapport_cyclique = 255;
	}
	// VITESSE PROPORTIONNELLE NÉGATIVE
	else if (speed < -decalage) {
		PORTB = clear_bit(PORTB, PB2); // INVERSÉ
		float abs_speed = -speed;
		rapport_cyclique = (int)(seuil_friction + ((abs_speed - decalage) / (100.0 - decalage)) * (255.0 - seuil_friction));
	}
	// ARRÊT TOTAL (Zone morte)
	else {
		rapport_cyclique = 0;
	}
	
	// Envoi du rapport cyclique au moteur de gauche
	pwm0_ctrl_PB4((uint8_t)rapport_cyclique);
}