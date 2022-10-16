

//#include "pwm_basic.h"

/*
Converts a requested dipole value to a PWM duty cycle value. This uses a linear approximation of the dipole test results found 
at https://docs.google.com/spreadsheets/d/1HabvoySxrTA8_qCVNMNoQwynStzFnOvv/edit#gid=589234245

The linear approximation has a slope of 0.04 Am^2 / V, therefore the voltage for a requested dipole, D, is V = D / 0.04

The maximum dipole is +-0.2Am^2 at 5V
*/

#define DIPOLE_SLOPE 0.04

#define MAX_DIPOLE 0.2

#define MAX_VOLTAGE 5.0

#define MAX_PWM 255.0

float dipoleToVoltage(float dipole);
