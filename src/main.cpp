#include <Arduino.h>
#include <IRremote.h>
#include <WiFi.h>
#include <esp_now.h>
#include "messages/shoot_message.h"

uint8_t towerMac[] = { 0x2C, 0xBC, 0xBB, 0x0D, 0xFB, 0x90 };

esp_now_peer_info_t peerInfo;

shoot_message shotMessage;

void OnDataSent(const uint8_t *macAddress, esp_now_send_status_t status) {
  Serial.print("\r\n Packet Send Status::\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

const int IR_LED_PIN = 23;
const int RED_BTN_PIN = 25;
const int BLUE_BTN_PIN = 33;

const uint8_t RED_SHOOT_COMMAND = 0x35;
const uint8_t BLUE_SHOOT_COMMAND = 0x7;
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
  pinMode(RED_BTN_PIN, INPUT_PULLUP);
  pinMode(BLUE_BTN_PIN, INPUT_PULLUP);

  IrSender.begin(IR_LED_PIN);
}

void shoot(bool isRed)
{
  shotMessage.type = 1;
  shotMessage.isRed = isRed;
  esp_err_t result = esp_now_send(towerMac, (uint8_t *) &shotMessage, sizeof(shotMessage));

  if (result == ESP_OK) {
    Serial.println("Shot message sent successfully!");
  } else {
    Serial.println("Failed to send shot message");
  }

  uint8_t shootCommand = isRed ? RED_SHOOT_COMMAND : BLUE_SHOOT_COMMAND;
  IrSender.sendNEC(0x00, shootCommand, SHOOT_COMMAND_REPEATS);

}

void loop() {
  int redBtnState = digitalRead(RED_BTN_PIN);
  int blueBtnState = digitalRead(BLUE_BTN_PIN);
  // Serial.println("red: " + String(redBtnState));
  // Serial.println("blue: " + String(blueBtnState));
  if (redBtnState == LOW) {
    shoot(RED_SHOOT_COMMAND);
    delay(500);
  }
  else if (blueBtnState == LOW) {
    shoot(BLUE_SHOOT_COMMAND);
    delay(500);
  }
}