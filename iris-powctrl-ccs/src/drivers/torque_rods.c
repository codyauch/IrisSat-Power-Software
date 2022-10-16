
#include "torque_rods.h"
//#include "pwm_basic.h"


float dipoleToVoltage(float dipole) {

    if(abs(dipole) > MAX_DIPOLE) {
        return MAX_VOLTAGE * ((dipole >= 0) ? 1 : -1); //convert to proper sign with ternary
    }

    float reqVoltage = 0.0;
    reqVoltage = dipole / DIPOLE_SLOPE;

    if(abs(reqVoltage) > MAX_VOLTAGE) {
        return MAX_VOLTAGE * ((reqVoltage >= 0) ? 1 : -1);
    } else {
        return reqVoltage;
    }

}

int getSign(float val) {

}
