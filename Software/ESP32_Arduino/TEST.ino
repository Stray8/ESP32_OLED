#include "WiFi.h"
#include <HTTPClient.h>
#include <SSD1306.h>
#include <ArduinoJson.h>
#include <OLEDDisplayFonts.h>

#define OLED_ADDRESS 0x3c
#define OLED_SDA 21
#define OLED_SCL 22
#define led 2
#define UART_BAUD 115200

DynamicJsonDocument doc(1024);
DynamicJsonDocument doc1(1024);

SSD1306 display(OLED_ADDRESS, OLED_SDA, OLED_SCL);
HTTPClient http;

const char* ssid = "AH-Office";
const char* passport = "aihangkeji123";
const char* ntpServer = "pool.ntp.org"; //网络时间服务器
const long gmtOffset_sec = 8 * 3600;
const int daylightOffset_sec = 0;

String ip2Str(IPAddress ip)
{
    String s = "";
    for (int i = 0; i < 4; i++)
    {
        s += i ? "." + String(ip[i]) : String(ip[i]);
    }
    return s;
}
void print_LocalTime()
{
    /*
    结构体timeinfo里的tm_mon范围在0~11
    tm_year加上1900是实际年份
    tm_wday取值在0~6    
    */
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
        return;
    }
    Serial.println(&timeinfo, "%F, %T, %A");
    display.drawString(0, 0, ((String)(timeinfo.tm_year + 1900) + "." + 
        (String)(timeinfo.tm_mon + 1) + "." + (String)(timeinfo.tm_mday)).c_str());
    display.drawString(50, 0, ((String)timeinfo.tm_hour + ":" + 
        (String)timeinfo.tm_min + ":" + (String)timeinfo.tm_sec).c_str());
    switch (timeinfo.tm_wday)
    {
        case 0:
            display.drawString(95, 0, "Sunday");
            break;
        case 1:
            display.drawString(95, 0, "Monday");
            break;
        case 2:
            display.drawString(95, 0, "Tuesday");
            break;
        case 3:
            display.drawString(95, 0, "Wednesday");
            break;
        case 4:
            display.drawString(95, 0, "Thursday");
            break;
        case 5:
            display.drawString(95, 0, "Friday");
            break;
        case 6:
            display.drawString(95, 0, "Saturday");
            break;
    }
}

void setup()
{  
    pinMode(led, OUTPUT);
    
    display.init();
    display.flipScreenVertically();
    Serial.begin(UART_BAUD);
    WiFi.begin(ssid, passport);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        digitalWrite(led, LOW);
        display.drawString(0, 0, "Waiting");
        Serial.print(".");
    }
    Serial.println("Connected");
    Serial.println(WiFi.localIP());
    digitalWrite(led, HIGH);
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    display.clear();
    display.drawString(0, 0, "WiFi Connected");
    display.drawString(0, 10, ("WiFi name:" + (String)WiFi.SSID()).c_str());
    display.drawString(0, 20, ("IP:" + ip2Str(WiFi.localIP())).c_str());
    display.display();
}

void loop()
{    
    delay(1000);
    display.clear();
    http.begin("https://api.seniverse.com/v3/weather/now.json?key=SYOcALCe0PNOZEkWb&location=hangzhou&language=en&unit=c"); 
    //("http://www.weather.com.cn/data/cityinfo/101270101.html");
    int httpCode = http.GET();
    if(httpCode == HTTP_CODE_OK)
    { 
        String payload = http.getString();
        Serial.println(payload);
        /*
        print_LocalTime();  
        String pageData = http.getString();
        //Serial.println(pageData);
        deserializeJson(doc,pageData);
        JsonObject obj = doc.as<JsonObject>();
        String weatherInfo = obj["weatherinfo"];
        deserializeJson(doc1,weatherInfo);
        JsonObject obj1 = doc1.as<JsonObject>();
        String city = obj1["city"];
        String temp1 = obj1["temp1"];
        String temp2 = obj1["temp2"];
        String weather = obj1["weather"];
        String cityInfo ="place:"+ city;
        String tempInfo ="temp: " + temp1 + "~" + temp2;
        String cityWeatherinfo = "weather: " + weather;
        Serial.println("get weather situation");
    
        Serial.println(cityInfo.c_str());
        display.drawString(0, 10, "Owner: xdz");
        display.drawString(0, 20, "Weather Situation:");
        display.drawString(0, 30, "Place:" + city);
        display.drawString(0, 40, "Temp:" + temp1 + "~" + temp2 + "℃");
        display.drawString(0, 50, "Weather:" + weather);
        
        Serial.println(tempInfo.c_str());
        Serial.println(cityWeatherinfo.c_str());
        */
    }
    else
    {
        Serial.println("GET ERR");
    }

    http.end();
    display.display();

}