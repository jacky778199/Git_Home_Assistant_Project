// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
	Name:       ESP12_HTU2X_PMS7003_MQTT.ino
	Created:	2019/1/12 下午 03:30:28
	Author:     DESKTOP-MHOFJUB\Jacky
*/
#include <SoftwareSerial.h>

#include <Wire.h>
#include <HTU21D.h>
//NodeMCU 1.0, WeMos D1 Mini...............SDA-- GPIO4/D2             SCL--GPIO5/D1

#include <ESP8266WiFi.h> 
#include <PubSubClient.h>


#define START_1 0x42
#define START_2 0x4d

#define DATA_LENGTH_H        0
#define DATA_LENGTH_L        1
#define PM1_0_CF1_H          2
#define PM1_0_CF1_L          3
#define PM2_5_CF1_H          4
#define PM2_5_CF1_L          5
#define PM10_CF1_H           6
#define PM10_CF1_L           7
#define PM1_0_ATMOSPHERE_H   8
#define PM1_0_ATMOSPHERE_L   9
#define PM2_5_ATMOSPHERE_H   10
#define PM2_5_ATMOSPHERE_L   11
#define PM10_ATMOSPHERE_H    12
#define PM10_ATMOSPHERE_L    13
#define UM0_3_H              14
#define UM0_3_L              15
#define UM0_5_H              16
#define UM0_5_L              17
#define UM1_0_H              18
#define UM1_0_L              19
#define UM2_5_H              20
#define UM2_5_L              21
#define UM5_0_H              22
#define UM5_0_L              23
#define UM10_H               24
#define UM10_L               25
#define VERSION              26
#define ERROR_CODE           27
#define CHECKSUM             29
// add pms7003 function
SoftwareSerial pms7003_serial(0, 2); // RX, TX // D3=GPIO0  D4=GPIO2
enum MODE { MODE_ACTIVE, MODE_PASSIVE };
MODE _mode = MODE_ACTIVE;
byte Data_byte_array[100];

byte Incoming_byte1;
byte Incoming_byte2;

//
const char* ssid = "Ting";
const char* password = "jacky825030";
const char* mqttServer = "m15.cloudmqtt.com";  // MQTT伺服器位址
const char* mqttUserName = "zpbgoxgd";  // 使用者名稱，隨意設定。
const char* mqttPwd = "Rpj9Qd9KuzeI";  // MQTT密碼
const char* clientID = "jackynode12e";      // 用戶端ID，隨意設定。
const char* topic_temp = "temp";
const char* topic_air = "air";

unsigned long prevMillis = 0;// 暫存經過時間（毫秒）
unsigned long prevMillis2 = 0;
const long interval = 5000;  // 上傳資料的間隔時間，20秒。
String msgStr = "";      // 暫存MQTT訊息字串

float temp;  // 暫存溫度
float hum;   // 暫存濕度
// float Compensated_Humidity;

HTU21D myHTU21D(HTU21D_RES_RH12_TEMP14);
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
		}
		else {
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
	client.setServer(mqttServer, 12242);
	while (myHTU21D.begin() != true)
	{
		Serial.println(F("HTU21D, SHT21 sensor is faild or not connected")); //(F()) saves string to flash & keeps dynamic memory free
		delay(5000);
	}
	Serial.println(F("HTU21D, SHT21 sensor is active"));

	pms7003_serial.begin(9600);  // GPIO2 (D4 pin on ESP-12E Development Board)
//	passiveMode();    // Switch to passive mode
	Serial.println("setup() finish");

}

