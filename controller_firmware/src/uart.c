
// Inclusions _________________________________________________________________

#include "uart.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "fifo.h"



// ----------------------------------------------------------------------------
// UART (tout ce qui est commun aux deux uarts)
//-----------------------------------------------------------------------------

// Constante globale privée ___________________________________________________

static const uint16_t baudrate_to_UBRR[] = {

	#if F_CPU == 8000000UL     // Fosc = 8.0000MHz
	207,    // BAUDRATE_2400	Error : 0.2 %
	103,    // BAUDRATE_4800 	Error : 0.2 %
	51,     // BAUDRATE_9600 	Error : 0.2 %
	25,     // BAUDRATE_19200	Error : 0.2 %
	12,     // BAUDRATE_38400	Error : 0.2 %
	8,      // BAUDRATE_57600	Error : -3.5 %
	3,      // BAUDRATE_115200	Error : 8.5 %
	1,      // BAUDRATE_230400	Error : 8.5 %
	1,      // BAUDRATE_250000	Error : 0.0 %

	#elif F_CPU == 16000000UL     /*Fosc = 16.0000MHz*/
	416,    /* BAUDRATE_2400 */
	207,    /* BAUDRATE_4800 */
	103,    /* BAUDRATE_9600 */
	51,     /* BAUDRATE_19200 */
	25,     /* BAUDRATE_38400 */
	16,     /* BAUDRATE_57600 */
	8,      /* BAUDRATE_115200 */
	3,      /* BAUDRATE_230400 */
	3,      /* BAUDRATE_250000 */

	#elif F_CPU == 20000000UL   /*Fosc = 20.0000MHz*/
	520,    /* BAUDRATE_2400 */
	259,    /* BAUDRATE_4800 */
	129,    /* BAUDRATE_9600 */
	64,     /* BAUDRATE_19200 */
	32,     /* BAUDRATE_38400 */
	21,     /* BAUDRATE_57600 */
	10,     /* BAUDRATE_115200 */
	4,      /* BAUDRATE_230400 */
	4,      /* BAUDRATE_250000 */

	#else
	#error "Vous avez oublie de definir F_CPU dans les symboles et uart.c en a obligatoirement besoin"
	#endif
};



// ----------------------------------------------------------------------------
// UART0
//-----------------------------------------------------------------------------

#ifdef UART_ENABLE_UART_0

// Variables globales privées _________________________________________________

static volatile uint8_t rx_buffer_0[UART_0_RX_BUFFER_SIZE];
static volatile uint8_t tx_buffer_0[UART_0_TX_BUFFER_SIZE];

static volatile uint8_t rx_nb_line_0 = 0;
static volatile bool rx_overflow_flag_0 = false;

static fifo_s rx_fifo_0;
static fifo_s tx_fifo_0;


// Macros _____________________________________________________________________

#define disable_data_empty_interrupt_0() UCSR0B = clear_bit(UCSR0B, UDRIE0)
#define enable_data_empty_interrupt_0() UCSR0B = set_bit(UCSR0B, UDRIE0)

#define disable_rx_complete_interrupt_0() UCSR0B = clear_bit(UCSR0B, RXCIE0)
#define enable_rx_complete_interrupt_0() UCSR0B = set_bit(UCSR0B, RXCIE0)


// Interupts __________________________________________________________________

/**
	\brief interupt quand le data register (UDRE) est prêt à recevoir d'autres données pour UART 0
*/
ISR(USART0_UDRE_vect){

	UDR0 = fifo_pop_uint8(&tx_fifo_0);

	if(fifo_is_empty(&tx_fifo_0)){
		
		disable_data_empty_interrupt_0();
	}
}

/**
	\brief interupt quand le data register (UDR) a reçu une nouvelle donnée pour UART 0
*/
ISR(USART0_RX_vect){
	
	uint8_t byte = UDR0; // On backup le registre pour en faire une seule lecture
	
	if(fifo_is_full(&rx_fifo_0)){
		
		rx_overflow_flag_0 = true;
	}
	else{
		
		fifo_push_uint8(&rx_fifo_0, byte);
		
		if((char)byte == UART_LINE_SEPARATOR){
			rx_nb_line_0++;
		}
	}
}


// Définition de fonctions globales publiques _________________________________

