#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// Blynk authorization token
#define BLYNK_TEMPLATE_ID "TMPL2qeU5yWIk"
#define BLYNK_TEMPLATE_NAME "moteur diseal"
#define BLYNK_AUTH_TOKEN "MLJ3HFHa4DPvy-_CkCfIg4k2IVCZ1lOQ"
char auth[] = "MLJ3HFHa4DPvy-_CkCfIg4k2IVCZ1lOQ";
// WiFi credentials
const char* ssid = "HUAWEI P20 Pro";
const char* password = "322ef93fcf80";

#define RELAY_PIN 2
#define TEMPERATURE_PIN 3
#define PRESSURE_PIN 4
#define WATER_PRESSURE_PIN 5
 int temperature,pressure, waterPressure;
 
// Thresholds
const int seuilTempMax = 90;        // Seuil de température maximale acceptable pour le moteur
const int seuilPressionHuileMin = 25;  // Seuil minimal de pression d'huile acceptable
const int seuilPressionEauMin = 10;    // Seuil minimal de pression d'eau de citerne acceptable
unsigned long tempsDebut;  // Pour mesurer le temps écoulé
bool moteurActive = false;  // Indicateur pour suivre l'état du moteur

BlynkTimer timer;
void setup() {
  Serial.begin(115200);
  
//Define wifi connection
  Serial.println("check_wifi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //check_wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.println(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(1000);

   // Initialize WiFi and Blynk
  Blynk.begin(auth, ssid, password);
  
    // Initialize sensor pins
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(TEMPERATURE_PIN, INPUT);
  pinMode(PRESSURE_PIN, INPUT);
  pinMode(WATER_PRESSURE_PIN, INPUT);
  
    // Allume le moteur diesel
  digitalWrite(RELAY_PIN, HIGH);
  delay(1000);

  // Éteint le moteur diesel
  digitalWrite(RELAY_PIN, LOW);
  delay(1000);
}

void loop() {
   Blynk.run();
// Attendez que le bouton soit enfoncé pour activer le moteur
  if (Blynk.readButton(V1) == HIGH) {
   // Si le moteur n'est pas déjà activé, commencez le cycle
    if (!moteurActive) {
      Serial.println("Démarrage du cycle...");
      moteurActive = true;
      Blynk.notify("Démarrage du cycle...");
      tempsDebut = millis();  // Enregistre le temps de début
    }
    unsigned long tempsEcoule = millis() - tempsDebut;
  // Active le moteur pendant 4 heures
    if (tempsEcoule < 14400000) {
      digitalWrite(RELAY_PIN, HIGH);
      delay(1000);  // Attendez 1 seconde entre les boucles pour éviter le blocage du système
      // Effectue le test de SENSOR après 5 minutes de fonctionnement
      if (tempsEcoule > 300000) {
        ReadSensor();
        if (temperature > seuilTempMax || pressure < seuilPressionHuileMin || waterPressure < seuilPressionEauMin) {
          digitalWrite(RELAY_PIN, LOW);
      }
    }
    // Ajouter ici d'autres actions ou capteurs en fonction de vos besoins
     delay(1000);  // Attendez 1 minute avant de vérifier à nouveau les conditions
  }
   // Après 4 heures, désactive le moteur et passe à une période de repos d'une heure
    else {
      digitalWrite(RELAY_PIN, LOW);
      moteurActive = false;
      Serial.println("Cycle terminé. Repos pendant 1 heure.");
      Blynk.notify("Cycle terminé. Repos pendant 1 heure.");
      delay(3600000);  // Attendre 1 heure (1 heure * 60 minutes * 60 secondes * 1000 millisecondes)
    }
  } 
  else {

    digitalWrite(RELAY_PIN, LOW);
   // Ajouter ici d'autres actions ou capteurs en fonction de vos besoins
   delay(2000);  // Attendez 1 minute avant de vérifier à nouveau les conditions
  }
}
/*Read Sensor*/
void ReadSensor(){
  
// Lecture de la température
  temperature = analogRead(TEMPERATURE_PIN);
// Lecture de la pression
  pressure = analogRead(PRESSURE_PIN);
   // Lecture de la pression d'eau
  waterPressure = analogRead(WATER_PRESSURE_PIN);

  //affichage les valeurs mesurés
  Serial.println("Temperature du moteur: " + String(temperature));
  Blynk.virtualWrite(V5, temperature);
  Serial.println("Pression d'huile: " + String(pressure));
  Blynk.virtualWrite(V6, pressure);
  Serial.println("Pression d'eau de citerne: " + String(waterPressure));
  Blynk.virtualWrite(V7, waterPressure);

  delay(1000);
}
