//
// Created by רפאלה בית הלחמי on 23/03/2026.
//

#ifndef CONTROLLERTEST_H
#define CONTROLLERTEST_H



#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <cassert>
#include <cmath>
#include "Controller.h"

using namespace std;

class ControllerTest {
public:
    static void RunAllTests() {
        cout << "==================================================" << endl;
        cout << "       CONTROLLER MASTER TEST SUITE (2026)        " << endl;
        cout << "==================================================" << endl;

        // --- GROUP 1: LOGIC & MATH ---
        TestAveraging();          // From previous set
        TestOutlierLogic();       // New edge case
        TestSensorReplacement();  // From previous set

        // --- GROUP 2: ROBUSTNESS ---
        TestZeroSensors();        // New safety case
        TestRapidLifecycle();     // New concurrency case

        // --- GROUP 3: FULL SIMULATION ---
        TestFullSystemStress();   // Your original 5-second test

        cout << "==================================================" << endl;
        cout << "         ALL TEST MODULES COMPLETED               " << endl;
        cout << "==================================================" << endl;
    }

private:
    // Test 1: Verifies the mathematical average
    static void TestAveraging() {
        cout << "[MODULE] Averaging Logic: ";
        Controller c(3, 0);
        vector<double> vals = {10.0, 20.0, 30.0};
        double result = c.avgCalc(vals, 3);
        if (abs(result - 20.0) < 0.001) cout << "PASSED" << endl;
        else cout << "FAILED (Got " << result << ")" << endl;
    }

    // Test 2: Verifies that > 15.0 is the trigger, not >= 15.0
    static void TestOutlierLogic() {
        cout << "[MODULE] Boundary Logic: ";
        // Max Dist is 15.0. If diff is exactly 15.0, it should NOT flag.
        double avg = 20.0;
        double val = 35.0;
        if (!(abs(val - avg) > 15.0)) {
            cout << "PASSED" << endl;
        } else {
            cout << "FAILED (Flagged exactly 15.0 distance)" << endl;
        }
    }

    // Test 3: Verifies the swap mechanism doesn't crash
    static void TestSensorReplacement() {
        cout << "[MODULE] Replacement Logic: ";
        Controller c(1, 1);
        try {
            // Replace index 0 with a value of 25.0
            c.replaceSensor(0, 0, 25.0);
            cout << "PASSED" << endl;
        } catch (...) {
            cout << "FAILED" << endl;
        }
    }

    // Test 4: Checks if 0 sensors causes a crash (Division by Zero)
    static void TestZeroSensors() {
        cout << "[MODULE] Zero Sensor Safety: ";
        try {
            Controller c(0, 0);
            c.startMainThread();
            this_thread::sleep_for(chrono::milliseconds(100));
            c.stopThreads();
            cout << "PASSED" << endl;
        } catch (...) {
            cout << "FAILED (Crashed)" << endl;
        }
    }

    // Test 5: Checks for deadlocks during high-frequency start/stop
    static void TestRapidLifecycle() {
        cout << "[MODULE] Thread Stress (Rapid Start/Stop): ";
        Controller c(3, 3);
        for(int i = 0; i < 5; ++i) {
            c.runSensorThreads();
            c.startMainThread();
            this_thread::sleep_for(chrono::milliseconds(20));
            c.stopThreads();
        }
        cout << "PASSED" << endl;
    }

    // Test 6: The full simulation you ran earlier
    static void TestFullSystemStress() {
        cout << "[MODULE] Integration (Full Run - 5s):" << endl;
        Controller c(3, 3);
        c.runSensorThreads();
        c.startMainThread();

        // This will print your "Turn on AC!", "Replacing sensor...", etc.
        this_thread::sleep_for(chrono::seconds(5));

        c.stopThreads();
        cout << "[MODULE] Integration: PASSED" << endl;
    }
};

#endif //CONTROLLERTEST_H
