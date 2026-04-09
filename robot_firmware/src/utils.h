#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

// Inclusions _________________________________________________________________

#include <stdint.h>
#include <stdbool.h>


// Macros _____________________________________________________________________

/**
    \brief Pour mettre à 1 un seul bit à l'intérieur d'un byte
	\param[in]  byte    le byte à modifier
    \param[in]  bit     le numéro du bit à modifier à partir de LSB = 0
    \return le byte modifié

    \code
    uint8_t mon_byte= 0b00001111;
    mon_byte = set_bit(mon_byte, 6);
    \endcode

    Produit le résultat suivant :

        mon_byte = 0b01001111
*/
#define set_bit(byte, bit)              ((byte) | (1 << (bit)))


/**
    \brief Pour mettre à 0 un seul bit à l'intérieur d'un byte
	\param[in]  byte    le byte à modifier
    \param[in]  bit     le numéro du bit à modifier à partir de LSB = 0
    \return le byte modifié

    \code
    uint8_t mon_byte= 0b00001111;
    mon_byte = clear_bit(mon_byte, 1);
    \endcode

    Produit le résultat suivant :

        mon_byte = 0b00001101
*/
 #define clear_bit(byte, bit)            ((byte) & ~(1 << (bit)))


/**
    \brief Pour toggler un seul bit à l'intérieur d'un byte
	\param[in]  byte    le byte à modifier
    \param[in]  bit     le numéro du bit à modifier à partir de LSB = 0
    \return le byte modifié

    \code
    uint8_t mon_byte= 0b00001111;
    mon_byte = toggle_bit(mon_byte, 1);
    \endcode

    Produit le résultat suivant :

        mon_byte = 0b00001101
*/
#define toggle_bit(byte, bit)           ((byte) ^ (1 << (bit)))


/**
    \brief Pour mettre à une certaine valeur un seul bit à l'intérieur d'un byte
	\param[in]  byte    le byte à modifier
    \param[in]  bit     le numéro du bit à modifier à partir de LSB = 0
    \param[in]  value   la valeur à appliquer (ne peut être que 0 ou 1)
    \return le byte modifié

    \code
    uint8_t mon_byte= 0b00001111;
    mon_byte = write_bit(mon_byte, 1, 0);
    \endcode

    Produit le résultat suivant :

        mon_byte = 0b00001101
*/
#define write_bit(byte, bit, value)     (((byte) & ~(1 << (bit))) | ((value) << (bit)))


/**
    \brief Pour lire un seul bit à l'intérieur d'un byte
	\param[in]  byte    le byte à lire
    \param[in]  bit     le numéro du bit à lire à partir de LSB = 0
    \return le bit lu (ne peut être que 0 ou 1 )

    \code
    uint8_t mon_byte= 0b00001111;
    uint8_t val_retour;
    val_retour = read_bit(mon_byte, 1);
    \endcode

    Produit le résultat suivant :

        val_retour = 1;
*/
#define read_bit(byte, bit)             (((byte) >> (bit)) & 1)

/**
    \brief Pour mettre à 1 plusieurs bits à l'intérieur d'un byte
	\param[in]  byte    le byte à modifier
    \param[in]  mask    le masque des bits qui seront mis à 1
    \return le byte modifié

    \code
    uint8_t mon_byte= 0b00001111;
    mon_byte = set_bits(mon_byte, 0b00111100);
    \endcode

    Produit le résultat suivant :

        mon_byte = 0b00111111;
*/
#define set_bits(byte, mask)            ((byte) | (mask))


/**
    \brief Pour mettre à 0 plusieurs bits à l'intérieur d'un byte
	\param[in]  byte    le byte à modifier
    \param[in]  mask    le masque des bits qui seront mis à 0
    \return le byte modifié

    \code
    uint8_t mon_byte= 0b00001111;
    mon_byte = clear_bits(mon_byte, 0b00111100);
    \endcode

    Produit le résultat suivant :

        mon_byte = 0b00000011;
*/
#define clear_bits(byte, mask)          ((byte) & ~(mask))


/**
    \brief Pour toggler plusieurs bits à l'intérieur d'un byte
    \param[in]  byte    le byte à modifier
    \param[in]  mask    le masque des bits qui seront basculés
    \return le byte modifié

    \code
    uint8_t mon_byte= 0b00001111;
    mon_byte = toggle_bits(mon_byte, 0b00111100);
    \endcode

    Produit le résultat suivant :

        mon_byte = 0b00110011;
*/
#define toggle_bits(byte, mask)         ((byte) ^ (mask))


