#include <LiquidCrystal_I2C.h>
#include <SimpleDHT.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include <avr/wdt.h>
#include <Servo.h>

EthernetServer serveur(80);
LiquidCrystal_I2C lcd(0x27, 16, 2);

byte mac[] = {0x90, 0xA2, 0xDA, 0x0F, 0xDF, 0xAB};

byte temperature = 0;
byte humidity = 0;

int pinDHT11 = 2;
SimpleDHT11 dht11;

int initialState = 200;

int R = 6;
int G = 5;
int B = 4;

bool automatic = false;

byte temperatureOutdoor = 0;
byte humidityOutdoor = 0;
int pinDHT11Outdoor = 3;
SimpleDHT11 dht11Outdoor;

Servo servoSun;
Servo servoCloud;
Servo servoRain;

void setup() {
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");

  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  digitalWrite(R, HIGH);
  digitalWrite(G, HIGH);
  digitalWrite(B, HIGH);


  servoSun.attach(8);
  servoCloud.attach(9);
  servoRain.attach(10);

  servoSun.write(initialState);
  servoCloud.write(initialState);
  servoRain.write(initialState);

  Serial.begin (9600);

  Ethernet.begin (mac);
  Serial.print("\nLe serveur est sur l'adresse : ");
  Serial.println(Ethernet.localIP());
  lcd.clear();
  lcd.print("IP:");
  lcd.setCursor(3, 0);
  lcd.print(Ethernet.localIP());
  serveur.begin();
}


void loop() {
  getTempHum();
  //manageLcd();

  EthernetClient client = serveur.available();

  if (client) {
    Serial.println("Client en ligne\n");

    if (client.connected()) {
      String httpResponse;
      while (client.available()) {
        char c = client.read();
        httpResponse += c;

        Serial.write(c);

        if (httpResponse.indexOf("/reboot") != -1) {
          client.println("SUCCESS");
          client.stop();
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Rebooting...");
          delay(2000);
          reboot();
        } else if (httpResponse.indexOf("/data") != -1) {
          //Header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json;charset=utf-8");
          client.println("Access-Control-Allow-Origin: *");
          client.println("Server: Arduino");
          client.println("Connnection: close");
          client.println();

          //Content
          client.print("{\"outdoor\": {\"temperature\": \"" + (String)temperatureOutdoor + "\", \"humidity\": \"" + (String)humidityOutdoor + "\"}, \"indoor\": {\"temperature\": \"" + (String)temperature + "\", \"humidity\": \"" + (String)humidity + "\"}, \"automatic\": \"" + (String)automatic + "\"}");

          //Ends
          client.println();
          client.stop();
          Serial.println("Fin de communication avec le client");

        } else if (httpResponse.indexOf("/red-green") != -1) {
          digitalWrite(R, HIGH);
          digitalWrite(G, HIGH);
          digitalWrite(B, LOW);
          //sendHttpResponse(client, "success");
        } else if (httpResponse.indexOf("/green-blue") != -1) {
          digitalWrite(R, LOW);
          digitalWrite(G, HIGH);
          digitalWrite(B, HIGH);
          //sendHttpResponse(client, "success");
        } else if (httpResponse.indexOf("/blue-red") != -1) {
          digitalWrite(R, HIGH);
          digitalWrite(G, LOW);
          digitalWrite(B, HIGH);
          //sendHttpResponse(client, "success");
        } else if (httpResponse.indexOf("/red") != -1) {
          digitalWrite(R, HIGH);
          digitalWrite(G, LOW);
          digitalWrite(B, LOW);
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json;charset=utf-8");
          client.println("Access-Control-Allow-Origin: *");
          client.println("Server: Arduino");
          client.println("Connnection: close");
          client.println();
          client.print("{\"status\":\"success\"}");
          client.println();
          client.stop();
          //sendHttpResponse(client, "success");
        } else if (httpResponse.indexOf("/green") != -1) {
          digitalWrite(R, LOW);
          digitalWrite(G, HIGH);
          digitalWrite(B, LOW);
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json;charset=utf-8");
          client.println("Access-Control-Allow-Origin: *");
          client.println("Server: Arduino");
          client.println("Connnection: close");
          client.println();
          client.print("{\"status\":\"success\"}");
          client.println();
          client.stop();
          //sendHttpResponse(client, "success");
        } else if (httpResponse.indexOf("/blue") != -1) {
          digitalWrite(R, LOW);
          digitalWrite(G, LOW);
          digitalWrite(B, HIGH);
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json;charset=utf-8");
          client.println("Access-Control-Allow-Origin: *");
          client.println("Server: Arduino");
          client.println("Connnection: close");
          client.println();
          client.print("{\"status\":\"success\"}");
          client.println();
          client.stop();
          //sendHttpResponse(client, "success");
        } else if (httpResponse.indexOf("/white") != -1) {
          digitalWrite(R, HIGH);
          digitalWrite(G, HIGH);
          digitalWrite(B, HIGH);
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json;charset=utf-8");
          client.println("Access-Control-Allow-Origin: *");
          client.println("Server: Arduino");
          client.println("Connnection: close");
          client.println();
          client.print("{\"status\":\"success\"}");
          client.println();
          client.stop();
          //sendHttpResponse(client, "success");
        } else if (httpResponse.indexOf("/auto?on=true") != -1) {
          automatic = true;
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json;charset=utf-8");
          client.println("Access-Control-Allow-Origin: *");
          client.println("Server: Arduino");
          client.println("Connnection: close");
          client.println();
          client.print("{\"status\":\"success\",\"automatic\":\"" + (String)automatic + "\"}");
          client.println();
          client.stop();
        } else if (httpResponse.indexOf("/auto?on=false") != -1) {
          automatic = false;
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json;charset=utf-8");
          client.println("Access-Control-Allow-Origin: *");
          client.println("Server: Arduino");
          client.println("Connnection: close");
          client.println();
          client.print("{\"status\":\"success\",\"automatic\":\"" + (String)automatic + "\"}");
          client.println();
          client.stop();
        }

        delay(1);
      }

      client.println("Nothing to see here ! Please go to /data");
      client.stop();
    }

  }
}

