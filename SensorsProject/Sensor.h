
#ifndef SENSOR_H
#define SENSOR_H

#include "SensorsData.h"

enum class SensorType {
  Temperature,
  Humidity
};

class Sensor {

  private:
    double value;
    double error;
    double estimatedValue;
    double var;

    double addNoise();
    double kalmanFilter();

  public:

    SensorType type;
    explicit Sensor(double val = 0, double err = 1, double est = 0, double v = 0.7,
      SensorType t = SensorType::Temperature) :
    value(val), error(err), estimatedValue(est), var(v), type(t) {}
    Sensor(Sensor& s) = delete;
    Sensor& operator=(const Sensor& s) = delete;
    virtual ~Sensor() = default;
    virtual void readValue();
    double getValue() const { return value;}
    virtual void writeToData(SensorsData& d, double val, int i) = 0;

  protected:
    void setValue(const double newVal) { value = newVal;}
};

class TemperatureSensor : public Sensor {

  public:
    TemperatureSensor() : Sensor(25.8, 0.8, 25.8, 0.9, SensorType::Temperature) {}
    ~TemperatureSensor() override = default;
    void writeToData(SensorsData& d, double val, int i) override;
};

class HumiditySensor : public Sensor {

  public:
    HumiditySensor() : Sensor(50.3, 0.9, 50.3, 0.5, SensorType::Humidity) {}
    ~HumiditySensor() override = default;
    void writeToData(SensorsData& d, double val, int i) override;
};



#endif //SENSOR_H
