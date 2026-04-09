
/******************************************************************************
Includes
******************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>	// Pour lire depuis la flash
#include <util/delay.h>

#include "twi.h"

/******************************************************************************
Defines
******************************************************************************/
// Constante globale privée ___________________________________________________

static const uint8_t bitrate_to_TWBR[] = {

	#if F_CPU == 8000000UL     // Fosc = 8.0000MHz
	72,		//TWI_BITRATE_50K   Slow mode
	32,		//TWI_BITRATE_100K  Standard mode
	12,		//TWI_BITRATE_200K  Medium mode
	2,		//TWI_BITRATE_400K  Fast mode

	#elif F_CPU == 16000000UL     /*Fosc = 16.0000MHz*/
	152,	//TWI_BITRATE_50K   Slow mode
	72,		//TWI_BITRATE_100K  Standard mode
	32,		//TWI_BITRATE_200K  Medium mode
	12,		//TWI_BITRATE_400K  Fast mode

	#elif F_CPU == 20000000UL   /*Fosc = 20.0000MHz*/
	192,	//TWI_BITRATE_50K   Slow mode
	92,		//TWI_BITRATE_100K  Standard mode
	42,		//TWI_BITRATE_200K  Medium mode
	17,		//TWI_BITRATE_400K  Fast mode

	#else
	#error "Vous avez oublie de definir F_CPU dans les symboles et twi.c en a obligatoirement besoin"
	#endif
};



#define TWCR_CONTROL_MASK       ((1 << TWINT) | (1 << TWEA) | (1 << TWSTA) | (1 << TWSTO))
#define TWSR_PRESCALER_MASK		((1 << TWPS1) | (1 << TWPS0))
#define TWSR_STATUS_MASK        0b11111000

#define TX_FLAG_SHIFT 0		// Position du bit dans le busy_flag
#define RX_FLAG_SHIFT 1		// Position du bit dans le busy_flag

typedef enum{
	
	PRESCALER_1 =  0b00,
	PRESCALER_4 =  0b01,
	PRESCALER_16 = 0b10,
	PRESCALER_64 = 0b11,
	
} twi_prescaler_e;

#define TWSR_PRESCALER_VALUE PRESCALER_1


/******************************************************************************
Macros
******************************************************************************/

/* start condition */
#define send_start_condition()  TWCR = write_bits(TWCR, TWCR_CONTROL_MASK, (1 << TWINT) | (0 << TWEA) | (1 << TWSTA) | (0 << TWSTO))

/* clear TWINT */
#define clear_twint()			TWCR = write_bits(TWCR, TWCR_CONTROL_MASK, (1 << TWINT) | (0 << TWEA) | (0 << TWSTA) | (0 << TWSTO))

/* stop condition */
#define send_stop_condition()   TWCR = write_bits(TWCR, TWCR_CONTROL_MASK, (1 << TWINT) | (0 << TWEA) | (0 << TWSTA) | (1 << TWSTO))


// Déclaration de variables globales privées ___________________________________

static twi_error_e last_error = TWI_ERROR_NONE;


// Déclaration de fonctions locales privées ____________________________________

static bool wait_for_twint(void);

static bool send_start_address_register(uint8_t address, uint8_t start_register);

static void reset_twi(void);

/******************************************************************************
Global functions
******************************************************************************/

void twi_init(void){
	
	// Désactiver la réducation de power dans le cas où elle était activée
	PRR0 = clear_bit(PRR0, PRTWI);	// Un 0 désactive

    twi_set_bitrate(TWI_DEFAULT_BITRATE);

    // Choix du prescaler
    TWSR = write_bits(TWSR, TWSR_PRESCALER_MASK, TWSR_PRESCALER_VALUE);

    TWCR = ((1 << TWEN) |   // TWI Enable Bit : that's the whole point
            (0 << TWIE));   // TWI Interrupt Enable : pas en polling
	
	twi_set_pullups(false);
}

void twi_set_pullups(bool enable_pullups){
	
	if(enable_pullups){
		PORTC = set_bits(PORTC, (1 << PORTC1) | (1 << PORTC0));
	}
	else{
		PORTC = clear_bits(PORTC, (1 << PORTC1) | (1 << PORTC0));
	}
}

void twi_set_bitrate(twi_bitrate_e bitrate){
	TWBR = bitrate_to_TWBR[bitrate];
}


