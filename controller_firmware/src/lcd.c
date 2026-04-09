// Includes ___________________________________________________________________

#include "lcd.h"

#include <avr/io.h>
#include <util/delay.h>

#ifdef LCD_REDIRECT_STDOUT_TO_LCD
#include <stdio.h>
#endif // LCD_REDIRECT_STDOUT_TO_LCD



// ----------------------------------------------------------------------------
// Sous Module HD44780 (Accès direct, mais difficile au LCD)
//-----------------------------------------------------------------------------

// Defines conditionnels
#ifndef HD44780_ENABLE_4BIT_OPERATION // Donc on veut une opération en 8 bit
	
	#define FUNCTION_SET		0b00111000	// Pour opération en 8 bits, deux lignes, caractère 5x8
	#define DATA_MASK			0b11111111	// On utilise le port au complet
	#define FIRST_FUNCTION_SET FUNCTION_SET	// Rien de spécial à faire avec le premier envoi
	
#else	// Donc on veut une opération en 4 bit

	#define FUNCTION_SET		0b00101000							// Pour opération en 4 bits, deux lignes, caractère 5x8
	#define DATA_MASK			(0b00001111 << HD44780_DATA_SHIFT)	// On utilise seulement 4 bits du ports et ceux-ci dépendent du shift
	#define FIRST_FUNCTION_SET	(FUNCTION_SET >> (4 - HD44780_DATA_SHIFT))	// Le premier envoi ne doit contenir que le high nibble du function set
	
#endif

// Macros privées _____________________________________________________________

#define FALLING_EDGE()  HD44780_E_PORT = clear_bit(HD44780_E_PORT, HD44780_E_SHIFT)
#define RISING_EDGE()   HD44780_E_PORT = set_bit(HD44780_E_PORT, HD44780_E_SHIFT)
#define WRITE_MODE()	HD44780_RW_PORT = clear_bit(HD44780_RW_PORT, HD44780_RW_SHIFT)
#define READ_MODE()		HD44780_RW_PORT = set_bit(HD44780_RW_PORT, HD44780_RW_SHIFT)
#define COMMAND_MODE()  HD44780_RS_PORT = clear_bit(HD44780_RS_PORT, HD44780_RS_SHIFT)
#define DATA_MODE()     HD44780_RS_PORT = set_bit(HD44780_RS_PORT, HD44780_RS_SHIFT)


// Variable globale privée ____________________________________________________

static bool is_in_data_mode = false;


// Déclaration de fonction privée _____________________________________________

static void clock_data(uint8_t data);
static void clock_command(uint8_t command);
static void clock_byte(uint8_t byte);
static bool wait_for_busy_flag(int16_t timeout_us);

#ifdef HD44780_USE_CGRAM_FOR_FRENCH_ACCENT	// Si on veut utiliser les accents, l'utilisation de la CGRAM devient proscrite à l'extérieur de ce fichier
static void hd44780_write_cgram(uint8_t slot, const uint8_t* bitmap_array);
#endif


// Définition de fonction publiques ___________________________________________

