import QtQuick 2.0
import qtrpi.gpio 1.0

Rectangle {

    width: 800
    height: 480

    color: "blue"

    Timer {
        interval:  100
        running: true
        onTriggered: {
            recRed.opacity = pinRed.input() === GPIO.High ? 1.0 : 0.1;
            recGreen.opacity = pinRed.input() === GPIO.High ? 1.0 : 0.1;
        }
    }

    Rectangle {
        id: recRed
        x:100
        y:100
        width: 200
        height: 200
        color: "red"
    }

    GPIO {
        id: pinRed
        pinMode: GPIO.Bcm
        pin: 22
        Component.onCompleted: {
            pinRed.setup(GPIO.Input);
        }
    }

    Rectangle {
        id: recGreen
        x:500
        y:100
        width: 200
        height: 200
        color: "green"
    }

    GPIO {
        id:pinGreen
        pinMode: GPIO.Bcm
        pin: 27
        Component.onCompleted: {
            pinRed.setup(GPIO.Input);
        }

    }
}
