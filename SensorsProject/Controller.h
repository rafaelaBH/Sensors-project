
#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <condition_variable>
#include <memory>
#include <thread>
#include <atomic>
#include <vector>
#include "Sensor.h"
#include "SensorsData.h"

using namespace std;

class Controller {

  private:
    int numTempSensors;
    atomic<bool> mainFlag; // atomic so that the values are immediately updated in all threads
    int numHumSensors;
    thread mainThread;
    vector<unique_ptr<Sensor>> sensors;
    vector<thread> threads;
    vector<unique_ptr<atomic<bool>>> flags; // atomic so that the values are immediately updated in all threads
    mutex m;
    SensorsData d;
    vector<unique_ptr<atomic<bool>>> needReplacement; // atomic so that the values are immediately updated in all threads
    friend class ControllerTest; //for testing

    double avgCalc(vector<double>& v, int size);
    void runMainThread();

  public:
    Controller(int numTemp, int numHum);
    Controller(Controller& c) = delete;
    Controller& operator=(const Controller& c) = delete;
    ~Controller();
    void runSensorThreads(); // start threads for all sensors
    void stopThreads();
    void startMainThread();
    void replaceSensor(int indexD, int index, double newVal);
};



#endif //CONTROLLER_H