void uart0_init(){
	
	PRR0 = clear_bit(PRR0, PRUSART0);	// On désactive la réduction de la consommation électrique

	/* configure asynchronous operation, no parity, 1 stop bit, 8 data bits,  */
	UCSR0C = (	(0 << UMSEL01) | /*USART Mode Select : Asynchronous USART*/
	(0 << UMSEL00) | /*USART Mode Select : Asynchronous USART*/
	(0 << UPM01) |	 /*Parity Mode : No parity*/
	(0 << UPM00) |   /*Parity Mode : No parity*/
	(0 << USBS0) |	 /*Stop Bit Select : 1-bit*/
	(1 << UCSZ01) |  /*Character Size : 8-bit*/
	(1 << UCSZ00) |  /*Character Size : 8-bit*/
	(0 << UCPOL0));  /*0 when asynchronous mode is used*/

	/* enable RxD/TxD and ints */
	UCSR0B = (  (1 << RXCIE0) |  /*RX Complete Interrupt Enable*/
	(0 << TXCIE0) |  /*TX Complete Interrupt Enable */
	(0 << UDRIE0) |  /*Data Register Empty Interrupt Enable */
	(1 << RXEN0) |   /*Receiver Enable*/
	(1 << TXEN0) |   /*Transmitter Enable*/
	(0 << UCSZ02));  /*Character Size : 8-bit*/

	UCSR0A = (  (0 << U2X0) |    /*Double the USART Transmission Speed*/
	(0 << MPCM0));   /*Multi-processor Communication Mode*/

	/*initialisation des fifos respectifs */
	fifo_init_uint8(&rx_fifo_0, (uint8_t*)rx_buffer_0, UART_0_RX_BUFFER_SIZE);
	fifo_init_uint8(&tx_fifo_0, (uint8_t*)tx_buffer_0, UART_0_TX_BUFFER_SIZE);
	
	uart0_set_baudrate(DEFAULT_BAUDRATE);
}

void uart0_set_baudrate(uart_baudrate_e baudrate){

	UBRR0 = baudrate_to_UBRR[baudrate];	/// \todo (iouri#1#): implémenter qqch qui empéche la corruption de la transmission.  La mise à jour de UBRR est immédiate.  Voir doc p. 196
}

void uart0_put_byte(uint8_t byte){
	
	// On attend à l'infini qu'il y ait de la place dans le buffer. 
	while(fifo_is_full(&tx_fifo_0));
	
	disable_data_empty_interrupt_0();	//on commence par désactiver l'interruption pour éviter que celle-ci se produise pendant qu'on ajoute un caractère au buffer
	fifo_push_uint8(&tx_fifo_0, byte);
	enable_data_empty_interrupt_0();	// On active l'interrupt aprés avoir incrémenté le pointeur d'entrée pour éviter un dead lock assez casse-tête
}

void uart0_put_string(const char* string){
	
	while(*string != '\0'){

		// On attend à l'infini qu'il y ait de la place dans le buffer. 
		while(fifo_is_full(&tx_fifo_0));

		//on commence par désactiver l'interruption pour éviter que celle-ci
		//se produise pendant qu'on ajoute un caractère au buffer
		disable_data_empty_interrupt_0();

		while((*string != '\0') && !(fifo_is_full(&tx_fifo_0))){

			fifo_push_uint8(&tx_fifo_0, *string);
			string++;
		}

		// On active l'interrupt après avoir incrémenté le pointeur
		// d'entrée pour éviter un dead lock assez casse-tête
		enable_data_empty_interrupt_0();
	}
}

void uart0_put_float(float f,  uint8_t min_int_digit, uint8_t nb_frac_digit, bool show_positive_sign){
	float_to_char_function(f, min_int_digit, nb_frac_digit, show_positive_sign, (void (*)(char))&uart0_put_byte);	// Le cast bizzare c'est juste pour faire disparaître le warning du compilateur
}

void uart0_plot_float(const char* label, float f, uint8_t nb_frac_digit){
	
	uart0_put_string(label);		// On commence par envoyer le label
	uart0_put_byte(':');			// Suivi d'un deux points
	uart0_put_byte(' ');			// Un espace
	uart0_put_float(f, 1, nb_frac_digit, false);	// La valeur en float
	uart0_put_byte('\r');			// Un backslash r
	uart0_put_byte('\n');			// Un backslash n
}

uint8_t uart0_get_byte(void){

	uint8_t byte;
	
	disable_rx_complete_interrupt_0();

	byte = fifo_pop_uint8(&rx_fifo_0);
	
	if(byte == UART_LINE_SEPARATOR){
		
		rx_nb_line_0--;
	}

	enable_rx_complete_interrupt_0();

	return byte;
}

int uart0_get_line(char* out_buffer, uint8_t buffer_length){
	
	uint8_t line_length = 0 ;
	
	if(rx_nb_line_0 == 0){
		return line_length;
	}
	
	uint8_t byte = 0;
	
	disable_rx_complete_interrupt_0();
	
	while(line_length < (buffer_length - 1)){	// le -1 est pour laisser de la place pour le \0

		byte = fifo_pop_uint8(&rx_fifo_0);
		*out_buffer = byte;
		out_buffer++;
		line_length++;
		
		if(byte == UART_LINE_SEPARATOR){
			rx_nb_line_0--;
			break;
		}
	}

	enable_rx_complete_interrupt_0();
	
	*out_buffer = '\0';

	return line_length;
}

