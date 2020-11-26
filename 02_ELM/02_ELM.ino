// Projekt Arduino-hc_05-elm327-car
// Autor:
//

#include <LiquidCrystal.h> //Dołączenie bilbioteki od wyświetlacza 2x16
LiquidCrystal lcd(3, 4, 5, 6, 7, 8); //Informacja o podłączeniu nowego wyświetlacza - piny do ktorych jest podlaczony

#include <SoftwareSerial.h>
const int RX1 = 12;
const int TX1 = 13;
SoftwareSerial SoftSerialOne(RX1,TX1);


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

  pinMode(RX1, INPUT);
  pinMode(TX1, OUTPUT);
  SoftSerialOne.begin(9600);

  // przerwanie od przycisku
  attachInterrupt(digitalPinToInterrupt(ButtonPin), button_func, RISING);
 
  lcd.begin(16, 2); //Deklaracja typu wyświetlacza - 2x16
  // Ekran startowy
  lcd.setCursor(0, 0); //Ustawienie kursora
  lcd.print("Czytnik ELM327"); //Wyświetlenie tekstu
  lcd.setCursor(0, 1); //Ustawienie kursora
  lcd.print("Na Arduino"); //Wyświetlenie tekstu
  delay(2000);
  lcd.clear();

// Zestawienie połączenia z ELM
    while(1){
      lcd.setCursor(0, 0);
      lcd.print("                    ");
      lcd.setCursor(0, 1);
      lcd.print("Connecting......    ");
      SoftSerialOne.print("\nConecting ");
        

      Serial.begin(38400);   //initializazja uart z prędkością 38400
    
      Serial.println("ATZ"); // komenda AT 'reset all' - zasilanie zostanie wylaczone, a nastepnie ponownie wlaczone, wszystkie ustawienia wracaja do domyslnych
      lcd.setCursor(0, 0);
      lcd.print("ELM327 BT    ");
      delay(3000);
      
       ReadData();// Czytaj UART
    
                                                  
      if (BuildINString.substring(1,3)=="TZ")   // połączenie nawiązane poprawnie 
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
          SoftSerialOne.print("\nNO CONNECTION ");
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
          // wyswietlanie na wyswietlaczu aktualnej temperatury płynu chłodzącego
          lcd.setCursor(0, 0);
          lcd.print("Battery Voltage "); 
          if(flaga1){
            clean_down();
            flaga1=0;
          }
      
          //resetowanie ostatniego stringa, bo zaraz przyjdzie nowy
          BuildINString = "";  
      
        Serial.println("AT RV");  //  komenda AT o wysłanie aktualnego napiecia akumulatora
        delay(1000);
      
        // Odbieranie odpowiedzi na AT RV
        ReadData();  //Przypisanie ich do BuildINString
      
         DisplayString = BuildINString.substring(6,11); // od 5 do 8  
      
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
      
          //resetowanie ostatniego stringa, bo zaraz przyjdzie nowy
          BuildINString = "";  
      
        Serial.println("0105");  //  Coolant temp PID - zapytanie 0105
        delay(1000);
      
        // Odbieranie odpowiedzi na PID 0105
        ReadData();  //Przypisanie ich do BuildINString
      
      
         // otrzymana wartoscc ma forme "41 05 98 3B A0 13" interesuje nas tylko ostatnia para cyfr, więc znaki nr 11 i 12
        WorkingString = BuildINString.substring(11,13); 
        SoftSerialOne.print("\notemp chlodn BuildINSTRING: ");
        SoftSerialOne.print(BuildINString);
        SoftSerialOne.print(" temp chlod WorkingString: ");
        SoftSerialOne.print(WorkingString);  
      
          A = strtol(WorkingString.c_str(),NULL,16);  //konwersja z stringa ( w notacji HEX = 16) do long int/ .c_str() konwertuje str na ciąg, który może być zapisany w tablicy znaków
      
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
      
        Serial.println("010F");  // Send Coolant PID request 0105
        delay(1000);
      
        ReadData();  
      
         // otrzymana wartoscc ma forme "41 0F 2C" interesuje nas tylko ostatnia para cyfr, więc znaki nr 5 i 6 !! 11 i 12
        WorkingString = BuildINString.substring(11,13);  
        SoftSerialOne.print("\ntemperatura wlotowa BuildINSTRING: ");
        SoftSerialOne.print(BuildINString);
        SoftSerialOne.print(" temp wlot WorkingString: ");
        SoftSerialOne.print(WorkingString); 
      
          A = strtol(WorkingString.c_str(),NULL,16);  //konwersja z stringa ( w notacji HEX = 16) do long int/ .c_str() konwertuje str na ciąg, który może być zapisany w tablicy znaków
      
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
      
      
         // otrzymana wartoscc ma forme "41 0C 0B  D2 41 0C 0B D0" interesuje nas tylko ostatnie 4 cyfry, więc znaki nr 5, 6, 7 i 8. !! 15 16 18 19
        WorkingString = BuildINString.substring(11,13);  // bardziej znaczacy
        WorkingString2 = BuildINString.substring(14, 16);
        SoftSerialOne.print("\nobroty BuildINSTRING: ");
        SoftSerialOne.print(BuildINString);
        SoftSerialOne.print(" obroty WorkingString: ");
        SoftSerialOne.print(WorkingString);
        SoftSerialOne.print(" obroty WorkingString2: ");
        SoftSerialOne.print(WorkingString2);
        
      //konwersja z stringa ( w notacji HEX = 16) do long int
         A = strtol(WorkingString.c_str(),NULL,16); // bardziej znaczacy bajt
         B = strtol(WorkingString2.c_str(),NULL,16); // mniej znaczacy bajt

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
      
         // otrzymana wartoscc ma forme "41 0D 00 41 0D 00" interesuje nas tylko ostatnia para cyfr, więc znaki nr 11 i 12
        WorkingString = BuildINString.substring(11,13);  
        SoftSerialOne.print("\npredkosc BuildINSTRING: ");
        SoftSerialOne.print(BuildINString);
        SoftSerialOne.print(" predkosc WorkingString: ");
        SoftSerialOne.print(WorkingString); 
      
         DisplayString = WorkingString + " km/h            ";  
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
      
        //resets the received string to NULL  Without it it repeated last string.
        BuildINString = "";  
      
        Serial.println("0104");  // Send PID request 010D
        delay(1000);
      
        // Receive complete string from the serial buffer
        ReadData();  //replaced below code
      
      
         // otrzymana wartoscc ma forme "41 04 74" interesuje nas tylko ostatnia para cyfr, więc znaki nr 5 i 6 !! 10 i 11
        WorkingString = BuildINString.substring(11,13);  
        SoftSerialOne.print("\nobciązenie BuildINSTRING: ");
        SoftSerialOne.print(BuildINString);
        SoftSerialOne.print("obciązenie WorkingString: ");
        SoftSerialOne.print(WorkingString) ;
      
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
        // wyswietlanie na wyswietlaczu aktualnej prędkości pojazdu
        lcd.setCursor(0, 0);
        lcd.print("Fuel Tank LVL   "); 
        if(flaga1){
          clean_down();
          flaga1=0;
        }
      
        BuildINString = "";  
      
        Serial.println("012F");  // Send PID request 010D
        delay(1000);
      
        ReadData(); 
      
         // otrzymana wartoscc ma forme "41 2F 90" interesuje nas tylko ostatnia para cyfr, więc znaki nr 5 i 6 !! 11 i 12
        WorkingString = BuildINString.substring(11,13);
        SoftSerialOne.print("\n poziom paliwa BuildINSTRING: ");
        SoftSerialOne.print(BuildINString);   
        SoftSerialOne.print("poziom paliwa WorkingString: ");
        SoftSerialOne.print(WorkingString);
      
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
      
      
