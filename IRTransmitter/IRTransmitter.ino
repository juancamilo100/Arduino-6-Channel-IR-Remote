#include <Button.h>
#include <avr/wdt.h>
#include <IRremote.h>
#include <PinChangeInt.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>

#define INCREMENT_BUTTON_ONE_PIN (2)
#define DECREMENT_BUTTON_ONE_PIN (4)
#define INCREMENT_BUTTON_TWO_PIN (5)
#define DECREMENT_BUTTON_TWO_PIN (6)
#define RELAY_BUTTON_PIN (7)
#define CENTER_BUTTON_PIN (8)

#define INCREMENT_BUTTON_ONE_SIGNAL (0xA90)
#define DECREMENT_BUTTON_ONE_SIGNAL (0xF50)
#define INCREMENT_BUTTON_TWO_SIGNAL (0xC20)
#define DECREMENT_BUTTON_TWO_SIGNAL (0xE78)
#define CENTER_BUTTON_SIGNAL (0xD12)
#define RELAY_BUTTON_SIGNAL (0xB33)
#define SIGNAL_SIZE (12)

#define DEBOUNCE_MS (20)
#define PULLUP true        //To keep things simple, we use the Arduino's internal pullup resistor.
#define INVERT true        //Since the pullup resistor will keep the pin high unless the
#define PRESS_AND_HOLD 800

IRsend irsend;

int modulatingFrequencyKhz = 38;

Button incrementButton1(INCREMENT_BUTTON_ONE_PIN, PULLUP, INVERT, DEBOUNCE_MS);    //Declare the button
Button decrementButton1(DECREMENT_BUTTON_ONE_PIN, PULLUP, INVERT, DEBOUNCE_MS);    //Declare the button
Button incrementButton2(INCREMENT_BUTTON_TWO_PIN, PULLUP, INVERT, DEBOUNCE_MS);    //Declare the button
Button decrementButton2(DECREMENT_BUTTON_TWO_PIN, PULLUP, INVERT, DEBOUNCE_MS);    //Declare the button
Button relayButton(RELAY_BUTTON_PIN, PULLUP, INVERT, DEBOUNCE_MS);    //Declare the button
Button centerButton(CENTER_BUTTON_PIN, PULLUP, INVERT, DEBOUNCE_MS);    //Declare the button

void MonitorMotorButton(Button button, const unsigned int buttonSignal, char signalSize)
{
  if (button.wasPressed() || button.pressedFor(PRESS_AND_HOLD))
  {
    irsend.sendSony(buttonSignal, signalSize);
    delay(80);
  }
  else if (button.wasReleased())
  {
    delay(50);
    sleepNow();
  }
}

void MonitorRelayButton(Button button, const unsigned int buttonSignal, char signalSize)
{
    if (button.wasPressed())
  {
    Serial.println("Button Pressed");
    irsend.sendSony(buttonSignal, signalSize);
    delay(80);
  }
  else if (button.wasReleased())
  {
    Serial.println("Entering Sleep Mode");
    delay(50);
    sleepNow();
  }
}

void MonitorAllButtons()
{
  MonitorMotorButton(incrementButton1, INCREMENT_BUTTON_ONE_SIGNAL, SIGNAL_SIZE);
  MonitorMotorButton(decrementButton1, DECREMENT_BUTTON_ONE_SIGNAL, SIGNAL_SIZE);
  MonitorMotorButton(incrementButton2, INCREMENT_BUTTON_TWO_SIGNAL, SIGNAL_SIZE);
  MonitorMotorButton(decrementButton2, DECREMENT_BUTTON_TWO_SIGNAL, SIGNAL_SIZE);
  MonitorMotorButton(centerButton, CENTER_BUTTON_SIGNAL, SIGNAL_SIZE);
  MonitorRelayButton(relayButton, RELAY_BUTTON_SIGNAL, SIGNAL_SIZE);
}

void UpdateAllButtonState()
{
  incrementButton1.read();
  incrementButton2.read();
  decrementButton1.read();
  decrementButton2.read();
  centerButton.read();
  relayButton.read();
}

void setup()
{
  delay(20);
  sleepNow();
}

void loop() {
  UpdateAllButtonState();
  MonitorAllButtons();
}
/***********************************************/
void sleepNow()
{
  set_sleep_mode(SLEEP_MODE_PWR_SAVE);

  attachPinChangeInterrupt(INCREMENT_BUTTON_ONE_PIN, pinInterrupt, FALLING);
  attachPinChangeInterrupt(DECREMENT_BUTTON_ONE_PIN, pinInterrupt, FALLING);
  attachPinChangeInterrupt(INCREMENT_BUTTON_TWO_PIN, pinInterrupt, FALLING);
  attachPinChangeInterrupt(DECREMENT_BUTTON_TWO_PIN, pinInterrupt, FALLING);
  attachPinChangeInterrupt(RELAY_BUTTON_PIN, pinInterrupt, FALLING);
  attachPinChangeInterrupt(CENTER_BUTTON_PIN, pinInterrupt, FALLING);

  sleep_enable();
  sleep_mode();   // Put the device to sleep:

  // Upon waking up, sketch continues from this point.
  sleep_disable();
}

void pinInterrupt()
{
  detachInterrupt(0);
}