/**
    \brief Pour mettre à une certaine valeur plusieurs bits à l'intérieur d'un byte
    \param[in]  byte    le byte à modifier
    \param[in]  mask    le masque des bits qui seront modifiées
    \param[in]  value   la valeur à appliquer
    \return le byte modifié

    \code
    uint8_t mon_byte= 0b00001111;
    mon_byte = write_bits(mon_byte, 0b00111100, 0b01010101);
    \endcode

    Produit le résultat suivant :

        mon_byte = 0b00010111;
*/
#define write_bits(byte, mask, value)   (((byte) & ~(mask)) | ((value) & (mask)))


/**
    \brief Pour lire la valeur de plusieurs bits à l'intérieur d'un byte avec les bits
    non intéressants masqués à 0
	\param[in]  byte    le byte à lire
    \param[in]  mask    le masque des bits qui seront réellement lus
    \return la valeur masquée

    \code
    uint8_t mon_byte= 0b00001111;
    uint8_t val_retour;
    val_retour = read_bits(mon_byte, 0b00111100);
    \endcode

    Produit le résultat suivant :

        val_retour = 0b00001100;
*/
#define read_bits(byte, mask)           ((byte) & (mask))


// Déclaration de fonctions publiques _________________________________________

// ----------------------------------------------------------------------------
// Conversion texte vers nombre
// ----------------------------------------------------------------------------

/**
    \brief Converti un caractère représentant un chiffre en sa valeur
    \param[in]  character   Le caractère en question
    \return La valeur ou zéro si le caractère n'avait pas rapport
*/
uint8_t char_to_uint(char character);


/**
    \brief Converti une string représentant un nombre en sa valeur
    \param[in]  string  La string en question
    \return La valeur ou de la cochonnerie si la string n'avait pas rapport
*/
uint32_t string_to_uint(const char* string);


/**
    \brief Converti un array de char représentant un nombre en sa valeur
    \param[in]  char_array  Le array en question
    \param[in]  size    La grosseur intéressante du array puisque celui n'est pas
    obligé de se terminer par le caractère nul
    \return La valeur ou de la cochonnerie si le array n'avait pas rapport
*/
uint32_t char_array_to_uint(const char* char_array, uint8_t size);


/**
    \brief Converti un caractère représentant un chiffre hexadécimal en sa valeur
    \param[in]  character   Le caractère en question
    \return La valeur ou zéro si le caractère n'avait pas rapport
*/
uint8_t hex_char_to_uint(char character);


/**
    \brief Converti une string représentant un nombre hexadécimal en sa valeur
    \param[in]  string  La string en question
    \return La valeur ou de la cochonnerie si la string n'avait pas rapport
*/
uint32_t hex_string_to_uint(const char* string);


/**
    \brief Converti un array de char représentant un nombre hexadécimal en sa valeur
    \param[in]  char_array  Le array en question
    \param[in]  size    La grosseur intéressante du array puisque celui n'est pas
    obligé de se terminer par la caractère nul
    \return La valeur ou de la cochonnerie si le array n'avait pas rapport
*/
uint32_t hex_char_array_to_uint(const char* char_array, uint8_t size);


// ----------------------------------------------------------------------------
// Conversion nombre vers texte
// ----------------------------------------------------------------------------

/**
    \brief Convertit un nombre en le caractère ASCII qui le représente
    \param[in]  digit   Le nombre entre 0 et 9
    \return Le caractère ASCII qui représente le paramètre
    \warning C'est une erreur d'appeler cette fonction avec un paramètre qui est négatif
    ou plus grand que 9
*/
char uint_to_char(uint8_t digit);

/**
    \brief Convertit un nombre hexadécimal en le caractère ASCII qui le représente
    \param[in]  hex_digit   Le nombre entre 0 et 15
    \return Le caractère ASCII qui représente le paramètre
    \warning C'est une erreur d'appeler cette fonction avec un paramètre qui est négatif
    ou plus grand que 15

    Pour les nombre entre 10 et 15, la sortie est une lettre majuscule entre A et F
*/
char uint_to_hex_char(uint8_t hex_digit);

