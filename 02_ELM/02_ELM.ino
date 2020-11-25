// Projekt Arduino-hc_05-elm327-car
//zmienić to paskudne goto~!
//

#include <LiquidCrystal.h> //Dołączenie bilbioteki od wyświetlacza 2x16
LiquidCrystal lcd(3, 4, 5, 6, 7, 8); //Informacja o podłączeniu nowego wyświetlacza - piny do ktorych jest podlaczony

#define ButtonPin 2

byte inData;
char inChar;
String BuildINString="";
String DisplayString="";
String WorkingString="";
long DisplayValue;
long A;

int tribe = 0;

void setup() {
  // put your setup code here, to run once:

  attachInterrupt(digitalPinToInterrupt(ButtonPin), button_func, RISING);
  
  lcd.begin(16, 2); //Deklaracja typu wyświetlacza - 2x16
  lcd.setCursor(0, 0); //Ustawienie kursora
  lcd.print("Czytnik ELM327"); //Wyświetlenie tekstu
  lcd.setCursor(0, 1); //Ustawienie kursora
  lcd.print("Na Arduino"); //Wyświetlenie tekstu
  delay(2000);
  lcd.clear();

//  Retry:
//  lcd.setCursor(0, 0);
//  lcd.print("                    ");
//  lcd.setCursor(0, 1);
//  lcd.print("Connecting......    ");
//
//  Serial.begin(38400);   //initialize Serial 
//
//    Serial.println("ATZ"); // komenda AT 'reset all' - zasilanie zostanie wylaczone, a nastepnie ponownie wlaczone, wszystkie ustawienia wracaja do domyslnych
//  lcd.setCursor(0, 0);
//  lcd.print("ELM327 TZ    ");
//  delay(2000);
//   ReadData();
//
//                                              // If used substring(1,4)=="ATZ" needed a space before ATZ in Serial Monitor and it did not work
//  if (BuildINString.substring(1,3)=="TZ")    // MIATA RESPONSE TO ATZ IS ATZ[[[ELM327 V1.5  OR AT LEAST THAT IS WHAT ARDUINO HAS IN THE BUFFER
//    {
//      lcd.clear();
//      lcd.setCursor(0, 0);
//      lcd.print("Welcome");
//      lcd.setCursor(9, 0);
//      //lcd.print(BuildINString);   //Echo response to screen "Welcome  ELM327"
//      lcd.setCursor(0, 1);
//      lcd.print("Connection OK         ");
//      delay(1500);
//      lcd.clear();
//    }
//    else
//    {
//      lcd.setCursor(0, 0);
//      lcd.print("Error             ");
//      lcd.setCursor(0, 1);
//      lcd.print("No Connection!         ");
//      delay(1500);
//      lcd.clear();
//      goto Retry;
//    }

    while(1){
  lcd.setCursor(0, 0);
  lcd.print("                    ");
  lcd.setCursor(0, 1);
  lcd.print("Connecting......    ");

  Serial.begin(38400);   //initialize Serial 

    Serial.println("ATZ"); // komenda AT 'reset all' - zasilanie zostanie wylaczone, a nastepnie ponownie wlaczone, wszystkie ustawienia wracaja do domyslnych
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
      break;
    }
    else
    {
      lcd.setCursor(0, 0);
      lcd.print("Error             ");
      lcd.setCursor(0, 1);
      lcd.print("No Connection!         ");
      delay(1500);
      lcd.clear();
      }
    }
    

  Serial.println("0100");          // wybor modulu 01 PID
  lcd.setCursor(0, 0);
  lcd.print("Initialzing.....");          
  delay(4000);
   ReadData();
   lcd.setCursor(0, 0);            //Added 12-10-2016
   lcd.print("Initialized.....");  //Added 12-10-2016
   delay(1000);
   lcd.clear();

  
}

void loop() {
  // put your main code here, to run repeatedly:
   switch(tribe){
     case 0:
      coolant_temp();
     break;
    
     case 1:
      input_temp();
     break;

     case 2:
      engine_rpm();
     break;

     case 3:
      car_speed();
     break;

     case 4:
      engine_load();
     break;
 
     default:
      coolant_temp();
   }
}

//---======###### FUNKCJE ######======---

