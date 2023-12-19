// MeasurementModel.h
#ifndef MeasurementModel_h
#define MeasurementModel_h

#include <Arduino.h>
#include "MeasurementModel.h"

class MeasurementModel
{
private:
public:
    float Value;
    String UnitCode;
    MeasurementModel();
    MeasurementModel(float value, String unitCode);
};
#endif
