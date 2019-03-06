 // Board:                                    SDA                    SCL
//  ESP8266 ESP-01:.......................... GPIO0/D5               GPIO2/D3
#include <Wire.h>
#include <HTU21D.h>
#include <ESP8266WiFi.h> 
#include <PubSubClient.h>

HTU21D myHTU21D(HTU21D_RES_RH12_TEMP14);
const char* ssid = "Ting";
const char* password = "jacky825030";

//const char* ssid = "Sapido_BRE71n_d2415a";
//const char* password = "";

const char* mqttServer = "jackysensor.duckdns.org";  // MQTT伺服器位址
const char* mqttUserName = "pi";  // 使用者名稱，隨意設定。
const char* mqttPwd = "825030";  // MQTT密碼
const char* clientID = "Simple_esp01_htu21_storage_room";      // 用戶端ID，隨意設定。
const char* topic = "storage_room";

unsigned long prevMillis = 0;  // 暫存經過時間（毫秒）
const long interval = 5000;  // 上傳資料的間隔時間，20秒。
String msgStr = "";      // 暫存MQTT訊息字串

float temp;  // 暫存溫度
float hum;   // 暫存濕度
// float Compensated_Humidity;

WiFiClient espClient;
PubSubClient client(espClient);


void setup_wifi() {
  delay(10);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect(clientID, mqttUserName, mqttPwd)) {
      Serial.println("MQTT connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);  // 等5秒之後再重試
    }
  }
}

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqttServer, 1880);
  while (myHTU21D.begin(0,2) != true)
  {
    Serial.println("HTU21D, SHT21 sensor is faild or not connected"); //(F()) saves string to flash & keeps dynamic memory free
    delay(5000);
  }
    Serial.println("HTU21D, SHT21 sensor is active");
  }
 
  

  

void loop() {
  if (!client.connected()) 
  {
    reconnect();
  }
  client.loop();

  // 等待20秒
  if (millis() - prevMillis > interval) 
  {
    prevMillis = millis();

    // 讀取 HTU 的溫濕度資料
 /* DEMO - 1 */
 
  Serial.print(F("Temperature.........: ")); Serial.print(myHTU21D.readTemperature() ); Serial.println(F(" +-0.3C"));

    // 組合MQTT訊息；field1填入溫度、field2填入濕度
    msgStr=msgStr+myHTU21D.readTemperature();
    
    // 宣告字元陣列
    byte arrSize = msgStr.length() + 1;
    char msg[arrSize];

    Serial.print("Publish message: ");
    Serial.println(msgStr);
    msgStr.toCharArray(msg, arrSize); // 把String字串轉換成字元陣列格式
    client.publish("temp/storage_room", msg);       // 發布MQTT主題與訊息
        Serial.print(F("MQTT>> temp/storage_room ; "));Serial.println(msg);
    msgStr = "";
    delay(100);
    
    msgStr=msgStr+myHTU21D.readHumidity();
    arrSize = msgStr.length() + 1;
    msgStr.toCharArray(msg, arrSize);
    client.publish("hum/storage_room", msg); 
        Serial.print(F("MQTT>> hum/storage_room ; "));Serial.println(msg);
    msgStr = "";
    
  }
     /* back to lib. default resolution */
 // myHTU21D.softReset();
 // myHTU21D.setResolution(HTU21D_RES_RH12_TEMP14);


}