/**
    \brief Converti un entier non signé de 8 bits en une string
    \param[out] out_string  La string de destination
    \param[in]  number      Le nombre à convertir
    \return     Le nombre de caractères ajoutés à la string sans compter le '\0'
    \warning    La string doit être assez longue pour contenir la conversion. Dans le
    cas d'un 8 bits, ça prend une string qui fait minimalement 4 caractères de long.

    La string de sortie a toujours 3 caractères et ce peu importe la longueur du nombre.
    Par conséquent, si le nombre est plus petit que 100, le résultat sera "paddé" de 0

    Le retour de la fonction permet de faire des appels successifs et de progressivement
    construire une string complexe

    \code

    char string[16];
    uint8_t string_index;
    string_index = uint8_to_string(string, 42);

    string[string_index] = '!';
    string[string_index + 1] = '\0';

    \endcode

    produira la string suivante :

        042!

*/
uint8_t uint8_to_string(char* out_string, uint8_t number);

/**
    \brief Converti un entier non signé de 16 bits en une string
    \param[out] out_string  La string de destination
    \param[in]  number      Le nombre à convertir
    \return     Le nombre de caractères ajoutés à la string sans compter le '\0'
    \warning    La string doit être assez longue pour contenir la conversion. Dans le
    cas d'un 16 bits, ça prend une string qui fait minimalement 6 caractères de long.

    La string de sortie a toujours 5 caractères et ce peu importe la longueur du nombre.
    Par conséquent, si le nombre est plus petit que 10 000, le résultat sera "paddé" de 0

    Le retour de la fonction permet de faire des appels successifs et de progressivement
    construire une string complexe

    \code

    char string[16];
    uint8_t string_index;
    string_index = uint16_to_string(string, 42);

    string[string_index] = '!';
    string[string_index + 1] = '\0';

    \endcode

    produira la string suivante :

        00042!

*/
uint8_t uint16_to_string(char* out_string, uint16_t number);

/**
    \brief Converti un entier non signé de 32 bits en une string
    \param[out] out_string  La string de destination
    \param[in]  number      Le nombre à convertir
    \return     Le nombre de caractères ajoutés à la string sans compter le '\0'
    \warning    La string doit être assez longue pour contenir la conversion. Dans le
    cas d'un 32 bits, ça prend une string qui fait minimalement 11 caractères de long.

    La string de sortie a toujours 10 caractères et ce peu importe la longueur du nombre.
    Par conséquent, si le nombre est plus petit que 1 000 000 000, le résultat sera "paddé" de 0

    Le retour de la fonction permet de faire des appels successifs et de progressivement
    construire une string complexe

    \code

    char string[16];
    uint8_t string_index;
    string_index = uint32_to_string(string, 42);

    string[string_index] = '!';
    string[string_index + 1] = '\0';

    \endcode

    produira la string suivante :

        0000000042!

*/
uint8_t uint32_to_string(char* out_string, uint32_t number);

/**
    \brief Converti un entier hexadécimal de 8 bits en une string
    \param[out] out_string  La string de destination
    \param[in]  number      Le nombre à convertir
    \return     Le nombre de caractères ajoutés à la string sans compter le '\0'
    \warning    La string doit être assez longue pour contenir la conversion. Dans le
    cas d'un 8 bits, ça prend une string qui fait minimalement 3 caractères de long.

    Le retour de la fonction permet de faire des appels successifs et de progressivement
    construire une string complexe

    \code

    char string[16];
    uint8_t string_index;
    string_index = uint8_to_hex_string(string, 0x2A);

    string[string_index] = '!';
    string[string_index + 1] = '\0';

    \endcode

    produira la string suivante :

        2A!

*/
uint8_t uint8_to_hex_string(char* out_string, uint8_t number);

/**
    \brief Converti un entier hexadécimal de 16 bits en une string
    \param[out] out_string  La string de destination
    \param[in]  number      Le nombre à convertir
    \return     Le nombre de caractères ajoutés à la string sans compter le '\0'
    \warning    La string doit être assez longue pour contenir la conversion. Dans le
    cas d'un 16 bits, ça prend une string qui fait minimalement 5 caractères de long.

    Le retour de la fonction permet de faire des appels successifs et de progressivement
    construire une string complexe

    \code

    char string[16];
    uint8_t string_index;
    string_index = uint16_to_hex_string(string, 0x002A);

    string[string_index] = '!';
    string[string_index + 1] = '\0';

    \endcode

    produira la string suivante :

        002A!

*/
uint8_t uint16_to_hex_string(char* out_string, uint16_t number);

/**
    \brief Converti un entier hexadécimal de 32 bits en une string
    \param[out] out_string  La string de destination
    \param[in]  number      Le nombre à convertir
    \return     Le nombre de caractères ajoutés à la string sans compter le '\0'
    \warning    La string doit être assez longue pour contenir la conversion. Dans le
    cas d'un 16 bits, ça prend une string qui fait minimalement 9 caractères de long.

    Le retour de la fonction permet de faire des appels successifs et de progressivement
    construire une string complexe

    \code

    char string[16];
    uint8_t string_index;
    string_index = uint32_to_hex_string(string, 0x0000002A);

    string[string_index] = '!';
    string[string_index + 1] = '\0';

    \endcode

    produira la string suivante :

        0000002A!

*/
uint8_t uint32_to_hex_string(char* out_string, uint32_t number);

