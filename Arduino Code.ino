#include <Wire.h>         /* Specific Libraries required for sensor module */
#include <OneWire.h> // For temperature //
#include <DallasTemperature.h> // For temperature//
#include <LiquidCrystal_I2C.h>; // For LCD with I2C module//
#define  ONE_WIRE_BUS 7

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire); // Instantiates temperature sensor object//

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27,16,2); // Instantiates lcd object.

float Celsius = 0;
unsigned int EchoPin = 4;   // The Arduino's  Pin4 connection to US-100 Echo / RX
unsigned int TrigPin = 6;   // The Arduino's Pin6 connected to US-100 Trig / TX
int sensorPin = A0; 
unsigned long Time_Echo_us = 0; //Time between trigger and echo recieved for ultrasconic sensor
unsigned int pingSpeed = 50; // How frequently are we going to send out a ping (in milliseconds). 50ms would be 20 times a second.
unsigned long pingTimer=0; // Holds the next ping time.

float calibration_value = 21.34-13.75; // Calibration value for Ph calculation.
int phval = 0; 

unsigned long int avgval;
int buffer_arr[10],temp; // Buffer values for 10 readings of temperature

//output pins of buzzer and the RGB colors of LED
int blueled = 9; 
int redled = 11;
int greenled = 10;
int buzzer = 3;


void setup() {
 
  
  sensors.begin();
  Serial.begin(9600); // setup serial monitor
  pingTimer = millis();
  pinMode(EchoPin, INPUT);    // Set EchoPin input mode.
  pinMode(TrigPin, OUTPUT);   // Set TriggerPin output mode.
  pinMode(blueled, OUTPUT);    // Set LED blue Pin output mode.
  pinMode(redled, OUTPUT);     // Set LED red Pin output mode.
  pinMode(greenled, OUTPUT);    // Set LED green Pin output mode.
  pinMode(buzzer, OUTPUT);
  pinMode(sensorPin,INPUT);
  pinMode(A2,INPUT);// Setbuzzer   Pin output mode.
 Initialization(); // Initializes the lCD display.
}

void loop() {
  
  
  lcd.clear(); // tO print starting message.
  lcd.setCursor(4,0);
 lcd.print("STARTING");
 lcd.setCursor(4,1);
  lcd.print("PROCESS");
  delay(1000);
  
  /* Comment out any of the below functions to test specific sensors as per your wish*/
  //Water_level(); // Process the readings from Ultrasonic sensor
  //Turbidity(); // Process the readings from  Turbidity sensor
  //pH(); // Process the readings from pH sensor
  
  Temperature(); // Process the readings from Temperature  sensor
  //delay(2000);  
  

}


// Function to print Initialization messages.//
void Initialization()
{
  
   digitalWrite(buzzer,HIGH);
    delay(1000);
    digitalWrite(buzzer,LOW);
  digitalWrite(blueled,HIGH);
  lcd.begin(16,2);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);//set cursor (colum by row) indexing from 0
  lcd.print("INITIALIZING ALL");
  lcd.setCursor(0,1);
  lcd.print("PARAMETERS");
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print("PARAMETERS.");
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print("PARAMETERS..");
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print("PARAMETERS...");
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print("PARAMETERS....");
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print("PARAMETERS.....");
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print("PARAMETERS......");
  Serial.println("Initializing All Parameters........");
  delay(4000);
  Serial.println("Initializing Done.");
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("INITIALIZATION");
  lcd.setCursor(6,1);
  lcd.print("DONE  ");
  delay(2000);
  digitalWrite(blueled,LOW);
  lcd.clear();
}
// Funtion to calculate the Distance of water surface from ultra sonic sensor
void Water_level() {
    Serial.println(" ");
   // Set the all colours of LED and buzzer to low.
  digitalWrite(blueled, LOW);
  digitalWrite(greenled, LOW);
  digitalWrite(redled, LOW);
  digitalWrite(buzzer, LOW);
  lcd.clear( );
  lcd.setCursor(0,0);//set cursor (colum by row) indexing from 0
  lcd.print("READING THE"); // Prints quoted message on LCD
  lcd.setCursor(0,1);
  lcd.print("WATER DIST");
  delay(2000);
  Serial.println("Taking Readings from Water Level Sensor");  //Prints quoted message on serial monitor
  digitalWrite(blueled, HIGH);
  level_check();
  
}


