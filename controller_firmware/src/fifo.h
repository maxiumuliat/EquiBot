#ifndef FIFO_H_INCLUDED
#define FIFO_H_INCLUDED

/******************************************************************************
Includes
******************************************************************************/

#include "utils.h"


/******************************************************************************
Defines et typedef
******************************************************************************/

typedef struct{

    void*		ptr;
    uint8_t     length;
	uint8_t		item_size;
    uint8_t     in_offset;
    uint8_t     out_offset;
    bool        is_empty;
    bool        is_full;
} fifo_s;


/******************************************************************************
Prototypes
******************************************************************************/

/**
    \brief Fait l'initialisation de la structure pour le FIFO
	\param fifo Un pointeur sur une structure FIFO vide
	\param ptr_buffer Un pointeur sur un tableau de bytes
	\param buffer_length La grosseur du tableau de bytes
*/
void fifo_init(fifo_s* fifo, void* ptr_buffer, uint8_t buffer_length, uint8_t item_size);
void fifo_init_uint8(fifo_s* fifo, uint8_t* ptr_buffer, uint8_t buffer_length);
void fifo_init_float(fifo_s* fifo, float* ptr_buffer, uint8_t buffer_length);

void fifo_push(fifo_s* fifo, void* ptr_value_in);
void fifo_push_uint8(fifo_s* fifo, uint8_t value);
void fifo_push_float(fifo_s* fifo, float value);

void fifo_pop(fifo_s* fifo, void* ptr_value_out);
uint8_t fifo_pop_uint8(fifo_s* fifo);
float fifo_pop_float(fifo_s* fifo);

void fifo_peek(fifo_s* fifo, int16_t index, void* ptr_value_out);
uint8_t fifo_peek_uint8(fifo_s* fifo, int16_t index);
float fifo_peek_float(fifo_s* fifo, int16_t index);

float fifo_average_float(fifo_s* fifo);

uint8_t fifo_get_nb_item(fifo_s* fifo);

void fifo_clean(fifo_s* fifo);

/**
    \return true s'il est vide, false s'il contient 1 byte ou plus
*/
bool fifo_is_empty(fifo_s* fifo);
bool fifo_is_full(fifo_s* fifo);	


#endif // FIFO_H_INCLUDED