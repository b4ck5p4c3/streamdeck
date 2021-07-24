#include <Arduino.h>
#include <BleKeyboard.h>

// Sets the name of the bluetooth keyboard
// which shows up in the bluetooth menu of your device.
BleKeyboard bleKeyboard("B4CKSP4CE StreamDeck");

// Buttons. They can be a press buttons, radio buttons' groups [] or fix buttons *
// [ 22, 5, 26, 14, 0 ]
// *21, 32, *13, *34

constexpr size_t keystrokeSize = 3;

struct Key {
    uint8_t pin;
    int state;
    uint8_t sendOnRise[keystrokeSize];
    uint8_t sendOnFall[keystrokeSize];
};

static std::vector<Key> keys = { 
  // top row
  {
    .pin = 22,
    .state = 0,
    .sendOnRise = {KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_F1},
    .sendOnFall = {0, 0, 0},
  },
  {
    .pin = 5,
    .state = 0,
    .sendOnRise = {KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_F2},
    .sendOnFall = {0, 0, 0},
  },
  {
    .pin = 26,
    .state = 0,
    .sendOnRise = {KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_F3},
    .sendOnFall = {0, 0, 0},
  },
  {
    .pin = 14,
    .state = 0,
    .sendOnRise = {KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_F4},
    .sendOnFall = {0, 0, 0},
  },
  {
    .pin = 0,
    .state = 0,
    .sendOnRise = {KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_F5},
    .sendOnFall = {0, 0, 0},
  },
  // bottom row
  {
    .pin = 21,
    .state = 0,
    .sendOnRise = {KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_F9},
    .sendOnFall = {KEY_LEFT_CTRL, KEY_LEFT_ALT, KEY_F9},
  },
  {
    .pin = 32,
    .state = 0,
    .sendOnRise = {KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_F10},
    .sendOnFall = {KEY_LEFT_CTRL, KEY_LEFT_ALT, KEY_F10},
  },
  {
    .pin = 13,
    .state = 0,
    .sendOnRise = {KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_F11},
    .sendOnFall = {KEY_LEFT_CTRL, KEY_LEFT_ALT, KEY_F11},
  },
  {
    .pin = 34,
    .state = 0,
    .sendOnRise = {KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_F12},
    .sendOnFall = {KEY_LEFT_CTRL, KEY_LEFT_ALT, KEY_F12},
  }
};

void initKeysStates() {
    for (auto& key : keys) {
        pinMode(key.pin, INPUT_PULLDOWN);
        key.state = digitalRead(key.pin);
    }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE keyboard");

  bleKeyboard.begin();
  initKeysStates();
}

void handleEdge(Key& key) {
  const auto keystroke = key.state == 1 ? key.sendOnRise : key.sendOnFall;
  for (int i = 0; i < keystrokeSize; i++) {
    if (keystroke[i] == 0) {
      break;
    }
    bleKeyboard.press(keystroke[i]);
  }
  // release keys in backward order to prevent sending of unexpected keystrokes 
  // after release of modifiers
  for (int i = keystrokeSize - 1; i >= 0; i--) {
    if (keystroke[i] == 0) {
      break;
    }
    bleKeyboard.release(keystroke[i]);
  }
}

void loop() {
  if (bleKeyboard.isConnected()) {
    for (auto& key : keys) {
      const auto newState = digitalRead(key.pin);
      if (newState != key.state) {
        key.state = newState;
        handleEdge(key);
      }
    }
  }
  delay(10);
}
