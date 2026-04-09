/******************************************************************************
Includes
******************************************************************************/

#include "fifo.h"

#include <math.h>

/******************************************************************************
Global functions
******************************************************************************/

// Init _______________________________________________________________________

void fifo_init(fifo_s* fifo, void* ptr_buffer, uint8_t buffer_length, uint8_t item_size) {

    fifo->ptr = ptr_buffer;
    fifo->length = buffer_length;
    fifo->item_size = item_size;
    fifo->in_offset = 0;
    fifo->out_offset = 0;
    fifo->is_empty = true;
    fifo->is_full = false;
}

void fifo_init_uint8(fifo_s* fifo, uint8_t* ptr_buffer, uint8_t buffer_length) {

    fifo_init(fifo, ptr_buffer, buffer_length, sizeof(uint8_t));
}

void fifo_init_float(fifo_s* fifo, float* ptr_buffer, uint8_t buffer_length) {

    fifo_init(fifo, ptr_buffer, buffer_length, sizeof(float));
}

// Push _______________________________________________________________________

void fifo_push(fifo_s* fifo, void* ptr_value_in) {

    // Si le buffer est plein il n'est pas question de rien "pusher"
    if (fifo->is_full) {
        return;
    }

    // Pour tous les bytes pointés par ptr_value
    for (uint8_t i = 0; i < fifo->item_size; i++) {

        uint8_t byte = *((uint8_t*)ptr_value_in + i);
        ((uint8_t*)fifo->ptr)[fifo->in_offset * fifo->item_size + i] = byte;
    }

    fifo->is_empty = false;

    // On incrémente le offset d'entrée
    fifo->in_offset++;

    // Si on est rendu à la fin du rolling buffer, on recommence à écrire au début
    if (fifo->in_offset >= fifo->length) {
        fifo->in_offset = 0;
    }

    // si l'index d'entrée rattrape celui de sortie c'est que le buffer est plein
    if (fifo->in_offset == fifo->out_offset) {
        fifo->is_full = true;
    }
}

void fifo_push_uint8(fifo_s* fifo, uint8_t value) {
    fifo_push(fifo, &value);
}

void fifo_push_float(fifo_s* fifo, float value) {
    fifo_push(fifo, &value);
}

// Pop ________________________________________________________________________

void fifo_pop(fifo_s* fifo, void* ptr_value_out) {

    // Si le buffer est vide, il n'est pas question de rien "pop"
    if (fifo->is_empty) {
        return; // Dans le cas où la fonction échoue, le pointeur restera inchangé
    }

    // Pour tous les bytes pointés par ptr_value
    for (uint8_t i = 0; i < fifo->item_size; i++) {

        uint8_t byte = ((uint8_t*)fifo->ptr)[fifo->out_offset * fifo->item_size + i];
        *((uint8_t*)ptr_value_out + i) = byte;
    }

    fifo->is_full = false;

    // On incrémente le offset de sortie
    fifo->out_offset++;

    // Si on vient de "pop" le dernier item du buffer, on recommence au début
    if (fifo->out_offset >= fifo->length) {
        fifo->out_offset = 0;
    }

    // si l'index de sortie rattrape celui d'entrée c'est que le buffer est vide
    if (fifo->out_offset == fifo->in_offset) {
        fifo->is_empty = true;
    }
}

uint8_t fifo_pop_uint8(fifo_s* fifo) {

    uint8_t value = 0; // Dans le cas où la fonction échoue, c'est la valeur par défaut qui sera retournée
    fifo_pop(fifo, &value);
    return value;
}

float fifo_pop_float(fifo_s* fifo) {

    float value = NAN; // Dans le cas où la fonction échoue, c'est la valeur par défaut qui sera retournée
    fifo_pop(fifo, &value);
    return value;
}

void fifo_peek(fifo_s* fifo, int16_t index, void* ptr_value_out) {

    // Si le buffer est vide il n'est pas question de rien "peek"
    if (fifo->is_empty) {
        return; // ptr_value_out reste inchangé
    }

    int16_t buffer_offset = 0;

    if (index >= 0) { // Si on demande de "peek" à partir du premier item

        if (index > fifo_get_nb_item(fifo) - 1) { // Si on demande l'impossible
            return;                               // ptr_value_out reste inchangé
        }

        buffer_offset = fifo->out_offset + index;

        if (buffer_offset >= fifo->length) { // Si on dépasse la longueur du buffer
            buffer_offset -= fifo->length;
        }
    }

    else { // Si on demande de "peek" à partir du dernier item

        if (index < -fifo_get_nb_item(fifo)) { // Si on demande l'impossible
            return;                            // ptr_value_out reste inchangé
        }

        buffer_offset = fifo->in_offset + index; // Ici index est forcément négatif donc c'est une soustraction

        if (buffer_offset < 0) { // Si on dépasse la longueur du buffer
            buffer_offset += fifo->length;
        }
    }

    // Pour tous les bytes pointés par ptr_value
    for (uint8_t i = 0; i < fifo->item_size; i++) {

        uint8_t byte = ((uint8_t*)fifo->ptr)[buffer_offset * fifo->item_size + i];
        *((uint8_t*)ptr_value_out + i) = byte;
    }
}

uint8_t fifo_peek_uint8(fifo_s* fifo, int16_t index) {

    uint8_t value = 0; // Dans le cas où la fonction échoue, c'est la valeur par défaut qui sera retournée
    fifo_peek(fifo, index, &value);
    return value;
}

float fifo_peek_float(fifo_s* fifo, int16_t index) {

    float value = NAN; // Dans le cas où la fonction échoue, c'est la valeur par défaut qui sera retournée
    fifo_peek(fifo, index, &value);
    return value;
}


float fifo_average_float(fifo_s* fifo) {

    if (fifo->is_empty) { // Si le fifo est vide
        return NAN;       // Il n'y a aucune moyenne à faire ici
    }

    float average;

    float sum = 0;
    uint8_t nb_item = 0;
    uint8_t buffer_offset = fifo->out_offset;
	bool loop = true;

    while (loop) { // On ne peut pas mettre la condition ici parce quand le fifo est plein elle est fausse en partant

        sum += ((float*)fifo->ptr)[buffer_offset];
        nb_item++;

        buffer_offset++;

        if (buffer_offset >= fifo->length) { // Si on dépasse la longueur du buffer
            buffer_offset = 0;               // On recommence au début
        }
		
		if(buffer_offset == fifo->in_offset){	// si on n'a pas rattrapé le dernier item
			loop = false;
		}
    }

    average = sum / nb_item;

    return average;
}

uint8_t fifo_get_nb_item(fifo_s* fifo) {
	
	if(fifo->is_full){
		return 	fifo->length;
	}
    else if (fifo->in_offset >= fifo->out_offset) {
        return fifo->in_offset - fifo->out_offset;
    }
	else{
        return fifo->length - fifo->out_offset + fifo->in_offset;
    }
}

void fifo_clean(fifo_s* fifo) {

    fifo->in_offset = fifo->out_offset; // C'est un peu cheap de faire ça plutôt que remettre les deux à 0, mais c'est une ligne de code de moins
    fifo->is_full = false;
    fifo->is_empty = true;
}

bool fifo_is_empty(fifo_s* fifo) {
    return fifo->is_empty;
}

bool fifo_is_full(fifo_s* fifo) {
    return fifo->is_full;
}