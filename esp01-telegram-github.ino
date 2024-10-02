

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Wifi network station credentials
#define WIFI_SSID "MyWiFiName"
#define WIFI_PASSWORD "MyWiFiPassword"
// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "api TOKEN BOT"
#define CHAT_ID "123456789" //owner chatid, for the boot up message
const char* id_1 = "123456789"; //owner chat id
const char* id_2 = "123456789"; //second chat id can use the bot
//you can add more users but remember to add below in (chat_id == id_1 || chat_id == id_2)

const int relayPin = 0; //relay pin of esp01 board

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime;  // last time messages' scan has been done
bool Start = false;
const unsigned long BOT_MTBS = 1000;  // mean time between scan messages

//Serial monitor to see chat ID and messages
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    Serial.print(bot.messages[i].from_name);
    Serial.print(" ID: ");
    Serial.print(chat_id);
    Serial.print(" Message: ");
    Serial.println(text);

    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";

    if (text == "/send_test_action") {
      bot.sendChatAction(chat_id, "typing");
      delay(4000);
      bot.sendMessage(chat_id, "Did you see the action message?");

      // You can't use own message, just choose from one of bellow

      //typing for text messages
      //upload_photo for photos
      //record_video or upload_video for videos
      //record_audio or upload_audio for audio files
      //upload_document for general files
      //find_location for location data
      //more info here - https://core.telegram.org/bots/api#sendchataction
    }

    //you can add user by using the same syntax here and add the chat id on the define string above
    if (chat_id == id_1 || chat_id == id_2) {
      //This is the list of command that bot send when you write /start
      if (text == "/start") {
        String list = "Welcome " + from_name + ".\n";
        list += "Command List \n\n";
        list += "/send_test_action : to send test chat action message\n";
        list += "/random \n";
        list += "/example \n";
        list += "/turnon \n";
        list += "/turnoff \n";
        list += "/releon \n";
        list += "/releoff \n";
        bot.sendMessage(chat_id, list);
      }
      // you must add and IF condition every command you need
      }
      if (text == "/random") {
        int randomValue = random(1, 101);
        String random = "number " + String(randomValue);
        bot.sendMessage(chat_id, random, "");
      }
       if (text == "/example") {
        bot.sendMessage(chat_id, "This is an example message sent from ESP8266", "");
      }
      if (text == "/turnon") {
        digitalWrite(LED_BUILTIN, LOW);
        bot.sendMessage(chat_id, "Now the LED is on", "");
      }
      if (text == "/turnoff") {
        digitalWrite(LED_BUILTIN, HIGH);
        bot.sendMessage(chat_id, "Now the LED is off", "");
      }
      if (text == "/releon") {
        digitalWrite(relayPin, LOW);
        bot.sendMessage(chat_id, "Now the rele is on", "");
      }
      if (text == "/releoff") {
        digitalWrite(relayPin, HIGH);
        bot.sendMessage(chat_id, "Now the rele is off", "");
      }
    }
  }
}
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(relayPin, HIGH);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  secured_client.setTrustAnchors(&cert);  // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Retrieving time: ");
  configTime(0, 0, "pool.ntp.org");  // get UTC time via NTP
  time_t now = time(nullptr);
  while (now < 24 * 3600) {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);
  bot.sendMessage(CHAT_ID, "Bot Started", "");
}

void loop() {

  if (millis() - bot_lasttime > BOT_MTBS) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}
