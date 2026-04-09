
/******************************************************************************
Includes
******************************************************************************/

//#define DISABLE_UTILS_H_MACRO /* Obligatoire ici */
#include "utils.h"

#include <stdlib.h>	// Pour abs()


// Déclaration de fonctions privées ___________________________________________

static void string_builder_set_buffer(char* buffer, uint8_t buffer_length);

static void string_builder_append_char(char c);

static uint8_t string_builder_terminate(void);


// Définition de fonctions publiques __________________________________________

/** Conversion text to number ************************************************/
uint8_t char_to_uint(char caracter){

    uint8_t digit = 0;

    if((caracter >= '0') && (caracter <= '9')){

        digit = caracter - '0';
    }

    return digit;
}


uint32_t string_to_uint(const char* string){

    uint8_t string_index = 0;
    uint32_t power_of_ten = 1;
    uint32_t converted_uint = 0;

	while(string[string_index] != '\0'){
		string_index++;
	}

    while(string_index > 0){

        converted_uint += (char_to_uint(string[string_index - 1])) * power_of_ten;
        string_index--;
        power_of_ten = power_of_ten * 10;
    }

    return converted_uint;
}

uint32_t char_array_to_uint(const char* char_array, uint8_t size){

    uint32_t power_of_ten = 1;
    uint32_t converted_uint = 0;

    while(size > 0){

        converted_uint += (char_to_uint(char_array[size - 1])) * power_of_ten;
        size--;
        power_of_ten = power_of_ten * 10;
    }

    return converted_uint;

}

uint8_t hex_char_to_uint(char caracter){

    uint8_t nibble = 0;

    if((caracter >= '0') && (caracter <= '9')){

        nibble = caracter - '0';
    }

    else if((caracter >= 'a') && (caracter <= 'f')){

        nibble = caracter - 'a' + 0xA;
    }

    return nibble;
}


uint32_t hex_string_to_uint(const char* string){

    uint8_t string_index = 0;
    uint8_t nibble_count = 0;
    uint32_t converted_uint = 0;

    while(string[string_index] != '\0'){
	    string_index++;
    }

    while(string_index > 0){

        converted_uint |= (hex_char_to_uint(string[string_index - 1])) << (4 * nibble_count);
        string_index--;
        nibble_count++;
    }

    return converted_uint;
}

uint32_t hex_char_array_to_uint(const char* char_array, uint8_t size){

    uint8_t nibble_count = 0;
    uint32_t converted_uint = 0;

    while(size > 0){

        converted_uint |= (hex_char_to_uint(char_array[size - 1])) << (4 * nibble_count);
        size--;
        nibble_count++;
    }

    return converted_uint;
}


/** Conversion number to text ************************************************/

char uint_to_char(uint8_t digit){

    char caracter = '\0';

    if((digit >= 0) && (digit <= 9)){

        caracter = digit + '0';
    }

    return caracter;
}


char uint_to_hex_char(uint8_t hex_digit){

	char caracter = '\0';

	if((hex_digit >= 0x00) && (hex_digit <= 0x09)){

		caracter = hex_digit + '0';
	}
	
	else if((hex_digit >= 0x0A) && (hex_digit <= 0x0F)){
		
		caracter = hex_digit - 0x0A + 'A';
	}

	return caracter;
}


uint8_t uint8_to_string(char* out_string, uint8_t number){

    uint8_t anti_rest;  //après avoir fixé mon écran dans le vide pendant 15 minutes à chercher
    // un nom intelligent pour cette variable j'ai décidé de passer à l'action et de lui donner
    // le premier nom qui me passerait par la tête.
    uint8_t string_index = 0;
    uint8_t power_of_ten = 100;


    while(power_of_ten > 0){

        anti_rest = number / power_of_ten;

        out_string[string_index] = uint_to_char(anti_rest);

        number -= anti_rest * power_of_ten;

        string_index++;

        power_of_ten /= 10;
    }

    /* On ferme la string */
    out_string[string_index] = '\0';
	
	return string_index;
}