void hd44780_init(bool increment, bool cursor, bool blink){

	//On définit la valeur par défaut des ports
	HD44780_RS_PORT = clear_bit(HD44780_RS_PORT, HD44780_RS_SHIFT);   //command mode
	HD44780_RW_PORT = clear_bit(HD44780_RW_PORT, HD44780_RW_SHIFT);   //write mode

	// On change la direction des ports
	HD44780_DATA_DDR = set_bits(HD44780_DATA_DDR, DATA_MASK);	// On met les bits du data mask en sortie
	HD44780_E_DDR = set_bit(HD44780_E_DDR, HD44780_E_SHIFT);	// En sortie	
	HD44780_RW_DDR = set_bit(HD44780_RW_DDR, HD44780_RW_SHIFT);	// En sortie
	HD44780_RS_DDR = set_bit(HD44780_RS_DDR, HD44780_RS_SHIFT);	// En sortie

	//initial wait
	_delay_ms(15);				// 15 ms pour Vcc = 5 V, 40 ms pour Vcc = 3.3 V. Voir datasheet (https://www.sparkfun.com/datasheets/LCD/HD44780.pdf) p.45

	// Le premier function set est toujours envoyé comme si on était en 8 bits, même si on est en 4 bits.
	HD44780_DATA_PORT = write_bits(HD44780_DATA_PORT, DATA_MASK, FIRST_FUNCTION_SET);	
	_delay_us(1);

	RISING_EDGE();
	_delay_us(1);				// Enable cycle time = 1000 ns. Voir datasheet (https://www.sparkfun.com/datasheets/LCD/HD44780.pdf) p.49

	FALLING_EDGE();

	_delay_ms(5);				// 4.1 ms. Voir datasheet (https://www.sparkfun.com/datasheets/LCD/HD44780.pdf) p.45

	clock_byte(FUNCTION_SET);	// On re-shoot le function set (pourquoi, je ne sais pas)

	_delay_us(100);				// 100 us Voir datasheet (https://www.sparkfun.com/datasheets/LCD/HD44780.pdf) p.45

	clock_byte(FUNCTION_SET);	// On re-shoot le function set (pourquoi, je ne sais pas)

	_delay_us(768);				// 768 us Aucune idée ça sort d'où

	hd44780_set_entry_mode(increment);
	hd44780_set_display_control(true, cursor, blink);
	
#ifdef HD44780_USE_CGRAM_FOR_FRENCH_ACCENT

	uint8_t bitmap_array[8];
	
	bitmap_array[0] = 0b00100;
	bitmap_array[1] = 0b00010;
	bitmap_array[2] = 0b01110;
	bitmap_array[3] = 0b00001;
	bitmap_array[4] = 0b01111;
	bitmap_array[5] = 0b10001;
	bitmap_array[6] = 0b01111;
	bitmap_array[7] = 0b00000;
	
	hd44780_write_cgram(HD44780_CHAR_GRAVE_A, bitmap_array);
	
	bitmap_array[0] = 0b00000;
	bitmap_array[1] = 0b00000;
	bitmap_array[2] = 0b01110;
	bitmap_array[3] = 0b10000;
	bitmap_array[4] = 0b10000;
	bitmap_array[5] = 0b10001;
	bitmap_array[6] = 0b01110;
	bitmap_array[7] = 0b00100;
	
	hd44780_write_cgram(HD44780_CHAR_CEDILLA_C, bitmap_array);
	
	bitmap_array[0] = 0b00100;
	bitmap_array[1] = 0b00010;
	bitmap_array[2] = 0b01110;
	bitmap_array[3] = 0b10001;
	bitmap_array[4] = 0b11111;
	bitmap_array[5] = 0b10000;
	bitmap_array[6] = 0b01110;
	bitmap_array[7] = 0b00000;
	
	hd44780_write_cgram(HD44780_CHAR_GRAVE_E, bitmap_array);
	
	bitmap_array[0] = 0b00100;
	bitmap_array[1] = 0b01000;
	bitmap_array[2] = 0b01110;
	bitmap_array[3] = 0b10001;
	bitmap_array[4] = 0b11111;
	bitmap_array[5] = 0b10000;
	bitmap_array[6] = 0b01110;
	bitmap_array[7] = 0b00000;
	
	hd44780_write_cgram(HD44780_CHAR_ACUTE_E, bitmap_array);	
	
	bitmap_array[0] = 0b00100;
	bitmap_array[1] = 0b01010;
	bitmap_array[2] = 0b01110;
	bitmap_array[3] = 0b10001;
	bitmap_array[4] = 0b11111;
	bitmap_array[5] = 0b10000;
	bitmap_array[6] = 0b01110;
	bitmap_array[7] = 0b00000;
	
	hd44780_write_cgram(HD44780_CHAR_CIRCUMFLEX_E, bitmap_array);
	
	bitmap_array[0] = 0b01010;
	bitmap_array[1] = 0b00000;
	bitmap_array[2] = 0b01110;
	bitmap_array[3] = 0b10001;
	bitmap_array[4] = 0b11111;
	bitmap_array[5] = 0b10000;
	bitmap_array[6] = 0b01110;
	bitmap_array[7] = 0b00000;
	
	hd44780_write_cgram(HD44780_CHAR_TREMA_E, bitmap_array);
	
	bitmap_array[0] = 0b01010;
	bitmap_array[1] = 0b00000;
	bitmap_array[2] = 0b01100;
	bitmap_array[3] = 0b00100;
	bitmap_array[4] = 0b00100;
	bitmap_array[5] = 0b00100;
	bitmap_array[6] = 0b01110;
	bitmap_array[7] = 0b00000;
	
	hd44780_write_cgram(HD44780_CHAR_TREMA_I, bitmap_array);
	
	bitmap_array[0] = 0b00100;
	bitmap_array[1] = 0b00010;
	bitmap_array[2] = 0b10001;
	bitmap_array[3] = 0b10001;
	bitmap_array[4] = 0b10001;
	bitmap_array[5] = 0b10011;
	bitmap_array[6] = 0b01101;
	bitmap_array[7] = 0b00000;
	
	hd44780_write_cgram(HD44780_CHAR_GRAVE_U, bitmap_array);	

#endif
		
	hd44780_clear_display();
}


