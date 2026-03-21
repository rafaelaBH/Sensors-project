
#ifndef SENSOR_H
#define SENSOR_H



class Sensor {

  private:
    double value;
    double error;
    double estimatedValue;
    double var;

    double addNoise();
    double kalmanFilter();

  public:
    explicit Sensor(double val = 0, double err = 1, double est = 0, double v = 0.7) :
    value(val), error(err), estimatedValue(est), var(v) {}
    Sensor(Sensor& s) = delete;
    virtual ~Sensor() = default;
    virtual void readValue();

  protected:
    double getValue() const { return value;}
    void setValue(const double newVal) { value = newVal;}
};

class TemperatureSensor : public Sensor {

  public:
    TemperatureSensor() : Sensor(0, 0.8, 25.8, 0.9) {}
    ~TemperatureSensor() override = default;
};

class HumiditySensor : public Sensor {

  public:
    HumiditySensor() : Sensor(0, 0.9, 50.3, 0.5) {}
    ~HumiditySensor() override = default;
};



#endif //SENSOR_H
