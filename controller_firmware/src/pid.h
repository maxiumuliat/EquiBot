#ifndef PID_H_INCLUDED
#define PID_H_INCLUDED

// Inclusions _________________________________________________________________

#include "utils.h"


// Déclaration de fonctions publiques _________________________________________

/**
    \brief Fait l'initialisation du module PID
	\return rien
*/
void pid_init(void);

/**
    \brief Change les gains qui sont utilisés pour le calcul du PID
	\param kp le gain propotionnel
	\param ki le gain intégrale
	\param kd le gain dérivé
	\return rien
*/
void pid_set_gains(float kp, float ki, float kd);

/**
    \brief Change le taux de correction de l'angle d'équilibre utilisé pour le calcul du PID
	\param fix_rate le taux de correction en radian par tic
	\return rien
	
	Avec une fréquence d'échantillionnage de 50 Hz il y a un tic tous les 20 ms. Par exemple. avec
	un fix_rate de 0.04 rad par tic, l'angle d'équilibre pourrait changer jusqu'à 2 rad en une seconde
	ce qui est équivalent à environ 115° en une seconde. Empiriquement, 2 rad en une seconde semble une
	bonne valeur de départ. Par la suite, il est toujours possible de modifier légèrement cette valeur
	pour obtenir des meilleures performances. 
*/
void pid_set_fix_rate(float fix_rate);

/**
    \brief Retourne si une bouvelle donnée est prête pour faire un tic de PID
	\return vrai si une nouvelle donnée est prête
*/
bool pid_is_new_data_ready(void);

/**
    \brief Calcule l'angle actuel à partir de l'unité de mesure inertielle
	\return un angle en radians
*/
float pid_get_current_angle(void);

/**
    \brief Retourne la valeur de l'angle déquilibre calculée par le PID
	\return un angle en radians
*/
float pid_get_equilibrium_angle(void);

/**
    \brief Fait un calcul du PID
	\param desired_angle l'angle désiré en radian (typiquement 0 rad)
	\param current_angle l'angle actuel en radian (typiquement celui retourné par pid_get_current_angle)
	\return une valeur qui représente le calcul du PID pour ce tic
*/
float pid_tic(float desired_angle, float current_angle);

/**
    \brief Remet l'état de PID à zéros
	\return rien
*/
void pid_reset(void);


#endif /* PID_H_INCLUDED */