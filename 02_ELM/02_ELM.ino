// Projekt Arduino-hc_05-elm327-car
//

#include <LiquidCrystal.h> //Dołączenie bilbioteki od wyświetlacza 2x16
LiquidCrystal lcd(2, 3, 4, 5, 6, 7); //Informacja o podłączeniu nowego wyświetlacza - piny do ktorych jest podlaczony

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2); //Deklaracja typu wyświetlacza - 2x16
  lcd.setCursor(0, 0); //Ustawienie kursora
  lcd.print("Czytnik ELM327"); //Wyświetlenie tekstu
  lcd.setCursor(0, 1); //Ustawienie kursora
  lcd.print("Na Arduino"); //Wyświetlenie tekstu
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
   lcd.clear();

}
