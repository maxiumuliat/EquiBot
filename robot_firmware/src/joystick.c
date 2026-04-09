// Inclusions _________________________________________________________________


#include "joystick.h"
#include "utils.h"
#include <avr/io.h>


// Définition de fonctions publiques __________________________________________

void adc_init(void){
	
	
	// Désactivation des fonctions numériques des broches du port A qui servent d'entrées analogiques
	DIDR0=set_bit(DIDR0, ADC1D);
	DIDR0=set_bit(DIDR0, ADC2D);
	// Configuration en entrée des broches du port A qui servent d'entrées analogiques
	DDRA=clear_bit(DDRA,PA1);
	DDRA=clear_bit(DDRA,PA2);
	// Sélection de la référence de tension: la tension d'alimentation
	ADMUX=set_bit(ADMUX,REFS0);
	ADMUX=clear_bit(ADMUX,REFS1);
	// Choix de format du résultat de conversion: shift à droite pour une conversion entre 0 et 1023
	ADMUX=clear_bit(ADMUX,ADLAR);
	// Choix du facteur de division de l'horloge
	ADCSRA=set_bit(ADCSRA,ADPS2);
	ADCSRA=set_bit(ADCSRA,ADPS1);
	ADCSRA=clear_bit(ADCSRA,ADPS0);
	// Activation le CAN
	ADCSRA=set_bit(ADCSRA,ADEN);
}

uint16_t adc_read(uint8_t channel){

	// Sélection de la broche d'entrée à convertir (selon la valeur passée en paramètre)
	ADMUX = write_bits(ADMUX, 0b00011111, channel);
	// Démarrage d'une conversion
	ADCSRA=set_bit(ADCSRA, ADSC);
	// Attente de la fin de conversion
	while(read_bit(ADCSRA, ADSC)) {
		
	}
	// Lecture et renvoi du résultat qui est stocké dans le registre ADC
	uint16_t result = ADC;
	// RAPPEL : La fonction retourne une valeur d'ADC entre 0 et 1023
	
	// (Le return 0 est placé ici temporairement pour permettre au code de compiler correctement. Vous devez modifier cette ligne.)
	return result;
}


void joystick_init(void){
	
	// Initialisation de l'ADC pour faire la lecture de la position verticale et horizontale du joystick
	adc_init();
	
	// Configuration de la broche du bouton (click) du joystick en entrée
	DDRA=clear_bit(DDRA, PA3);
	
	// Activation la pull-up du bouton (click) du joystick
	PORTA=set_bit(PORTA, PA3);
}

float joystick_get_vertical(void){
	
	uint16_t valeur_vert;
	
	// Lecture du canal 1
	valeur_vert=adc_read(1);
	
	// Convertir 0-1023 (10 bits) vers un float entre -100.0 et 100.0
	float valeur_convert_v=(valeur_vert-511.5)*(100.0/511.5);
	
	return valeur_convert_v;
}

float joystick_get_horizontal(void){
	
	float valeur_horiz;
	
	// Lecture du canal 2
	valeur_horiz=adc_read(2);
	
	// Convertir 0-1023 (10 bits) vers un float entre -100.0 et 100.0
	float valeur_convert_h=(valeur_horiz-511.5)*(100.0/511.5);
	
	return valeur_convert_h;
}

bool joystick_get_button_state(void){
	uint8_t states=read_bit(PINA, PA3);
	if (states==0) {
		return true;
		} else {
		return false;
	}
}

bool joystick_get_click(void){
	
	// Ceci est une variable statique qui conserve sa valeur entre les appels à la fonction
	static bool previous_state = false;
	
	// À chaque fois que la fonction est appelée, on va cherche l'état actuel du bouton
	bool current_state = joystick_get_button_state();
	
	// On crée une variable pour stocker si le bouton vient d'être enfoncé.
	// Par défaut, on considère que ce n'est pas le cas
	bool click_state = false;
	
	// Si le bouton n'était pas enfoncé la dernière fois et que maintenant il l'est
	if(!previous_state && current_state){
		click_state = true;
	}
	
	// On sauvergarde l'état actuel pour le prochain appel avant de quitter la fonction
	previous_state = current_state;
	
	return click_state;

}