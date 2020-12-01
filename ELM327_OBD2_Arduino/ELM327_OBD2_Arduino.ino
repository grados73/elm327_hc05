// Projekt Arduino-hc_05-elm327-car
// Autor:
//

#include <Wire.h>   // standardowa biblioteka Arduino
#include <LiquidCrystal_I2C.h> // dolaczenie pobranej biblioteki I2C dla LCD

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define ButtonPin 2 // PIN do którego podłączony jest przycisk


//zmienne do obsługi odebranych komunikatów
byte inData;
char inChar;
String BuildINString="";
String DisplayString="";
String WorkingString="";
String WorkingString2="";
long DisplayValue;
long A;
long B;
float F;
float F2;

// zmienne do liczenia czasu
unsigned long aktualnyCzas = 0;
unsigned long zapamietanyCzas = 0;
unsigned long roznicaCzasu = 0;

//flaga wskazująca zmianę trybu wyświetlania
int flaga1 = 0;
// tryb wyświetlania
int tribe = 0;

void setup() {

  // przerwanie od przycisku
  attachInterrupt(digitalPinToInterrupt(ButtonPin), button_func, RISING);
 
  lcd.begin(16, 2); //Deklaracja typu wyświetlacza - 2x16
  lcd.backlight(); // zalaczenie podwietlenia 
  // Ekran startowy
  lcd.setCursor(0, 0); //Ustawienie kursora
  lcd.print("Czytnik ELM327"); //Wyświetlenie tekstu
  lcd.setCursor(0, 1); //Ustawienie kursora
  lcd.print("Na Arduino"); //Wyświetlenie tekstu
  delay(2000);
  lcd.clear();

// Zestawienie połączenia z ELM
    while(1){ //pętla, aż zostanie nawiązane połączenie z ELM
      lcd.setCursor(0, 0);
        lcd.print("                    ");
      lcd.setCursor(0, 1);
        lcd.print("Connecting......    ");
       
      Serial.begin(38400);   //initializazja uart z prędkością 38400 do HC-05
    
      Serial.println("ATZ"); // komenda AT 'reset all' - zasilanie zostanie wylaczone, a nastepnie ponownie wlaczone, wszystkie ustawienia wracaja do domyslnych
      lcd.setCursor(0, 0);
      lcd.print("ELM327 BT    ");
      delay(3000); // oczekiwanie na restart i odpowiedź, 
      
       ReadData();// Czytaj UART
                                                     
      if (BuildINString.substring(1,3)=="TZ")   // połączenie nawiązane poprawnie -  odpowiedz od ELM
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Welcome         ");
          lcd.setCursor(9, 0);
          lcd.setCursor(0, 1);
          lcd.print("Connection OK         ");
          delay(1500);
          lcd.clear();
          break;
        }
        else // brak połączenia z ELM
        {
          lcd.setCursor(0, 0);
          lcd.print("Error             ");
          lcd.setCursor(0, 1);
          lcd.print("No Connection!         ");
          delay(1500);
          lcd.clear();
          }
    }

    Serial.println("0100");          // wybor modulu 01 poleceń PID
    lcd.setCursor(0, 0);
    lcd.print("Initialzing.....");          
    delay(2000);
    ReadData();
    lcd.setCursor(0, 0);            
    lcd.print("Initialized.....");  
    delay(1000);
    lcd.clear();
}

//PETLA GŁÓWNA PROGRAMU
void loop() {

  aktualnyCzas = millis(); // Pobierz liczbę ms od startu 
  
   switch(tribe){
     case 0:
      battery_voltage();
     break;
     
     case 1:
      coolant_temp();
     break;
    
     case 2:
      input_temp();
     break;

     case 3:
      engine_rpm();
     break;

     case 4:
      car_speed();
     break;

     case 5:
      engine_load();
     break;
 
     case 6:
      tank_level();
     break;
     
     default:
     ;
   }
}


//#############################################################################################################################
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
//======================================================================================================================================================
//--------------------------------------------------------------------====== OBSŁUGA ODEBRANYCH WIADOMOŚCI ======---------------------------------------
//------------------------------------------------------------------------------ NAPIECIE AKUMULATORA --------------------------------------------------
      
      void battery_voltage(){ 
          // wyswietlanie na wyswietlaczu aktualnego napiecia na baterii
          lcd.setCursor(0, 0);
          lcd.print("Battery Voltage "); 
          if(flaga1){ // jesli byla zmiana trybu
              clean_down();
              flaga1=0;
          }
      
           //resetowanie ostatniego otrzymanego stringa, aby zrobić miejsce na nowy
           BuildINString = "";  
      
           Serial.println("AT RV");  //  komenda AT o wysłanie aktualnego napiecia akumulatora
           delay(1000);
      
           // Odbieranie odpowiedzi na AT RV
           ReadData();  //Przypisanie ich do BuildINString
           // wiadomosc ma format "ATRV 14.4V"
           DisplayString = BuildINString.substring(6,11); // interesuje nas wiadomosc na bajtach od 6 do 10 (XY,Z)  
      
           DisplayString = (DisplayString) + "          ";  
           lcd.setCursor(0, 1);
           lcd.print(DisplayString); 
           delay(500);
        
      }