bool twi_read_register_blocking(uint8_t address, uint8_t start_register, uint8_t* buffer, uint16_t nb_register){

	// On envoi le start condition, l'adresse et le registre
	if(!send_start_address_register(address, start_register)){
		return false;
	}
	
	// On envoie un repeated start
	send_start_condition();	
	
	if(!wait_for_twint()){
		return false;
	}
	
	// Si le status n'est pas "A repeated START condition has been transmitted", ça ne va pas bien
	if((TWSR & TWSR_STATUS_MASK) != 0x10){
		last_error = TWI_ERROR_WRONG_STATUS_REPEATED_START;
		return false;
	}
	
	// On réenvoie l'adresse, mais + read cette fois-ci (SLA + R)
	TWDR = (address << 1) | 0b00000001;	// Un 1 au LSB pour read
	
	// On repart la machine
	clear_twint();
	
	if(!wait_for_twint()){
		return false;
	}
	
	// Si le status n'est pas "SLA+R has been transmitted; ACK has been received", ça ne va pas bien
	if((TWSR & TWSR_STATUS_MASK) != 0x40){
		last_error = TWI_ERROR_WRONG_STATUS_SLA_R;
		return false;
	}	

	if(nb_register > 1){ // Si il va y avoir d'autre données après la prochaine
		// on relance la machine et on demande d'ACK le tout
		TWCR = (1 <<  TWINT) | (1 << TWEA) | (0 << TWSTA) | (0 << TWSTO) | (1 << TWEN);
	}		
	else{	// Si on est rendu à la dernière donnée à aller chercher
		// on relance la machine et on demande d'NACK dans ce cas-ci
		TWCR = (1 <<  TWINT) | (0 << TWEA) | (0 << TWSTA) | (0 << TWSTO) | (1 << TWEN);
	}
	
	bool repeat_receive_data = true;
	
	while(repeat_receive_data){
	
		if(!wait_for_twint()){
			return false;
		}
		
		*buffer = TWDR;	// On va chercher le byte reçu	
		
		// Si le status est "Data byte has been received; ACK has been returned	"
		if((TWSR & TWSR_STATUS_MASK) == 0x50){
			
			buffer++;	// On incrémente le pointeur pour la prochaine donnée
			nb_register--;	// On réduit le nombre de données à recevoir
			
			if(nb_register > 1){ // Si il va y avoir d'autre données après la prochaine
				// on relance la machine et on demande d'ACK le tout
				TWCR = (1 <<  TWINT) | (1 << TWEA) | (0 << TWSTA) | (0 << TWSTO) | (1 << TWEN);
			}
			else{	// Si on est rendu à la dernière donnée à aller chercher
				// on relance la machine et on demande d'NACK dans ce cas-ci
				TWCR = (1 <<  TWINT) | (0 << TWEA) | (0 << TWSTA) | (0 << TWSTO) | (1 << TWEN);
			}
		}
		
		// Sinon si le status est "Data byte has been received; ACK has been returned	"
		else if((TWSR & TWSR_STATUS_MASK) == 0x58){	// Data byte has been received; NOT ACK has been returned (Théoriquement on ne devrait rentrer ici seulement si nb_register == 0)
			
			// On envoie un stop condition
			send_stop_condition();
			repeat_receive_data = false;	// On veut sortir du while
		}
		else{
			last_error = TWI_ERROR_WRONG_STATUS_DATA_R;
			return false;
		}		
	}	
	
	return true;
}

bool twi_write_register_blocking(uint8_t address, uint8_t start_register, const uint8_t* buffer, uint16_t nb_register){

	// On envoi le start condition, l'adresse et le registre
	if(!send_start_address_register(address, start_register)){
		return false;
	}
	
	while(nb_register > 0){
		
		TWDR = *buffer;
			
		buffer++;	// On incrémente le pointeur pour la prochaine donnée
		nb_register--;	// On réduit le nombre de données à recevoir
			
		clear_twint(); // on relance la machine
		
		if(!wait_for_twint()){
			return false;
		}
		
		// Si le status n'est pas "Data byte has been transmitted; ACK has been received", ça ne va pas bien
		if((TWSR & TWSR_STATUS_MASK) != 0x28){
			last_error = TWI_ERROR_WRONG_STATUS_DATA_W;
			return false;
		}
	}
	
	send_stop_condition();
	
	return true;
}