/*void manageLcd() {
  lcd.setCursor(0, 0);
  lcd.print("IP:             ");
  lcd.setCursor(3, 0);
  lcd.print(Ethernet.localIP());
  delay(2000);
  if(automatic) {
    lcd.clear();
    lcd.print("Automatic: On");
  }
  }*/

/*void sendHttpResponse(EthernetClient client, String statusMsg) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json;charset=utf-8");
  client.println("Access-Control-Allow-Origin: *");
  client.println("Server: Arduino");
  client.println("Connnection: close");
  client.println();
  client.print("{\"status\":\"" + statusMsg + "\"}");
  client.println();
  client.stop();
  }*/

void getTempHum() {
  lcd.setCursor(0, 1);
  lcd.print("Tmp." + (String)temperatureOutdoor + (char)223 + "C " + "Hum." + (int)humidityOutdoor + "%");

  byte data[40] = {0};
  if (dht11.read(pinDHT11, &temperature, &humidity, data)) {
    Serial.print("Read DHT11 failed");
    return;
  }

  byte dataOutdoor[40] = {0};
  if (dht11Outdoor.read(pinDHT11Outdoor, &temperatureOutdoor, &humidityOutdoor, dataOutdoor)) {
    Serial.print("Read DHT11 failed");
    return;
  }

  if (temperatureOutdoor >= 20 && humidityOutdoor <= 50) {
    servoSun.write(90);
  } else {
    servoSun.write(initialState);
  }

  if (humidityOutdoor > 50 && humidityOutdoor <= 70) {
    servoCloud.write(90);
  } else {
    servoCloud.write(initialState);
  }

  if (humidityOutdoor > 70) {
    servoRain.write(90);
    servoCloud.write(initialState);
  } else {
    servoRain.write(initialState);
  }

  if (automatic) {
    automaticLed();
  }

  delay(1000);
}

void automaticLed() {
  if (temperatureOutdoor >= 20 && temperatureOutdoor < 23 && humidityOutdoor <= 50) {
    digitalWrite(R, HIGH);
    digitalWrite(G, HIGH);
    digitalWrite(B, LOW);
  } else if (temperatureOutdoor >= 23 && temperatureOutdoor < 26) {
    digitalWrite(R, LOW);
    digitalWrite(G, HIGH);
    digitalWrite(B, HIGH);
  } else if (temperatureOutdoor >= 26) {
    digitalWrite(R, HIGH);
    digitalWrite(G, LOW);
    digitalWrite(B, HIGH);
  } else {
    digitalWrite(R, HIGH);
    digitalWrite(G, HIGH);
    digitalWrite(B, HIGH);
  }
}

void reboot() {
  wdt_enable(WDTO_15MS);
  while (1) {}
}
