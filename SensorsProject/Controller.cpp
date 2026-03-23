
#include "Controller.h"

#include <iostream>

# define MAX_TEMP_ALLOWED 30.0
# define MIN_TEMP_ALLOWED 10.0
# define MAX_HUM_ALLOWED 60.0
# define MIN_HUM_ALLOWED 40.0
# define MAX_DIST_TEMP 15.0
# define MAX_DIST_HUM 20.0

Controller::Controller(int numTemp, int numHum): numTempSensors(numTemp), mainFlag(false), numHumSensors(numHum),
d{vector<double>(numTemp, 0), vector<double>(numHum, 0)} {
    for (int i = 0; i < numTemp; ++i) {
        sensors.push_back(make_unique<TemperatureSensor>());
    }
    for (int i = 0; i < numHum; ++i) {
        sensors.push_back(make_unique<HumiditySensor>());
    }
    // making all vectors the same size
    threads.resize(sensors.size());
    needReplacement.resize(sensors.size());
    flags.resize(sensors.size());
    for (auto& f : flags) {
        f = make_unique<atomic<bool>>(false);
    }
    for (auto& n : needReplacement) {
        n = make_unique<atomic<bool>>(false);
    }
}

Controller::~Controller() {
    stopThreads();
}

void Controller::runSensorThreads() {
    for (int i = 0; i < static_cast<int>(sensors.size()); ++i) {
        flags[i]->store(true);
        auto lambda = [this, i]() {
            while (flags[i]->load()) {
                try {
                    sensors[i]->readValue();
                }
                catch (const runtime_error& e) {
                    needReplacement[i]->store(true);
                    this_thread::sleep_for(chrono::milliseconds(50));
                    continue;
                }
                {
                    std::lock_guard<mutex> lock(m); // lock d so that multiple threads won't change it simultaneously
                    if (i < numTempSensors) d.tempValues[i] = sensors[i]->getValue();
                    else d.humValues[i - numTempSensors] = sensors[i]->getValue();
                }
                this_thread::sleep_for(chrono::seconds(1)); // repeats every 1 second
            }
        }; //function that threads[i] will run
        threads[i] = thread(lambda);
    }
}

double Controller::avgCalc(vector<double>& v, int size) {
    if (size == 0) return 0.0;
    double sum = 0;
    for (double d : v) {
        sum += d;
    }
    return sum / size;
}

void Controller::replaceSensor(int indexD, int index, double avg) {
    if (!mainFlag.load()) return;
    const SensorType t = sensors[index]->type;
    // replace flawed sensor with a new one
    {
        lock_guard<mutex> lock(m);
        if (t == SensorType::Temperature) sensors[index] = std::make_unique<TemperatureSensor>();
        else sensors[index] = std::make_unique<HumiditySensor>();
        sensors[index]->writeToData(d, avg, indexD);
    }
}

void Controller::runMainThread() {
    while (mainFlag.load()) {
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
        for (int i = 0; i < numTempSensors; ++i) {
            if (abs(tempVector[i] - avgTemp) > MAX_DIST_TEMP) {
                cout << "Temperature sensor number " << i+1 << " is flawed!\n";
                needReplacement[i]->store(true);
            }
        }
        for (int i = 0; i < numTempSensors; ++i) {
            if (needReplacement[i]->load()) {
                cout << "Replacing temp sensor " << i+1 << " mid-run\n";
                needReplacement[i]->store(false);
                replaceSensor(i, i, avgTemp);
            }
        }
        for (int i = 0; i < numHumSensors; ++i) {
            if (abs(humVector[i] - avgHum) > MAX_DIST_HUM) {
                cout << "Humidity sensor number " << i+1 << " is flawed!\n";
                needReplacement[i+numTempSensors]->store(true);
            }
        }
        for (int i = 0; i < numHumSensors; ++i) {
            if (needReplacement[i+numTempSensors]->load()) {
                cout << "Replacing hum sensor " << i+1 << " mid-run\n";
                needReplacement[i+numTempSensors]->store(false);
                replaceSensor(i, i+numTempSensors, avgHum);
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

void Controller::startMainThread() {
    mainFlag = true;
    mainThread = std::thread(&Controller::runMainThread, this);
}

void Controller::stopThreads() {
    // stops all threads
    for (int i = 0; i < static_cast<int>(flags.size()); ++i) {
        flags[i]->store(false);
    }
    mainFlag.store(false);
    // joins all threads
    for (thread& t : threads) {
        if (t.joinable()) t.join();
    }
    if (mainThread.joinable()) mainThread.join();
}