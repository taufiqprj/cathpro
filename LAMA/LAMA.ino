#define REFERENCE A1

#define VREF 5.0F
#define ADCBIT 1023.0

float mapFloat(float x, float inmin, float inmax, float outmin, float outmax) {
  return (x - inmin) * (outmax - outmin) / (inmax - inmin) + outmin;
}

void setup(void) {
  Serial.begin(9600);
  pinMode(REFERENCE, INPUT);
}

void loop(void) {
  float ireference = ADCBIT - analogRead(REFERENCE);
  float rreference = map(ireference, 0, 870, 0, ADCBIT);
  float vreference = rreference * (VREF / ADCBIT);
  float oreference = mapFloat(vreference, 4.7, 0.7, 0.0, VREF);
  Serial.print(ireference);
  Serial.print(" ");
  Serial.print(rreference);
  Serial.print(" ");
  Serial.println(vreference);
  delay(100);
}
