// Inclusions _________________________________________________________________

#include "pid.h"

#include <math.h>

#include "utils.h"
#include "imu.h"
#include "twi.h"

// Déclaration de variables globales privées __________________________________

static float static_kp = 0.0;
static float static_ki = 0.0;
static float static_kd = 0.0;

static float static_fix_rate = 0.0;

static float previous_error = 0.0;
static float cumulative_error = 0.0;
static float equilibrium_angle = 0.0;

static float theta_rad = NAN;

// Déclaration de constantes globales privées _________________________________

// ACCÉLÉROMÈTRE ----------------------------------------------------------
// Constante qui représente la multiplication à faire pour passer de
// la valeur arbitraire signée sur 16 bits à une valeur en unitées concrètes.
// Numérateur : plage max d'accélération soit 8G
// Dénominateur : valeur max d'un 16 bits signé soit 2^15 - 1
static const float ACCELEROMETER_TO_G_MULTIPLIER = 8. / 32767.;

// GYROSCOPE ----------------------------------------------------------
// Constante qui représente la multiplication à faire pour passer de
// la valeur arbitraire signée sur 16 bits à une valeur en unitées concrètes.
// Numérateur : plage max de la vitesse angulaire soit 250°/s (250 /180 * PI = 4,363 rad/s)
// Dénominateur : valeur max d'un 16 bits signé soit 2^15 - 1
const float GYROSCOPE_TO_ANGULAR_VELOCITY_MULTIPLIER = 4.363 / 32767.;

// Déclaration de la constante delta temps;
const float DELTA_T = 1. / 50.;

// Définition de fonctions publiques __________________________________________

void pid_init(void){
	
	static_kp = 0.0;
	static_ki = 0.0;
	static_kd = 0.0;
	
	static_fix_rate = 0.0;
	theta_rad = NAN;
	
	// Initialisation de la communication I2C (TWI) et du capteur IMU
	twi_init();
	imu_init();
	
	// ACCÉLÉROMÈTRE ----------------------------------------------------------
	// Configuration de l'accéléromètre à une fréquence d'échantillonage de 50Hz et une plage max d'accélération de 8G
	imu_set_accelerometer_config(DATA_RATE_50HZ, ACCELERATION_RANGE_8G);
	
	// GYROSCOPE ----------------------------------------------------------
	// Configuration du gyroscope à une fréquence d'échantillonage de 50Hz et une plage max de la vitesse angulaire (250°/s)
	imu_set_gyroscope_config(DATA_RATE_50HZ, GYROSCOPE_RANGE_250DPS);
}

void pid_set_gains(float kp, float ki, float kd){
	static_kp = kp;
	static_ki = ki;
	static_kd = kd;
}

void pid_set_fix_rate(float fix_rate){
	static_fix_rate = fix_rate;
}

bool pid_is_new_data_ready(void){
	return imu_is_new_data_ready();
}


float pid_get_current_angle(void){
	
	// Rafraîchissement/Sauvegarde l'échantillon (nouvelles données de l'unité de mesure inertielle (imu))
	imu_refresh();
	
	/************** ACCÉLÉROMÈTRE : ANGLE DÉDUIT DE L'ACCÉLÉRATION **************/
	// Lecture de la valeur de l’échantillon de l'accéléromètre dans l'axe x et y
	int16_t raw_accel_x = imu_get_acceleration_x(); 
	int16_t raw_accel_y = imu_get_acceleration_y();
	
	// Conversion d'un nombre arbitraire entre -32768 et 32767 à un nombre en G de -8 à 8 G
	float real_accel_x = (float)raw_accel_x * ACCELEROMETER_TO_G_MULTIPLIER;
	float real_accel_y = (float)raw_accel_y * ACCELEROMETER_TO_G_MULTIPLIER;
	
	// Conversion d'une valeur x et y pour obtenir un angle en radian (0 RAD à la verticale)
	float theta_accel_rad = atan2f(-real_accel_y, real_accel_x);
	/***************************************************************************/
	
	
	/************ GYROSCOPE : ANGLE DÉDUIT DE LA VITESSE ANGULAIRE ************/
	// Lecture de la valeur de l’échantillon du gyroscope dans l'axe z
	int16_t raw_gyro_z = imu_get_angular_velocity_z();
	
	// Conversion d'un nombre arbitraire entre -32768 et 32767 à une vitesse angulaire de -4.363 rad/s à 4.363 rad/s
	float angular_velocity = (float)raw_gyro_z * GYROSCOPE_TO_ANGULAR_VELOCITY_MULTIPLIER;

	// Intégrale de la vitesse angulaire dans le temps pour obtenir une position angulaire
	float theta_gyro_rad_increment = angular_velocity * DELTA_T;
	/***************************************************************************/
	
	
	/********************** COMBINAISON DES 2 ANGLES EN 1 **********************/
	// Ratio de l'angle final provenant du gyroscope et de l'accéléromètre
	const float GYROSCOPE_RATIO = 0.98; // On se fie à 98% au gyro pour la stabilité à court terme
	const float ACCELEROMETER_RATIO = 1.0 - GYROSCOPE_RATIO;
	
	// Si on est au premier tour
	if(isnan(theta_rad)){
		// L'angle final doit être celui fourni à 100% par l'accéléromètre
		theta_rad = theta_accel_rad;
	}
	// Si on n'est pas au premier tour
	else{
		// L'angle final doit être un mélange des deux angles fournis par le gyroscope et l'accéléromètre selon les ratios choisis
		theta_rad = (theta_rad + theta_gyro_rad_increment) * GYROSCOPE_RATIO + (theta_accel_rad * ACCELEROMETER_RATIO);
	}
	
	return theta_rad;
}

float pid_get_equilibrium_angle(void){
	return equilibrium_angle;
}

float pid_tic(float desired_angle, float current_angle){
	
	// Erreur
	float current_error = equilibrium_angle + desired_angle - current_angle;
	
	// Dérivée de l'erreur
	float delta_error = (current_error - previous_error) / DELTA_T;	
	
	// Intégrale de l'erreur
	cumulative_error += current_error * DELTA_T;
	
	// Formule semi-magique pour trouver de combien on doit modifier le vrai angle d'équilibre
	float equilibrium_angle_increment = static_fix_rate * cumulative_error + static_fix_rate * current_error;
	equilibrium_angle += equilibrium_angle_increment;
	
	// On sauvegarde l'erreur actuelle pour faire la dérivée au prochain tour de boucle
	previous_error = current_error;
	
	// On retourne la valeur du PID		
	return (current_error * static_kp + delta_error * static_kd + cumulative_error * static_ki) * 100.0;
}

void pid_reset(void){
	
	previous_error = 0;
	cumulative_error = 0;
	equilibrium_angle = 0;
	
	// Changer la valeur de theta_rad pour forcer la fonction pid_get_current_angle() à retourner 100%
	// du theta calculé à l'accéléromètre pour un seul tic
	theta_rad = NAN;
}