#ifndef IMU_H_INCLUDED
#define IMU_H_INCLUDED

#include "utils.h"

typedef enum {
	
	ACCELERATION_RANGE_2G = 0,
	ACCELERATION_RANGE_4G = 1,
	ACCELERATION_RANGE_8G = 2,	// Défaut
	ACCELERATION_RANGE_16G = 3,
	
}imu_accelerometer_range_e;

typedef enum {
	
	GYROSCOPE_RANGE_2000DPS = 0,	// Défaut
	GYROSCOPE_RANGE_1000DPS = 1,
	GYROSCOPE_RANGE_500DPS = 2,	
	GYROSCOPE_RANGE_250DPS = 3,
	GYROSCOPE_RANGE_125DPS = 4,
	
}imu_gyroscope_range_e;

typedef enum {
	
	// DATA_RATE_12HZ,	// Incompatible avec le gyroscope
	DATA_RATE_25HZ	= 0x06,
	DATA_RATE_50HZ	= 0x07,
	DATA_RATE_100HZ	= 0x08,
	DATA_RATE_200HZ	= 0x09,
	DATA_RATE_400HZ = 0x0A,
	DATA_RATE_800HZ = 0x0B,
	DATA_RATE_1K6HZ	= 0x0C,
	// DATA_RATE_3K2HZ,	// Incompatible avec l'accéléromètre
	// DATA_RATE_6K4HZ,	// Incompatible avec l'accéléromètre
	
}imu_data_rate_e;


typedef enum {
	
	ERROR_NO_ERROR = 0,					// Aucune erreur
	ERROR_INITIAL_COMMUNICATION,		// Erreur de communication avec le IMU (probablement mal branché)
	ERROR_INVALID_CHIP_ID,				// Le CHIP ID n'est pas 0x24 (WTF)
	ERROR_ONGOING_COMMUNICATION,
	ERROR_INIT_TIMEOUT_BUSTED,
	ERROR_ACCELEROMETER_CONFIG_FAILED,	// Erreur de communication avec le IMU pour établir configurer l'accéléromètre
	ERROR_GYROSCOPE_CONFIG_FAILED,		// Erreur de communication avec le IMU pour établir la plage du gyroscope
	ERROR_INTERRUPT_CONFIG_FAILED,		// Erreur de communication avec le IMU pour activer l'interrupt
	ERROR_READ_STATUS_FAILED,			// Erreur de communication avec le IMU pour lire le status
	ERROR_READ_DATA_FAILED,				// Erreur de communication avec le IMU pour lire les valeurs de l'accéléromètre et du gyroscope
	ERROR_DATA_READY_TIMEOUT_BUSTED,
	ERROR_DATA_IS_INVALID,
	
}imu_error_e;



bool imu_init(void);

bool imu_set_accelerometer_config(imu_data_rate_e data_rate, imu_accelerometer_range_e accelerometer_range);
bool imu_set_gyroscope_config(imu_data_rate_e data_rate, imu_gyroscope_range_e gyroscope_range);

bool imu_set_data_ready_interrupt(bool interrupt_enabled);

bool imu_is_new_data_ready(void);

bool imu_refresh(void);

int16_t imu_get_acceleration_x(void);
int16_t imu_get_acceleration_y(void);
int16_t imu_get_acceleration_z(void);
int16_t imu_get_angular_velocity_x(void);
int16_t imu_get_angular_velocity_y(void);
int16_t imu_get_angular_velocity_z(void);

imu_error_e imu_get_last_error(void);

#endif /* IMU_H_INCLUDED */