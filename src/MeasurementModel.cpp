// MeasurementModel.cpp
#include "MeasurementModel.h"

#include <Arduino.h>

MeasurementModel::MeasurementModel() {}

MeasurementModel::MeasurementModel(float value, String unitCode)
{
    Value = value;
    UnitCode = unitCode;
}