// ================ БИБЛИОТЕКИ =====================

#include <Arduino.h>
#include <esp8266WiFi.h>
#include <PubSubClient.h>

// =============== НАСТРОЙКА =======================

#define ssid "POCO X3 Pro"
#define password "12345687"

#define mqtt_server "mqtt.dealgate.ru"
#define mqtt_port 1883
#define mqtt_user "itman7144"
#define mqtt_password "Parol2007dg7144"
#define switchTopic "SC-power"
#define tempSensorTopic "SC-temperature"

#define OUTPUT_PIN D1
#define TEMP_SENSOR_PIN D2

WiFiClient espClient;
PubSubClient client(espClient);

#include <microDS18B20.h>
MicroDS18B20<TEMP_SENSOR_PIN> sensor;

// =============== ПОДКЛЮЧЕНИЕ ====================

void WiFi_connect()
{
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("[WIFI] Подключение к Wi-Fi");
  }
  Serial.print("[WIFI] Подключен Wi-Fi: ");
  Serial.println(WiFi.SSID());
}

void MQTTcallback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("[MQTT] Получено сообщение из топика: ");
  Serial.print(switchTopic);
  Serial.print(" | Сообщение: ");
  for (unsigned int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  String data_pay;
  for (unsigned int i = 0; i < length; i++)
  {
    data_pay += String((char)payload[i]);
  }

  if (String(topic) == switchTopic)
  {
    if (data_pay == "0")
    {
      digitalWrite(OUTPUT_PIN, LOW);
    }
    else if (data_pay == "1")
    {
      digitalWrite(OUTPUT_PIN, HIGH);
    }
  }
}

void mqtt_connect()
{
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(MQTTcallback);

  while (!client.connected())
  {
    Serial.println("[MQTT] Подключение к MQTT");
    String client_id = "ESP8266-client-" + String(WiFi.macAddress());
    Serial.printf("[MQTT] Клиент %s подключается к MQTT\n", client_id.c_str());

    if (client.connect(client_id.c_str(), mqtt_user, mqtt_password))
    {
      Serial.println("[MQTT] Подключено к MQTT");
      client.subscribe(switchTopic);
    }
    else
    {
      Serial.print("[ERROR] Ошибка со статусом: ");
      Serial.print(client.state());
      delay(1000);
    }
  }

  digitalWrite(LED_BUILTIN, !LOW);
}

// ============ ОСНОВНЫЕ БЛОКИ ====================

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(OUTPUT_PIN, OUTPUT);
  Serial.begin(115200);

  WiFi_connect();
  mqtt_connect();

  for (int i = 0; i < 6; i++)
  {
    digitalWrite(OUTPUT_PIN, !digitalRead(OUTPUT_PIN));
    delay(250);
  }
}

void loop()
{
  client.loop();

  sensor.requestTemp();
  static uint32_t sensTempTimer;
  if (millis() - sensTempTimer > 1000)
  {
    sensTempTimer = millis();
    if (sensor.readTemp())
    {
      float temperature = sensor.getTemp();
      char data[5] = "";
      dtostrf(temperature, 4, 2, data);
      Serial.println(temperature);
      client.publish(tempSensorTopic, data);
    }
    else
    {
      Serial.println("error");
    }
  }

  static uint32_t timerLed;
  if (millis() - timerLed > 500)
  {
    timerLed = millis();
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
}
