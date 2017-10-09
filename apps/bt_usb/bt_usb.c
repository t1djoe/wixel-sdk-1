/** bt_usb app:
This app allows you to turn a Wixel into a Bluetooth to USB adapter.
== Pinout ==
P1_6 = TX:  transmits data to Bluetooth module
P1_7 = RX:  receives data from Bluetooth module
*/

/*
 * TODO: Support for USB CDC ACM control signals.
 * TODO: use LEDs to give feedback about USB activity.
 * TODO: UART flow control
 * TODO: give feedback about framing, parity, overrun errors to user and also to USB host
 * TODO: allow any control signal (or inverted control signal) to be mapped
 *       to any Wixel pin
 * TODO: add parameter for placing TX and RX in any of the 4 available locations
 * TODO: add a parameter for doing inverted TTL serial on RX and TX.
 * TODO: support the SendBreak request?
 * TODO: look at other usb-to-serial converters to see what their default values
 *       are for DTR and RTS (the current defaults set by usb_cdc_acm.lib are RTS=1,DTR=0)
 */

/** Dependencies **************************************************************/
#include <cc2511_map.h>
#include <board.h>
#include <random.h>
#include <time.h>

#include <usb.h>
#include <usb_com.h>

#include <uart1.h>

/** Functions *****************************************************************/
void updateLeds()
{
    usbShowStatusWithGreenLed();
    LED_YELLOW(usbComRxControlSignals() & ACM_CONTROL_LINE_DTR);
    LED_RED(0);
}

void usbToUartService()
{
    uint8 signals;

    // Data
    while(usbComRxAvailable() && uart1TxAvailable())
    {
        uart1TxSendByte(usbComRxReceiveByte());
    }

    while(uart1RxAvailable() && usbComTxAvailable())
    {
        usbComTxSendByte(uart1RxReceiveByte());
    }

}

void lineCodingChanged()
{
    uart1SetBaudRate(usbComLineCoding.dwDTERate);
    uart1SetParity(usbComLineCoding.bParityType);
    uart1SetStopBits(usbComLineCoding.bCharFormat);
}

void main()
{
    systemInit();
    usbInit();
    usbComLineCodingChangeHandler = &lineCodingChanged;

    uart1Init();
    lineCodingChanged();

    while(1)
    {
        boardService();
        updateLeds();
        usbComService();
        usbToUartService();
    }
}
