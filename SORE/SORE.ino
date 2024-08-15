// Konfigurasi pin
const int referencePin = A1;  // Pin untuk membaca tegangan referensi
const int pwmPin = 5;         // Pin output PWM

// Variabel kontrol
double setpoint = 850;  // Setpoint dalam mV
double input, output;
double error;

bool firstIncrement = true;
bool firstDecrement = true;
double lastinput=0;

// Parameter kontrol
const double maxIncrementStep = 100.0;   // Langkah increment maksimum
const double maxDecrementStep = 20.0;  // Langkah decrement maksimum
const int deadband = 1;     // Zona mati dalam mV

// Batasan output
const double outMin = 0;
const double outMax = 240;

// Variabel untuk penghalusan
const int numReadings = 10;
double readings[numReadings];
int readIndex = 0;
double total = 0;

void setup() {
  pinMode(pwmPin, OUTPUT);
  Serial.begin(9600);
  
  // Inisialisasi array pembacaan
  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
  }
}

double calculateAdaptiveStep(double error, bool isDecrement) {
  // Hitung step berdasarkan jarak dari setpoint
  double absError = abs(error);
  double maxStep = isDecrement ? maxDecrementStep : maxIncrementStep;
  
  // Menggunakan kurva non-linear untuk memberikan respons yang lebih agresif pada error besar
  double normalizedError = absError / 500.0;  // Normalisasi error ke range 0-1
  double step = maxStep * pow(normalizedError, 0.7);  // Menggunakan eksponen 0.7 untuk kurva non-linear
  
  return constrain(step, 0.1, maxStep);
}

void loop() {
  // Membaca dan menghaluskan nilai referensi
  int rawValue = analogRead(referencePin);
  double currentReading = (rawValue / 1023.0) * 5000.0;
  
  total = total - readings[readIndex];
  readings[readIndex] = currentReading;
  total = total + readings[readIndex];
  readIndex = (readIndex + 1) % numReadings;
  
  input = total / numReadings;
  
  // Hitung error
  error = setpoint - input;
  
  // Kontrol output
if (abs(error) > deadband) {
    if (error > 0) {
        // Kenaikan (increment)
        
        
        double incrementStep = calculateAdaptiveStep(error, false);
        output = min(output + incrementStep, outMax);
        if (firstIncrement) {
            // This is the first increment
            Serial.println("First increment detected");
            firstIncrement = false; // Reset the flag
            output=output+240;
        }
        if(input > 0.45*setpoint){
          output=0;
          lastinput=input;
        }
        // Reset the firstDecrement flag when increment occurs
        firstDecrement = true;

    } else {
        // Penurunan (decrement)
        

        double decrementStep = calculateAdaptiveStep(error, true);
        output = max(output - decrementStep, outMin);
        
        // Reset the firstIncrement flag when decrement occurs
        // if (firstDecrement) {
        //     // This is the first decrement
        //     Serial.println("First decrement detected");
        //     firstDecrement = false; // Reset the flag
        //     output=output+100;
        // }
        firstIncrement = true;
    }
}
  
  // Terapkan output ke PWM dengan pulse modulation
  analogWrite(pwmPin, (int)output);
  delay(5);
  analogWrite(pwmPin, 0);
  delay(70);
  
  // Tampilkan informasi
  Serial.print("out ");
  Serial.print(output);
  Serial.print(" in ");
  Serial.print(input);
  Serial.print(" error ");
  Serial.print(error);
  Serial.print(" incStep ");
  Serial.print(calculateAdaptiveStep(error, false));
  Serial.print(" decStep ");
  Serial.println(calculateAdaptiveStep(error, true));
  
  // delay(10);  // Jeda singkat untuk stabilitas
}