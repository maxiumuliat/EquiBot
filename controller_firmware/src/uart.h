#ifndef UART_H_INCLUDED
#define UART_H_INCLUDED

/******************************************************************************
Includes
******************************************************************************/

#include "utils.h"

// Switch _____________________________________________________________________

#define UART_ENABLE_UART_0
//#define UART_ENABLE_UART_1

// ----------------------------------------------------------------------------
// UART (tout ce qui est commun aux deux uarts)
//-----------------------------------------------------------------------------

// Types et constantes ________________________________________________________

typedef enum{

    BAUDRATE_2400 = 0,
    BAUDRATE_4800,
    BAUDRATE_9600,
    BAUDRATE_19200,
    BAUDRATE_38400,
    BAUDRATE_57600,
    BAUDRATE_115200,
    BAUDRATE_230400,
    BAUDRATE_250000,

}uart_baudrate_e;

#define DEFAULT_BAUDRATE BAUDRATE_9600

#define UART_LINE_SEPARATOR	'\n'


// Définition de fonctions publiques __________________________________________

// ----------------------------------------------------------------------------
// UART0
//-----------------------------------------------------------------------------

#ifdef UART_ENABLE_UART_0

// Constantes _________________________________________________________________

#define UART_0_RX_BUFFER_SIZE 150
#define UART_0_TX_BUFFER_SIZE 150


// Définition de fonctions publiques __________________________________________

/**
    \brief Fait l'initialisation du UART
	\return rien
*/
void uart0_init(void);

/**
    \brief Définit le baudrate du UART
	\param baudrate La valeur du baudrate
*/
void uart0_set_baudrate(uart_baudrate_e baudrate);

/**
    \brief Ajoute un byte au rolling buffer à envoyer par le UART
    \param byte le byte à ajouter qui sera éventuellement envoyé
*/
void uart0_put_byte(uint8_t byte);

/**
    \brief Ajoute la string (par copie) au rolling buffer à envoyer par le UART.
    \param un pointeur sur le premier char de la string

	La copie s'arrête au premier \0. Ce dernier n'est pas copié. Si la string est plus
	longue que l'espace qui est libre dans le buffer, la fonction va patiemment attendre
	que de l'espace se libère. Dans cette situation, cette fonction peut être très longue
	à retourner, ce qui pourrait briser des timings critiques dans le code.
*/
void uart0_put_string(const char* string);

/**
    \brief Envoie la représentation en caractères ASCII d'un float
    \param f La variable à convertir en caractères et a envoyer
	\param min_int_digit le nombre minimal de chiffres à envoyer avant la virgule
	\param nb_frac_digit le nombre de chiffres à envoyer après la virgule
	\param show_positive_sign si == true, le signe + précède les nombres positifs
*/
void uart0_put_float(float f,  uint8_t min_int_digit, uint8_t nb_frac_digit, bool show_positive_sign);

/**
    \brief Envoie un float dans le bon format pour être affiché par un serial plotter
	\param label Une string courte décrivant la variable
    \param f La variable
	\param nb_frac_digit le nombre de chiffres à envoyer après la virgule
*/
void uart0_plot_float(const char* label, float f,  uint8_t nb_frac_digit);

/**
    \brief Retire un byte au rolling buffer reçu par le UART.
    \return le byte reçu

    Si le buffer est vide cette fonction retourne '\0'.  Si le caractère à retourner
    est '\0' la fonction retourne aussi cette valeur.  Pour faire la différence entre
    les deux il faut préalablement vérifier si le buffer est vide avec la fonction
    uart_is_rx_buffer_empty()
*/
uint8_t uart0_get_byte(void);