void hd44780_clear_display(){

	clock_command(0b00000001);     //Clear Display
	
	const int16_t SAFE_DELAY_US = 1700;	// Mesuré à 1.51 ms = 1510 us < 1700 us
	wait_for_busy_flag(SAFE_DELAY_US);
}


void hd44780_set_entry_mode(bool increment){

	// Voir datasheet (https://www.sparkfun.com/datasheets/LCD/HD44780.pdf) p. 24
	clock_command(0b00000100 | (increment << 1));     //Entry mode set
	
	const int16_t SAFE_DELAY_US = 50;	// Mesuré à 27 us < 50 us
	wait_for_busy_flag(SAFE_DELAY_US);
}


void hd44780_set_display_control(bool display, bool cursor, bool blink){

	// Voir datasheet (https://www.sparkfun.com/datasheets/LCD/HD44780.pdf) p. 24
	clock_command(0b00001000 | (display << 2) | (cursor << 1) | (blink << 0));     //Display on/off control
	
	const int16_t SAFE_DELAY_US = 50;	// Mesuré à 24 us < 50 us
	wait_for_busy_flag(SAFE_DELAY_US);
}


void hd44780_set_cursor_position(uint8_t row, uint8_t col){

	uint8_t address = 0;

	//On commence par ajouter le offset de la ligne
	switch(row){
	case 0:

		address = 0x00;
		break;

	case 1:

		address = 0x40;
		break;
	}

	//Puis on ajoute le offset de la colone
	address += col;

	clock_command(0b10000000 | address);     //Set DDRAM address
	
	const int16_t SAFE_DELAY_US = 0x7FFF;	// Jamais mesuré donc je mets une valeur ridicule pour l'instant
	wait_for_busy_flag(SAFE_DELAY_US);
}


void hd44780_shift_cursor(hd44780_shift_e shift){

	uint8_t right_left;

	if(shift == HD44780_SHIFT_RIGHT){

		right_left = 0b00000100;
	}

	else{

		right_left = 0b00000000;
	}

	clock_command(0b00010000 | right_left);     //Cursor or display shift
	
	const int16_t SAFE_DELAY_US = 0x7FFF;	// Jamais mesuré donc je mets une valeur ridicule pour l'instant
	wait_for_busy_flag(SAFE_DELAY_US);
}