//---====== CZYTANIE UART ======---
void ReadData(){
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

//---====== OBSŁUGA ODEBRANYCH WIADOMOŚCI ======---

//--- TEMPERATURA PŁYNU CHŁODNICZEGO ---
void coolant_temp(){
  // wyswietlanie na wyswietlaczu aktualnej temperatury płynu chłodzącego
  lcd.setCursor(0, 0);
  lcd.print("Coolant Temp    "); 

  //resets the received string to NULL  Without it it repeated last string.
  BuildINString = "";  

  Serial.println("0105");  // Send Coolant PID request 0105
  delay(1000);

  // Receive complete string from the serial buffer
  ReadData();  //replaced below code


   // otrzymana wartoscc ma forme "41 05 98 3B A0 13" interesuje nas tylko ostatnia para cyfr, więc znaki nr 11 i 12
  WorkingString = BuildINString.substring(11,13);   

    A = strtol(WorkingString.c_str(),NULL,16);  //konwersja z stringa ( w notacji HEX = 16) do long int/ .c_str() konwertuje str na ciąg, który może być zapisany w tablicy znaków

   DisplayValue = A;
   DisplayString = String(DisplayValue - 40) + " C            ";  // od otrzymanej wartości musimy odjąć 40 aby otrzymać wynik w `C
   lcd.setCursor(0, 1);
   lcd.print(DisplayString); 
   delay(500);
  
}
//--- TEMPERATURA  POWIETRZA WLOTOWEGO ---
void input_temp(){
  // wyswietlanie na wyswietlaczu aktualnej temperatury płynu chłodzącego
  lcd.setCursor(0, 0);
  lcd.print("Input Temp    "); 

  //resets the received string to NULL  Without it it repeated last string.
  BuildINString = "";  

  Serial.println("010F");  // Send Coolant PID request 0105
  delay(1000);

  // Receive complete string from the serial buffer
  ReadData();  //replaced below code


   // otrzymana wartoscc ma forme "41 0F 2C" interesuje nas tylko ostatnia para cyfr, więc znaki nr 5 i 6
  WorkingString = BuildINString.substring(5,7);   

    A = strtol(WorkingString.c_str(),NULL,16);  //konwersja z stringa ( w notacji HEX = 16) do long int/ .c_str() konwertuje str na ciąg, który może być zapisany w tablicy znaków

   DisplayValue = A;
   DisplayString = String(DisplayValue - 40) + " C            ";  // od otrzymanej wartości musimy odjąć 40 aby otrzymać wynik w `C
   lcd.setCursor(0, 1);
   lcd.print(DisplayString); 
   delay(500);
  
}

//--- OBROTY SILNIKA ---
void engine_rpm(){
  // wyswietlanie na wyswietlaczu aktualnych obrotów
  lcd.setCursor(0, 0);
  lcd.print("Engine RPM    "); 

  //resets the received string to NULL  Without it it repeated last string.
  BuildINString = "";  

  Serial.println("010C");  // Send PID request 010C
  delay(1000);

  // Receive complete string from the serial buffer
  ReadData();  //replaced below code


   // otrzymana wartoscc ma forme "41 0C 0B  D2 41 0C 0B D0" interesuje nas tylko ostatnie 4 cyfry, więc znaki nr 5, 6, 7, 8.
  WorkingString = BuildINString.substring(5,9);   

   A = strtol(WorkingString.c_str(),NULL,16);  //konwersja z stringa ( w notacji HEX = 16) do long int

   DisplayValue = int(A/4); // aby otrzymać wartość RPM, trzeba otrzymaną wartość podzielić przez 4
   DisplayString = String(DisplayValue) + " RPM            ";  
   lcd.setCursor(0, 1);
   lcd.print(DisplayString); 
   delay(500);
  
}

//--- PRĘDKOŚĆ ---
void car_speed(){
  // wyswietlanie na wyswietlaczu aktualnej prędkości pojazdu
  lcd.setCursor(0, 0);
  lcd.print("Car Speed    "); 

  //resets the received string to NULL  Without it it repeated last string.
  BuildINString = "";  

  Serial.println("010D");  // Send PID request 010D
  delay(1000);

  // Receive complete string from the serial buffer
  ReadData();  //replaced below code


   // otrzymana wartoscc ma forme "41 0D 00 41 0D 00" interesuje nas tylko ostatnia para cyfr, więc znaki nr 11 i 12
  WorkingString = BuildINString.substring(11,13);   

   A = strtol(WorkingString.c_str(),NULL,16);  //konwersja z stringa ( w notacji HEX = 16) do long int

   DisplayValue = A; // aby otrzymać wartość RPM, trzeba otrzymaną wartość podzielić przez 4
   DisplayString = String(DisplayValue) + " km/h            ";  
   lcd.setCursor(0, 1);
   lcd.print(DisplayString); 
   delay(500);
  
}

//--- OBCIĄŻENIE SILNIKA ---
void engine_load(){
  // wyswietlanie na wyswietlaczu aktualnego obciążenia silnika
  lcd.setCursor(0, 0);
  lcd.print("Engine Load    "); 

  //resets the received string to NULL  Without it it repeated last string.
  BuildINString = "";  

  Serial.println("0104");  // Send PID request 010D
  delay(1000);

  // Receive complete string from the serial buffer
  ReadData();  //replaced below code


   // otrzymana wartoscc ma forme "41 04 74" interesuje nas tylko ostatnia para cyfr, więc znaki nr 5 i 6
  WorkingString = BuildINString.substring(5,7);   

   A = strtol(WorkingString.c_str(),NULL,16);  //konwersja z stringa ( w notacji HEX = 16) do long int

  // DisplayValue = int((A/255)*100); // aby otrzymać % obciazenia, dzielimy otrzymana wartosc przez wartosci max (FF = 255) i mnozymy *100%
   DisplayValue = A;
   DisplayString = String(DisplayValue) + " %            ";  
   lcd.setCursor(0, 1);
   lcd.print(DisplayString); 
   delay(500);
  
}



void button_func(){
  lcd.clear();
  if(tribe < 4 ) tribe++;
  else tribe = 0;
  delay(20);
}