/**
    \brief Converti un entier signé de 8 bits en une string
    \param[out] out_string  La string de destination
    \param[in]  number      Le nombre à convertir
    \return     Le nombre de caractères ajoutés à la string sans compter le '\0'
    \warning    La string doit être assez longue pour contenir la conversion. Dans le
    cas d'un 8 bits, ça prend une string qui fait minimalement 4 caractères de long.

    La string de sortie aura le signe comme préfixe suivi de 3 caractères représentant
    la valeur absolue du nombre. Le préfixe peut être '+' ou '-'. Si le nombre est plus
    petit que 100 et plus grand que -100, le résultat sera "paddé" de 0

    Le retour de la fonction permet de faire des appels successifs et de progressivement
    construire une string complexe

    \code

    char string[16];
    uint8_t string_index;
    string_index = int8_to_string(string, -42);

    string[string_index] = '!';
    string[string_index + 1] = '\0';

    \endcode

    produira la string suivante :

        -042!

*/
uint8_t int8_to_string(char* out_string, int8_t number);

/**
    \brief Converti un entier signé de 16 bits en une string
    \param[out] out_string  La string de destination
    \param[in]  number      Le nombre à convertir
    \return     Le nombre de caractères ajoutés à la string sans compter le '\0'
    \warning    La string doit être assez longue pour contenir la conversion. Dans le
    cas d'un 16 bits, ça prend une string qui fait minimalement 7 caractères de long.

    La string de sortie aura le signe comme préfixe suivi de 3 caractères représentant
    la valeur absolue du nombre. Le préfixe peut être '+' ou '-'. Si le nombre est plus
    petit que 10000 et plus grand que -10000, le résultat sera "paddé" de 0

    Le retour de la fonction permet de faire des appels successifs et de progressivement
    construire une string complexe

    \code

    char string[16];
    uint8_t string_index;
    string_index = int16_to_string(string, -42);

    string[string_index] = '!';
    string[string_index + 1] = '\0';

    \endcode

    produira la string suivante :

        -00042!

*/
uint8_t int16_to_string(char* out_string, int16_t number);

/**
    \brief Converti un entier signé de 32 bits en une string
    \param[out] out_string  La string de destination
    \param[in]  number      Le nombre à convertir
    \return     Le nombre de caractères ajoutés à la string sans compter le '\0'
    \warning    La string doit être assez longue pour contenir la conversion. Dans le
    cas d'un 32 bits, ça prend une string qui fait minimalement 12 caractères de long.

    La string de sortie aura le signe comme préfixe suivi de 3 caractères représentant
    la valeur absolue du nombre. Le préfixe peut être '+' ou '-'. Si le nombre est plus
    petit que 1 000 000 000 et plus grand que -1 000 000 000, le résultat sera "paddé" de 0

    Le retour de la fonction permet de faire des appels successifs et de progressivement
    construire une string complexe

    \code

    char string[16];
    uint8_t string_index;
    string_index = int32_to_string(string, -42);

    string[string_index] = '!';
    string[string_index + 1] = '\0';

    \endcode

    produira la string suivante :

        -0000000042!

*/
uint8_t int32_to_string(char* out_string, int32_t number);

/**
	TODO
*/
uint8_t float_to_string(char* buffer, uint8_t buffer_length, float f);

/**
	TODO
*/
uint8_t float_to_string_format(char* buffer, uint8_t buffer_length, float f, int8_t min_int_digit, int8_t nb_frac_digit, bool show_positive_sign);

/**
	TODO
*/
void uint16_to_char_function(uint16_t i, int8_t min_int_digit, void (*char_function)(char));

/**
	TODO
*/
void int16_to_char_function(int16_t i, int8_t min_int_digit, void (*char_function)(char));


/**
	TODO
*/
void float_to_char_function(float f, int8_t min_int_digit, int8_t nb_frac_digit, bool show_positive_sign, void (*char_function)(char));


// ----------------------------------------------------------------------------
// Traitement de signal
// ----------------------------------------------------------------------------

bool detect_rising_edge(bool current_state);

bool detect_falling_edge(bool current_state);

float limit_float(float value, float limit);

#endif // UTILS_H_INCLUDED