void hd44780_write_char(char c){
	
	uint8_t unsigned_c = (uint8_t)c;
	
#ifdef HD44780_USE_CGRAM_FOR_FRENCH_ACCENT

	if(unsigned_c > HD44780_CHAR_LEFT_ARROW){
			
		switch(unsigned_c){
		case 0xC0:	//À
		case 0xC1:	//A accent aigue
		case 0xC2:	//Â
		case 0xC3:	//A ???
		case 0xC4:	//Ä
			unsigned_c = 'A';
			break;
				
		case 0xC7:	//Ç
			unsigned_c = 'C';
			break;
				
		case 0xC8:	//È
		case 0xC9:	//É
		case 0xCA:	//Ê
		case 0xCB:	//Ë
			unsigned_c = 'E';
			break;
				
		case 0xCC:	//Ì
		case 0xCD:	//I accent aigue
		case 0xCE:	//Î
		case 0xCF:	//Ï
			unsigned_c = 'I';
			break;
				
		case 0xD2:	//Ò
		case 0xD3:	//O accent aigue
		case 0xD4:	//Ô
		case 0xD5:	//O ???
		case 0xD6:	//Ö
			unsigned_c = 'O';
			break;
				
		case 0xD9:	//Ù
		case 0xDA:	//U accent aigue
		case 0xDB:	//Û
		case 0xDC:	//Ü
			unsigned_c = 'U';
			break;
				
		case 0xE0:	//à
			unsigned_c = HD44780_CHAR_GRAVE_A;
			break;
				
		case 0xE1:	//a accent aigue
		case 0xE2:	//â
		case 0xE3:	//a ???			
			unsigned_c = 'a';
			break;
				
		case 0xE4:	//ä
			unsigned_c = HD44780_CHAR_TREMA_A;	// pour une raison inexplicable, le caractère ä exsite dans la ROM
			break;
				
		case 0xE7:	//ç
			unsigned_c = HD44780_CHAR_CEDILLA_C;
			break;
				
		case 0xE8:	//è
			unsigned_c = HD44780_CHAR_GRAVE_E;
			break;
				
		case 0xE9:	//é
			unsigned_c = HD44780_CHAR_ACUTE_E;
			break;
				
		case 0xEA:	//ê
			unsigned_c = HD44780_CHAR_CIRCUMFLEX_E;
			break;
				
		case 0xEB:	//ë
			unsigned_c = HD44780_CHAR_TREMA_E;
			break;
				
		case 0xEC:	//ì
		case 0xED:	//i accent aigue
		case 0xEE:	//î
			unsigned_c = 'i';
			break;
				
		case 0xEF:	//ï
			unsigned_c = HD44780_CHAR_TREMA_I;
			break;
				
		case 0xF2:	//ò
		case 0xF3:	//o accent aigue
		case 0xF4:	//ô
		case 0xF5:	//o ???			
			unsigned_c = 'o';
			break;
				
		case 0xF6:	//ö
			unsigned_c = HD44780_CHAR_TREMA_O;	// pour une raison innexplicable, le caractère ö exsite dans la ROM
			break;
				
		case 0xF9:	//ù			
			unsigned_c = HD44780_CHAR_GRAVE_U;
			break;
				
		case 0xFA:	//u accent aigue
		case 0xFB:	//û			
			unsigned_c = 'u';
			break;
				
		case 0xFC:	//ü
			unsigned_c = HD44780_CHAR_TREMA_U;	// pour une raison innexplicable, le caractère ü exsite dans la ROM
			break;
				
		default:
			unsigned_c = 0b10100101;  //une boule pas rapport
			break;
		}
	}
	
#endif // HD44780_USE_CGRAM_FOR_FRENCH_ACCENT

	clock_data(unsigned_c); 
		
	const int16_t SAFE_DELAY_US = 50;	// Mesuré à 24, 27 et 27 us < 50
	wait_for_busy_flag(SAFE_DELAY_US);
}


void hd44780_write_cgram(uint8_t slot, const uint8_t* bitmap_array){

	const int16_t SAFE_DELAY_US = 50;	// Pas mesuré

	//Vérification que slot est inclusivement entre 0 et 7 + décalage
	slot = (slot & 0b00000111) << 3;

	clock_command(0b01000000 | slot);     //Set CGRAM address
	
	wait_for_busy_flag(SAFE_DELAY_US);

	for(uint8_t i = 0; i < 8; i++){

		clock_data(bitmap_array[i]);
		
		wait_for_busy_flag(SAFE_DELAY_US);
	}
	
	// Il faut indiquer que les prochains byte vont dans la DDRAM et non pas la CGRAM
	hd44780_set_cursor_position(0,0);
}