//---------------------------------------------------------------------------------------------------------------------------------------------------------------         
//---------------------------------------------------------------------TEMPERATURA PŁYNU CHŁODNICZEGO -----------------------------------------------------------

      void coolant_temp(){
          // wyswietlanie na wyswietlaczu aktualnej temperatury płynu chłodzącego
          lcd.setCursor(0, 0);
          lcd.print("Coolant Temp   "); 
          if(flaga1){
              clean_down();
              flaga1=0;
          }
      
          //resetowanie ostatniego otrzymanego stringa, aby zrobić miejsce na nowy
          BuildINString = "";  
      
          Serial.println("0105");  //  Coolant temp PID - zapytanie 0105
          delay(1000);
      
          // Odbieranie odpowiedzi na PID 0105
          ReadData();  //Przypisanie ich do BuildINString
      
      
          // otrzymana wartoscc ma forme "010541 05 61 41 05 61" interesuje nas tylko ostatnia para cyfr, więc znaki nr 11 i 12
           WorkingString = BuildINString.substring(11,13); 
        
           A = strtol(WorkingString.c_str(),NULL,16);  //konwersja z stringa ( w notacji HEX = 16) do long int/ .c_str() 
                                                       //konwertuje str na ciąg, który może być zapisany w tablicy znaków
        
           DisplayValue = A;
           DisplayString = String(DisplayValue - 40) + " C            ";  // od otrzymanej wartości musimy odjąć 40 aby otrzymać wynik w `C
           lcd.setCursor(0, 1);
           lcd.print(DisplayString); 
           delay(500);
        
      }
//---------------------------------------------------------------------------------------------------------------------------------------------------------------      
//-------------------------------------------------------------- TEMPERATURA  POWIETRZA WLOTOWEGO ---------------------------------------------------------------
     
      void input_temp(){
        // wyswietlanie na wyswietlaczu aktualnej temperatury płynu chłodzącego
        lcd.setCursor(0, 0);
        lcd.print("Input Temp      "); 
        if(flaga1){
          clean_down();
          flaga1=0;
        }
      
        BuildINString = "";  
      
        Serial.println("010F"); // PID 010F
        delay(1000);
      
        ReadData();  
      
         // otrzymana wartoscc ma forme "010F41 0F 32" interesuje nas tylko ostatnia para cyfr, więc znaki nr 11 i 12
        WorkingString = BuildINString.substring(11,13);  
      
         A = strtol(WorkingString.c_str(),NULL,16);  //konwersja z stringa ( w notacji HEX = 16) do long int/ .c_str()
         DisplayValue = A;
         DisplayString = String(DisplayValue - 40) + " C            ";  // od otrzymanej wartości musimy odjąć 40 aby otrzymać wynik w `C
         lcd.setCursor(0, 1);
         lcd.print(DisplayString); 
         delay(500);
        
      }
//---------------------------------------------------------------------------------------------------------------------------------------------------     
//------------------------------------------------------------ OBROTY SILNIKA -----------------------------------------------------------------------
     
      void engine_rpm(){
        // wyswietlanie na wyswietlaczu aktualnych obrotów
        lcd.setCursor(0, 0);
        lcd.print("Engine RPM      ");
        if(flaga1){
          clean_down();
          flaga1=0;
        }
      
        BuildINString = "";  
      
        Serial.println("010C");  // Send PID request 010C
        delay(1000);
      
        // Receive complete string from the serial buffer
        ReadData();  //replaced below code
      
      
         // otrzymana wartoscc ma forme "010C41 0C 0B  D2 41 0C 0B D0" interesuje nas tylko ostatnie 4 cyfry, więc znaki nr 11, 12 i 14 i 15
        WorkingString = BuildINString.substring(11,13);  // bardziej znaczace bajty
        WorkingString2 = BuildINString.substring(14, 16); // mniej znaczące bajty
        
        //konwersja z stringa ( w notacji HEX = 16) do long int
         A = strtol(WorkingString.c_str(),NULL,16); // bardziej znaczacy bajt
         B = strtol(WorkingString2.c_str(),NULL,16); // mniej znaczacy bajt

         //konwesrja do float
         F= float(A); 
         F2= float(B);
      
         DisplayValue = long((256.0*F+F2)/4.0); // wzor na wartosc RPM
         DisplayString = String(DisplayValue) + "  RPM            ";  
         lcd.setCursor(0, 1);
         lcd.print(DisplayString); 
         delay(500);  
      }
