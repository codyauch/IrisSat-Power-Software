/*
 * thermal_control.h
 *
 *  Created on: May 10, 2022
 *      Author: Jayden McKoy
 */

#ifndef INCLUDE_THERMAL_CONTROL_H_
#define INCLUDE_THERMAL_CONTROL_H_

#define PRIMARY 0
#define SECONDARY 1
// Enums
// https://docs.google.com/spreadsheets/d/1kHfVPHJu_iTolEP6apsBMY5lSO3iS8NllYlZJ03JtWE/edit#gid=0
typedef enum
{
    BATTERY_SADDLE_PRIMARY,
    BATTERY_SADDLE_SECONDARY,
    BATTERY_SOLAR_PANEL_1,
    BATTERY_SOLAR_PANEL_2,
    BATTERY_SOLAR_PANEL_3,
    BATTERY_SOLAR_PANEL_4,
    PCU_PROCESSOR
} ADC_Bank_A_Channels;

// Structs
typedef struct {
    float lower_value, upper_value;
    float lower_margin, upper_margin;
} Limit_t;

// Functions
void LowPowerThermalControl(void);
void MainThermalControl(void);
void BatteryHeaterControl(void);
void CheckThermistorStatus(void);
void SetThermistorStatus(bool primaryStatus, bool secondaryStatus);
uint8_t GetThermistorStatus(void);
void SetThermistorMargins(float lower, float upper);
bool CheckLimits(float value, Limit_t * limits);

#endif /* INCLUDE_THERMAL_CONTROL_H_ */