// Définition de fonctions privées ____________________________________________

static void clock_data(uint8_t data){
	
	if(!is_in_data_mode){
		DATA_MODE();
		is_in_data_mode = true;
	}
	
	clock_byte(data);
}


static void clock_command(uint8_t command){
	
	if(is_in_data_mode){
		COMMAND_MODE();
		is_in_data_mode = false;
	}
	
	clock_byte(command);
}


#ifndef HD44780_ENABLE_4BIT_OPERATION
static void clock_byte(uint8_t byte){	// La définition de la fonction clock_byte en mode 8 bit

	HD44780_DATA_PORT = byte;
	
	_delay_us(1);

	RISING_EDGE();

	_delay_us(1);	// Enable cycle time = 1000 ns. Voir datasheet (https://www.sparkfun.com/datasheets/LCD/HD44780.pdf) p.49

	FALLING_EDGE();
}
#else
static void clock_byte(uint8_t byte){ // La définition de la fonction clock_byte en mode 4 bit
	
	uint8_t high_nibble = (byte & 0b11110000) >> 4;
	uint8_t low_nibble = (byte & 0b00001111) >> 0;

	HD44780_DATA_PORT = write_bits(HD44780_DATA_PORT, DATA_MASK, high_nibble << HD44780_DATA_SHIFT);
	
	_delay_us(1);

	RISING_EDGE();

	_delay_us(1);	// Enable cycle time = 1000 ns. Voir datasheet (https://www.sparkfun.com/datasheets/LCD/HD44780.pdf) p.49

	FALLING_EDGE();
	
	HD44780_DATA_PORT = write_bits(HD44780_DATA_PORT, DATA_MASK, low_nibble << HD44780_DATA_SHIFT);
	
	_delay_us(1);

	RISING_EDGE();

	_delay_us(1);	// Enable cycle time = 1000 ns. Voir datasheet (https://www.sparkfun.com/datasheets/LCD/HD44780.pdf) p.49

	FALLING_EDGE();
}
#endif


static bool wait_for_busy_flag(int16_t timeout_us){
	
	const int BUSY_FLAG_BIT = 7;
	const int CLOCK_LOW_TIME_US = 1;
	const int CLOCK_HIGH_TIME_US = 1;
	
	bool is_wait_successful = true; // Jusqu'à preuve du contraire, l'opération devrait réussir
	
	if(is_in_data_mode){
		COMMAND_MODE();
		is_in_data_mode = false;
	}
	
	HD44780_DATA_DDR = clear_bits(HD44780_DATA_DDR, DATA_MASK);	// On change temporairement la direction du data port
	HD44780_DATA_PORT = clear_bits(HD44780_DATA_PORT, DATA_MASK);	// On s'assure que les pull-up sont désactivées
	READ_MODE();
	
	uint8_t data = 0xFF;
	
	while(read_bit(data, BUSY_FLAG_BIT)){	// Tant que le busy flag vaut 1
		
		_delay_us(CLOCK_LOW_TIME_US);
		timeout_us -= CLOCK_LOW_TIME_US;	
				
		RISING_EDGE();			
		_delay_us(CLOCK_HIGH_TIME_US);	
		timeout_us -= CLOCK_HIGH_TIME_US;
					
		FALLING_EDGE();
		
#ifndef HD44780_ENABLE_4BIT_OPERATION	// Donc on veut une opération en 8 bit
		data = HD44780_DATA_PIN;	// Les données sont valides après un falling edge
#else									// Donc on veut une opération en 4bit
		uint8_t high_nibble = (HD44780_DATA_PIN & DATA_MASK) >> HD44780_DATA_SHIFT;	// Les données sont valides après un falling edge
		_delay_us(CLOCK_LOW_TIME_US);
		timeout_us -= CLOCK_LOW_TIME_US;
	
		RISING_EDGE();
		_delay_us(CLOCK_HIGH_TIME_US);
		timeout_us -= CLOCK_HIGH_TIME_US;
	
		FALLING_EDGE();
		uint8_t low_nibble = (HD44780_DATA_PIN & DATA_MASK) >> 4;	// Les données sont valides après un falling edge
	
		data = ((high_nibble << 4) & 0b11110000) | ((low_nibble << 0) & 0b00001111);
#endif
		
		if(timeout_us <= 0){	//Si le délai d'attente a été dépassé
			is_wait_successful = false;
			break;
		}
	}

	WRITE_MODE();
	HD44780_DATA_DDR = set_bits(HD44780_DATA_DDR, DATA_MASK);	// On change temporairement la direction du data port

	return is_wait_successful;
}