void uart0_clean_rx_buffer(void){
	
	disable_rx_complete_interrupt_0();
	fifo_clean(&rx_fifo_0);
	rx_nb_line_0 = 0;	
	enable_rx_complete_interrupt_0();
}

void uart0_flush(void){
	while(!fifo_is_empty(&tx_fifo_0));
}

bool uart0_is_tx_buffer_empty(void){
	return fifo_is_empty(&tx_fifo_0);
}

bool uart0_is_tx_buffer_full(void){
	return fifo_is_full(&tx_fifo_0);
}

bool uart0_is_rx_buffer_empty(void){
	return fifo_is_empty(&rx_fifo_0);
}

uint8_t uart0_rx_buffer_nb_line(void){
	return rx_nb_line_0;
}

bool uart0_get_rx_overflow_flag(void){
	return rx_overflow_flag_0;
}

void uart0_reset_rx_overflow_flag(void){
	rx_overflow_flag_0 = false;
}

#endif // UART_ENABLE_UART_0



// ----------------------------------------------------------------------------
// UART1
//-----------------------------------------------------------------------------

#ifdef UART_ENABLE_UART_1

// Variables globales privées _________________________________________________

static volatile uint8_t rx_buffer_1[UART_1_RX_BUFFER_SIZE];
static volatile uint8_t tx_buffer_1[UART_1_TX_BUFFER_SIZE];

static volatile uint8_t rx_nb_line_1 = 0;
static volatile bool rx_overflow_flag_1 = false;

static fifo_s rx_fifo_1;
static fifo_s tx_fifo_1;


// Macros _____________________________________________________________________

#define disable_data_empty_interrupt_1() UCSR1B = clear_bit(UCSR1B, UDRIE1)
#define enable_data_empty_interrupt_1() UCSR1B = set_bit(UCSR1B, UDRIE1)

#define disable_rx_complete_interrupt_1() UCSR1B = set_bit(UCSR1B, RXCIE1)
#define enable_rx_complete_interrupt_1() UCSR1B = set_bit(UCSR1B, RXCIE1)


// Interupts __________________________________________________________________

/**
	\brief interupt quand le data register (UDRE) est prêt à recevoir d'autres données pour UART 1
*/
ISR(USART1_UDRE_vect){

	UDR1 = fifo_pop_uint8(&tx_fifo_1);

	if(fifo_is_empty(&tx_fifo_1)){

		disable_data_empty_interrupt_1();
	}
}


/**
\brief interupt quand le data register (UDR) a reéu une nouvelle donnée
pour UART 1
*/
ISR(USART1_RX_vect){
	
	uint8_t byte = UDR1;	// On backup le registre pour en faire une seule lecture
	
	if(fifo_is_full(&rx_fifo_0)){
		
		rx_overflow_flag_0 = true;
	}
	else{
		
		fifo_push_uint8(&rx_fifo_1, byte);
		
		if((char)byte == UART_LINE_SEPARATOR){
			rx_nb_line_1++;
		}
	}
}


// Définition de fonction globales publiques __________________________________

void uart1_init(){
	
	PRR0 = clear_bit(PRR0, PRUSART1);	// On désactive la réduction de la consommation électrique

	/* configure asynchronous operation, no parity, 1 stop bit, 8 data bits,  */
	UCSR1C = (	(0 << UMSEL01) | /*USART Mode Select : Asynchronous USART*/
	(0 << UMSEL00) | /*USART Mode Select : Asynchronous USART*/
	(0 << UPM01) |	 /*Parity Mode : No parity*/
	(0 << UPM00) |   /*Parity Mode : No parity*/
	(0 << USBS0) |	 /*Stop Bit Select : 1-bit*/
	(1 << UCSZ01) |  /*Character Size : 8-bit*/
	(1 << UCSZ00) |  /*Character Size : 8-bit*/
	(0 << UCPOL0));  /*0 when asynchronous mode is used*/

	UCSR1B = (  (1 << RXCIE0) |  /*RX Complete Interrupt Enable*/
	(0 << TXCIE0) |  /*TX Complete Interrupt Enable */
	(0 << UDRIE0) |  /*Data Register Empty Interrupt Enable */
	(1 << RXEN0) |   /*Receiver Enable*/
	(1 << TXEN0) |   /*Transmitter Enable*/
	(0 << UCSZ02));  /*Character Size : 8-bit*/

	UCSR1A = (  (0 << U2X0) |    /*Double the USART Transmission Speed*/
	(0 << MPCM0));   /*Multi-processor Communication Mode*/

	/*initialisation des fifos respectifs */
	fifo_init_uint8(&rx_fifo_1, (uint8_t*)rx_buffer_1, UART_1_RX_BUFFER_SIZE);	// Le cast en uint8_t* sert à faire disparaître le volatile
	fifo_init_uint8(&tx_fifo_1, (uint8_t*)tx_buffer_1, UART_1_TX_BUFFER_SIZE);	// Le cast en uint8_t* sert à faire disparaître le volatile
	
	uart1_set_baudrate(DEFAULT_BAUDRATE);
}

