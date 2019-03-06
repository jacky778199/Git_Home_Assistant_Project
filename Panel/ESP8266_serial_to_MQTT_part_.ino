
// For ESP01 use

#include <ESP8266WiFi.h> 
#include <PubSubClient.h>

const char* ssid = "Ting";
const char* password = "jacky825030";
const char* mqttServer = "m15.cloudmqtt.com";  // MQTT伺服器位址
const char* mqttUserName = "zpbgoxgd";  // 使用者名稱，隨意設定。
const char* mqttPwd = "Rpj9Qd9KuzeI";  // MQTT密碼
const char* clientID = "Home_to_Mega";      // 用戶端ID，隨意設定。
//const char* topic = "default_topic";
String topic = "default_topic";

//const char* ssid = "Sapido_BRE71n_d2415a";
//const char* password = "";
/*
const char* mqttServer = "m15.cloudmqtt.com";  // MQTT伺服器位址
const char* mqttUserName = "zpbgoxgd";  // 使用者名稱，隨意設定。
const char* mqttPwd = "Rpj9Qd9KuzeI";  // MQTT密碼
const char* clientID = "jackynode12e";      // 用戶端ID，隨意設定。
const char* topic = "temp";

*/

unsigned long prevMillis = 0;  // 暫存經過時間（毫秒）
const long interval = 5000;  // 上傳資料的間隔時間，20秒。
String msgStr = "";      // 暫存MQTT訊息字串
char incoming_message[100];
byte chr;
char msg_char[100];
char topic_char[100];
int i = 0;
int position_of_Data = 0;
int position_of_End = 0;
bool flag_receive_new_data = 0 ;

WiFiClient espClient;
PubSubClient client(espClient);


void setup() {
	Serial.begin(9600);
	setup_wifi();
	client.setServer(mqttServer, 12242);
	Serial.println(" setup stage finished");
}

void loop() 
{
	if (!client.connected())
	{
		reconnect();
	}
	client.loop();

	// 等待20秒

	
		/*
		資料由MEGA端送來 格式
		Topic=XXXXXX;Data=OOOOOOOO
		用分號;分隔  用\n代表結尾
		Topic=test123;Data=test456
		*/

		int zero = 0;
		// Analize incoming Serial message (Topic or Data) 
		if (Serial.available() > 0)
		{
			chr = Serial.read();
			//while ((chr = Serial.read()) != '\n')
			//{
				// 確認字元值不等於-1，而且索引i小於3（確保僅讀取前三個字）
				if ((126> chr > 0) && (i < 100))
				{
					incoming_message[i] = chr;  //char incoming_message[100];
					Serial.print("incoming_message[ "); Serial.print(i); Serial.print(" ]=");
					Serial.println(incoming_message[i]);

//					Serial.print("  < Serial_Write chr=");
//					Serial.write(chr);  // ASIC letter it self
//					Serial.print(">  Serail_Print chr=");
//					Serial.print(chr);  // ASIC number
//					Serial.println(" >  ");
					
					if (chr == 59)
					{
						position_of_Data = i + 1;
//						Serial.print("position_of_Data=");
//						Serial.println(position_of_Data);
						//break;   // WDT watch dog reset problem  ,so add it to leave while loop
					}
//					delay(1);
					i++;
				}
			//}
			delay(1);
			if (chr == '\n')
			{
				position_of_End = i;
				Serial.print("position_of_End=");
				Serial.println(position_of_End);
				i = 0;
				flag_receive_new_data = 1;
			}
		}
		delay(1);

		if (millis() - prevMillis > interval || flag_receive_new_data == 1 )
		{
				prevMillis = millis();

				topic = "";
				//	}
				//  TOPIC 欄位 TOPCI= is 6 letter so k=6
				for (int k = 6; k < (position_of_Data - 1); k++)
				{
					topic = topic + incoming_message[k] ;  //const char* topic = "default_topic";
					topic_char[zero] = incoming_message[k];
					zero++;
				}
				zero = 0;
				// Data 欄位
				for (int k = position_of_Data + 5; k < position_of_End; k++)
				{
	//				msgStr = msgStr + incoming_message[k];   // String msgStr = ""; It still keep adding!!

					msg_char[zero] = incoming_message[k]; // char msg_char;
					zero++;

				}
				
//				Serial.print("topic: ");
//				Serial.println(topic);
				//Serial.print("msgStr: ");
				//Serial.println(msgStr);
//				Serial.print("topic_char: ");
//				Serial.print(topic_char);
				Serial.print("msg_char: ");
				Serial.println(msg_char);



//				Serial.print("arrSize = ");
//				Serial.println(arrSize);

//				Serial.print(">>>Publish message  Topic is:");
//				Serial.print(topic_char);
//				Serial.print(" Data is:");
//				Serial.println(msg_char);
				//    msgStr.toCharArray(msg, arrSize); // 把String字串轉換成字元陣列格式 把資料塞到 msg
				if (flag_receive_new_data == 1)
				{
					client.publish(topic_char, msg_char); // 發布MQTT主題與訊息
//					Serial.print("arrSize = ");
//					Serial.println(arrSize);

					Serial.print(">>>Publish message  Topic is:");
					Serial.print(topic_char);
					Serial.print(" Data is:");
					Serial.print(msg_char);
					Serial.println("---Publish-End");
					msgStr = "";
					memset(msg_char, 0, 100);
					memset(topic_char, 0, sizeof(topic_char));
					flag_receive_new_data = 0;
//				Serial.println("<< client.publish Data Cleaned msgStr,msg_char,topic_char >>");
				}
		}
		
		/*
		msgStr=msgStr+myHTU21D.readHumidity();
		arrSize = msgStr.length() + 1;
		msgStr.toCharArray(msg, arrSize);
		client.publish("hum", msg);
		msgStr = "";
		*/

	//}


}


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
