#include <ESP8266WiFi.h>       //Ability to work with Wi-Fi
#include <ESP8266HTTPClient.h> //Ability to work with HTTP Get requests
#include <WiFiClient.h>        //Ability to work with Wi-Fi communication (own access point)
#include <Arduino_JSON.h>      //Parsing the weather indicators from Internet
#include "DHT.h"               //Ability to using the DHT11 humidity and temperature sensor
#include <Adafruit_NeoPixel.h> //Ability to work with WS2812B LED strip

#define DEBUG //Macros for hiding the console messages  

#define DHTPIN 4              // pin D2 for DHT11
#define DHTTYPE DHT11         // sensor type

// LIGHT DEFINES //
#define PIN          D5       //PIN for LED strip
#define LED_COUNT    17  

#define BRIGHTNESS   50   
// END LIGHT DEFINES //

// COLOR DEFINES //
#define RED         strip.Color(255, 0, 0)
#define ORANGE      strip.Color(255, 69, 0)
#define GREEN       strip.Color(0, 255, 0)
#define WINTER_BLUE strip.Color(63, 90, 100)
#define YELLOW      strip.Color(255, 255, 0)
#define BLUE        strip.Color(0, 0, 255)
#define BLACK       strip.Color(0, 0, 0)
// END COLOR DEFINES //

DHT dht(DHTPIN, DHTTYPE); //sensor DHT11 object for communication 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, PIN, NEO_GRB + NEO_KHZ800); //strip declaration 

const String _SSID = "YOUR_WIFI"; //Your wi-fi name
const String _PASSWORD = "YOUR_PASSWORD"; //Your wi-fi password

// Your Domain name with URL path or IP address with path
//
const String API_KEY = "YOUR_API_KEY"; //API KEY for http get request to forecast site

// Replace with your country code and city
//
const String CITY = "YOUR_CITY"; //Your city for the forecast

unsigned short timerDelay = 10000; //timer to get the HTTP request-responce results not permanent
unsigned short lastTime = 0; // variable for comparison the timer with the past time

String jsonBuffer; //Hold the HTTP response object (forecast in json)
String HttpGETRequest(const char* serverName); //function prototype of the HTTP protocol describe

enum BORDERS {MIN,MAX}; //special enum that associate the arrays indexes with min and max values
const uint8_t SIZE = 2;
uint8_t tempValues[SIZE] = {16,22}; 
uint8_t humidityValues[SIZE] = {40,55};

//All information variables
//
uint8_t clouds;
uint8_t humidityOutdoor;
uint8_t temperatureIndoor;
uint8_t humidityIndoor;

  
void setup() 
{
  WiFi.begin(_SSID, _PASSWORD);

  #ifdef DEBUG
    Serial.begin(115200);
    Serial.println("Connecting");
  
    while(WiFi.status() != WL_CONNECTED) 
    {
      delay(1000);
      Serial.print(".");
    }
  
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
 
    Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");
  #endif

  dht.begin();
  strip.begin();                                             
  strip.setBrightness(BRIGHTNESS); 
  strip.show();    
}   
   
void loop() 
{
  // Send an HTTP GET request every 10 seconds
  //
  if ((millis() - lastTime) > timerDelay) 
  {
    // Check WiFi connection status
    //
    if(WiFi.status()== WL_CONNECTED)
    {
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + CITY  + "&APPID=" + API_KEY; //GET request forming
      
      jsonBuffer = HttpGETRequest(serverPath.c_str());//Get the forecast in json object
      JSONVar myObject = JSON.parse(jsonBuffer); //Try to parse the object (get the values from keys. For example key: temperature and value: 22)
  
      // JSON.typeof(jsonVar) can be used to get the type of the var
      //
      if (JSON.typeof(myObject) == "undefined") 
      {
        #ifdef DEBUG
          Serial.println("Parsing input failed!");
        #endif
        
        SetColor(RED); //Signalize the person that the forecast is not downloaded
        return;
      }

      //Macros for Serial port vision
      //
      #ifdef DEBUG
        Serial.print("JSON object = ");
        Serial.println(myObject);
        Serial.print("Temperature: ");
        Serial.println(myObject["clouds"]["all"]);
        Serial.print("Humidity: ");
        Serial.println(myObject["main"]["humidity"]);
      #endif
      
      clouds = (int)myObject["clouds"]["all"];
      humidityOutdoor = (int)myObject["main"]["humidity"];
    }
    else
    {
      #ifdef DEBUG
        Serial.println("WiFi Disconnected"); //Something wrong with Wi-Fi
      #endif
    }
    
    lastTime = millis(); //Count the passed time
  }

    #ifdef DEBUG 
      Serial.print("temperature=");
      Serial.println(dht.readTemperature());
      Serial.print("humidity=");
      Serial.println(dht.readHumidity());
    #endif
    
     temperatureIndoor = dht.readTemperature();
     humidityIndoor = dht.readHumidity();

     #ifdef DEBUG
      Serial.print("clouds=");
      Serial.println(clouds);
      Serial.print("humidity=");
      Serial.println(humidityOutdoor);
      Serial.print("temperature=");
      Serial.println(temperatureIndoor);
      Serial.print("humidity=");
      Serial.println(humidityIndoor);
    #endif

    //Check and compare the temperature with MIN and MAX value
    //
    if(temperatureIndoor < tempValues[MIN])
    {
      SetColor(WINTER_BLUE);
    }
    else if(temperatureIndoor > tempValues[MAX])
    {
      SetColor(ORANGE);
    }
    else
    {
      SetColor(GREEN); //Everything is OK!
    }
      
    SetColor(strip.Color(255,255,(uint32_t)clouds*2.55)); //Associate clouds with color (yellow - clear sky and sun, white - 100% clouds)

    //Check the humidity and compare it with min and max with Inernet or without
    //  
    if(humidityIndoor < humidityValues[MIN])
    {
      SetColor(BLUE);
      
      if(humidityOutdoor > 0) //if the wi-fi connection hasn't lost
      {
        strip.setBrightness(BRIGHTNESS/(float)(humidityIndoor/humidityOutdoor)); //We suggest that the humidity outdoor must be bigger than indoor humidity
      }
      else
      {
        strip.setBrightness(BRIGHTNESS/2); //Set half brightness if the air is dry
      }
      
      strip.show(); //show the brightness
    }
    else if(humidityIndoor > humidityValues[MAX])
    {
      if(humidityOutdoor > 0)
      {
        strip.setBrightness(BRIGHTNESS/(float)(humidityOutdoor/humidityIndoor)); //We suggest that the humidity outdoor must be bigger than indoor humidity
      }
      else
      {
        strip.setBrightness(BRIGHTNESS*2); //Set x2 brightness (visible bath effect)
      }
    }
    else
    {
      SetColor(BLUE);
      strip.setBrightness(BRIGHTNESS); // Standart brightness
      strip.show();
    }
}


String HttpGETRequest(const char* serverName) 
{
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String 
  payload = "{}"; 
  
  if (httpResponseCode>0) 
  {
    #ifdef DEBUG
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    #endif
      
    payload = http.getString();
  }
  else 
  {
    SetColor(RED);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
    #ifdef DEBUG
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    #endif
  }
  // Free resources
  http.end();

  return payload;
}


// Fill the dots one after the other with a color
//
void SetColor(uint32_t color) 
{
  for(uint8_t i=0; i<LED_COUNT; i++)
  {
    strip.setPixelColor(i, color); 
    strip.show();
    delay(50);                  
  }

  delay(3000);
}