bool twi_write_register_blocking_progmem(uint8_t address, uint8_t start_register, const uint8_t* buffer, uint16_t nb_register){

	// On envoi le start condition, l'adresse et le registre
	if(!send_start_address_register(address, start_register)){
		return false;
	}
	
	while(nb_register > 0){
		
		TWDR = pgm_read_byte(buffer);
		
		buffer++;	// On incrémente le pointeur pour la prochaine donnée
		nb_register--;	// On réduit le nombre de données à recevoir
		
		clear_twint(); // on relance la machine
		
		if(!wait_for_twint()){
			return false;
		}
		
		// Si le status n'est pas "Data byte has been transmitted; ACK has been received", ça ne va pas bien
		if((TWSR & TWSR_STATUS_MASK) != 0x28){
			last_error = TWI_ERROR_WRONG_STATUS_DATA_W;
			return false;
		}
	}
	
	send_stop_condition();
	
	return true;
}

int twi_get_last_error(){
	return last_error;
}


// Définition de fonctions locales privées ____________________________________

static bool wait_for_twint(void){	
	
	uint8_t max_delay_us = 100;	// 100 us c'est déjà long
	
	// Tant que TWINT vaut 0
	while(!read_bit(TWCR, TWINT)){
		
		if(max_delay_us == 0){

			reset_twi();	// Comme aurait dit mon grand-père, ça va mal à la shop
			last_error = TWI_ERROR_TIMEOUT;
			return false;
		}
		
		max_delay_us--;
		_delay_us(1);
	}
	
	// Selon la valeur dans le status register
	switch (TWSR & TWSR_STATUS_MASK){
		
	case 0x00:	// Bus error due to an illegal START or STOP condition
		send_stop_condition(); // voir datasheet (https://ww1.microchip.com/downloads/aemDocuments/documents/MCU08/ProductDocuments/DataSheets/ATmega164A_PA-324A_PA-644A_PA-1284_P_Data-Sheet-40002070B.pdf) p.234
		last_error = TWI_ERROR_ILLEGAL_START_OR_STOP;
		return false;
		
	case 0x20:	// SLA+W has been transmitted; NOT ACK has been received
	case 0x48:	// SLA+R has been transmitted; NOT ACK has been received
	case 0x30:	// Data byte has been transmitted; NOT ACK has been received
		send_stop_condition();
		last_error = TWI_ERROR_NOT_ACK_RECEIVED;
		return false;
		
	case 0x38:	// Arbitration lost in SLA+W or data bytes (C'est une catastrophe!)
		clear_twint(); // Ça ne va vraiment pas bien, on abandonne
		last_error = TWI_ERROR_ARBITRATION_LOST;
		return false;
		
	case 0xF8:	// No relevant state information available; TWINT = “0”
		//ne devrait jamais se produire
		while(true);
	}
	
	last_error = 0;
	return true;
}
	
static bool send_start_address_register(uint8_t address, uint8_t start_register){
			
	// Start condition
	send_start_condition();	
	
	if(!wait_for_twint()){
		return false;
	}	
	
	// Si le status n'est pas "A START condition has been transmitted", ça ne va pas bien
	if((TWSR & TWSR_STATUS_MASK) != 0x08){
		last_error = TWI_ERROR_WRONG_STATUS_START;
		return false;
	}
	
	// On shoot le SLA + W
	TWDR = (address << 1) & 0b11111110;	// Un 0 au LSB pour write
	
	// On repart la machine
	clear_twint();
	
	if(!wait_for_twint()){
		return false;
	}
	
	// Si le status n'est pas "SLA+W has been transmitted; ACK has been received", ça ne va pas bien
	if((TWSR & TWSR_STATUS_MASK) != 0x18){
		last_error = TWI_ERROR_WRONG_STATUS_SLA_W;
		return false;
	}
	
	// On shoot le registre
	TWDR = start_register;
	
	// On repart la machine
	clear_twint();
	
	if(!wait_for_twint()){
		return false;
	}	
	
	// Si le status n'est pas "Data byte has been transmitted; ACK has been received", ça ne va pas bien
	if((TWSR & TWSR_STATUS_MASK) != 0x28){
		last_error = TWI_ERROR_WRONG_STATUS_DATA_W;
		return false;
	}
	
	return true;
}

static void reset_twi(void){
	
	TWCR = 0;	// Violent mais efficace
	
	_delay_ms(1); // Probablement trop long
	
	TWCR = set_bit(TWCR, TWEN);
}


