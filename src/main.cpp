#include <Arduino.h>
#include <IRremote.h>
#include <WiFi.h>
#include <esp_now.h>
#include "messages.h"

uint8_t towerMac[] = { 0x2C, 0xBC, 0xBB, 0x0D, 0xFB, 0x90 };

esp_now_peer_info_t peerInfo;

shoot_message shotMessage;

btn_press_message btnMessage;

void OnDataSent(const uint8_t *macAddress, esp_now_send_status_t status) {
  Serial.print("\r\n Packet Send Status::\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

const int IR_LED_PIN = 23;
const int RED_BTN_PIN = 25;
const int TRIGGER_BTN_PIN = 33;
const int HAMMER_BTN_PIN = 32;
const int SAFETY_BTN_PIN = 25;

// const uint8_t RED_SHOOT_COMMAND = 0x35;
// const uint8_t BLUE_SHOOT_COMMAND = 0x7;
const uint8_t SHOOT_COMMAND = 0x7;
const uint8_t SHOOT_COMMAND_REPEATS = 3; 

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, towerMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  pinMode(IR_LED_PIN, OUTPUT);
  // pinMode(RED_BTN_PIN, INPUT_PULLUP);
  // pinMode(BLUE_BTN_PIN, INPUT_PULLUP);
  pinMode(TRIGGER_BTN_PIN, INPUT_PULLUP);
  pinMode(HAMMER_BTN_PIN, INPUT_PULLUP);
  pinMode(SAFETY_BTN_PIN, INPUT_PULLUP);

  IrSender.begin(IR_LED_PIN);
}

void sendButtonMessage(uint8_t button, bool value) {
  btnMessage.type = 2;
  btnMessage.button = button;
  btnMessage.value = value;
  
  esp_err_t result = esp_now_send(towerMac, (uint8_t *) &btnMessage, sizeof(btnMessage));
  if (result == ESP_OK) {
    Serial.println("Btn [" + (String)button + "] message sent successfully!");
  } else {
    Serial.println("Failed to send btn trigger message");
  }
}

// void shoot(bool isRed)
void handleTriggerClick()
{
  sendButtonMessage(0, true);

  uint8_t shootCommand = SHOOT_COMMAND;
  IrSender.sendNEC(0x00, shootCommand, SHOOT_COMMAND_REPEATS);
}

void handleSafetyClick() {
  sendButtonMessage(2, true);
}

void handleHammerClick() {
  sendButtonMessage(1, true);
}

void loop() {
  // int redBtnState = digitalRead(RED_BTN_PIN);
  // int blueBtnState = digitalRead(BLUE_BTN_PIN);
  int triggerBtnState = digitalRead(TRIGGER_BTN_PIN);
  int hammerBtnState = digitalRead(HAMMER_BTN_PIN);
  int safetyBtnState = digitalRead(SAFETY_BTN_PIN);
  // Serial.println("red: " + String(redBtnState));
  // Serial.println("blue: " + String(blueBtnState));
  if (triggerBtnState == LOW) {
    handleTriggerClick();
    delay(500);
  }
  if (safetyBtnState == LOW) {
    handleSafetyClick();
    delay(500);
  }
  if (hammerBtnState == LOW) {
    handleHammerClick();
    delay(500);
  }
  // if (redBtnState == LOW) {
  //   shoot(RED_SHOOT_COMMAND);
  //   delay(500);
  // }
  // else if (blueBtnState == LOW) {
  //   shoot(BLUE_SHOOT_COMMAND);
  //   delay(500);
  // }
}