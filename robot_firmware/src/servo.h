#ifndef SERVO_H_INCLUDED
#define SERVO_H_INCLUDED

// Inclusions _________________________________________________________________

#include "utils.h"


// Constantes globales publiques ______________________________________________

#define SERVO_INITIAL_RATIO_PD4 50
#define SERVO_INITIAL_RATIO_PD5 50


// Déclaration de fonctions publiques _________________________________________

/**
    \brief Fait l'initialisation nécéssaire pour contrôler un servomoteur. En
	appelant cette fonction, le timer 1 ainsi que les broches PD4 et PD5 deviennent
	utilisées exclusivement par ce module.
    \return Rien
*/
void servo_init(void);

/**
    \brief Change l'angle d'un servomoteur qui serait connecté à PD4. Le paramètre
	ratio doit inclusivement être compris entre 0 et 100. O représente l'angle minimal
	et 100 représente l'angle maximal. Toute valeur entre les deux extrêmes représente
	un angle intermédiaire proportionel à la valeur passée. Une valeur inférieure à 0
	est interprètée comme étant 0. Une valeur supérieure à 100 est interprètée comme 100.
	\param[in]  ratio   Une valeur dans l'interval [0., 100.] réprésentant l'angle désiré
    \return Rien
*/
void servo_ctrl_PD4(float ratio);

/**
    \brief Change l'angle d'un servomoteur qui serait connecté à PD4. Le paramètre
	ratio doit inclusivement être compris entre 0 et 100. O représente l'angle minimal
	et 100 représente l'angle maximal. Toute valeur entre les deux extrêmes représente
	un angle intermédiaire proportionel à la valeur passée. Une valeur inférieure à 0
	est interprètée comme étant 0. Une valeur supérieure à 100 est interprètée comme 100.
	\param[in]  ratio   Une valeur dans l'interval [0., 100.] réprésentant l'angle désiré
    \return Rien
*/
void servo_ctrl_PD5(float ratio);


#endif /* SERVO_H_INCLUDED */