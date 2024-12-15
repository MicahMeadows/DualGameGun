#include <Arduino.h>
#include <IRremote.h>

const int IR_LED_PIN = 23;
const int RED_BTN_PIN = 25;
const int BLUE_BTN_PIN = 33;

const uint8_t RED_SHOOT_COMMAND = 0x35;
const uint8_t BLUE_SHOOT_COMMAND = 0x7;
const uint8_t SHOOT_COMMAND_REPEATS = 3; 

void setup() {
  Serial.begin(115200);
  pinMode(IR_LED_PIN, OUTPUT);
  pinMode(RED_BTN_PIN, INPUT_PULLUP);
  pinMode(BLUE_BTN_PIN, INPUT_PULLUP);

  IrSender.begin(IR_LED_PIN);
}

void shoot(uint8_t command)
{
  IrSender.sendNEC(0x00, command, SHOOT_COMMAND_REPEATS);
}

void loop() {
  int redBtnState = digitalRead(RED_BTN_PIN);
  int blueBtnState = digitalRead(BLUE_BTN_PIN);
  Serial.println("red: " + String(redBtnState));
  Serial.println("blue: " + String(blueBtnState));
  if (redBtnState == LOW) {
    shoot(RED_SHOOT_COMMAND);
    delay(1000);
  }
  else if (blueBtnState == LOW) {
    shoot(BLUE_SHOOT_COMMAND);
    delay(1000);
  }
}