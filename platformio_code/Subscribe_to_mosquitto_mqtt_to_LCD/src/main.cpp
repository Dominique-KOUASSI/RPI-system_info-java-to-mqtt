#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>  // Ajouter la bibliothèque Arduino JSON
#include "Wire.h"
#include "DFRobot_RGBLCD1602.h"

// Configuration du LCD
DFRobot_RGBLCD1602 lcd(/*RGBAddr*/0x60, /*lcdCols*/16, /*lcdRows*/2);  // 16 caractères et 2 lignes

// Configuration du réseau WiFi
const char* ssid = "UNIFI_IDO2";  // Remplacez par votre SSID WiFi
const char* password = "99Bidules!";  // Remplacez par votre mot de passe WiFi

// Configuration du serveur MQTT
const char* mqtt_server = "192.168.20.221";  // Adresse IP de votre serveur Mosquitto
const char* mqtt_user = "ccnb";  // Nom d'utilisateur MQTT (si nécessaire)
const char* mqtt_password = "ccnb";  // Mot de passe MQTT (si nécessaire)

String memory_usage = "";
String cpu_usage = "";
String up_time = "";
String ip_address = "";


WiFiClient espClient;
PubSubClient client(espClient);

// Fonction callback appelée lorsqu'un message est reçu
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message reçu sur le topic : ");
  Serial.println(topic);

  // Convertir le payload en chaîne de caractères
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';  // Terminaison de la chaîne

  Serial.print("Payload brut : ");
  Serial.println(message);

  // Parser le JSON
  StaticJsonDocument<200> doc;  // Adapter la taille en fonction des besoins
  DeserializationError error = deserializeJson(doc, message);

  if (error) {
    Serial.print("Erreur de parsing JSON : ");
    Serial.println(error.c_str());
    return;
  }

  // Afficher les couples clé-valeur
  Serial.println("Couples clé-valeur :");
  for (JsonPair kv : doc.as<JsonObject>()) {
    Serial.print("Clé : ");
    Serial.print(kv.key().c_str());
    Serial.print(", Valeur : ");
    Serial.println(kv.value().as<String>());

    // Afficher sur l'écran LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(kv.key().c_str());
    lcd.setCursor(0, 1);
    lcd.print(kv.value().as<String>());
    delay(2000);  // Pause pour afficher chaque paire
  }
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connexion à ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connecté");
  Serial.print("Adresse IP : ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Boucle tant qu'il n'est pas connecté au serveur MQTT
  while (!client.connected()) {
    Serial.print("Connexion au serveur MQTT...");
    // Tentative de connexion avec authentification
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("connecté");
      // Souscrire au topic "testTopic"
      client.subscribe("testTopic");
      Serial.println("Souscrit au topic 'testTopic'");
    } else {
      Serial.print("échec, code erreur: ");
      Serial.print(client.state());
      Serial.println(" Réessai dans 5 secondes");
      delay(5000);
    }
  }
}

void setup() {
  lcd.init();
  lcd.setColorWhite();

  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);  // Configurer l'adresse IP et le port du serveur MQTT
  client.setCallback(callback);        // Associer la fonction callback pour les messages reçus
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
