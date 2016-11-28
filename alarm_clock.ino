#include <IRremote.h>
#include <Wire.h>
#include<EEPROM.h>
#include <RTClib.h>
#include <LiquidCrystal.h>

#include "pitches.h"
LiquidCrystal lcd(2, 9, 4, 5, 6, 7);
RTC_DS1307 RTC;

int tmp,Inc,hor,mIn,add=11;

int nxt=8; // next button
int inc=10; // increment button
int set=12; // set alarm
int off=0;
#define buz 11

int Hor,Min,Sec;

unsigned char wake_up = 0x00;
unsigned char trigger = 0x00;
///////////////////////////////////////Function to adjust the time//////////////////////////////////
void time()
{
  int tmp=1,mins=0,hors=0,secs=0;
  while(tmp==1) {
    off=0;
    wake_up = 0x00;
    Serial.write(wake_up);
    if(digitalRead(inc)==0) {
      Hor++;
      if(Hor==24) {
        Hor=0;
      }
    }
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Set Alarm Time ");
    
    lcd.setCursor(0,1);
    if(Hor<=9)
    lcd.print("0");
    lcd.print(Hor);
    lcd.print(":");
    lcd.print(Min);
    lcd.print(":");
    lcd.print(Sec);
    delay(200);
    lcd.setCursor(0,1);
    lcd.print("  ");
    lcd.print(":");
    lcd.print(Min);
    lcd.print(":");
    lcd.print(Sec);
    delay(200);
    if(digitalRead(nxt)==0) {
      hor=Hor;
      EEPROM.write(add++,hor);
      tmp=2;
      while(digitalRead(nxt)==0);
    }
  }
  
  while(tmp==2) {
    if(digitalRead(inc)==0) {
      Min++;
      if(Min==60) {
        Min=0;
      }
    }
    lcd.setCursor(0,1);
    lcd.print(Hor);
    lcd.print(":");
    if(Min<=9)
    lcd.print("0");
    lcd.print(Min);
    lcd.print(":");
    lcd.print(Sec);
    lcd.print("  ");
    delay(200);
    lcd.setCursor(0,1);
    lcd.print(Hor);
    lcd.print(":");
    lcd.print("  ");
    lcd.print(":");
    lcd.print(Sec);
    lcd.print("  ");
    delay(200);
    if(digitalRead(nxt)==0) {
      mIn=Min;
      EEPROM.write(add++, mIn);
      tmp=0;
      while(digitalRead(nxt)==0);
    }
  }
  off=1; // alarm is on
  delay(10);
}

///////////////////////////////////////////function to sound the buzzer//////////////////////////////////
// notes in the song 'Mukkathe Penne'
int melody[] = {
  NOTE_D4, NOTE_G4, NOTE_FS4, NOTE_A4,
  NOTE_G4, NOTE_C5, NOTE_AS4, NOTE_A4,
  NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_FS4, NOTE_DS4, NOTE_D4,
  NOTE_C4, NOTE_D4,0,
  
  NOTE_D4, NOTE_G4, NOTE_FS4, NOTE_A4,
  NOTE_G4, NOTE_C5, NOTE_D5, NOTE_C5, NOTE_AS4, NOTE_C5, NOTE_AS4, NOTE_A4,      //29               //8
  NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_FS4, NOTE_DS4, NOTE_D4,
  NOTE_C4, NOTE_D4,0,
  
  NOTE_D4, NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_DS5, NOTE_D5,
  NOTE_C5, NOTE_AS4, NOTE_A4, NOTE_C5,
  NOTE_C4, NOTE_D4, NOTE_DS4, NOTE_FS4, NOTE_D5, NOTE_C5,
  NOTE_AS4, NOTE_A4, NOTE_C5, NOTE_AS4,             //58
  
  NOTE_D4, NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_DS5, NOTE_D5,
  NOTE_C5, NOTE_D5, NOTE_C5, NOTE_AS4, NOTE_C5, NOTE_AS4, NOTE_A4, NOTE_C5, NOTE_G4,
  NOTE_A4, 0, NOTE_AS4, NOTE_A4, 0, NOTE_G4,
  NOTE_G4, NOTE_A4, NOTE_G4, NOTE_FS4, 0,
  
  NOTE_C4, NOTE_D4, NOTE_G4, NOTE_FS4, NOTE_DS4,
  NOTE_C4, NOTE_D4, 0,
  NOTE_C4, NOTE_D4, NOTE_G4, NOTE_FS4, NOTE_DS4,
  NOTE_C4, NOTE_D4, END
  
};

