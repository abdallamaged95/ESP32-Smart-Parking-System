#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <pwmWrite.h>
#include <LiquidCrystal_I2C.h>

#define LED1 5
#define LED2 17
#define LED3 16
#define IR1 33
#define IR2 32
#define IR3 35
#define ENTER 14
#define EXIT 12

void handle_homepage(AsyncWebServerRequest *request);
void handle_available(AsyncWebServerRequest *request);
void handle_not_available(AsyncWebServerRequest *request);
void handleNotFound(AsyncWebServerRequest *request);
void handleLeaving(AsyncWebServerRequest *request);
String SendHomePageHTML(bool);
String SendAvailableHTML();
String SendNotAvailableHTML();
String SendLeavingHTML();

const char* ssid     = "noop";
const char* password = "20201700463";

AsyncWebServer server(80);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Pwm servo = Pwm();

bool state = false;
bool LEDState = HIGH;
bool Door1 = false;
bool Door2 = false;
int free_slots;

void setup()
{
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Request Parking");
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(IR1 ,INPUT);
  pinMode(IR2 ,INPUT);
  pinMode(IR3 ,INPUT);
  delay(10);

  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());

  server.on("/" ,HTTP_GET ,handle_homepage);
  server.on("/available",HTTP_GET ,handle_available);
  server.on("/notavailable",HTTP_GET ,handle_not_available);
  server.on("/goodbye",HTTP_GET ,handleLeaving);
  server.onNotFound(handleNotFound);
  server.begin();

}

void loop() {
  // servo.writeServo(EXIT ,0);
  bool ir1 = digitalRead(IR1);
  bool ir2 = digitalRead(IR2);
  bool ir3 = digitalRead(IR3);
  digitalWrite(LED1 ,ir1);
  digitalWrite(LED2 ,ir2);
  digitalWrite(LED3 ,ir3);

  free_slots = ir1 + ir2 + ir3;
  Serial.println(free_slots);
  delay(500);
  if (free_slots) state = true;
  else state = false;
  // servo.writeServo(EXIT ,90);
  // delay(1000);
}

void handle_homepage(AsyncWebServerRequest *request)
{
  servo.writeServo(ENTER ,0);
  servo.writeServo(EXIT ,180);
  for (int i = 0 ; i < (request->params()) ; i++)
  {
    AsyncWebParameter *pars = request->getParam(i);
    Serial.print(pars->name() + " : ");
    Serial.println(pars->value());
    Serial.println("==================");
  }
  request->send(200, "text/html", SendHomePageHTML(state));

  Door2 = true;
  if (Door1){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Welcome");
    lcd.setCursor(0,1);
    lcd.print(WiFi.localIP());
    Serial.println("Opening Enter Door");

    servo.writeServo(ENTER ,90);
    delay(3000);
    servo.writeServo(ENTER ,0);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Request Parking");
    lcd.setCursor(0,1);
    lcd.print(WiFi.localIP());
    Serial.println("Closing Enter Door");

    Door1 = false;
  }
}

void handle_available(AsyncWebServerRequest *request) 
{
  Door1 = true;
  request->send(200, "text/html", SendAvailableHTML()); 
}

