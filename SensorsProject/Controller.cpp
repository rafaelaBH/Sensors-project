
#include "Controller.h"

# define MAX_TEMP_ALLOWED 30.0
# define MIN_TEMP_ALLOWED 10.0
# define MAX_HUM_ALLOWED 75.0
# define MIN_HUM_ALLOWED 50.0
# define MAX_DIST_TEMP 15.0
# define MAX_DIST_HUM 20.0

Controller::Controller(int numTemp, int numHum): numTempSensors(numTemp), numHumSensors(numHum),
d{vector<double>(numTemp, 0), vector<double>(numHum, 0)} {
    for (int i = 0; i < numTemp; ++i) {
        sensors.push_back(make_unique<TemperatureSensor>());
    }
    for (int i = 0; i < numHum; ++i) {
        sensors.push_back(make_unique<HumiditySensor>());
    }
    // making all vectors the same size
    threads.resize(sensors.size());
    flags.resize(sensors.size(), true);
}

Controller::~Controller() {
    stopThreads();
}

void Controller::runSensorThreads() {
    for (int i = 0; i < sensors.size(); ++i) {
        auto lambda = [this, i]() {
            while (flags[i]) {
                sensors[i]->readValue();
                std::lock_guard<mutex> lock(m); // lock d so that multiple threads won't change it simultaneously
                if (i < numTempSensors) d.tempValues[i] = sensors[i]->getValue();
                else d.humValues[i - numTempSensors] = sensors[i]->getValue();
                this_thread::sleep_for(chrono::seconds(1)); // repeats every 1 second
            }
        }; //function that threads[i] will run
        threads[i] = thread(lambda);
    }
}

double Controller::avgCalc(vector<double>& v, int size) {
    double sum = 0;
    for (double d : v) {
        sum += d;
    }
    return sum / size;
}


void Controller::runMainThread() {
    while (true) {
        vector<double> tempVector;
        vector<double> humVector;
        // separate block so that the lock automatically releases after the ending bracket
        {
            std::lock_guard<mutex> lock(m);
            tempVector = d.tempValues;
            humVector = d.humValues;
        }
        double avgTemp = avgCalc(tempVector, numTempSensors);
        double avgHum = avgCalc(humVector, numHumSensors);
        for (int i = 0; i < numTempSensors; i++) {
            if (tempVector[i] > avgTemp + MAX_DIST_TEMP || tempVector[i] < avgTemp - MAX_DIST_TEMP) {
                cout << "Temperature sensor number " << i+1 << " is flawed!\n";
            }
        }
        for (int i = 0; i < numHumSensors; i++) {
            if (humVector[i] > avgHum + MAX_DIST_HUM || humVector[i] < avgHum - MAX_DIST_HUM) {
                cout << "Humidity sensor number " << i+1 << " is flawed!\n";
            }
        }
        if (avgTemp > MAX_TEMP_ALLOWED) {
            cout << "Turn on AC!\n";
        }
        if (avgTemp < MIN_TEMP_ALLOWED) {
            cout << "Turn off AC!\n";
        }
        if (avgHum > MAX_HUM_ALLOWED) {
            cout << "Turn on dehumidifier!\n";
        }
        if (avgHum < MIN_HUM_ALLOWED) {
            cout << "Turn on humidifier!\n";
        }
        this_thread::sleep_for(chrono::seconds(2));
    }
}


void Controller::stopThreads() {
    // stops all threads
    for (int i = 0; i < flags.size(); ++i) {
        flags[i] = false;
    }
    // joins all threads
    for (thread& t : threads) {
        if (t.joinable()) t.join();
    }
}