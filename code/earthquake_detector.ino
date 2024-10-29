#include <Wire.h>
#include <MPU6050.h>

// Définition des broches
const int buzzerPin = 9;
const int resetButtonPin = 2;  // Bouton de réinitialisation
MPU6050 mpu;

// Seuils de détection
const float seismicThreshold = 1.5; // Seuil en g

// Variables globales
float xAccel, yAccel, zAccel;
float totalAccel;
bool systemActive = true;     // État du système
bool alarmActive = false;

void setup() {
  // Configuration des broches
  pinMode(buzzerPin, OUTPUT);
  pinMode(resetButtonPin, INPUT_PULLUP);  // Bouton avec résistance pull-up
  
  // Démarrage de la communication série
  Serial.begin(9600);
  
  // Initialisation du MPU6050
  Wire.begin();
  mpu.initialize();
  
  // Message d'initialisation
  Serial.println("=== Système de détection sismique initialisé ===");
  Serial.println("Seuil d'alerte : 1.5g");
  Serial.println("Appuyez sur le bouton RESET pour réinitialiser après une alerte");
  Serial.println("-------------------------------------------");
  delay(1000);
}

void loop() {
  // Vérification du bouton de réinitialisation
  if (digitalRead(resetButtonPin) == LOW) {
    resetSystem();
    delay(500);  // Debounce
    return;
  }

  // Si le système est arrêté, on ne fait que clignoter le buzzer
  if (!systemActive) {
    blinkAlarm();
    return;
  }

  // Lecture des accélérations
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);
  
  // Conversion en g (±2g range)
  xAccel = ax / 16384.0;
  yAccel = ay / 16384.0;
  zAccel = az / 16384.0;
  
  // Calcul de l'accélération totale (magnitude du vecteur)
  totalAccel = sqrt(xAccel*xAccel + yAccel*yAccel + zAccel*zAccel);
  
  // Affichage des valeurs
  Serial.println("\n--- Lecture des accélérations ---");
  Serial.print("X: "); Serial.print(xAccel, 3); Serial.println("g");
  Serial.print("Y: "); Serial.print(yAccel, 3); Serial.println("g");
  Serial.print("Z: "); Serial.print(zAccel, 3); Serial.println("g");
  Serial.print("Accélération totale: "); Serial.print(totalAccel, 3); Serial.println("g");
  
  // Vérification du dépassement de seuil
  if (totalAccel > seismicThreshold) {
    Serial.println("!!! DÉPASSEMENT DU SEUIL !!!");
    triggerEmergencyState();
  }
  
  delay(100);
}

void triggerEmergencyState() {
  systemActive = false;  // Arrêt du système
  alarmActive = true;
  Serial.println("\n!!! ALERTE SISMIQUE !!!");
  Serial.println("Système en arrêt de sécurité");
  Serial.println("Appuyez sur RESET pour réinitialiser le système");
}

void blinkAlarm() {
  // Son d'alarme plus intense et continu
  tone(buzzerPin, 2000, 300);
  delay(400);
  tone(buzzerPin, 1000, 300);
  delay(400);
}

void resetSystem() {
  systemActive = true;
  alarmActive = false;
  noTone(buzzerPin);
  Serial.println("\n=== Système réinitialisé ===");
  Serial.println("Reprise de la surveillance...");
}