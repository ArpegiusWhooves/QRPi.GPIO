#include "qrpigpio_plugin.h"
#include "gpio.h"

#include <qqml.h>
#include <cstdlib>

#include <QDebug>

void QRPiGPIOPlugin::registerTypes(const char *uri)
{
    // @uri qrpi.gpio
    // detect board revision and set up accordingly

#ifndef QTRPI_MOCKUP
    if (get_rpi_info(&rpiinfo))
    {
       qDebug() << "This module can only be run on a Raspberry Pi!";
       setup_error = 1;
       return;
    }

    if (rpiinfo.p1_revision == 1) {
       pin_to_gpio = &pin_to_gpio_rev1;
    } else if (rpiinfo.p1_revision == 2) {
       pin_to_gpio = &pin_to_gpio_rev2;
    } else { // assume model B+ or A+ or 2B
       pin_to_gpio = &pin_to_gpio_rev3;
    }

    if( std::atexit( cleanup ) )
    {
        cleanup();
        return;
    }

    if( std::atexit( event_cleanup_all ) )
    {
        event_cleanup_all();
        return;
    }

    if(!setup()){
        return;
    }
#endif

    qmlRegisterType<GPIO>(uri, 1, 0, "GPIO");

}

