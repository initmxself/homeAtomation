  //dht11 humidity and temperature sensor
  #include "DHT.h"
  #define DHTPIN 11
  #define DHTTYPE DHT11
  DHT dht(DHTPIN, DHTTYPE);
  
  // HX711 circuit wiring
  #include "HX711.h"
  HX711 scale;
  const int dOut = 2;
  const int sCk = 3;

 /* Corrosion evasion.
  *  Use digital pin 9, 8 as switches for the water level sensor 
  *  and moisture sensor. This means these sensors are only switched on when needed. 
  */
  int wSwitch = 9; 
  int mSwitch = 8;
  
  // the following code is for the water level sensor
  const int full = 575;
  const int low = 490;
  int waterSensor = A4; 
  
  int growthLED = 4;
  int cuctusPin = A0; 
  int deskLamp = 5; 
  int deskPump = A3; 
  int kettlePn = 7; 
  int tableLed = 6;
  int plantPump = A5; 
  int plantFan = 10;  
  int limit = 100;
  int tankVolume;
  int moistureValue;
  int kettleVolume;
  long scaleReading;
  long kThreshVol = 20000;
  
  
  
  String piCmd = ""; // receive data from the raspberry pi
  
  
  void setup() {
    Serial.begin(9600);
    pinMode(growthLED, OUTPUT);
    pinMode(wSwitch, OUTPUT);
    pinMode(mSwitch, OUTPUT);
    pinMode(deskLamp, OUTPUT);
    pinMode(deskPump, OUTPUT);
    pinMode(kettlePn, OUTPUT);
    pinMode(cuctusPin, INPUT);
    pinMode(waterSensor,INPUT);
    pinMode(plantPump, OUTPUT);
    pinMode(plantFan, OUTPUT);
    pinMode(tableLed, OUTPUT);
    scale.begin(dOut, sCk);
    dht.begin();    
  }


  void loop() {
    //kettleVolume = analogRead(scalePin);
    long scaleReading = scale.read();

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
     float humVal = dht.readHumidity();
    // Read temperature as Celsius (the default)
     float tempVal = dht.readTemperature();

     // Check if any reads failed and exit early (to try again).
     if (isnan(humVal) || isnan(tempVal)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
     }
  
     if (Serial.available() > 0) {
        
        piCmd = Serial.readStringUntil('\n');

        if(piCmd=="/jarfill"){
          digitalWrite(deskPump, HIGH); 
          Serial.println("Tap switched on...\n"); 
          delay(40000);
          digitalWrite(deskPump, LOW);
          Serial.println("Tap switched off...\n"); 
          delay(1000);
        }else if (piCmd=="fan0n"){
          digitalWrite(plantFan, HIGH);
          Serial.println("Fan switched on.");
        }else if (piCmd=="fan0ff"){
          Serial.println("Fan switched off.");
          digitalWrite(plantFan, LOW);
        }else if (piCmd=="led0n"){
          digitalWrite(growthLED, HIGH);
          Serial.println("Growth led switched on.");
        }else if (piCmd=="led0ff"){
          Serial.println("Growth led is off.");
          digitalWrite(growthLED, LOW);
        }else if (piCmd=="/h20"){
          digitalWrite(plantPump, HIGH);
          delay(15000);
          Serial.println("Watering started...\n"); 
          digitalWrite(plantPump, LOW);
          delay(1000);
          Serial.println("Done watering...\n"); 
        }
        else if (piCmd=="lamp0n"){
          Serial.println("Desk lamp switched on.");
          digitalWrite(deskLamp, HIGH);
          digitalWrite(tableLed, HIGH); 
        }else if (piCmd=="lamp0ff"){
          Serial.print("Switching off the desk lamp!");
          digitalWrite(deskLamp, LOW);
          digitalWrite(tableLed, LOW); 
         }else if (piCmd=="ktl0n"){
            if (scaleReading < kThreshVol){
              Serial.println("There isn\'t enough water in the kettle!");
            }
            if(scaleReading>=kThreshVol){
              digitalWrite(kettlePn, HIGH);
              Serial.println("Kettle switched on...");
              delay(240000);
              digitalWrite(kettlePn, LOW);
              Serial.println("Kettle switching off...");
            }
            else{
                Serial.print("Water volume not confirmed :(");
            }
          }
          
          if (piCmd=="/waterl"){
            digitalWrite(wSwitch, HIGH);
            delay(5000);
            tankVolume = analogRead(waterSensor);
            if(tankVolume<low){
              Serial.println("Tank volume is below half.");
            }else if(tankVolume>=full){
              Serial.println("There is enough water on the tank.");
            }else{
              Serial.println("Water level is medium.");
            }
            Serial.print("Tank volume: ");
            Serial.println(tankVolume);
            digitalWrite(wSwitch, LOW);
          }
          else if (piCmd=="/temp"){
            Serial.print("Temperature: "); 
            Serial.print(tempVal);
            Serial.println(" °C");
          }else if (piCmd=="/hum"){
            Serial.print("Humidity: "); 
            Serial.print(humVal);
            Serial.println(" %");
          }else if (piCmd=="/mois"){
            digitalWrite(mSwitch, HIGH);
            delay(5000);
            moistureValue = analogRead(cuctusPin);
            Serial.print("Moisture content: ");
            Serial.println(moistureValue);
            digitalWrite(mSwitch, LOW);
          }
          else if (piCmd=="/kv"){
            Serial.print("Kettle volume: ");
            Serial.println(scaleReading);
          }
          else if (piCmd=="/all"){
            digitalWrite(mSwitch, HIGH);
            digitalWrite(wSwitch, HIGH);
            delay(5000);
            tankVolume = analogRead(waterSensor);
            moistureValue = analogRead(cuctusPin);
            Serial.print("Temperature: ");
            Serial.print(tempVal);
            Serial.println(F("°C"));
            Serial.print("Humidity: ");
            Serial.print(humVal);
            Serial.println("%");
            Serial.print("Kettle volume: ");
            Serial.print(scaleReading);
            Serial.println(" ml");
            Serial.print("Moisture content: ");
            Serial.println(moistureValue);
            Serial.print("Tank volume: ");
            Serial.print(tankVolume);
            Serial.println();
            digitalWrite(mSwitch, LOW);
            digitalWrite(wSwitch, LOW);
          }

          /* 
           *  Temperature regulation. If the temperature value is less that the minimum value, a growth light is switched on. 
           *  In case the temperature is higher than expected, a cooling fan starts running.
           */
     
          if (tempVal<11){
            Serial.println("Temperature is very low. Turning on lights ...");
            while(tempVal<11){
              digitalWrite(growthLED, HIGH);
            }
    
          }else if (tempVal >= 30){
            Serial.println("Temperature is very high! Turning on fan...");
            digitalWrite(plantFan, HIGH);
            Serial.println(tempVal);
            delay(240000);
            digitalWrite(plantFan, LOW);
          }
  
          if(tankVolume<limit){
            Serial.println("Plant tank is running low on water, please refill.");
          } 
  
          /*code for watering the plant*/
          if (moistureValue<limit){
            Serial.println("The plant needs watering. Watering now...");
            digitalWrite(plantPump,HIGH);
            delay(200000);
            digitalWrite(plantPump,LOW);
            delay(1000);
            Serial.println("Watering done...");
          }
     }
}