// note durations: 8 = quarter note, 4 = 8th note, etc.
int noteDurations[] = {       //duration of the notes
  8,4,8,4,
  4,4,4,12,
  4,4,4,4,4,4,
  4,16,4,
  
  8,4,8,4,
  4,2,1,1,2,1,1,12,
  4,4,4,4,4,4,
  4,16,4,
  
  4,4,4,4,4,4,
  4,4,4,12,
  4,4,4,4,4,4,
  4,4,4,12,
  
  4,4,4,4,4,4,
  2,1,1,2,1,1,4,8,4,
  2,6,4,2,6,4,
  2,1,1,16,4,
  
  4,8,4,4,4,
  4,16,4,
  4,8,4,4,4,
  4,20,
};

int speed=90;  //higher value, slower notes


void Buz()
{
  if(Serial.available()){
    trigger = Serial.read();
    Serial.print("trigger:");
    Serial.print(trigger);
  }
  if(trigger == 0x01) {
    off = 0;
  }
  
  if(off==1) {

    for (int thisNote = 0; melody[thisNote]!=-1; thisNote++) {
      if(Serial.available()){
        trigger = Serial.read();
        Serial.print("trigger:");
        Serial.print(trigger);
      }
      
      if(trigger == 0x01) {
        off = 0;
      }
      else {
        Serial.write(wake_up);
        Serial.println(wake_up);
        int noteDuration = speed*noteDurations[thisNote];
        tone(11, melody[thisNote],noteDuration*.95);
        delay(noteDuration);
        
        noTone(11);
      }
      //wake_up = 0;
      //Serial.write(wake_up);
    }
  }
  trigger = 0x00;
}


//////////////////////////////////////////function to compare the alarm time with current RTC time//////////////////
void TimeCheck()
{
  int tem[17];
  for(int i=11;i<17;i++) {
    tem[i]=EEPROM.read(i);
  }
  if(Hor == tem[11] && Min == tem[12] && off==1) {
    add=11;
    lcd.clear();
    lcd.print("git up or       ");
    lcd.setCursor(0,1);
    lcd.print("git rekt        ");
    wake_up = 0x01;
    Buz();
  }

}

////////////////////////////////////////////////////////////setup///////////////////////////
void setup()
{
  Wire.begin();
  RTC.begin();
 
  
  lcd.begin(16,2);
  pinMode(inc, INPUT);
  pinMode(set, INPUT);
  pinMode(nxt, INPUT);
  pinMode(set, INPUT);
  pinMode(buz, OUTPUT);
  digitalWrite(nxt, HIGH);
  digitalWrite(set, HIGH);
  digitalWrite(inc, HIGH);

  lcd.setCursor(0,0);
  lcd.print("  Alarm Clock  ");
  delay(2000);

  if(!RTC.isrunning()) {
    RTC.adjust(DateTime(__DATE__,__TIME__));
  }
  Serial.begin(9600);
}
////////////////////////////////////////////////////////////loop/////////////////////////////////////
void loop()
{
  DateTime now = RTC.now();
  if(digitalRead(set) == 0) {
    
    current();
    time();
    delay(1000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  Alarm On");
    wake_up = 0x01;
    delay(2000);
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Time:");
  lcd.setCursor(6,0);
  Hor=now.hour(),DEC;
  if(Hor<=9) {
    lcd.print("0");
    lcd.print(Hor=now.hour(),DEC);
  }
  else
  lcd.print(Hor=now.hour(),DEC);
  lcd.print(":");
  Min=now.minute(),DEC;
  if(Min<=9) {
    lcd.print("0");
    lcd.print(Min=now.minute(),DEC);
  }
  else
  lcd.print(Min=now.minute(),DEC);
  wake_up = 0x00;
  Serial.write(wake_up);
  Serial.println(wake_up);

  lcd.print(":");
  Sec=now.second(),DEC;
  if(Sec<=9) {
    lcd.print("0");
    lcd.print(Sec=now.second(),DEC);
  }
  else
  lcd.print(Sec=now.second(),DEC);
  lcd.setCursor(0,1);
  lcd.print("Date: ");
  lcd.print(now.day(),DEC);
  lcd.print("/");
  lcd.print(now.month(),DEC);
  lcd.print("/");
  lcd.print(now.year(),DEC);
  TimeCheck();
  delay(1000);
}
///////////////////////////////////////////////////////function to get current RTC time//////////////////////////////
void current()
{
  lcd.setCursor(0,1);
  lcd.print(Hor);
  lcd.print(":");
  lcd.print(Min);
  lcd.print(":");
  lcd.print(Sec);
}

