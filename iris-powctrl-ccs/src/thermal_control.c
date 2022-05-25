#include "main.h"
#include "thermal_control.h"
#include "peripheral_driver.h"

Limit_t ThermistorErrorLimits = {-273.0,100.0,0.05,0.05};
Limit_t ThermistorControlLimits = {12.0,20.0,0.0,0.0};

bool primary_thermistor_on = false;
bool thermistor_error_status[2] = {true,true};

void LowPowerThermalControl(void)
{

}

void MainThermalControl(void)
{
    // Check on the thermistors to ensure they are functional.
    CheckThermistorStatus();
    // Heater Control
    BatteryHeaterControl();
}

void BatteryHeaterControl(void)
{
    float tempCelsius;
    uint8_t thermistorStatus;
    // Get temperature
    thermistorStatus = GetThermistorStatus();
    switch(thermistorStatus)
    {
    case 0:
        // PROBLEM! NEITHER OF THE BATTERY SADDLE THERMISTORS ARE FUNCTIONING!!!
        // Switch to redundant heater?
        return;
    case 1:
        // Only the primary thermistor is working, so use it's reading
        tempCelsius = readTemperature(BATTERY_SADDLE_PRIMARY);
        break;
    case 2:
        // Only the secondary thermistor is working, so use it's reading
        tempCelsius = readTemperature(BATTERY_SADDLE_SECONDARY);
        break;
    case 3:
        // Both thermistors are working, so take an average
        tempCelsius = readTemperature(BATTERY_SADDLE_PRIMARY);
        tempCelsius += readTemperature(BATTERY_SADDLE_SECONDARY);
        tempCelsius /= 2;
        break;
    default:
        // Outside the range of status codes,
        // Must have been some sort of error,
        // So just return
        return;
    }
    // Enable/disable heater
    if(tempCelsius > ThermistorControlLimits.upper_value && primary_thermistor_on)
    {
        primary_thermistor_on = false;
        setLoadSwitch(LS_HTR,GPIO_INPUT_PIN_HIGH);
    }
    else if (tempCelsius < ThermistorControlLimits.lower_value && !primary_thermistor_on)
    {
        primary_thermistor_on = true;
        setLoadSwitch(LS_HTR,GPIO_INPUT_PIN_LOW);
    }
}

void CheckThermistorStatus(void)
{
    // Primary
    float primaryTemp = readTemperature(0);
    if(!CheckLimits(primaryTemp,&ThermistorErrorLimits)){
        thermistor_error_status[PRIMARY] = false;
        // Send error telemetry!!!
    }
    // Secondary
    float secondaryTemp = readTemperature(1);
    if(!CheckLimits(secondaryTemp,&ThermistorErrorLimits)){
        thermistor_error_status[SECONDARY] = false;
        // Send error telemetry!!!
    }
}

void SetThermistorStatus(bool primaryStatus, bool secondaryStatus)
{
    thermistor_error_status[PRIMARY] = primaryStatus;
    thermistor_error_status[SECONDARY] = secondaryStatus;
}
uint8_t GetThermistorStatus(void)
{
    uint8_t status = 0;
    if(thermistor_error_status[PRIMARY])
        status |= BIT0;
    if(thermistor_error_status[SECONDARY])
        status |= BIT1;
    return status;
}

void SetThermistorMargins(float lower, float upper)
{
    ThermistorErrorLimits.lower_margin = lower;
    ThermistorErrorLimits.upper_margin = upper;
}

bool CheckLimits(float value, Limit_t * limits)
{
    // Calculate limits
    float lower = limits->lower_value * (1.0 - limits->lower_margin);
    float upper = limits->upper_value * (1.0 - limits->upper_margin);
    return lower > value || value > upper;
}





