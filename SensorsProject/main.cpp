#include <iostream>
#include <thread>
#include <vector>
#include <cassert>
#include <memory>
#include <chrono>
#include "Controller.h"
#include "ControllerTest.h"

using namespace std;


int main() {
    // Seed for the random flaws in your Sensor class
    srand(time(0));

    ControllerTest::RunAllTests();

    return 0;
}