uint8_t uint16_to_string(char* out_string, uint16_t number){

    uint8_t anti_rest;  //après avoir fixé mon écran dans le vide pendant 15 minutes à chercher
    // un nom intelligent pour cette variable j'ai décidé de passer à l'action et de lui donner
    // le premier nom qui me passerait par la tête.
    uint8_t string_index = 0;
    uint16_t power_of_ten = 10000;


    while(power_of_ten > 0){

        anti_rest = number / power_of_ten;

        out_string[string_index] = uint_to_char(anti_rest);

        number -= anti_rest * power_of_ten;

        string_index++;

        power_of_ten /= 10;
    }

    /* On ferme la string */
    out_string[string_index] = '\0';
	
	return string_index;
}


uint8_t uint32_to_string(char* out_string, uint32_t number){

    uint8_t anti_rest;  //après avoir fixé mon écran dans le vide pendant 15 minutes à chercher
    // un nom intelligent pour cette variable j'ai décidé de passer à l'action et de lui donner
    // le premier nom qui me passerait par la tête.
    uint8_t string_index = 0;
    uint32_t power_of_ten = 1000000000;


    while(power_of_ten > 0){

        anti_rest = number / power_of_ten;

        out_string[string_index] = uint_to_char(anti_rest);

        number -= anti_rest * power_of_ten;

        string_index++;

        power_of_ten /= 10;
    }

    /* On ferme la string */
    out_string[string_index] = '\0';
	
	return string_index;
}


uint8_t uint8_to_hex_string(char* out_string, uint8_t number){
	 
	out_string[0] = uint_to_hex_char((number >> 4) & 0b00001111);
	out_string[1] = uint_to_hex_char(number & 0b00001111);
	out_string[2] = '\0';
	
	return 2;
}


uint8_t uint16_to_hex_string(char* out_string, uint16_t number){
	
	out_string[0] = uint_to_hex_char((uint8_t)((number >> 12) & 0b0000000000001111));
	out_string[1] = uint_to_hex_char((uint8_t)((number >> 8) & 0b0000000000001111));
	out_string[2] = uint_to_hex_char((uint8_t)((number >> 4) & 0b0000000000001111));
	out_string[3] = uint_to_hex_char((uint8_t)(number & 0b0000000000001111));
	out_string[4] = '\0';
		 
	return 4;
}


uint8_t uint32_to_hex_string(char* out_string, uint32_t number){
	
	out_string[0] = uint_to_hex_char((uint8_t)((number >> 28) & 0b00000000000000000000000000001111));
	out_string[1] = uint_to_hex_char((uint8_t)((number >> 24) & 0b00000000000000000000000000001111));
	out_string[2] = uint_to_hex_char((uint8_t)((number >> 20) & 0b00000000000000000000000000001111));
	out_string[3] = uint_to_hex_char((uint8_t)((number >> 16) & 0b00000000000000000000000000001111));
	out_string[4] = uint_to_hex_char((uint8_t)((number >> 12) & 0b00000000000000000000000000001111));
	out_string[5] = uint_to_hex_char((uint8_t)((number >> 8) & 0b00000000000000000000000000001111));
	out_string[6] = uint_to_hex_char((uint8_t)((number >> 4) & 0b00000000000000000000000000001111));
	out_string[7] = uint_to_hex_char((uint8_t)(number & 0b00000000000000000000000000001111));
	out_string[8] = '\0';
	
	return 8;
}


uint8_t int8_to_string(char* out_string, int8_t number){

    /* Détermination du signe */
    if(number < 0){

        out_string[0] = '-';
    }
    else{

        out_string[0] = '+';
    }

    /* Une fois que le signe est sorti, reste juste à convertir la valeur absolue du reste */
	// Le +1 c'est pour tenir compte du signe
    return uint8_to_string(&out_string[1], (uint8_t)abs(number)) + 1;
}


uint8_t int16_to_string(char* out_string, int16_t number){

    /* Détermination du signe */
    if(number < 0){

        out_string[0] = '-';
    }
    else{

        out_string[0] = '+';
    }

    /* Une fois que le signe est sorti, reste juste à convertir la valeur absolue du reste */
	// Le +1 c'est pour tenir compte du signe
    return uint16_to_string(&out_string[1], (uint16_t)abs(number)) + 1;

}


uint8_t int32_to_string(char* out_string, int32_t number){

    /* Détermination du signe */
    if(number < 0){

        out_string[0] = '-';
    }
    else{

        out_string[0] = '+';
    }

    /* Une fois que le signe est sorti, reste juste à convertir la valeur absolue du reste */
	// Le +1 c'est pour tenir compte du signe
    return uint32_to_string(&out_string[1], (uint32_t)abs(number)) + 1;

}


