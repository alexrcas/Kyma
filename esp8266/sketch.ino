#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "Wire.h"
#include "Adafruit_INA219.h"

Adafruit_INA219 ina219;

const char *ssid = "SSID";
const char *password = "PASSWORD";

const char *mqtt_server = "192.168.1.100";
const char *mqtt_topic = "/data";

WiFiClient espClient;
PubSubClient client(espClient);

// ----------------------------------------------------------------
// Conexión WiFi
// ----------------------------------------------------------------
void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// ----------------------------------------------------------------
// Re-conexión MQTT si se cae
// ----------------------------------------------------------------
void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Conectando a MQTT...");
    if (client.connect("WemosINA219"))
    {
      Serial.println(" conectado.");
    }
    else
    {
      Serial.print(" fallo, rc=");
      Serial.print(client.state());
      Serial.println(" intentando de nuevo en 2 segundos");
      delay(2000);
    }
  }
}

// ----------------------------------------------------------------
// SETUP
// ----------------------------------------------------------------
void setup()
{
  Serial.begin(9600);
  delay(100);

  setup_wifi();

  client.setServer(mqtt_server, 1883);

  if (!ina219.begin())
  {
    Serial.println("Failed to find INA219 chip");
    while (1)
    {
      delay(10);
    }
  }

  Serial.print("BV");
  Serial.print("\t");
  Serial.print("SV");
  Serial.print("\t");
  Serial.print("LV");
  Serial.print("\t");
  Serial.print("C");
  Serial.print("\t");
  Serial.println("P");
}

// ----------------------------------------------------------------
// LOOP
// ----------------------------------------------------------------
void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  float shuntvoltage = ina219.getShuntVoltage_mV();
  float busvoltage = ina219.getBusVoltage_V();
  float current_mA = ina219.getCurrent_mA();
  float power_mW = ina219.getPower_mW();
  float loadvoltage = busvoltage + (shuntvoltage / 1000);

  Serial.print(busvoltage);
  Serial.print("\t");
  Serial.print(shuntvoltage);
  Serial.print("\t");
  Serial.print(loadvoltage);
  Serial.print("\t");
  Serial.print(current_mA);
  Serial.print("\t");
  Serial.println(power_mW);

  char payload[200];
  snprintf(payload, sizeof(payload),
           "{\"timestamp\":%lu,\"v\":%.3f,\"mA\":%.3f,\"mW\":%.3f}",
           millis(),   // o Date.now() equivalente en ESP (ms desde arranque)
           busvoltage, // v
           current_mA, // mA
           power_mW);  // mW

  client.publish(mqtt_topic, payload);

  delay(1000);
}
