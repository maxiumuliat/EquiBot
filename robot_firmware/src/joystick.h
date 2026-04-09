#ifndef JOYSTICK_H_INCLUDED
#define JOYSTICK_H_INCLUDED


// Inclusions _________________________________________________________________

#include "utils.h"


// Déclaration de fonctions publiques _________________________________________

/**
    \brief Initialise le module de l'ADC
    \return rien.

	Le module de l'ADC utilise le PORT A. 

			    +---- ----+
			  --| 1  U 40 |-- ADC 0
			  --| 2    39 |-- ADC 1
			  --| 3    38 |-- ADC 2
			  --| 4    37 |-- ADC 3
			  --| 5    36 |-- ADC 4
			  --| 6    35 |-- ADC 5
			  --| 7    34 |-- ADC 6
			  --| 8    33 |-- ADC 7
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
void adc_init(void);

/**
    \brief Fait une conversion de la valeur analogique présente sur une entrée
    \param[in]	channel	Le channel sur lequel la conversion doit être effectuée (entre PA0 et PA7 inclusivement)
    \return La valeur convertie.

	C'est une erreur d'appeler cette fonction avec un channel négatif ou plus grand que 7

	Il est important de noter que cette fonction ne s'exécute pas instantanément. La conversion
	prend un certain temps à s'effectuer et la fonction attend la fin de la conversion avant de
	retourner. C'est une mauvaise idée d'appeler cette fonction dans une boucle avec des délais
	critiques.
*/
uint16_t adc_read(uint8_t channel);

/**
    \brief Initialise tous les modules et périphériques nécessaires à l'utilisation d'un joystick analogue
    \return rien.

	Les fonctions liées au joystick s'attendent à ce que celui-ci soit branché de la façon suivante :

	  +---- ----+
	--| 1  U 40 |-- 
	--| 2    39 |-- Vertical
	--| 3    38 |-- Horizontal
	--| 4    37 |-- Bouton
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
void joystick_init(void);

/**
    \brief Fait une lecture de l'axe vertical du joystick et la normalise en une valeur entre -100 et 100
    \return La valeur de l'axe vertical normalisée.
*/
float joystick_get_vertical(void);

/**
    \brief Fait une lecture de l'axe horizontal du joystick et la normalise en une valeur entre -100 et 100
    \return La valeur de l'axe horizontal normalisée.
*/
float joystick_get_horizontal(void);

/**
    \brief Fait une lecture de l'état du bouton du joystick et le convertit en logique directe
    \return Si true, le bouton est enfoncé, si false, le bouton est relâché
*/
bool joystick_get_button_state(void);

/**
    \brief Détecte un changement d'état sur le bouton
    \return True seulement si le bouton est nouvellement enfoncé.
*/
bool joystick_get_click(void);


#endif /* JOYSTICK_H_INCLUDED */