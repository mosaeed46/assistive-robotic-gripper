#include <Servo.h>

Servo s;

const int buttonPin = 7;
const int servoPin = 3;

const int STOP_US = 1480;
const int FWD_US = 1650;
const int REV_US = 1270;

const unsigned long MAX_FWD_MS = 3750;

bool lastButton = HIGH;
int mode = 0;
// 0 = idle
// 1 = moving forward
// 2 = stopped, waiting for return press
// 3 = returning

unsigned long forwardStartTime = 0;
unsigned long forwardHeldTime = 0;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  s.attach(servoPin);
  s.writeMicroseconds(STOP_US);
}

void loop() {
  bool b = digitalRead(buttonPin);

  // Auto-stop if the forward time limit is reached.
  if (mode == 1 && millis() - forwardStartTime >= MAX_FWD_MS) {
    forwardHeldTime = MAX_FWD_MS;
    mode = 2;
    s.writeMicroseconds(STOP_US);
  }

  // Respond once per button press (HIGH to LOW transition).
  if (lastButton == HIGH && b == LOW) {
    if (mode == 0) {
      mode = 1;
      forwardStartTime = millis();
      s.writeMicroseconds(FWD_US);
    } else if (mode == 1) {
      forwardHeldTime = millis() - forwardStartTime;
      mode = 2;
      s.writeMicroseconds(STOP_US);
    } else if (mode == 2) {
      mode = 3;
      forwardStartTime = millis();
      s.writeMicroseconds(REV_US);
    }
  }

  // Stop after the return duration matches the forward duration.
  if (mode == 3 && millis() - forwardStartTime >= forwardHeldTime) {
    mode = 0;
    s.writeMicroseconds(STOP_US);
  }

  lastButton = b;
  delay(20);
}
