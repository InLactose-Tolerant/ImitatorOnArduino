//Importing libraries

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <LinkedList.h>

//Defining pins
const uint8_t green_led = 2;
const uint8_t led_button = 3;
const uint8_t buzzer_button = 4;
const uint8_t buzzer = 5;
const uint8_t latch_button = 6;
const uint8_t red_led = 8;

uint8_t maximum_time = 10; // If you want to test how many seconds your arduino can handle, be my guest. My one crashed at 12s

//Defining variables
boolean recorded = false;//will see if there is a recording to play or not
LinkedList<unsigned uint8_t> pattern = LinkedList<uint8_t>(); //1 is LED, 2 is Buzzer, 3 is buzzer and led, 0 is nothing. Will store the button pressed
LinkedList<unsigned int> timing = LinkedList<unsigned int>(); //Will store the time each button is pressed for EXACT timings
unsigned int current;//A variable that will (soon) hold the time that will be inputed into timming list
unsigned int prev_current = 0; //To track the difference in time to find out how much has passed
uint8_t timer = 0; //For the LCD display to show the timer
boolean blink = true; //Just for the blinking of red led and LCD while recording



LiquidCrystal_I2C lcd(0x27, 16, 2); //Defining LCD with comm. address + size

void setup() {
  Wire.begin();//For I2C communication
  lcd.init();//To talk to the LCD
  lcd.backlight();//Creating backlight
  
  //Defining pins
  pinMode(green_led, OUTPUT);
  pinMode(led_button, INPUT_PULLUP);
  pinMode(buzzer_button, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  pinMode(latch_button, INPUT_PULLUP);
  pinMode(red_led, OUTPUT);

}

void loop() {
  timer = 0;//reset timer
  if (digitalRead(latch_button) == HIGH){//If the latch button is unpressed
    if (recorded == false){ //If nothing has been recorded i,e user just booted it up
      lcd.setCursor(0, 0);
      lcd.print("Nothing"); //Print Nothing saved :(

    
      lcd.setCursor(0, 1);
      lcd.print("saved :(");
    }
    else{
      lcd.setCursor(0, 0);// If something is recorded than gives instructions to click buzzer and led button
      lcd.print("Click both button"); 

    
      lcd.setCursor(0, 1);
      lcd.print("To play recorded");
      if (digitalRead(buzzer_button) == LOW && digitalRead(led_button) == LOW){ // If the two buttons are pressed show the replay
        replay();
      }
    }
  }
  else{//else if the latch IS pressed
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Recording in"); 
      delay(1000);
//starting a 3 countdown timer, with red led + buzzer before starting recording function
      lcd.setCursor(0, 1);
      lcd.print("3");
      digitalWrite(red_led, HIGH);
      digitalWrite(buzzer, HIGH);
      delay(500);
      digitalWrite(red_led, LOW);
      digitalWrite(buzzer, LOW);
      delay(500);
      lcd.setCursor(0, 1);     
      lcd.print("2");
      digitalWrite(red_led, HIGH);
      digitalWrite(buzzer, HIGH);
      delay(500);
      digitalWrite(red_led, LOW);
      digitalWrite(buzzer, LOW);
      delay(500);
      lcd.setCursor(0, 1);  
      lcd.print("1");
      digitalWrite(red_led, HIGH);
      digitalWrite(buzzer, HIGH);
      delay(500);
      digitalWrite(red_led, LOW);
      digitalWrite(buzzer, LOW);
      delay(500);  
      lcd.clear();
      delay(500);
      pattern.clear();
      timing.clear();
      recording();

  }
}

void recording(){
  while (digitalRead(latch_button) == LOW && timer < maximum_time){//While the latch button remains pressed and the total time is under 10s (Otherwise it would crash after 12 s)
    current = millis(); //Function to wait a millisecond and stored in current
    if (current - prev_current >= 1000){// If it has been 1000 seconds since last checked
      timer += 1; //increase lcd timer by 1
      prev_current = current;//set difference between current and pre current to zero again
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print(timer);
      if (blink == true){
        lcd.setCursor(0, 0);
        lcd.print("Recording");
        blink = false;
      }
      else{
        lcd.setCursor(0, 0);
        lcd.print("in progress");
        blink = true;
      }
      if (digitalRead(red_led) == LOW){// to make red LED flahs every second
        digitalWrite(red_led, HIGH);
      }
      else{
        digitalWrite(red_led, LOW);
      }
    }
    if (digitalRead(led_button) == LOW && digitalRead(buzzer_button) == LOW){ //if the user presses both buttons
      pattern.add(3);//add 3 to patterns list and the relative time to the timing list
      timing.add(current);
      digitalWrite(green_led, HIGH);//shine both green led and buzzer
      digitalWrite(buzzer, HIGH);
      delay(100);//just so user can see
    }
    else if (digitalRead(led_button) == LOW) {//if the user presses the green led button only
      pattern.add(1);//add 1 to the pattern list and the relative time to the timing list
      timing.add(current);
      digitalWrite(green_led, HIGH);
      digitalWrite(buzzer, LOW);
      delay(100);    
    }
    else if (digitalRead(buzzer_button) == LOW){//if user pressed buzzer button only
      pattern.add(2);//add 2 to the pattern list and the relative time to the timing list
      timing.add(current);
      digitalWrite(buzzer, HIGH);
      digitalWrite(green_led, LOW);
      delay(100);
    }
    else{
      pattern.add(0);//if nothing is being pressed
      timing.add(current);// add 0 to the pattern list and relative time to the timing list
      digitalWrite(buzzer, LOW);//reset buzzer and green led
      digitalWrite(green_led, LOW);
      delay(100);
    }

  }
  digitalWrite(red_led, LOW);//after user is done recording turn of LED
  lcd.clear();
  while (digitalRead(latch_button) == LOW){//waits for user to press latch button
    lcd.setCursor(0, 0);
    lcd.print("Please press");
    lcd.setCursor(0, 1);
    lcd.print("To stop recording");
  }
  lcd.clear();
  recorded = true;

}

void replay(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Replaying");
  current = 0;//resets variables
  prev_current = 0;
  for (int i = 0; i<pattern.size(); i += 1){//repeat until i = pattern size
    
    current = millis();
    if (current - prev_current >= 1000){
      timer += 1;//there is another timer shown here
      prev_current = current;
      lcd.setCursor(0, 1);
      lcd.print(timer);
      }
    while (timing.get(i) > current){//waits for the corresponding relative time tp be = to time passed in replay
      current = millis();//continues the timer but stops everything else
    }
    if (pattern.get(i) == 3){//if corresponding number in list is 3, then play both items, and similar to other ones as well
      digitalWrite(buzzer, HIGH);
      digitalWrite(green_led, HIGH);
      delay(100);

    }
  
    else if (pattern.get(i) == 2){
      digitalWrite(buzzer, HIGH);
      digitalWrite(green_led, LOW);
      delay(100);

    }
    else if (pattern.get(i) == 1){
      digitalWrite(green_led, HIGH);
      digitalWrite(buzzer, LOW);
      delay(100);

    }
    else{
      digitalWrite(buzzer, LOW);
      digitalWrite(green_led, LOW);
      delay(100);
    }
  }
    lcd.setCursor(0,1);
    lcd.print("Done");//tell user replay is done
    delay(2000);
    lcd.clear();
  
}