/**
    \brief Retourne une ligne du buffer: tous les caractères du buffer de réception jusqu'au premier séparateur UART_LINE_SEPARATOR
	\return le nb de caractères de la ligne lue (excluant le caractère \0 final)
	
    Pour que cette fonction puisse fonctionner le buffer qui lui est passé doit minimalement
	avoir deux bytes de long. Un byte pour retourner au moins un caractère et un autre pour
	le \0. Si le buffer est plus petit que ça, cette fonction va corrompre la mémoire sans
	prévenir.

	La fonction termine la string avec un \0 et	retourne.
	S'il n'y a pas de ligne complète dans le buffer, la fonction renvoie un string vide (\0). 
	Comme le UART est relativement très lent par rapport au processeur, il faut
	attendre un peu avant d'appeler cette fonction.

	Si le buffer est vide au moment d'appeler cette fonction, celle-ci retourne une string
	vide terminée par \0

	Si le uart est utilisé pour communiquer autre chose que des strings, par exemple
	des données binaires, il se peut très bien que la valeur 0 soit présente dans le buffer.
	Dans ce cas il est impossible de faire la distinction entre une valeur 0 et un \0.

	Si le buffer contient plus de bytes que buffer_length - 1, la fonction va copier
	buffer - 1 bytes et utiliser le dernier byte pour le \0. Par conséquent, le buffer de
	réception ne sera totalement vidé. Pour savoir si cette situation s'est produite, un
	appel à la fonction uart_is_rx_buffer_empty() retournera false.

	Pour garantir que cette situation ne se produira jamais, il suffit de passer un
	buffer plus gros que UART_RX_BUFFER_SIZE
*/
int uart0_get_line(char* out_buffer, uint8_t buffer_length);

/**
    \brief Vide le buffer de réception

	Dans cette situation les bytes reçus sont juste effacés et perdus à jamais.
*/
void uart0_clean_rx_buffer(void);

/**
    \brief Attend que tous les caractères dans le buffer soient envoyés. Cette
	fonction bloque l'éxcécution du code.
*/
void uart0_flush(void);

/**
    \brief Indique si le buffer de réception est vide.
    \return true si il est vide, false s'il contient 1 byte ou plus
*/
bool uart0_is_rx_buffer_empty(void);

/**
    \brief Indique si le buffer de transmission est vide.
    \return true si il est vide, false s'il contient 1 byte ou plus
*/
bool uart0_is_tx_buffer_empty(void);

/**
    \brief Indique le nombre de lignes dans le buffer de réception.
    \return le nombre de ligne dans le buffer
*/
uint8_t uart0_rx_buffer_nb_line(void);

/**
    \brief Indique si des données ont été perdues à cause d'un débordement du buffer de réception
    \return si == true, des données ont été perdues
*/
bool uart0_get_rx_overflow_flag(void);

/**
    \brief Remet à zéro le flag de détection de débordement
*/
void uart0_reset_rx_overflow_flag(void);

#endif	// UART_ENABLE_UART_0



// ----------------------------------------------------------------------------
// UART1
//-----------------------------------------------------------------------------

#ifdef UART_ENABLE_UART_1

// Constantes _________________________________________________________________

#define UART_1_RX_BUFFER_SIZE 16
#define UART_1_TX_BUFFER_SIZE 16


// Définition de fonctions publiques __________________________________________

/**
    \brief Fait l'initialisation du UART
	\return rien
*/
void uart1_init(void);

/**
    \brief Définit le baudrate du UART
	\param baudrate La valeur du baudrate
*/
void uart1_set_baudrate(uart_baudrate_e baudrate);

/**
    \brief Ajoute un byte au rolling buffer à envoyer par le UART
    \param byte le byte à ajouter qui sera éventuellement envoyé
*/
void uart1_put_byte(uint8_t byte);

/**
    \brief Ajoute la string (par copie) au rolling buffer à envoyer par le UART.
    \param un pointeur sur le premier char de la string

	La copie s'arrête au premier \0. Ce dernier n'est pas copié. Si la string est plus
	longue que l'espace qui est libre dans le buffer, la fonction va patiemment attendre
	que de l'espace se libère. Dans cette situation, cette fonction peut être très longue
	à retourner, ce qui pourrait briser des timings critiques dans le code.
*/
void uart1_put_string(const char* string);

