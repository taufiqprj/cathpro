// Deklarasi pin
const int anodePin = 5; // Pin PWM untuk mengontrol arus anoda
const int referencePin = A1; // Pin analog untuk membaca sensor referensi

// Potensial target (mV)
const float targetPotential = -850.0;

// Variabel PID
float Kp = 2.0; // Koefisien Proportional
float Ki = 0.5; // Koefisien Integral
float Kd = 1.0; // Koefisien Derivative
float previousError = 0.0;
float integral = 0.0;

void setup() {
  pinMode(anodePin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // Baca nilai dari sensor referensi
  int sensorValue = analogRead(referencePin);
  float measuredPotential = sensorValue * (5000.0 / 1023.0); // Konversi nilai ADC ke mV

  Serial.print("Measured Potential: ");
  Serial.print(measuredPotential);
  Serial.println(" mV");

  // Hitung error
  float error = targetPotential - measuredPotential;

  // Hitung integral
  integral += error;

  // Hitung derivative
  float derivative = error - previousError;

  // Hitung output PID
  float output = Kp * error + Ki * integral + Kd * derivative;

  // Batasi output agar berada di rentang PWM (0-255)
  output = constrain(output, 0, 255);

  // Berikan arus ke anoda berdasarkan output PID
  analogWrite(anodePin, output);

  // Simpan error sebelumnya untuk perhitungan berikutnya
  previousError = error;

  // Tunggu sebelum loop berikutnya
  delay(50); // Pembacaan dilakukan setiap 1 detik
}