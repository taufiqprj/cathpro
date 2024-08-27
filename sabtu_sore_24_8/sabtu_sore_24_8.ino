#include <EEPROM.h>

#define ANALOG_IN_PIN A2
#define CLK 6
#define DT 7
#define SW 8
#define SELECTOR_1 2
#define SELECTOR_2 3

const int vref = A3;
float pwm = 0;
float vin;
float vout;
float vrefval;
const float factor = 5.128;
const float vcc = 5.00;
float adc_voltage = 0.0;
float in_voltage = 0.0;
float adc_voltage2 = 0.0;
float in_voltage2 = 0.0;
float R1 = 30000.0;
float R2 = 7500.0;
float ref_voltage = 5.0;
float ref_voltage2 = 5.0;
int adc_value = 0;
int adc_value2 = 0;
float batas_atas;
float batas_bawah;
float setPoint = 0.3;

int currentStateCLK;
int lastStateCLK;
String currentDir = "";
unsigned long lastButtonPress = 0;

enum Mode { OFF,
            MANUAL,
            AUTO };
Mode currentMode = OFF;
Mode lastMode = OFF;
bool adjustingSetPoint = true;

// EEPROM addresses
const int SETPOINT_ADDR = 0;
const int PWM_ADDR = 4;

void setup() {
  pinMode(5, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);
  pinMode(SELECTOR_1, INPUT_PULLUP);
  pinMode(SELECTOR_2, INPUT_PULLUP);
  Serial.begin(9600);

  // Read saved values from EEPROM
  EEPROM.get(SETPOINT_ADDR, setPoint);
  EEPROM.get(PWM_ADDR, pwm);

  updateLimits();

  lastStateCLK = digitalRead(CLK);
}

void loop() {
  updateMode();
  handleRotaryEncoder();
  led();

  if (currentMode == AUTO) {
    runAutoMode();
  } else if (currentMode == MANUAL) {
    runManualMode();
  }



  // Save to EEPROM when mode changes to MANUAL or AUTO
  if (currentMode != lastMode && (currentMode == MANUAL || currentMode == AUTO)) {
    // EEPROM.put(SETPOINT_ADDR, setPoint);
    // EEPROM.put(PWM_ADDR, pwm);
    // Serial.println("Values saved to EEPROM");
  }

  lastMode = currentMode;
}

void updateMode() {
  if (digitalRead(SELECTOR_1) == LOW && digitalRead(SELECTOR_2) == HIGH) {
    currentMode = MANUAL;
  } else if (digitalRead(SELECTOR_1) == HIGH && digitalRead(SELECTOR_2) == LOW) {
    currentMode = AUTO;
  } else {
    currentMode = OFF;
  }
}

void led() {
  if (currentMode == OFF) {
    digitalWrite(9, 1);
    digitalWrite(10, 0);
    digitalWrite(11, 0);
    analogWrite(5, 0);
  }
  if (currentMode == MANUAL) {
    digitalWrite(9, 0);
    digitalWrite(10, 1);
    digitalWrite(11, 0);
  }
  if (currentMode == AUTO) {
    digitalWrite(9, 0);
    digitalWrite(10, 0);
    digitalWrite(11, 1);
  }
}

void handleRotaryEncoder() {
  currentStateCLK = digitalRead(CLK);

  if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {
    if (digitalRead(DT) != currentStateCLK) {
      currentDir = "CCW";
    } else {
      currentDir = "CW";
    }

    if (currentMode == MANUAL) {
      if (true) {
        pwm += (currentDir == "CW" ? 1 : -1);
        pwm = constrain(pwm, 0, 255);
      } else {
        pwm += (currentDir == "CW" ? 1 : -1);
        pwm = constrain(pwm, 0, 255);
      }
    }
    // else if (currentMode == MANUAL) {
    //   pwm += (currentDir == "CW" ? 1 : -1);
    //   pwm = constrain(pwm, 0, 255);
    // }
    readVoltage();
    displayInfo();
    EEPROM.put(PWM_ADDR, pwm);
  }

  lastStateCLK = currentStateCLK;

  int btnState = digitalRead(SW);
  if (btnState == LOW && millis() - lastButtonPress > 50) {
    if (currentMode == MANUAL) {
      // adjustingSetPoint = !adjustingSetPoint;
      Serial.print("save setPoint  ");
      readVoltage();
      Serial.println(in_voltage2);
      setPoint = in_voltage2; 
      EEPROM.put(SETPOINT_ADDR, in_voltage2);
      updateLimits();
    }
    lastButtonPress = millis();
    delay(300);
  }
}

void updateLimits() {
  batas_bawah = setPoint * 97 / 100;
  batas_atas = setPoint * 103 / 100;
}

void readVoltage() {
  float average = 0.0;
  for (int i = 0; i < 20; i++) {
    adc_value2 = analogRead(A2);
    adc_voltage2 = (adc_value2 * ref_voltage2) / 1024.0;
    in_voltage2 = adc_voltage2 / (R2 / (R1 + R2));
    average += in_voltage2;
    delay(3);
  }
  in_voltage2 = average / 20;
}

void runAutoMode() {
  readVoltage();
  if (in_voltage2 > batas_bawah && in_voltage2 < batas_atas) {
    // Do nothing, PWM stays the same
  } else if (in_voltage2 < batas_bawah) {
    pwm += 1;
  } else if (in_voltage2 > batas_atas) {
    pwm -= 1;
  }

  pwm = constrain(pwm, 1, 250);
  analogWrite(5, round(pwm));
  displayInfo();
}

void runManualMode() {
  // readVoltage();
  analogWrite(5, round(pwm));
  // displayInfo();
}

void displayInfo() {
  Serial.print("Mode: ");
  Serial.print(currentMode == OFF ? "OFF" : (currentMode == MANUAL ? "MANUAL" : "AUTO"));
  Serial.print(" | SetPoint: ");
  Serial.print(setPoint, 2);
  Serial.print(" | PWM: ");
  Serial.print(pwm, 2);
  Serial.print(" | Lower Limit: ");
  Serial.print(batas_bawah, 2);
  Serial.print(" | Upper Limit: ");
  Serial.print(batas_atas, 2);
  if (currentMode == AUTO || currentMode == MANUAL) {
    Serial.print(" | Voltage: ");
    Serial.print(in_voltage2, 2);
  }
  Serial.println();
}