// ----------------------------------------------------------------------------
// Sous Module LCD (Accès indirect, mais plus facile au LCD)
//-----------------------------------------------------------------------------


// Constantes _________________________________________________________________

#define MAX_INDEX (LCD_NB_ROW * LCD_NB_COL)

// Variables globales statiques _______________________________________________

static uint8_t local_index;
static bool clear_required_flag;


// Déclaration de fonction privée _____________________________________________

bool shift_local_index(bool foward);
uint8_t index_to_col(uint8_t index);
uint8_t index_to_row(uint8_t index);


// Définition de fonction privée conditionnelle _______________________________

#ifdef LCD_REDIRECT_STDOUT_TO_LCD

static int lcd_putchar(char c, FILE* stream){

	lcd_write_char(c);
	return 0;
}

static FILE lcd_stdout = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);

#endif // LCD_REDIRECT_STDOUT_TO_LCD


// Définition de fonctions pupliques __________________________________________

void lcd_init(void){

	// On incrémente, sans curseur, sans blinking
	hd44780_init(true, false, false);

	local_index = 0;
	clear_required_flag = false;
	
#ifdef LCD_REDIRECT_STDOUT_TO_LCD

	stdout = &lcd_stdout;
	
#endif // LCD_REDIRECT_STDOUT_TO_LCD
}


void lcd_clear_display(){

	hd44780_clear_display();

	local_index = 0;
}

void lcd_clean_line(void){
	
	if(index_to_col(local_index) != 0){		// Si la colone == 0, c'est qu'on commence une nouvelle ligne, il n'y a rien à "cleaner" 
		
		uint8_t  initial_row = index_to_row(local_index);
		
		while(index_to_row(local_index) == initial_row){
			lcd_write_char(' ');
		}
	}
}

void lcd_set_cursor_col(uint8_t col){
	
	uint8_t row = index_to_row(local_index);
	
	lcd_set_cursor_position(row, col);
}

void lcd_set_cursor_row(uint8_t row){
	
	uint8_t col = index_to_col(local_index);
	
	lcd_set_cursor_position(row, col);
}

void lcd_set_cursor_position(uint8_t row, uint8_t col){

	if((col >= 0) && (col < LCD_NB_COL) && (row >= 0) && (row < LCD_NB_ROW)){

		hd44780_set_cursor_position(row, col);

		local_index = col + row * LCD_NB_COL;
	}
}


void lcd_shift_cursor(lcd_shift_e shift){

	switch(shift){
		case LCD_SHIFT_RIGHT:

		shift_local_index(true);

		break;

		case LCD_SHIFT_LEFT:

		shift_local_index(false);

		break;

		case LCD_SHIFT_UP:

		// Si on est sur la ligne du haut
		if(index_to_row(local_index) <= 0){

			// On se rend à la fin moins une ligne
			local_index += (MAX_INDEX - LCD_NB_COL);
			
			clear_required_flag = true;
		}

		else{
			
			// On recule d'une ligne
			local_index -= LCD_NB_COL;
		}

		break;

		case LCD_SHIFT_DOWN:

		// Si on est rendu à la dernière ligne
		if(index_to_row(local_index) >= LCD_NB_ROW - 1){

			// On ne garde que le numéro de colone (donc sa ramène sur la première ligne)
			local_index %= LCD_NB_COL;
			
			clear_required_flag = true;
		}

		else{

			// On avance d'une ligne
			local_index += LCD_NB_COL;
		}

		break;
		
		case LCD_SHIFT_END:
		
		local_index = ((index_to_row(local_index) + 1) * LCD_NB_COL) - 1;
		
		break;
		
		case LCD_SHIFT_START:
		
		local_index = index_to_row(local_index) * LCD_NB_COL;
		
		break;
		
		case LCD_SHIFT_TOP:
		
		// On ne garde que le numéro de colone (donc sa ramène sur la première ligne)
		local_index %= LCD_NB_COL;
		
		break;
		
		case LCD_SHIFT_BOTTOM:
		
		// On se rend à la fin moins une ligne
		local_index += (MAX_INDEX - LCD_NB_COL);
		
		break;
	}

	hd44780_set_cursor_position(index_to_row(local_index), index_to_col(local_index));
}


