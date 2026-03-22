
#include "Sensor.h"
#include "random"
#include "stdio.h"
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

// This keeps the Kalman logic encapsulated and private
void Sensor::readValue() {
    setValue(kalmanFilter());
}


