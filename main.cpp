#include <iostream>
#include <memory>
#include <vector>
#include "Sensor.h"

using namespace std;

const int NUMTEMP = 2;
const int NUMHUM = 3;

int main()
{
    vector<unique_ptr<Sensor>> sensors;
    for (int i = 0; i < NUMTEMP; ++i)
        sensors.push_back(std::make_unique<TemperatureSensor>());
    for (int i = 0; i < NUMHUM; ++i)
        sensors.push_back(std::make_unique<HumiditySensor>());
}