void level_check(){
  
  unsigned long Len_mm  = 0;
    
  if (millis() >= pingTimer) // pingSpeed milliseconds since last ping, do another ping. 
  {                                      
    pingTimer += pingSpeed;     // Set the next ping time.                         
    analogWrite(TrigPin,127); // Send out the ping, calls "echoCheck" function every 24uS where you can check the ping status. 
    
    Time_Echo_us = pulseIn(EchoPin, HIGH);
    if((Time_Echo_us < 60000) && (Time_Echo_us > 1)) // Pulse effective range (1, 60000).
    {   
  // Len_mm = (Time_Echo_us * 0.34mm/100) / 2 (mm)  // Formula to calculate distance from timeinterval/
        Len_mm = (Time_Echo_us*34/100)/2;
    }
    digitalWrite(blueled, LOW);
    lcd.clear( );
    lcd.setCursor(0,0);//set cursor (colum by row) indexing from 0
    lcd.print("WATER DIST:");
    lcd.setCursor(12,0);
    lcd.print(Len_mm);
    lcd.setCursor(14,0);
    lcd.print("mm");
    lcd.setCursor(0,1);
    Serial.print("Present Distance is: ");              // Output to the serial port monitor 
    Serial.print(Len_mm, DEC);                          // Output to the serial port monitor   
    Serial.println("mm");// Output to the serial port monitor  
  if(Len_mm > 70)
  {
    digitalWrite(blueled, LOW);
    digitalWrite(greenled, LOW);
    digitalWrite(redled, HIGH);   //Red LED glows since water level is low
    
    lcd.setCursor(0,1);    //set cursor (colum by row) indexing from 0
    lcd.print("LOW LEVEL");
    Serial.println("Water Level low");
    digitalWrite(buzzer,HIGH); // Buzzer turns on to notify us that distance is out of limits.
    delay(2000);
    digitalWrite(buzzer,LOW);
   
  }

  if(Len_mm >= 20 && Len_mm <= 70){
      digitalWrite(blueled, LOW);
      digitalWrite(greenled, HIGH); //Green LED glows since water level is withn limits.
      digitalWrite(redled, LOW);
  
      lcd.setCursor(0,1);//set cursor (colum by row) indexing from 0
      lcd.print("WATER LEVEL OKAY");
      Serial.println("Water Level normal");
      delay(2000);
  }
  
  if(Len_mm< 20){
    digitalWrite(blueled, LOW);
    digitalWrite(greenled, LOW);
   digitalWrite(redled, HIGH); //Red LED glows since water level is low
    
    
    
    digitalWrite(buzzer,HIGH);// Buzzer turns on to notify us that distance is out of limits.
    delay(2000);
    digitalWrite(buzzer,LOW);
    lcd.setCursor(0,1);//set cursor (colum by row) indexing from 0
    lcd.print("HIGH LEVEL");
    Serial.println("Water Level high");
    delay(2000);
   
   
     }
       
    delay(4000);
  }                                        

}
// Function to print turbidity value
void Turbidity() {
  Serial.println(" ");
  // Set the all colours of LED and buzzer to low.
  digitalWrite(blueled, LOW);
  digitalWrite(greenled, LOW);
  digitalWrite(redled, LOW);
  digitalWrite(buzzer, LOW);
  lcd.clear( );
  lcd.setCursor(0,0);//set cursor (colum by row) indexing from 0
  lcd.print("READING FOR");
  
  digitalWrite(blueled, HIGH);
  lcd.setCursor(0,1);
  lcd.print("TURBIDITY");
  delay(2000);
  Serial.println("Taking Readings from turbidity Sensor");
  
  Serial.print("Turbidity: ");
  lcd.clear( );
  
  
  int turbidity=0;
  //Take 10 readings.
  for(int i =0;i<10;i++)
  {int sensorValue  = analogRead(sensorPin);
  
  turbidity += map(sensorValue, 0,416, 100, 0); // maps vlotage value from sensor to a vale between 0 to 100 NTU.
  Serial.println(sensorValue);
  
  }
  if(turbidity<0)
  {
    turbidity  =0;
  }
  
  turbidity /= (10); // Average value
  digitalWrite(blueled, LOW);
  Serial.println(turbidity);
 
  lcd.clear( );
  lcd.setCursor(0,0);//set cursor (colum by row) indexing from 0
  lcd.print("TURBI LEV:");
  lcd.setCursor(11,0);
  lcd.print(turbidity);
  lcd.setCursor(13,0);
  lcd.print("NTU");
  
  
  if (turbidity <= 25) {
    digitalWrite(greenled, HIGH);  // Green LED glows since turbidity is low.
    digitalWrite(blueled, LOW);
    digitalWrite(redled, LOW);
    
    
    lcd.setCursor(0,1);//set cursor (colum by row) indexing from 0
    lcd.print("WATER VERY CLEAN");
    Serial.println("Its CLEAR ");
    delay(2000);
  }
  if ((turbidity > 25) ) {
    digitalWrite(greenled, LOW); 
    digitalWrite(blueled, LOW);
    digitalWrite(redled, HIGH); // Red LED glows since turbidity is high.
    
    lcd.setCursor(0,1);//set cursor (colum by row) indexing from 0
    lcd.print("WATER DIRTY");
    Serial.println("Its DIRTY ");
    digitalWrite(buzzer,HIGH); // Buzzer turns on to notify us that water is dirty
    delay(2000);
    digitalWrite(buzzer,LOW);
  }
 
  //delay(6000);
}