void lcd_set_cursor_style(bool is_visible, bool is_blinking){
	
	hd44780_set_display_control(true, is_visible, is_blinking);
}


void lcd_write_char(char character){

	bool unsynced;
	
	// S'il s'agit d'un des 32 premiers caractères ascii, on s'attend à un contrôle
	// plutôt qu'à l'affichage d'un caractère
	if(character >= 8 && character < ' '){
		
		switch (character){
		case '\n':	// 0x0A	new line
		//lcd_shift_cursor(LCD_SHIFT_DOWN);
		//lcd_shift_cursor(LCD_SHIFT_START);	// En 2024 j'ai décidé d'arriver en ville et d'accepter qu'un \n c'est une nouvelle ligne plutôt que \r\n
		lcd_clean_line();						// Plus tard, toujours en 2024, j'ai décidé que c'était beaucoup mieux (mais surtout plus logique) ainsi
			break;
		case '\r':
		lcd_shift_cursor(LCD_SHIFT_START);
			break;
		}
	}

	else{
		
		if(clear_required_flag == true){
			
			hd44780_clear_display();
			//hd44780_set_cursor_position(index_to_col(local_index), index_to_row(local_index));
			clear_required_flag = false;
		}
		
		hd44780_write_char(character);

		unsynced = shift_local_index(true);

		if(unsynced == true){

			hd44780_set_cursor_position(index_to_row(local_index), index_to_col(local_index));
		}
	}
}


void lcd_write_string(const char* string){

	while(*string != '\0'){

		lcd_write_char(*string);

		string++;
	}
}


void lcd_write_uint16(uint16_t u, int8_t min_int_digit){
	
	uint16_to_char_function(u, min_int_digit, &lcd_write_char);
}

void lcd_write_int16(int16_t i, int8_t min_int_digit){
	
	int16_to_char_function(i, min_int_digit, &lcd_write_char);
}

void lcd_write_float(float f, int8_t min_int_digit, int8_t nb_frac_digit,  bool show_positive_sign){
	
	float_to_char_function(f, min_int_digit, nb_frac_digit, show_positive_sign, &lcd_write_char);	// Le cast bizzare c'est juste pour faire disparaétre le warning du compilateur
}


// Définition de fonctions privées ____________________________________________

uint8_t index_to_col(uint8_t index){

	return index % LCD_NB_COL;
}


uint8_t index_to_row(uint8_t index){

	return index / LCD_NB_COL;
}


bool shift_local_index(bool foward){

	uint8_t previous_row;

	previous_row = index_to_row(local_index);

	/* Si on est dans le sens foward */
	if(foward == true){

		/* Si on est à la fin */
		if(local_index >= MAX_INDEX - 1){

			local_index = 0;
		}

		else{

			local_index++;
		}
	}

	/* Si on est dans le sens décrémental foward ou incrémental backward*/
	else{

		/* Si on est au début */
		if(local_index <= 0){
			
			local_index = MAX_INDEX - 1;
		}

		else{

			local_index--;
		}
	}

	/* Si la row actuelle ne correspond pas à l'ancienne il va falloir manuellement
	déplacer le curseur */
	return (previous_row != index_to_row(local_index));
}