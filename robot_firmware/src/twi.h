#ifndef TWI_H_INCLUDED
#define TWI_H_INCLUDED

/******************************************************************************
Includes
******************************************************************************/

#include "utils.h"

/******************************************************************************
Defines
******************************************************************************/

// Switch _____________________________________________________________________
//#define TWI_ENABLE_ASYNC

#ifdef TWI_ENABLE_ASYNC
	#define TWI_TX_BUFFER_SIZE 24
	#define TWI_RX_BUFFER_SIZE 24
#endif // TWI_ENABLE_ASYNC

// Enum for common TWI bitrates
typedef enum {
	BITRATE_50K,   // Slow mode
	BITRATE_100K,  // Standard mode
	BITRATE_200K,  // Medium mode
	BITRATE_400K,  // Fast mode
	BITRATE_1M,    // Fast mode plus
} twi_bitrate_e;

#define TWI_DEFAULT_BITRATE BITRATE_100K // Standard mode

typedef enum {
	
	TWI_ERROR_NONE = 0,					// Aucune erreur
	TWI_ERROR_TIMEOUT,
	TWI_ERROR_ILLEGAL_START_OR_STOP,
	TWI_ERROR_NOT_ACK_RECEIVED,
	TWI_ERROR_ARBITRATION_LOST,
	TWI_ERROR_WRONG_STATUS_START,
	TWI_ERROR_WRONG_STATUS_REPEATED_START,
	TWI_ERROR_WRONG_STATUS_SLA_W,
	TWI_ERROR_WRONG_STATUS_SLA_R,
	TWI_ERROR_WRONG_STATUS_DATA_W,
	TWI_ERROR_WRONG_STATUS_DATA_R,
	
}twi_error_e;


/******************************************************************************
Prototypes
******************************************************************************/

/**
    \brief Fait l'initialisation du TWI
*/
void twi_init(void);

void twi_set_pullups(bool enable_pullups);

void twi_set_bitrate(twi_bitrate_e bitrate);

bool twi_read_register_blocking(uint8_t address, uint8_t start_register, uint8_t* buffer, uint16_t nb_register);

bool twi_write_register_blocking(uint8_t address, uint8_t start_register, const uint8_t* buffer, uint16_t nb_register);

bool twi_write_register_blocking_progmem(uint8_t address, uint8_t start_register, const uint8_t* buffer, uint16_t nb_register);

int twi_get_last_error();

#endif // TWI_H_INCLUDED
