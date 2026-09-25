#include <AlmostRandom.h>
#include <InsertionSort.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

AlmostRandom ar;
LiquidCrystal_I2C lcd(0x27,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display

//starting mode
int mode = 10;

//buffer for time
char buffer[16];

unsigned long vorigetijd = 0;
const long interval = 1000;

//deployed time measure
int sec = 0;
int min = 0;
int uur = 0;

//timer
int timertime = 10;
int timermin = 0;
int timersec = 0;

//sensors
int stickpress = 0;
int sticky = 500;
int stickx = 500;
bool press = true;
bool ref0 = false;
bool ref5 = false;

String opties[] = {"Hallo", "goede (middag)?", "Bonjour", "welcome back", "booting...", "on standby", "ready"};
int lengte = sizeof(opties) / sizeof(opties[0]);
int random_raw =  ar.getRandomInt();
int random_index = (abs(random_raw) % lengte) + 1;

bool frame = false;

void setup()
{
  Serial.begin(9600);
  pinMode(8, OUTPUT);
  pinMode(4, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
}

void loop()
{
  stickpress = digitalRead(4);
  sticky = analogRead(A1);
  stickx = analogRead(A0);

  unsigned long huidigetijd = millis();//time active

  if (huidigetijd - vorigetijd >= interval) {
    vorigetijd = huidigetijd;

      sec++;
    
    if (sec > 59){
      min++;
      sec = 0;
    }

    if (min > 59){
      uur++;
      min = 0;
    }

    if (mode == 10 && frame == false){
      lcd.print(opties[random_index]);
      frame = true;
    } else if (mode == 10 && sec > 15){
      mode = 1;
    }
    
    if (mode == 1){//print time active

      lcd.clear();
      sprintf(buffer, "%d:%d:%d ", uur, min, sec); 

      lcd.setCursor(0,0);
      lcd.print(buffer);
      
    } else  if (mode == 2){//timer module
      
      digitalWrite(8,LOW);
      lcd.clear();
      lcd.setCursor(0,0);

      if (timertime > 0){
        timertime--;
        timermin = timertime/60;
        timersec = timertime-timermin*60;

        sprintf(buffer, "%02d:%02d ", timermin, timersec);
        lcd.print(buffer); 
      }
      if (timertime == 0){
        lcd.setCursor(0,0);
        lcd.print("de tijd is op");
        digitalWrite(8, HIGH);
        delay(1000);
        digitalWrite(8, LOW);
        delay(1000);
        digitalWrite(8, HIGH);
        delay(1000);
        mode = 1;
        digitalWrite(8, LOW);
      }
    }
  }

  if (press == true && stickpress == 0){
    lcd.setCursor(0,1);
    mode = 0;
  }
    if (mode == 0 && ref0 == false){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("timer       home");
    lcd.setCursor(0,1);
    lcd.print("        ?       ");
    ref0 = true;
    }

    if (mode == 0 && ref0 == true){

    if(stickx < 100 && sticky < 100){
      mode = 1;
      ref0 = false;
    } else if (stickx < 100 && sticky > 950){
      mode = 5;
      ref0 = false;
      timertime = 0;
    }

    }
    if (mode == 5 && ref5 == false){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("+1min      +5min");
      lcd.setCursor(0,1);
      lcd.print("          +10sec");

      delay(300);
      ref5 = true;
      press = false;
    } else if (mode == 5 && ref5 == true){

      if(stickx < 100 && sticky < 100){//right top
        timertime = timertime + 300;

        timermin = timertime/60;
        timersec = timertime-timermin*60;
        sprintf(buffer, "%02d:%02d    ", timermin, timersec);
        lcd.setCursor(0, 1);
        lcd.print(buffer);
        delay(1000);

      } else if (stickx < 100 && sticky > 900){// left top
        timertime = timertime + 60;

        timermin = timertime/60;
        timersec = timertime-timermin*60;
        sprintf(buffer, "%02d:%02d    ", timermin, timersec);
        lcd.setCursor(0, 1);
        lcd.print(buffer);
        delay(1000);

      } else if (stickx > 900 && sticky < 100){
        timertime = timertime + 10;

        timermin = timertime/60;
        timersec = timertime-timermin*60;
        sprintf(buffer, "%02d:%02d    ", timermin, timersec);
        lcd.setCursor(0, 1);
        lcd.print(buffer);
        delay(1000);

      } else if (stickpress == 0){
        mode = 2;
        delay (500);
        press = true;
        ref5 = false;
      }
    }
}
