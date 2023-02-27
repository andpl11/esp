#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#include "ESP8266WiFi.h"
#include <ESPAsyncWebServer.h>
#include "ESPAsyncTCP.h"


AsyncWebServer server(80);

class temp{
  public:
  int temp = 25;
};

temp new_temp;




// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);



// Data wire is plugged into port D2 on the ESP8266
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

float tempSensor1, tempSensor2, tempSensor3;

uint8_t sensor1[8] = { 0x28, 0xFF, 0xB4, 0x0B, 0xA1, 0x16, 0x03, 0x75  };
uint8_t sensor2[8] = { 0x28, 0xFF, 0xDD, 0x41, 0x21, 0x17, 0x04, 0x49  };
uint8_t sensor3[8] = { 0x28, 0x61, 0x64, 0x12, 0x3F, 0xFD, 0x80, 0xC6  };

/*Put your SSID & Password*/
const char* ssid = "1283-NET";  // Enter SSID here
const char* password = "meus2229";  //Enter Password here

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>temperatura</title>
    <style>
        body{
            background-color: black;
            color: white;
        }
        .main{
            border: 1px solid white;
            width: 300px;
            height: 150px;
            border-radius: 20px;
            position: absolute;
            top: 50%;
            left: 50%;
            transform: translate(-50%,-50%);
            text-align: center;
        }
        .main p{
            text-align: center;
        }
    </style>
</head>
<body>

    <div class="main">
        <p class="p1"></p>
        <p class="p2"></p>
        <p class="p3"></p>
        <input type="number" placeholder="25" class="inc" oninput="set()">
    </div>
    


    <script>

const p1 = document.querySelector('.p1');
const p2 = document.querySelector('.p2');
//const p3 = document.querySelector('.p3');
const button = document.querySelector(".inc");

setInterval(function () {
            const xhr = new XMLHttpRequest();
            xhr.open("GET", "/update");
            xhr.send();
            xhr.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    let str = this.responseText.split(";");
                    console.log(str);
                    p1.innerHTML = "Sensor 1 " +str[0]+"C";
                    p2.innerHTML = "Sensor 2 " +str[1]+"C";
                   // p3.innerHTML = "Sensor 3 " +str[2]+"C";
                }
            };  
        }, 500);


        const set = ()=>{
            console.log(button.value);
            const xhr = new XMLHttpRequest();
            xhr.open("GET","/value=" + button.value, true);
            xhr.send();
        }
    </script>
</body>
</html>
)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void odczyt_temperatury(){

   sensors.requestTemperatures();
  tempSensor1 = sensors.getTempC(sensor1); // Gets the values of the temperature
  tempSensor2 = sensors.getTempC(sensor2); // Gets the values of the temperature

  lcd.setCursor(0,0);
lcd.print("1-");
  lcd.setCursor(2,0);
  lcd.print(tempSensor1);
lcd.setCursor(9,0);
lcd.print("2-");
lcd.setCursor(11,0);
lcd.print(tempSensor2);

}



void setup() {
  Serial.begin(9600);

  pinMode(13, OUTPUT);
  pinMode(12, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);

digitalWrite(13, HIGH);//zgas diode stan wysoki

   lcd.init();
  lcd.backlight();
  lcd.clear();
  delay(10);


  sensors.begin();              

  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connecting...");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request){ 
   request->send(200, "text/plain", String(tempSensor1)+";"+String(tempSensor2)+";"+String(new_temp.temp));
  });

  server.on("/state", HTTP_GET, [](AsyncWebServerRequest *request){
    String state;

    if(request->hasParam("value"))
    {
      state = request->getParam("value")->value();
    }
    new_temp.temp = state.toInt();
    request->send(200);
  });

server.onNotFound(notFound);
  server.begin();
}
void loop() {


lcd.setCursor(0,1);
lcd.print("T_zad-");
lcd.setCursor(7,1);
lcd.print(new_temp.temp);

int SW_UP = digitalRead(12);
int SW_DOWN = digitalRead(14);

if(SW_UP == HIGH){
new_temp.temp++;
delay(10);

}

if(SW_DOWN == HIGH){
new_temp.temp--;
delay(10);
}
odczyt_temperatury();

//Serial.println(tempSensor1);

}