void loop() 
{
	if (!client.connected()) 
	{
		reconnect();
	}
	client.loop();

/*	passiveMode();
	delay(10);
	wakeUp();
	delay(50);
	passiveMode();
	delay(10);
*/	
	

	// 等待20秒
	if (millis() - prevMillis > interval)
	{
		Serial.println();
		prevMillis = millis();
		float temp_read_value;
		// 讀取 HTU 的溫濕度資料
	 /* DEMO - 1 */
		Serial.println(F("DEMO 1: 12-Bit Resolution"));
		Serial.print(F("Humidity............: ")); Serial.print(myHTU21D.readHumidity());            Serial.println(F(" +-2%"));
		Serial.print(F("Compensated Humidity: ")); Serial.print(myHTU21D.readCompensatedHumidity()); Serial.println(F(" +-2%"));

		Serial.println(F("DEMO 1: 14-Bit Resolution"));
		Serial.print(F("Temperature.........: ")); Serial.print(myHTU21D.readTemperature()); Serial.println(F(" +-0.3C"));


		temp_read_value = myHTU21D.readTemperature();
		Serial.print("temp_read_value = "); Serial.println(temp_read_value);
		// 組合MQTT訊息；field1填入溫度、field2填入濕度
		msgStr = msgStr + myHTU21D.readTemperature();

		// 宣告字元陣列
		byte arrSize = msgStr.length() + 1;
		char msg[arrSize];

		Serial.print("Publish message: ");
		Serial.println(msgStr);
		msgStr.toCharArray(msg, arrSize); // 把String字串轉換成字元陣列格式
		client.publish("temp_my_room", msg);       // 發布MQTT主題與訊息
		msgStr = "";

		delay(500);

		msgStr = msgStr + myHTU21D.readHumidity();
		arrSize = msgStr.length() + 1;
		msgStr.toCharArray(msg, arrSize);
		client.publish("hum_my_room", msg);
		msgStr = "";

		// Send PMS7003 data ---PM1.0 (Atmosphere)
		msgStr = msgStr + GetPM_Data(Data_byte_array, PM1_0_ATMOSPHERE_H, PM1_0_ATMOSPHERE_L);
		arrSize = msgStr.length() + 1;
		msgStr.toCharArray(msg, arrSize);
		client.publish("PM1.0_my_room", msg);
		msgStr = "";

		// Send PMS7003 data ---PM2.5 (Atmosphere)
		msgStr = msgStr + GetPM_Data(Data_byte_array, PM2_5_ATMOSPHERE_H, PM2_5_ATMOSPHERE_L);
		arrSize = msgStr.length() + 1;
		msgStr.toCharArray(msg, arrSize);
		client.publish("PM2.5_my_room", msg);
		msgStr = "";

		// Send PMS7003 data ---PM10 (Atmosphere)
		msgStr = msgStr + GetPM_Data(Data_byte_array, PM10_ATMOSPHERE_H, PM10_ATMOSPHERE_L);
		arrSize = msgStr.length() + 1;
		msgStr.toCharArray(msg, arrSize);
		client.publish("PM10_my_room", msg);
		msgStr = "";

	}
	/* back to lib. default resolution */
//	myHTU21D.softReset();
	myHTU21D.setResolution(HTU21D_RES_RH12_TEMP14);

//**** AIR sensor **** //
	
	//	prevMillis2 = millis();
	//	Serial.println("requestRead() !!");
	//	requestRead();
		if (pms7003_serial.available())
		{
			Serial.println("Data available !  Reading data...");
			Incoming_byte1 = pms7003_serial.read();
		//	Serial.print(Incoming_byte1); Serial.print(" / ");
			Incoming_byte2 = pms7003_serial.read();
		//	Serial.println(Incoming_byte2);

			if (Incoming_byte1 == 0x42 && Incoming_byte2 == 0x4D)   // 66 77
			{
				parse_data();
				Serial.println(" << Received data >>");
				Serial.print(" PM1.0 (Atmosphere) : ");
				Serial.println(GetPM_Data(Data_byte_array, PM1_0_ATMOSPHERE_H, PM1_0_ATMOSPHERE_L));

				Serial.print("PM 2.5 Atmosphere = ");
				Serial.println(GetPM_Data(Data_byte_array, PM2_5_ATMOSPHERE_H, PM2_5_ATMOSPHERE_L));
				
				Serial.print("PM 10 Atmosphere = ");
				Serial.println(GetPM_Data(Data_byte_array, PM10_ATMOSPHERE_H, PM10_ATMOSPHERE_L));


				// Serial receive finish , reset index
				Serial.println("Going to sleep for 5 seconds.");
				delay(500);
				//sleep();
				//delay(5000);
			}
		}


}



void passiveMode()
{
	uint8_t command[] = { 0x42, 0x4D, 0xE1, 0x00, 0x00, 0x01, 0x70 };
	_mode = MODE_PASSIVE;
	pms7003_serial.write(command, sizeof(command));
	/*
	for (int i = 0; i < 7; i++)
	{
		pms7003_serial.print(command[i]);
		Serial.print("command["); Serial.print(i); Serial.print("]=");
		Serial.write(command[i]);
		Serial.print(" / ");
		Serial.println(command[i]);
	}
	*/
//	Serial.println("<set MODE_PASSIVE Done>");
}

//** PMS7003 functions!
// Request read in Passive Mode.
void requestRead()
{
	if (_mode == MODE_PASSIVE)
	{
//		Serial.println("MODE_PASSIVE & Send requestRead");
		uint8_t command[] = { 0x42, 0x4D, 0xE2, 0x00, 0x00, 0x01, 0x71 };
		pms7003_serial.write(command, sizeof(command));

//		Serial.println("<Send requestRead Done>");
	}
}

void sleep()
{
	uint8_t command[] = { 0x42, 0x4D, 0xE4, 0x00, 0x00, 0x01, 0x73 };
	pms7003_serial.write(command, sizeof(command));
}

void wakeUp()
{
	uint8_t command[] = { 0x42, 0x4D, 0xE4, 0x00, 0x01, 0x01, 0x74 };
	pms7003_serial.write(command, sizeof(command));
}

void parse_data()
{
	int index = 0;
	for (index = 0; index <= 31; index++)
	{
		Data_byte_array[index] = pms7003_serial.read(); // 卡住了?!!

		Serial.print(" <<Data_byte_array["); Serial.print(index); Serial.print("]= ");
		Serial.print(Data_byte_array[index],HEX);
	}


}

unsigned int GetPM_Data(byte chrSrc[], byte bytHigh, byte bytLow)
{
	return (chrSrc[bytHigh] << 8) + chrSrc[bytLow];

}