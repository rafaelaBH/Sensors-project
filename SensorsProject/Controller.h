
#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <condition_variable>
#include <iostream>
#include <memory>
#include <thread>
#include <atomic>
#include <vector>
#include "Sensor.h"

using namespace std;

class Controller {

  private:
    int numTempSensors;
    int numHumSensors;
    vector<unique_ptr<Sensor>> sensors;
    vector<thread> threads;
    vector<atomic<bool>> flags; // using atomic so that the values are immediately updated in all threads
    condition_variable isUpdate;
    mutex m;
    struct Data {
      std::vector<double> tempValues;
      std::vector<double> humValues;
    };
    Data d;

    double avgCalc(vector<double>& v, int size);

  public:
    Controller(int numTemp, int numHum);
    Controller(Controller& c) = delete;
    Controller& operator=(const Controller& c) = delete;
    ~Controller();
    void runSensorThreads(); // start threads for all sensors
    void runMainThread(); // start main thread
    void stopThreads();
};



#endif //CONTROLLER_H
