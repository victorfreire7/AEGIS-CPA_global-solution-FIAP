#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 10, 5, 4, 3, 2);

int backLight    = 13;
int TEMPERATURA  = A0;
int LUMINOSIDADE = A1;
int VIBRACAO     = A2;

// Limiares de alerta
const float TEMP_MAX   = 35.0;
const float TEMP_MIN   = 10.0;
const int   LUZ_MAX    = 900;
const int   LUZ_MIN    = 50;
const int   VIB_LIMITE = 600;

void setup() {
  pinMode(backLight, OUTPUT);
  digitalWrite(backLight, HIGH);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("AEGIS  Capsule");
  lcd.setCursor(0, 1);
  lcd.print("Inicializando...");

  Serial.begin(9600);
  delay(2000);
}

void loop() {
  // --- Leituras ---
  int   vibRaw  = analogRead(VIBRACAO);
  float voltage = analogRead(TEMPERATURA) * (5.0 / 1024.0);
  float temp    = (voltage - 0.5) * 100.0;
  int   leitura = 1023 - analogRead(LUMINOSIDADE);

  // --- Avaliação de alertas ---
  bool alertaTemp = (temp > TEMP_MAX || temp < TEMP_MIN);
  bool alertaVib  = (vibRaw > VIB_LIMITE);
  bool alertaLuz  = (leitura > LUZ_MAX || leitura < LUZ_MIN);
  bool alerta     = alertaTemp || alertaVib || alertaLuz;

  // --- LCD ---
  lcd.clear();
  if (alerta) {
    lcd.setCursor(0, 0);
    lcd.print("! ALERTA: ");
    if      (alertaTemp) lcd.print("TEMP");
    else if (alertaVib)  lcd.print("VIB ");
    else if (alertaLuz)  lcd.print("LUZ ");

    lcd.setCursor(0, 1);
    if      (alertaTemp) { lcd.print("Temp:"); lcd.print(temp, 1); lcd.print("C"); }
    else if (alertaVib)  { lcd.print("Vib:");  lcd.print(vibRaw); }
    else if (alertaLuz)  { lcd.print("Luz:");  lcd.print(leitura); }
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Temp:");
    lcd.print(temp, 1);
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("Luz:");
    lcd.print(leitura);
    lcd.print(" Vib:");
    lcd.print(vibRaw);
  }

  // --- Telemetria JSON via Serial ---
  Serial.print("{");
  Serial.print("\"temp\":");   Serial.print(temp, 1);              Serial.print(",");
  Serial.print("\"lux\":");    Serial.print(leitura);               Serial.print(",");
  Serial.print("\"vib\":");    Serial.print(vibRaw);                Serial.print(",");
  Serial.print("\"alerta\":"); Serial.print(alerta ? "true" : "false");
  Serial.println("}");

  delay(1000);
}