void handle_not_available(AsyncWebServerRequest *request)
{
  request->send(200, "text/html", SendNotAvailableHTML());
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Sorry No Slots");
  lcd.setCursor(0,1);
  lcd.print("Available");

  delay (3000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Request Parking");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
}

void handleLeaving(AsyncWebServerRequest *request)
{
  request->send(200, "text/html", SendLeavingHTML()); 
  if(Door2){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Good Bye");
    lcd.setCursor(0,1);
    lcd.print(WiFi.localIP());
    Serial.println("Opening Exit door");

    servo.writeServo(EXIT ,90);
    delay(3000);
    servo.writeServo(EXIT ,180);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Request Parking");
    lcd.setCursor(0,1);
    lcd.print(WiFi.localIP());
    Serial.println("Closing Exit door");

    Door2 = false;
  }
}
void handleNotFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

String SendHomePageHTML(bool state)
{
  //Designing the Web Page
  String ptr = R"===(
<!DOCTYPE html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Smart Parking</title>
    <style>
      html {
        font-family: Helvetica;
        display: inline-block;
        margin: 0px auto;
        text-align: center;
      }
      body{margin-top: 50px;}
      h1 {
        color: #444444;
        margin: 50px auto 30px;
      }
      h3 {
        color: #444444;
        margin-bottom: 50px;
      }
      .Enter_button {
        display: block;
        width: 80px;
        background-color: #1abc9c;
        border: none;
        color: white;
        padding: 13px 30px;
        text-decoration: none;
        font-size: 25px;
        margin: 0px auto 35px;
        cursor: pointer;
        border-radius: 4px;
      }
      .Enter_button:active {
        background-color: #16a085;
      }
      .Close_button {
        display: block;
        width: 80px;
        background-color: #ff5757;
        border: none;
        color: white;
        padding: 13px 30px;
        text-decoration: none;
        font-size: 25px;
        margin: 0px auto 35px;
        cursor: pointer;
        border-radius: 4px;
      }
      .Close_button:active { background-color: #aa3333; }
      p {
        font-size: 14px;
        color: #888;
        margin-bottom: 10px;
      }
    </style>
  </head>
  <body>
    <h1>Welcome to our smart parking</h1>
    <h3>Using AsycWebServer Mode</h3>
  )===";

  ptr += (state)? 
          R"=(<p>Request Parking Area</p><a class="Enter_button" href="/available">PARK</a>)=":
          R"=(<p>Request Parking Area</p><a class="Enter_button" href="/notavailable">PARK</a>)=";
  
  ptr += R"===(
        <p>Leave Garage</p><a class="Close_button" href="/goodbye">LEAVE</a>
      </body>
    </html>
  )===";

  return ptr;
}
String SendAvailableHTML()
{
  String ptr = R"===(
<!DOCTYPE html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Smart Parking </title>
    <h1>Register here</h1>
    <style>
      html {
        font-family: Helvetica;
        display: inline-block;
        margin: 0px auto;
        text-align: center;
      }
      h1 {
        color: #444444;
        margin: 50px auto 30px;
      }
      .button { 
        display: block;
        width: 80px;
        background-color: #1abc9c;
        border: none;
        color: white;
        padding: 13px 30px;
        text-decoration: none;
        font-size: 25px;
        margin: 0px auto 35px;
        cursor: pointer;
        border-radius: 4px; 
        margin-top:2px
      }
      .button-on { background-color: #1abc9c; }
    </style>
  </head>

  <body>

    <script>
      function func(){
        var name = document.getElementById("Name").value;
        var phone = document.getElementById("Phone").value;
        var time = document.getElementById("Time").value;
        var email = document.getElementById("Email").value;
        window.location.href = "/?name=" + name + "&phone=" + phone + "&time=" + time +"&email=" + email ;
      }
    </script>

    <lable for=Name></lable>
    <input type="text" id="Name" name="Name" placeholder="Name">
    <br></br>
    <lable for=Phone></lable>
    <input type="text" id="Phone" name="Phone" placeholder="Phone number">
    <br></br>
    <lable for=Time></lable>
    <input type="text" id="Time" name="Time" placeholder="Parking time">
    <br></br>
    <input type="text" id="Email" name="Time" placeholder="Email">
    <br></br>
    <a onclick="func();" class=button> Submit</a>

  </body>
</html>

  )===";

  return ptr;
}
String SendNotAvailableHTML()
{
  String ptr = R"===(
    <!DOCTYPE html> 
    <html>
      <head>
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title> Smart Parking </title>
        <style>
          html {
            font-family: Helvetica;
            display: inline-block;
            margin: 0px auto;
            text-align: center;
          }
          body {
            margin-top: 50px;
          }
          h1 {
            color: #444444;
            margin: 50px auto 30px
          }
        </style>
      </head>

      <body>
        <h1>NO AVAILABLE PARKING AREAS</h1>
      </body>
    </html>

  )===";

  return ptr;
}

String SendLeavingHTML()
{
  String ptr = R"===(
    <!DOCTYPE html> 
    <html>
      <head>
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title> Smart Parking </title>
        <style>
          html {
            font-family: Helvetica;
            display: inline-block;
            margin: 0px auto;
            text-align: center;
          }
          body {
            margin-top: 50px;
          }
          h1 {
            color: #444444;
            margin: 50px auto 30px
          }
        </style>
      </head>

      <body>
        <h1> GoodBye </h1>
      </body>
    </html>

  )===";

  return ptr;
}