// Inclusions _________________________________________________________________

#include <avr/io.h>
#include "servo.h"


// Constantes globales privées conditionnelles ________________________________

#if F_CPU == 8000000UL     // Fosc = 8.0000MHz
	#define TIMER_1_TOP_VALUE 19999
	#define COUNT_MIN 800
	#define COUNT_MAX 2200	

#elif F_CPU == 16000000UL     /*Fosc = 16.0000MHz*/
	#define TIMER_1_TOP_VALUE 39999
	#define COUNT_MIN 1600
	#define COUNT_MAX 4400
	
#elif F_CPU == 20000000UL   /*Fosc = 20.0000MHz*/
	#define TIMER_1_TOP_VALUE 49999
	#define COUNT_MIN 2000
	#define COUNT_MAX 5500

#else
	#error "Vous avez oublie de definir F_CPU dans les symboles et servo.c en a obligatoirement besoin"
	
#endif

// Peu importe la fréquence, c'est la même formule
#define COUNT_RANGE (COUNT_MAX - COUNT_MIN)


// Définition de fonction publiques ___________________________________________

void servo_init(void){
		
	// On met en sortie les broches reliées au timer 1
	DDRD = set_bit(DDRD, PD4);
	DDRD = set_bit(DDRD, PD5);
		
	// 2.1- Mode de comparaison : "Clear OCnA/OCnB on Compare Match when up-counting...
	// ...Set OCnA/OCnB on Compare Match when downcounting."
	// (registre TCCR1A: COM1A1<-1,COM1A0<-0,COM1B1<-1,COM1B0<-0)
	TCCR1A = set_bit(TCCR1A,COM1A1);
	TCCR1A = clear_bit(TCCR1A,COM1A0);
	TCCR1A = set_bit(TCCR1A,COM1B1);
	TCCR1A = clear_bit(TCCR1A,COM1B0);
		
	// 2.2- Mode du compteur :  "Fast PWM mode (avec valeur TOP fixé par ICR1)"
	// (registre TCCR1B: WGM13<-1,WGM12<-1,WGM11<-1,WGM10<-0)
	TCCR1B = set_bit(TCCR1B,WGM13);
	TCCR1B = set_bit(TCCR1B,WGM12);
	TCCR1A = set_bit(TCCR1A,WGM11);
	TCCR1A = clear_bit(TCCR1A,WGM10);

	// 2.3- Fixer la valeur initiale du compteur 0 à 0 (registre TCNT1<-0)
	TCNT1 = 0;

	// 2.4- fixer la valeur maximale (TOP) du compteur 1 (registre ICR1<-top)
	ICR1 = TIMER_1_TOP_VALUE;
		
	// 2.5- Facteur de division de fréquence : 8 (registre TCCR1B: CS12<-0,CS11<-1,CS10<-0)
	// activer l'horloge avec facteur de division par 8
	TCCR1B = clear_bit(TCCR1B,CS12);
	TCCR1B = set_bit(TCCR1B,CS11);
	TCCR1B = clear_bit(TCCR1B,CS10);
		
	servo_ctrl_PD4(SERVO_INITIAL_RATIO_PD4);
	servo_ctrl_PD5(SERVO_INITIAL_RATIO_PD5);
}

void servo_ctrl_PD4(float ratio){
	
	// On force le paramètre à être dans l'interval permis
	if(ratio < 0.){
		ratio = 0.;
	}
	else if(ratio > 100.){
		ratio = 100.;
	}	

	// On converti le ratio en une valeur de comparaison pour le timer 1
	uint16_t count = (uint16_t)(ratio * COUNT_RANGE * 0.01) + COUNT_MIN;
	
	// On applique cette comparaison au comparateur B qui est relié à PD4
	OCR1B = count;	
}

void servo_ctrl_PD5(float ratio){
	
	// On force le paramètre à être dans l'interval permis
	if(ratio < 0.){
		ratio = 0.;
	}
	else if(ratio > 100.){
		ratio = 100.;
	}		

	// On converti le ratio en une valeur de comparaison pour le timer 1
	uint16_t count = (uint16_t)(ratio * COUNT_RANGE * 0.01) + COUNT_MIN;
	
	// On applique cette comparaison au comparateur A qui est relié à PD5
	OCR1A = count;
}