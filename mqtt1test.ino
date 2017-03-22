// 1 Светодиод подлкючен к GPIO13 (D7) пину
// 1 Светодиод подлкючен к GPIO15 (D8) пину

// todo: Датчик температуры ds18b20 к 2 пину


#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Wifi: SSID and password
const char* ssid = "<wifirouter>";       // Имя вайфай точки доступа
const char* password = "<password>";     // Пароль от точки доступа

const char *mqtt_server = "<server name or ip>";  // Имя сервера MQTT
const int   mqtt_port = 1883;                     // Порт для подключения к серверу MQTT
const char *mqtt_user = "<mqtt-user>";              // Логин от сервера
const char *mqtt_pass = "<mqtt-password>";          // Пароль от сервера

#define MQTT_CLIENT_ID "node1"
#define LED_PIN        13 // GPIO13
#define LED_TOPIC      MQTT_CLIENT_ID"/led1"

#define LED_PIN_YELLOW 15 // GPIO15
#define LED_TOPIC_YELLOW      MQTT_CLIENT_ID"/led2"

#define BUFFER_SIZE 100

bool LedState = false;
int tm=300;

// Функция получения данных от сервера
// function called when a MQTT message arrived
void callback(char* topic, byte* payload, unsigned int length)
{
  int stled;
  Serial.print(topic); // выводим в сериал порт название топика
  Serial.print(" => ");
  for (int i = 0; i < length; i++) { Serial.print((char)payload[i]); }
  Serial.println();

  if (String(topic) == LED_TOPIC) // проверяем из нужного ли нам топика пришли данные 
  {
    stled = payload[0]-'0'; // преобразуем полученные данные в тип integer
    Serial.println("We are here #1");
    digitalWrite(LED_PIN,stled); // включаем или выключаем светодиод в зависимоти от полученных значений данных
  }
  else if (String(topic) == LED_TOPIC_YELLOW) // проверяем из нужного ли нам топика пришли данные 
  {
    stled = payload[0]-'0'; // преобразуем полученные данные в тип integer
    Serial.println("We are here #2");
    digitalWrite(LED_PIN_YELLOW,stled); // включаем или выключаем светодиод в зависимоти от полученных значений данных
  }

}

WiFiClient wclient; 
PubSubClient client(wclient);

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_PIN_YELLOW, OUTPUT);
}

void loop() {
  // подключаемся к wi-fi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println("...");
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) 
       return;
    Serial.println("WiFi connected");
    Serial.print("IP address:");
    Serial.println(WiFi.localIP());
  }

  // подключаемся к MQTT серверу
  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      client.setServer(mqtt_server, mqtt_port);
      client.setCallback(callback);
      Serial.println("Connecting to MQTT server");
      if (client.connect(MQTT_CLIENT_ID, mqtt_user, mqtt_pass)) {
        Serial.println("Connected to MQTT server");
        client.subscribe(LED_TOPIC); // подписывааемся по топик с данными для светодиода
        Serial.print("Subscribing to ");
        Serial.println(LED_TOPIC);
        //---
        client.subscribe(LED_TOPIC_YELLOW); // подписывааемся по топик с данными для светодиода
        Serial.print("Subscribing to ");
        Serial.println(LED_TOPIC_YELLOW);
        } else {
          Serial.println("Could not connect to MQTT server"); 
        }
    }

    if (client.connected()){
      client.loop();
      TempSend();
    }
  }
} // конец основного цикла

// Функция отправки показаний с термодатчика
void TempSend(){
  if (tm==0)
    {
      //sensors.requestTemperatures(); // от датчика получаем значение температуры
      //float temp = sensors.getTempCByIndex(0);
      //client.publish("test/temp",String(temp)); // отправляем в топик для термодатчика значение температуры
      float temp = ( random(400) / 10 )+ 10;
      String msg;
      msg = String(temp);
      char message[5];
      msg.toCharArray(message,5);
       //publish sensor data to MQTT broker
      client.publish("node1/temp",message,true); 
      Serial.println(message);
      tm = 300; // пауза меду отправками значений температуры около /100 секунд
   }
  tm--; 
  delay(10); 
}
