#ifndef GPIO_H
#define GPIO_H

#ifndef QTRPI_MOCKUP
#include "c_gpio.h"
#include "event_gpio.h"
#include "cpuinfo.h"
#include "common.h"
#endif

#include <QObject>


class GPIO : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(GPIO)
    Q_PROPERTY(PinMode pinMode READ pinMode WRITE setPinMode NOTIFY pinModeChanged)
    Q_PROPERTY(int pin READ pin WRITE setPin NOTIFY pinChanged)

public:
    enum PinMode {
#ifndef QTRPI_MOCKUP
        ModeUnknown = MODE_UNKNOWN,
        Board = BOARD,
        Bcm = BCM,
        Serial = SERIAL,
        Spi = SPI,
        I2c = I2C,
        Pwm = PWM
#else
        ModeUnknown,Board,Bcm,Serial,Spi,I2c,Pwm
#endif
    };
    Q_ENUM(PinMode)

    enum PinDirection {
#ifndef QTRPI_MOCKUP
        Input = INPUT,
        Output = OUTPUT,
        Alt0 = ALT0
#else
        Input,Output,Alt0
#endif
    };
    Q_ENUM(PinDirection)

    enum PinState {
#ifndef QTRPI_MOCKUP
        High = HIGH,
        Low = LOW,
        Unknown = -1
#else
        High,Low,Unknown
#endif
    };
    Q_ENUM(PinState)

    enum PinPull {
#ifndef QTRPI_MOCKUP
        PullOff = PUD_OFF,
        PullDown = PUD_DOWN,
        PullUp = PUD_UP
#else
        PullOff,PullDown,PullUp
#endif
    };
    Q_ENUM(PinPull)

private:
    int chan_from_gpio(int gpio);

    int m_pin;
    PinMode m_pinMode;

    static int gpio_warnings;

public:
    GPIO(QObject *parent = nullptr);
    ~GPIO();
    int pin() const
    {
        return m_pin;
    }

    Q_INVOKABLE PinState input();

    PinMode pinMode() const
    {
        return m_pinMode;
    }

public slots:
    void setPin(int pin);
    void setPinMode(PinMode pinMode);

    void output(bool value);

    void setup(PinDirection direction, PinPull pull_up_down = PullOff, PinState initial = Unknown);
    void cleanup();


signals:
    void pinChanged(int pin);

    void pinModeChanged(PinMode pinMode);
};

#endif // GPIO_H
