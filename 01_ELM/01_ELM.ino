void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  Serial.begin(9600);

}

//czerwony RX

char cmd[100];
byte cmdIndex;

void exeCmd(){
  if(cmd[0] == '1') {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else if(cmd[0] == '2') {
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available() > 0){
    char c = (char)Serial.read();
    if(c == '\n'){
      cmd[cmdIndex] = 0;
      exeCmd();
      cmdIndex = 0;
    } else{
      cmd[cmdIndex] = c;
      if(cmdIndex < 99) {
        cmdIndex++;
      }
    }
  }
}