/**
    \brief Envoie la représentation en caractères ASCII d'un float
    \param f La variable à convertir en caractères et a envoyer
	\param min_int_digit le nombre minimal de chiffres à envoyer avant la virgule
	\param nb_frac_digit le nombre de chiffres à envoyer après la virgule
	\param show_positive_sign si == true, le signe + précède les nombres positifs
*/
void uart1_put_float(float f,  uint8_t min_int_digit, uint8_t nb_frac_digit, bool show_positive_sign);

/**
    \brief Envoie un float dans le bon format pour être affiché par un serial plotter
	\param label Une string courte décrivant la variable
    \param f La variable
	\param nb_frac_digit le nombre de chiffres à envoyer après la virgule
*/
void uart1_plot_float(const char* label, float f,  uint8_t nb_frac_digit);

/**
    \brief Retire un byte au rolling buffer reçu par le UART.
    \return le byte reçu

    Si le buffer est vide cette fonction retourne '\0'.  Si le caractère à retourner
    est '\0' la fonction retourne aussi cette valeur.  Pour faire la différence entre
    les deux il faut préalablement vérifier si le buffer est vide avec la fonction
    uart_is_rx_buffer_empty()
*/
uint8_t uart1_get_byte(void);

/**
    \brief Retourne une ligne du buffer: tous les caractères du buffer de réception jusqu'au premier séparateur UART_LINE_SEPARATOR
	\return le nb de caractères de la ligne lue (excluant le caractère \0 final)
	
    Pour que cette fonction puisse fonctionner le buffer qui lui est passé doit minimalement
	avoir deux bytes de long. Un byte pour retourner au moins un caractère et un autre pour
	le \0. Si le buffer est plus petit que ça, cette fonction va corrompre la mémoire sans
	prévenir.

	La fonction termine la string avec un \0 et	retourne.
	S'il n'y a pas de ligne complète dans le buffer, la fonction renvoie un string vide (\0). 
	Comme le UART est relativement très lent par rapport au processeur, il faut
	attendre un peu avant d'appeler cette fonction.

	Si le buffer est vide au moment d'appeler cette fonction, celle-ci retourne une string
	vide terminée par \0

	Si le uart est utilisé pour communiquer autre chose que des strings, par exemple
	des données binaires, il se peut très bien que la valeur 0 soit présente dans le buffer.
	Dans ce cas il est impossible de faire la distinction entre une valeur 0 et un \0.

	Si le buffer contient plus de bytes que buffer_length - 1, la fonction va copier
	buffer - 1 bytes et utiliser le dernier byte pour le \0. Par conséquent, le buffer de
	réception ne sera totalement vidé. Pour savoir si cette situation s'est produite, un
	appel à la fonction uart_is_rx_buffer_empty() retournera false.

	Pour garantir que cette situation ne se produira jamais, il suffit de passer un
	buffer plus gros que UART_RX_BUFFER_SIZE
*/
int uart1_get_line(char* out_buffer, uint8_t buffer_length);

/**
    \brief Vide le buffer de réception

	Dans cette situation les bytes reçus sont juste effacés et perdus à jamais.
*/
void uart1_clean_rx_buffer(void);

/**
    \brief Attend que tous les caractères dans le buffer soient envoyés. Cette
	fonction bloque l'éxcécution du code.
*/
void uart1_flush(void);

/**
    \brief Indique si le buffer de réception est vide.
    \return true si il est vide, false s'il contient 1 byte ou plus
*/
bool uart1_is_rx_buffer_empty(void);

/**
    \brief Indique si le buffer de transmission est vide.
    \return true si il est vide, false s'il contient 1 byte ou plus
*/
bool uart1_is_tx_buffer_empty(void);

/**
    \brief Indique le nombre de lignes dans le buffer de réception.
    \return le nombre de ligne dans le buffer
*/
uint8_t uart1_rx_buffer_nb_line(void);

/**
    \brief Indique si des données ont été perdues à cause d'un débordement du buffer de réception
    \return si == true, des données ont été perdues
*/
bool uart1_get_rx_overflow_flag(void);

/**
    \brief Remet à zéro le flag de détection de débordement
*/
void uart1_reset_rx_overflow_flag(void);

#endif // UART_ENABLE_UART_1

#endif // UART_H_INCLUDED