uint8_t float_to_string(char* buffer, uint8_t buffer_length, float f){
	
	const int DEFAULT_MIN_INT_DIGIT = 1;
	const int DEFAULT_NB_FRAC_DIGIT = 3;
	const bool DEFAULT_SHOW_POSITIVE_SIGN = true;
	return float_to_string_format(buffer, buffer_length, f, DEFAULT_MIN_INT_DIGIT, DEFAULT_NB_FRAC_DIGIT, DEFAULT_SHOW_POSITIVE_SIGN);
}


uint8_t float_to_string_format(char* buffer, uint8_t buffer_length, float f, int8_t min_int_digit, int8_t nb_frac_digit, bool show_positive_sign){
	
	string_builder_set_buffer(buffer, buffer_length);
	
	float_to_char_function(f, min_int_digit, nb_frac_digit, show_positive_sign, &string_builder_append_char);
	
	return string_builder_terminate();
}


void uint16_to_char_function(uint16_t u, int8_t min_int_digit, void (*char_function)(char)){
	
	uint16_t divider = 1;
	int8_t nb_int_digit = 1;
	
	while((min_int_digit > 1) || (u >= divider * 10)){
		nb_int_digit++;
		min_int_digit--;
		divider *= 10;
	}
	
	uint8_t digit;
	
	while(nb_int_digit > 0){
		
		digit = u / divider;
		
		(*char_function)(uint_to_char(digit));
		
		u -= digit * divider;
		divider /= 10;
		nb_int_digit--;
	}
}


void int16_to_char_function(int16_t i, int8_t min_int_digit, void (*char_function)(char)){
	
	if(i < 0) {
		(*char_function)('-');
		i = -i;
	}
	else{
		(*char_function)('+');
		//i = i; //Inutile
	}
	
	uint16_to_char_function(i, min_int_digit, char_function);
}


void float_to_char_function(float f, int8_t min_int_digit, int8_t nb_frac_digit, bool show_positive_sign, void (*char_function)(char)){
	
	if(f > 32767){	// La grosse valeur affichable par un 16 bit signé
		(*char_function)('+');
		(*char_function)('i');
		(*char_function)('n');
		(*char_function)('f');
		return;
	}
	
	if(f < -32768){	// La grosse valeur affichable par un 16 bit signé
		(*char_function)('-');
		(*char_function)('i');
		(*char_function)('n');
		(*char_function)('f');
		return;
	}
	
	if(f < 0) {
		(*char_function)('-');
		f = -f;
	}
	else if(show_positive_sign){
		(*char_function)('+');
		//f = f; //Inutile
	}
	
	uint16_t integral_part = (uint16_t)f; // On fait juste caster, ça nous débarasse de la partie décimale
	
	uint16_to_char_function(integral_part, min_int_digit, char_function);
	
	f -= (float)integral_part; // On soustrait la partie entière pour ne garder que la partie décimale
	
	int multiplier = 10;

	uint8_t digit;
	
	(*char_function)('.');
	
	while(nb_frac_digit > 0){
		
		digit = (uint8_t)(f * (float)multiplier);
		
		(*char_function)(uint_to_char(digit));
		
		f -= (float)digit / (float)multiplier;
		multiplier *= 10;
		nb_frac_digit--;
	}
}


// Traitement de signaux

float limit_float(float value, float limit){

	if(value > limit){
		value = limit;
	}
	else if (value < -limit){
		value = -limit;
	}
	return value;
}

// Définitions de fonctions privées ___________________________________________

static char* string_builder_buffer = NULL;
static uint8_t string_builder_buffer_length = 0;
static uint8_t string_builder_index = 0;

static void string_builder_set_buffer(char* buffer, uint8_t buffer_length){
	
	string_builder_buffer = buffer;
	string_builder_buffer_length = buffer_length;
	string_builder_index = 0;
}

static void string_builder_append_char(char c){
	
	string_builder_buffer[string_builder_index] = c;
	
	if(string_builder_index < string_builder_buffer_length - 2){	// -1 c'est l'index maximal, -2 c'est pour le \0
		string_builder_index++;
	}
}

static uint8_t string_builder_terminate(void){
	
	string_builder_buffer[string_builder_index] = '\0';
	
	return string_builder_index;
}