void uart1_set_baudrate(uart_baudrate_e baudrate){

	UBRR1 = baudrate_to_UBRR[baudrate];	/// \todo (iouri#1#): implémenter qqch qui empéche la corruption de la transmission.  La mise à jour de UBRR est immédiate.  Voir doc p. 196
}

void uart1_put_byte(uint8_t byte){
	
	// On attend à l'infini qu'il y ait de la place dans le buffer.
	while(fifo_is_full(&tx_fifo_1));
	
	disable_data_empty_interrupt_1();	//on commence par désactiver l'interruption pour éviter que celle-ci se produise pendant qu'on ajoute un caractère au buffer
	fifo_push_uint8(&tx_fifo_1, byte);
	enable_data_empty_interrupt_1();	// On active l'interrupt aprés avoir incrémenté le pointeur d'entrée pour éviter un dead lock assez casse-tête
}

void uart1_put_string(const char* string){
	
	while(*string != '\0'){

		// On attend à l'infini qu'il y ait de la place dans le buffer.
		while(fifo_is_full(&tx_fifo_1));

		//on commence par désactiver l'interruption pour éviter que celle-ci
		//se produise pendant qu'on ajoute un caractère au buffer
		disable_data_empty_interrupt_1();

		while((*string != '\0') && !(fifo_is_full(&tx_fifo_1))){

			fifo_push_uint8(&tx_fifo_1, *string);
			string++;
		}

		// On active l'interrupt aprés avoir incrémenté le pointeur
		// d'entrée pour éviter un dead lock assez casse-tête
		enable_data_empty_interrupt_1();
	}
}

void uart1_put_float(float f, uint8_t min_int_digit, uint8_t nb_frac_digit, bool show_positive_sign){
	float_to_char_function(f, min_int_digit, nb_frac_digit, show_positive_sign, (void (*)(char)) &uart1_put_byte);	// Le cast bizzare c'est juste pour faire disparaître le warning du compilateur
}

void uart1_plot_float(const char* label, float f, uint8_t nb_frac_digit){
	
	uart1_put_string(label);		// On commence par envoyer le label
	uart1_put_byte(':');			// Suivi d'un deux points
	uart1_put_byte(' ');			// Un espace
	uart1_put_float(f, 1, nb_frac_digit, false);	// La valeur en float
	uart1_put_byte('\r');			// Un backslash r
	uart1_put_byte('\n');			// Un backslash n
}

uint8_t uart1_get_byte(void){

	uint8_t byte;
	
	disable_rx_complete_interrupt_1();

	byte = fifo_pop_uint8(&rx_fifo_1);
	
	if(byte == UART_LINE_SEPARATOR){
		
		rx_nb_line_1--;
	}

	enable_rx_complete_interrupt_1();

	return byte;
}

int uart1_get_line(char* out_buffer, uint8_t buffer_length){
		
	uint8_t line_length = 0 ;
		
	if(rx_nb_line_0 == 0){
		return line_length;
	}
		
	uint8_t byte = 0;
		
	disable_rx_complete_interrupt_1();
		
	while(line_length < (buffer_length - 1)){	// le -1 est pour laisser de la place pour le \0

		byte = fifo_pop_uint8(&rx_fifo_1);
		*out_buffer = byte;
		out_buffer++;
		line_length++;
			
		if(byte == UART_LINE_SEPARATOR){
			rx_nb_line_1--;
			break;
		}
	}

	enable_rx_complete_interrupt_1();
		
	*out_buffer = '\0';

	return line_length;
}

void uart1_clean_rx_buffer(void){
	
	disable_rx_complete_interrupt_1();
	fifo_clean(&rx_fifo_1);
	rx_nb_line_1 = 0;
	enable_rx_complete_interrupt_1();
}

void uart1_flush(){
	while(!fifo_is_empty(&tx_fifo_1));
}

bool uart1_is_rx_buffer_empty(){
	return fifo_is_empty(&rx_fifo_1);
}

bool uart1_is_tx_buffer_empty(){
	return fifo_is_empty(&tx_fifo_1);
}

uint8_t uart1_rx_buffer_nb_line(){
	return rx_nb_line_1;
}

bool uart1_get_rx_overflow_flag(void){
	return rx_overflow_flag_1;
}

void uart1_reset_rx_overflow_flag(void){
	rx_overflow_flag_1 = false;
}

#endif // UART_ENABLE_UART_1