#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>

#include "utils.h"
#include "uart.h"
#include "pid.h"
#include "motor.h"
#include "servo.h"
#include "lcd.h" 

int main(void) {
	
	// Forcer broche RESET du ESP8266 à 1
	DDRA = set_bit(DDRA, PA6);
	PORTA = set_bit(PORTA, PA6);
	
	// Délai de 3 secondes
	_delay_ms(4000);
	
	// Initialisations
	uart0_init();
	uart0_clean_rx_buffer();
	motor_init();
	servo_init(); 
	pid_init();
	lcd_init();
	
	// Gains PID
	pid_set_gains(2.0, 7.0, 0.02);
	pid_set_fix_rate(0.04);
	
	sei();

	char cmd_recue[40];
	
	// Variables reçues de la manette (8 bits)
	int8_t valeur_x = 0;
	int8_t valeur_y = 0;
	uint8_t valeur_b = 0;
	
	const float SPEED_EXTRA_MAX = 0.20;
	const float SPEED_EXTRA_RATE = 2.5;
	float speed_extra = 0.0;
	float speed_delta = 0.0;

	while(1) {

		if(uart0_rx_buffer_nb_line() > 0) {
			
			uart0_get_line(cmd_recue, 40);
			
			sscanf(cmd_recue, "x=%hhd y=%hhd b=%hhd", &valeur_x, &valeur_y, &valeur_b);
			
			printf("%s", cmd_recue);
			
			if (valeur_b == 1) {
				pid_reset(); 
				valeur_b = 0; 
			}
			
		}

		if(pid_is_new_data_ready()) {
			
			float theta_rad = pid_get_current_angle();
			
			// Calcul effort pour equilibre
			float speed_pid = pid_tic(0.0, theta_rad);

			// Acceleration du robot
			float speed_goal = valeur_y * SPEED_EXTRA_MAX;
			
			if(speed_extra > speed_goal) {
				speed_extra -= SPEED_EXTRA_RATE;
				} else if (speed_extra < speed_goal) {
				speed_extra += SPEED_EXTRA_RATE;
			}
			
			// Calcul rotation (gauche/droite)
			speed_delta = valeur_x * 0.2;

			// Puissance finale roues
			float speed_total = speed_pid + speed_extra;
			
			motor_ctrl_left(-speed_total + speed_delta);
			motor_ctrl_right(-speed_total - speed_delta);
		}
	}
	
	return 0;
}

