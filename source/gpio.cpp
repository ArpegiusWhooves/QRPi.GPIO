#include "gpio.h"

#include <QDebug>

int GPIO::gpio_warnings = 1;

GPIO::GPIO(QObject *parent):
    QObject(parent),
    m_pin(0),
    m_pinMode(ModeUnknown)
{

}

GPIO::~GPIO()
{

}

#ifndef  QTRPI_MOCKUP

static int mmap_gpio_mem(void)
{
   int result;

   if (module_setup)
      return 0;

   result = setup();
   if (result == SETUP_DEVMEM_FAIL)
   {
      qCritical() << "No access to /dev/gpiomem .  Try running as root!";
      return 1;
   } else if (result == SETUP_MALLOC_FAIL) {
      qCritical() << "No memmory!";
      return 2;
   } else if (result == SETUP_MMAP_FAIL) {
      qCritical() << "Mmap of GPIO registers failed";
      return 3;
   } else { // result == SETUP_OK
      module_setup = 1;
      return 0;
   }
}

int GPIO::chan_from_gpio(int gpio)
{
   int chan;
   int chans;

   if (m_pinMode == BCM)
      return gpio;
   if (rpiinfo.p1_revision == 0)   // not applicable for compute module
      return -1;
   else if (rpiinfo.p1_revision == 1 || rpiinfo.p1_revision == 2)
      chans = 26;
   else
      chans = 40;
   for (chan=1; chan<=chans; chan++)
      if (*(*pin_to_gpio+chan) == gpio)
         return chan;
   return -1;
}

struct q_callback
{
   unsigned int gpio;
   QObject* cb;
   struct q_callback *next;
};
static struct q_callback *q_callbacks = nullptr;



GPIO::PinState GPIO::input()
{
    unsigned int gpio;

    switch (get_gpio_number(m_pinMode,m_pin, &gpio)) {
        case 0:
            break;
        case 3:
            qWarning() <<"Please set pin numbering mode using pinMode to GPIO.Board or GPIO.Bcm";
        return;
        case 4:
            qWarning() <<"The pin number is outside of valid cha on a Raspberry Pi";
        return;
        case 5:
            qWarning() <<"The pin is invalid on a this Raspberry Pi board.";
        return;
        default:
            break;
    }

    // check channel is set up as an input or output
    if (gpio_direction[gpio] != INPUT && gpio_direction[gpio] != OUTPUT)
    {
       qWarning() << "You must setup() the GPIO channel first";
       return Unknown;
    }

    if (check_gpio_priv())
       return Unknown;

    if (input_gpio(gpio)) {
       return High;
    }
    return Low;
}

void GPIO::setPin(int pin)
{
    if (m_pin == pin)
        return;

    m_pin = pin;
    emit pinChanged(m_pin);
}

void GPIO::setup(PinDirection direction, PinPull pud, PinState initial)
{
    // check module has been imported cleanly
    if (setup_error)
    {
       qCritical() << "Module not imported correctly!";
       return;
    }

    if (mmap_gpio_mem())
       return;

    if (direction != INPUT && direction != OUTPUT) {
       qWarning() << "An invalid direction was passed to setup()";
       return;
    }

    if(direction == OUTPUT)
        pud = PullOff;

    if (pud != PUD_OFF && pud != PUD_DOWN && pud != PUD_UP) {
       qWarning() << "Invalid value for pull_up_down - should be either PullOff, PullDown or PullUp";
       return;
    }

    unsigned int gpio;

    switch (get_gpio_number(m_pinMode,m_pin, &gpio)) {
        case 0:
            break;
        case 3:
            qWarning() <<"Please set pin numbering mode using pinMode to GPIO.Board or GPIO.Bcm";
        return;
        case 4:
            qWarning() <<"The pin number is outside of valid cha on a Raspberry Pi";
        return;
        case 5:
            qWarning() <<"The pin is invalid on a this Raspberry Pi board.";
        return;
        default:
            break;
    }

    int func = gpio_function(gpio);

    if (gpio_warnings &&                             // warnings enabled and
        ((func != 0 && func != 1) ||                 // (already one of the alt functions or
        (gpio_direction[gpio] == -1 && func == 1)))  // already an output not set from this program)
    {
       qWarning() << "This channel is already in use, continuing anyway. Use setwarnings(False) to disable warnings.";
    }

    if (rpiinfo.p1_revision == 0) { // compute module - do nothing
    } else if ((rpiinfo.p1_revision == 1 && (gpio == 0 || gpio == 1)) ||
               (gpio == 2 || gpio == 3)) {
       qWarning() << "A physical pull up resistor is fitted on this channel!";
    }

    if (direction == OUTPUT && (initial == LOW || initial == HIGH)) {
       output_gpio(gpio, initial);
    }

    setup_gpio(gpio, direction, pud);
    gpio_direction[gpio] = direction;

}

void GPIO::cleanup()
{
    unsigned int gpio;
    switch (get_gpio_number(m_pinMode,m_pin, &gpio)) {
        case 0:
            break;
        case 3:
            qWarning() <<"Please set pin numbering mode using pinMode to GPIO.Board or GPIO.Bcm";
        return;
        case 4:
            qWarning() <<"The pin number is outside of valid cha on a Raspberry Pi";
        return;
        case 5:
            qWarning() <<"The pin is invalid on a this Raspberry Pi board.";
        return;
        default:
            break;
    }

    event_cleanup(gpio);

    // set everything back to input
    if (gpio_direction[gpio] != -1) {
       setup_gpio(gpio, INPUT, PUD_OFF);
       gpio_direction[gpio] = -1;
    }
}

void GPIO::setPinMode(GPIO::PinMode new_mode)
{
    if (m_pinMode == new_mode)
        return;

    if (m_pinMode != MODE_UNKNOWN && new_mode != m_pinMode)
    {
       qWarning() << "A different mode has already been set!";
       return;
    }

    if (setup_error)
    {
       qCritical() << "Module not imported correctly!";
       return;
    }

    if (new_mode != BOARD && new_mode != BCM)
    {
       qWarning() << "An invalid mode was passed to setPinMode()";
       return;
    }

    if (rpiinfo.p1_revision == 0 && new_mode == BOARD)
    {
       qWarning() << "Board numbering system not applicable on compute module";
       return;
    }

    m_pinMode = new_mode;

    emit pinModeChanged(m_pinMode);
}

void GPIO::output(bool value)
{
    unsigned int gpio;

    switch (get_gpio_number(m_pinMode,m_pin, &gpio)) {
        case 0:
            break;
        case 3:
            qWarning() <<"Please set pin numbering mode using pinMode to GPIO.Board or GPIO.Bcm";
        return;
        case 4:
            qWarning() <<"The pin number is outside of valid cha on a Raspberry Pi";
        return;
        case 5:
            qWarning() <<"The pin is invalid on a this Raspberry Pi board.";
        return;
        default:
            break;
    }

    if (gpio_direction[gpio] != OUTPUT)
    {
       qWarning() << "The GPIO channel has not been set up as an OUTPUT";
       return;
    }

    if (check_gpio_priv())
       return;

    output_gpio(gpio, value);
}
#else
void GPIO::setPin(int pin){}
void GPIO::setup(PinDirection direction, PinPull pud, PinState initial) {}
void GPIO::cleanup(){}
void GPIO::setPinMode(GPIO::PinMode new_mode) {}
GPIO::PinState GPIO::input(){ return Unknown; }
void GPIO::output(bool value){}
#endif