//---------------------------------------------------------------------------------------------------------------------------------------------------------------     
//-------------------------------------------------------------------------- PRĘDKOŚĆ ---------------------------------------------------------------------------
      
      void car_speed(){
        // wyswietlanie na wyswietlaczu aktualnej prędkości pojazdu
        lcd.setCursor(0, 0);
        lcd.print("Car Speed       "); 
        if(flaga1){
            clean_down();
            flaga1=0;
        }
      
        BuildINString = "";  
      
        Serial.println("010D");  // Send PID request 010D
        delay(1000);
      
        ReadData(); 
      
         // otrzymana wartoscc ma forme "010D41 0D 00 41 0D 00" interesuje nas tylko ostatnia para cyfr, więc znaki nr 11 i 12
          WorkingString = BuildINString.substring(11,13); 
          A = strtol(WorkingString.c_str(),NULL,16);  //konwersja z stringa ( w notacji HEX = 16) do long int/ .c_str()
         DisplayValue = A; 
      
         DisplayString = String(DisplayValue) + " km/h            ";  
         lcd.setCursor(0, 1);
         lcd.print(DisplayString); 
         delay(100);
        
      }
//---------------------------------------------------------------------------------------------------------------------------------------------------------------      
//--------------------------------------------------------------------- OBCIĄŻENIE SILNIKA ----------------------------------------------------------------------
      void engine_load(){
        // wyswietlanie na wyswietlaczu aktualnego obciążenia silnika
        lcd.setCursor(0, 0);
        lcd.print("Engine Load    "); 
        if(flaga1){
            clean_down();
            flaga1=0;
        }
      
        BuildINString = "";  
      
        Serial.println("0104");  // PID 0104
        delay(1000);
      
        // Receive complete string from the serial buffer
        ReadData();  //replaced below code
      
      
         // otrzymana wartoscc ma forme "010441 04 74" interesuje nas tylko ostatnia para cyfr, więc znaki nr 11 i 12
        WorkingString = BuildINString.substring(11,13);  
      
         A = strtol(WorkingString.c_str(),NULL,16);  //konwersja z stringa ( w notacji HEX = 16) do long int
         F= float(A);
         
         DisplayValue = long(100.0*F/255.0); // aby otrzymać % obciazenia, dzielimy otrzymana wartosc przez wartosci max (FF = 255) i mnozymy *100%
        // DisplayValue = A;
         DisplayString = String(DisplayValue) + " %            ";  
         lcd.setCursor(0, 1);
         lcd.print(DisplayString); 
         delay(100);
        
      }

//---------------------------------------------------------------------------------------------------------------------------------------------------------------      
//--------------------------------------------------------------------- POZIOM PALIWA W BAKU --------------------------------------------------------------------

      void tank_level(){
        // wyswietlanie na wyswietlaczu aktualnego poziumu paliwa
        lcd.setCursor(0, 0);
        lcd.print("Fuel Tank LVL   "); 
        if(flaga1){
            clean_down();
            flaga1=0;
        }
      
        BuildINString = "";  
      
        Serial.println("012F");  // PID 012F
        delay(1000);
      
        ReadData(); 
      
         // otrzymana wartoscc ma forme "012F41 2F 90" interesuje nas tylko ostatnia para cyfr, więc znaki nr 11 i 12
        WorkingString = BuildINString.substring(11,13);
      
         A = strtol(WorkingString.c_str(),NULL,16);  //konwersja z stringa ( w notacji HEX = 16) do long int
         F= float(A);
          
         DisplayValue = long(100.0*F/256.0); // 
         DisplayString = String(DisplayValue) + " %            ";  
         lcd.setCursor(0, 1);
         lcd.print(DisplayString); 
         delay(500);
        
      }
//================================================================================================================================================================      
//---============--------------------------------- FUNKCJA PRZERWANIA WYWOŁANA PRZYCISKIEM -----------------------------------------------------------------------
      
      void button_func(){
        roznicaCzasu = aktualnyCzas - zapamietanyCzas; // obliczanie czasu od ostatniego wciśnięcia przycisku
        if (roznicaCzasu >= 1000UL){ // przelaczanie trybu max co 500ms
          zapamietanyCzas = aktualnyCzas;
          flaga1 = 1;
          if(tribe < 6 ) tribe++;
          else tribe = 0;
        }
      }
//--------------------------------------------====== CZYSZCZENIE WYSWIETLACZA ======---------------------------------------------------------------
       
      void clean_down(){
        lcd.setCursor(0, 1);
        lcd.print("                ");
      }
      
      