void pH(){
  Serial.println(" ");
  // Set the all colours of LED and buzzer to low.
  lcd.clear( );
  digitalWrite(blueled, LOW);
  digitalWrite(greenled, LOW);
  digitalWrite(redled, LOW);
  digitalWrite(buzzer, LOW);
  lcd.setCursor(0,0);//set cursor (colum by row) indexing from 0
  lcd.print("READING THE");
  digitalWrite(blueled, HIGH);
  lcd.setCursor(0,1);
  lcd.print("WATER PH");
  delay(2000);
  Serial.println("Taking Readings from PH Sensor");
  // Take 10 readings
  for(int i=0;i<10;i++) 
 { 
 buffer_arr[i]=analogRead(A2);
 delay(30);
 }
 //sort the array//
 for(int i=0;i<9;i++)
 {
 for(int j=i+1;j<10;j++)
 {
 if(buffer_arr[i]>buffer_arr[j])
 {
 temp=buffer_arr[i];
 buffer_arr[i]=buffer_arr[j];
 buffer_arr[j]=temp;
 }
 }
 }
 avgval=0;
 //take the average of middle 6 values starting from 3rd value//
 for(int i=2;i<8;i++)
 avgval+=buffer_arr[i];
 float volt=(float)avgval*5.0/1024/6; // Formula to convert voltage sensed to pH value.
 float phValue = -5.70 * volt + calibration_value;
 lcd.setCursor(0, 0);
 lcd.clear( );
 lcd.print("Ph Val:");
 digitalWrite(blueled, LOW);
 lcd.setCursor(8, 0);
 lcd.print(phValue);
 Serial.print("PH VALUE: ");
 Serial.println(phValue); 
    
 

 if(phValue >= 7.5){
    
    digitalWrite(blueled, LOW);
    digitalWrite(greenled, LOW);
    digitalWrite(redled, HIGH); // Red LED glows since pH is too high
    
    
    lcd.setCursor(0,1);//set cursor (colum by row) indexing from 0
    lcd.print("ALKALINITY HIGH");
    Serial.println("Water Alkalinity high");
    digitalWrite(buzzer,HIGH); // Buzzer turns on to notify us that pH is not in safe range.
    delay(2000);
    digitalWrite(buzzer,LOW);
   }

  if(phValue >= 5.5 && phValue < 7.50){
    digitalWrite(blueled, LOW);
    digitalWrite(greenled,HIGH); //Green LED glows since pH is in safe range.
    digitalWrite(redled, LOW);
    
     
    lcd.setCursor(0,1);//set cursor (colum by row) indexing from 0
    lcd.print("WATER IS SAFE");
    delay(2000);
    Serial.println("Water Is  neutral (safe)");
  }

  if(phValue < 5.5){
    
    digitalWrite(blueled, LOW);
    digitalWrite(greenled, LOW);
    digitalWrite(redled, HIGH); // Red LED glows since pH is too low
    
    lcd.setCursor(0,1);//set cursor (colum by row) indexing from 0
    lcd.print("ACIDITY HIGH");
    Serial.println("Water Acidity High");
    digitalWrite(buzzer,HIGH); // Buzzer turns on to notify us that pH is not in safe range.
    delay(2000);
    digitalWrite(buzzer,LOW);
   }
   
   delay(4000);
}
void Temperature(){
   // Set the all colours of LED and buzzer to low.
  digitalWrite(blueled, LOW);
  digitalWrite(greenled, LOW);
  digitalWrite(redled, LOW);
  Serial.println(" ");
  lcd.clear();
  lcd.setCursor(0,0); //set cursor (colum by row) indexing from 0
  lcd.print("TAKING READINGS");
  digitalWrite(blueled, HIGH);
  lcd.setCursor(0,1);
  lcd.print("FROM TEMP SENSOR");
  delay(2000);
  lcd.clear();
  sensors.requestTemperatures();
  Serial.println("Taking Readings from  Temperature Sensor");
  Serial.print("Temperature: ");
  digitalWrite(buzzer,LOW);
  Celsius = sensors.getTempCByIndex(0); // Get temp readings from sensor directly
  
  lcd.setCursor(0,0);//set cursor (colum by row) indexing from 0
  lcd.print("TEMP:");
  digitalWrite(blueled, LOW);
  lcd.setCursor(9,0);
  lcd.print(Celsius);
  lcd.setCursor(14,0);
  lcd.print(" C");
    

  Serial.print(Celsius);
  Serial.println(" C  ");
  
  if(Celsius > 40){
    digitalWrite(blueled, LOW);
    digitalWrite(greenled, LOW);
    digitalWrite(redled, HIGH); // Red LED glows since temperature is high
    
    lcd.setCursor(0,1);//set cursor (colum by row) indexing from 0
    lcd.print("HOT WATER");
    Serial.println("Water Temperature high");
    digitalWrite(buzzer,HIGH); // Buzzer turns on to notify us that temperature  is not in safe range.
    delay(2000);
    digitalWrite(buzzer,LOW);
  }

  if(Celsius >= 15 && Celsius <= 40){
    digitalWrite(blueled, LOW);
    digitalWrite(greenled,HIGH); //Green LED glows since temperature is in safe range.
    digitalWrite(redled, LOW);
    
   
    lcd.setCursor(0,1);//set cursor (colum by row) indexing from 0
    lcd.print("TEMP NORMAL");
    Serial.println("Water Temperature normal");
    delay(2000);
  }

  if(Celsius < 15){
    digitalWrite(blueled, LOW);
    digitalWrite(greenled, LOW);
    digitalWrite(redled, HIGH);  // Red LED glows since temperature is low
    
    lcd.setCursor(0,1);//set cursor (colum by row) indexing from 0
    lcd.print("COLD WATER");
    Serial.println("Water Temperature low");
    
   digitalWrite(buzzer,HIGH);// Buzzer turns on to notify us that temperature  is not in safe range.
    delay(2000);
    digitalWrite(buzzer,LOW);
  }
  //Process is compoleted. So, turn off all colours of LED and the BUZZER and print thank you.
    digitalWrite(blueled, LOW);
    digitalWrite(greenled, LOW);
    digitalWrite(redled, LOW);
    lcd.clear();
    lcd.setCursor(3,0);//set cursor (colum by row) indexing from 0
    lcd.print("COMPLETED");
    lcd.setCursor(4,1);
    lcd.print("PROCESS");
    delay(2000);
    lcd.clear();
    lcd.setCursor(3,0);//set cursor (colum by row) indexing from 0
    lcd.print("THANK YOU");
    
    
  delay(8000);
}
