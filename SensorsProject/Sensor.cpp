
#include "Sensor.h"

#include <stdexcept>

#include "random"
#include <cstdio>
using namespace std;

// Adds noise to value using normal distribution to simulate a small noise
double Sensor::addNoise() {
    static default_random_engine rand; // The engine is initialized to a const value, we want a different value every time
    normal_distribution<double> noise(0, var);
    return value + noise(rand);
}

//returns the estimated value after adding noise
double Sensor::kalmanFilter() {
    double kalmanGain = error / (error+var);
    double valueWithNoise = addNoise();
    estimatedValue = estimatedValue + kalmanGain * (valueWithNoise - estimatedValue);
    error = (1 - kalmanGain) * error;
    return estimatedValue;
}

// This keeps the Kalman logic encapsulated and private + simulates flaws in sensors
void Sensor::readValue() {
    double r = rand() / static_cast<double>(RAND_MAX);
    if (r < 0.05) setValue(rand() % 51 + 70);
    else if (r < 0.1) setValue(rand() % 21 - 50);
    else if (r < 0.2) throw runtime_error("flawed sensor - not working");
    setValue(kalmanFilter());
}

void TemperatureSensor::writeToData(SensorsData &d, double val, int i) {
    d.tempValues[i] = val;
}

void HumiditySensor::writeToData(SensorsData &d, double val, int i) {
    d.humValues[i] = val;
}




