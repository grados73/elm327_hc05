// Projekt Arduino-hc_05-elm327-car
//
//

#include <LiquidCrystal.h> //Dołączenie bilbioteki od wyświetlacza 2x16
LiquidCrystal lcd(2, 3, 4, 5, 6, 7); //Informacja o podłączeniu nowego wyświetlacza - piny do ktorych jest podlaczony

byte inData;
char inChar;
String BuildINString="";
String DisplayString="";
String WorkingString="";
long DisplayValue;
long A;

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2); //Deklaracja typu wyświetlacza - 2x16
  lcd.setCursor(0, 0); //Ustawienie kursora
  lcd.print("Czytnik ELM327"); //Wyświetlenie tekstu
  lcd.setCursor(0, 1); //Ustawienie kursora
  lcd.print("Na Arduino"); //Wyświetlenie tekstu
  delay(2000);
  lcd.clear();

  Retry:
  lcd.setCursor(0, 0);
  lcd.print("                    ");
  lcd.setCursor(0, 1);
  lcd.print("Connecting......    ");

  Serial.begin(38400);   //initialize Serial 

    Serial.println("ATZ");
  lcd.setCursor(0, 0);
  lcd.print("ELM327 TZ    ");
  delay(2000);
   ReadData();

                                              // If used substring(1,4)=="ATZ" needed a space before ATZ in Serial Monitor and it did not work
  if (BuildINString.substring(1,3)=="TZ")    // MIATA RESPONSE TO ATZ IS ATZ[[[ELM327 V1.5  OR AT LEAST THAT IS WHAT ARDUINO HAS IN THE BUFFER
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Welcome");
      lcd.setCursor(9, 0);
      //lcd.print(BuildINString);   //Echo response to screen "Welcome  ELM327"
      lcd.setCursor(0, 1);
      lcd.print("Connection OK         ");
      delay(1500);
      lcd.clear();
    }
    else
    {
      lcd.setCursor(0, 0);
      lcd.print("Error             ");
      lcd.setCursor(0, 1);
      lcd.print("No Connection!         ");
      delay(1500);
      lcd.clear();
      goto Retry;
    }

     Serial.println("0100");          // Works with This  only
  lcd.setCursor(0, 0);
  lcd.print("Initialzing.....");          //Initialize BUS  //lcd.print("0100 Sent"); 
  delay(4000);
   ReadData();
   lcd.setCursor(0, 0);            //Added 12-10-2016
   lcd.print("Initialized.....");  //Added 12-10-2016
   delay(1000);
   lcd.clear();

  
}

void loop() {
  // put your main code here, to run repeatedly:
   
   
// Added to display Coolant Temp
  lcd.setCursor(0, 0);
  lcd.print("Coolant Temp    "); 


  //resets the received string to NULL  Without it it repeated last string.
  BuildINString = "";  

  Serial.println("0105");  // Send Coolant PID request 0105
  //Serial.flush();         //Not sure if it's needed*********************************************
  delay(1000);

  // Receive complete string from the serial buffer
  ReadData();  //replaced below code


// Parse the received string, retrieve only the hex value for temperature  Example: 32 is the 11 and 12th character
// 0105[41 05 32   Correct value reading back  10 degrees. (11,13 or even 11,14 works)
WorkingString = BuildINString.substring(11,13);   

    A = strtol(WorkingString.c_str(),NULL,16);  //convert hex to decimnal

   DisplayValue = A;
   DisplayString = String(DisplayValue - 40) + " C            ";  // Subtract 40 from decimal to get the right temperature
   lcd.setCursor(0, 1);
   lcd.print(DisplayString); 
   delay(500);

//Check if if over 100 C ******************************************************
int B;
B = DisplayString.toInt();  //Convert String to Integer  .toInt()


   if (B >= 100){     //Check if if over 100 C
    
// ------- Quick 3 blinks of backlight  -------------
  for(int i = 0; i< 3; i++)
  {
    lcd.display();   //For I2C use lcd.backlight
    delay(250);
    lcd.noDisplay(); //For I2C use lcd.noBacklight
    delay(250);
  }
  lcd.display(); // finish with backlight on   //For I2C use lcd.backlight
   }
 

}

//Read Data Function ***********************************************************
void ReadData()
{
BuildINString="";  
  while(Serial.available() > 0)
  {
    inData=0;
    inChar=0;
    inData = Serial.read();
    inChar=char(inData);
    BuildINString = BuildINString + inChar;
  }
}
