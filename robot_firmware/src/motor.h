#ifndef MOTOR_H_INCLUDED
#define MOTOR_H_INCLUDED

// Inclusions _________________________________________________________________

#include "utils.h"


// Déclaration de fonctions publiques _________________________________________

/**
    \brief  Fait l'initialisation des registres nécessaires à la génération de modulation de largeur d'impulsion (PWM)
    \return rien.

	Cette fonction détermine le fonctionnement du compteur 0 (8 bits) de l'ATmega644A pour générer des signaux MLI 
	sur les broches PB3 et PB4.
*/
void pwm0_init(void);

/**
    \brief Détermine le rapport cyclique du PWM à la sortie PB3 (broche 4 du DIP)
	\param[in] compare_value Une valeur entre 0 et 255.
    \return rien.

	Une compare_value de 0 correspond à un PWM de 0% (rapport cyclique) et une compare_value de 255 correspond à un
	PWM de 100%. Cette relation est linéaire sur toute l'intervalle.

			    +---- ----+
			  --| 1  U 40 |--
			  --| 2    39 |--
			  --| 3    38 |--
		  PB3 --| 4    37 |--
			  --| 5    36 |--
			  --| 6    35 |--
			  --| 7    34 |--
			  --| 8    33 |--
			  --| 9    32 |--
			  --| 10   31 |--
			  --| 11   30 |--
			  --| 12   29 |--
			  --| 13   28 |--
			  --| 14   27 |--
			  --| 15   26 |--
			  --| 16   25 |--
			  --| 17   24 |--
	          --| 18   23 |--
			  --| 19   22 |--
			  --| 20   21 |--
			    +---------+
*/
void pwm0_ctrl_PB3(uint8_t compare_value);

/**
    \brief Détermine le rapport cyclique du PWM à la sortie PB4 (broche 5 du DIP)
	\param[in]	compare_value Une valeur entre 0 et 255
    \return rien.

	Une compare_value de 0 correspond à un PWM de 0% (rapport cyclique) et une compare_value de 255 correspond à un
	PWM de 100%. Cette relation est linéaire sur toute l'intervalle.

			    +---- ----+
			  --| 1  U 40 |--
			  --| 2    39 |--
			  --| 3    38 |--
		      --| 4    37 |--
		 PB4  --| 5    36 |--
			  --| 6    35 |--
			  --| 7    34 |--
			  --| 8    33 |--
			  --| 9    32 |--
			  --| 10   31 |--
			  --| 11   30 |--
			  --| 12   29 |--
			  --| 13   28 |--
			  --| 14   27 |--
			  --| 15   26 |--
			  --| 16   25 |--
			  --| 17   24 |--
	          --| 18   23 |--
			  --| 19   22 |--
			  --| 20   21 |--
			    +---------+
*/
void pwm0_ctrl_PB4(uint8_t compare_value);


/**
    \brief Fait l'initialisation des périphériques nécessaires au contrôle des
	moteurs par un pont en H
    \return rien.
	
	Les fonctions liées aux moteurs s'attendent à ce que ceux-ci soient branchés de la façon suivante :
	
					   +---- ----+
					 --| 1  U 40 |--
	Dir. Mot. Droit  --| 2    39 |--
	Dir. Mot. Gauche --| 3    38 |--
	PWM Mot. Droit	 --| 4    37 |--
	PWM Mot. Gauche	 --| 5    36 |--
					 --| 6    35 |--
					 --| 7    34 |--
					 --| 8    33 |--
					 --| 9    32 |--
					 --| 10   31 |--
					 --| 11   30 |--
					 --| 12   29 |--
					 --| 13   28 |--
					 --| 14   27 |--
					 --| 15   26 |--
	                 --| 16   25 |--
	                 --| 17   24 |--
					 --| 18   23 |--
					 --| 19   22 |--
	                 --| 20   21 |--
					   +---------+
*/
void motor_init(void);

/**
    \brief Contrôle la vitesse et la direction du moteur droit
	\param[in]	speed Une valeur décimale entre -100 et 100
    \return rien.
	
	Une valeur de speed de -100 représente la vitesse maximale dans le sens inverse.
	Une valeur de speed de 0 représente le moteur à l'arrêt
	Une valeur de speed de 100 représente la vitesse maximale dans le sens normal.
*/
void motor_ctrl_right(float speed);

/**
    \brief Contrôle la vitesse et la direction du moteur gauche
	\param[in]	speed Une valeur décimale entre -100 et 100
    \return rien.
	
	Une valeur de speed de -100 représente la vitesse maximale dans le sens inverse.
	Une valeur de speed de 0 représente le moteur à l'arrêt
	Une valeur de speed de 100 représente la vitesse maximale dans le sens normal.
*/
void motor_ctrl_left(float speed);

#endif /* MOTOR_H_INCLUDED */