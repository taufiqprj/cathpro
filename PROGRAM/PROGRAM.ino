const int selectorPin1 = 14;
const int selectorPin2 = 15;
const int selectorPin3 = 16;
const int pwmOutputPin = 17;
const int buttonPin = 4;
const int potPin = A1;
const int lm35Pin = A2;

int selectorState = 0;
int buttonState = 0;
int potValue = 0;
int tempValue = 0;
int pwmValue = 0;

void setup() {
  pinMode(selectorPin1, INPUT_PULLUP);
  pinMode(selectorPin2, INPUT_PULLUP);
  pinMode(selectorPin3, INPUT_PULLUP);
  pinMode(pwmOutputPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  
  Serial.begin(9600);
}

void loop() {
  // Baca status selector
  if (digitalRead(selectorPin1) == LOW) {
    selectorState = 1;
  } else if (digitalRead(selectorPin2) == LOW) {
    selectorState = 2;
  } else if (digitalRead(selectorPin3) == LOW) {
    selectorState = 3;
  } else {
    selectorState = 0;
  }

  // Baca nilai potensio dan suhu
  potValue = analogRead(potPin);
  tempValue = analogRead(lm35Pin);
  
  // Konversi nilai analog LM35 ke suhu dalam Celsius
  float tempCelsius = (tempValue * 5.0 / 1024.0) * 100;
  
  // Tentukan nilai PWM berdasarkan selector
  switch (selectorState) {
    case 1:
      pwmValue = 0;  // Mati
      break;
    case 2:
      pwmValue = map(potValue, 0, 1023, 0, 255);  // Berdasarkan potensio
      break;
    case 3:
      pwmValue = map(tempCelsius, 0, 100, 0, 255);  // Berdasarkan suhu (0-100°C)
      break;
    default:
      pwmValue = 0;  // Mati jika tidak ada selector yang aktif
  }

  // Output PWM
  analogWrite(pwmOutputPin,pwmValue);

  // Tampilkan informasi di Serial Monitor
  Serial.print("Selector: ");
  Serial.print(selectorState);
  Serial.print(" | Potensio: ");
  Serial.print(potValue);
  Serial.print(" | Suhu: ");
  Serial.print(tempCelsius);
  Serial.print("°C | PWM: ");
  Serial.println(pwmValue);

  delay(100);  // Delay kecil untuk stabilitas pembacaan
}