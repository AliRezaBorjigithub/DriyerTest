#include <FlashStorage_STM32.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
//#include "RTClib.h"
//RTC_DS1307 rtc;
//-----------------------------define input pin-----------------------------------
int enter_button =PA15;
int up_button =PA11;
int down_button =PB15;
int esc_button =PB13;
int start_stop_button =PA12;
int vent_button =PB12;
int lamp_button =PA8;
int infro_button =PB14;
int temp_sensor = PA0;
int humi_sensor = PA1;

//-------------------------define output pins----------------------------------
int infrored = PB11;
int lamp = PB10;
int mashhal = PB1;
int ventolation = PB0;
int fan = PA7;
int bip = PA6;
int lampLED = PB3;
int startLED = PB4;
int infroLED = PB5;
int ventLED = PB8;
//-------------------------define varibles-------------------------
int temp;
int humi;
String device_stut = "Off";
bool device_status = 0;
int current_temp;
int current_hum;
unsigned long start_time = 0;
unsigned long current_time;;
bool vent_state = false;
bool lamp_stat = false;
bool infro_stat = false;
bool mashhal_stat = false;
int passcode;
int passAddress = 220;
int humidityAddress = 230;
unsigned long infoPageTime;
int timer = 1;
int newtemp;
bool ventState = false;
unsigned long displyBlynk;
//----------------------------------define temp and hum scan -------------------------
unsigned long lasttempscan = 0;
unsigned long lasthumscan = 0;
unsigned long currenttime;

//------------------------------------------------define-tem
//int time1[] = {30, 40, 50, 60, 70, 80, 90, 100, 110};
//int time2[] = {35, 45, 55, 65, 75, 85, 95, 105, 115};
//int time3[] = {40, 50, 60, 70, 80, 90, 100, 110, 120};
//int temp1[] = {30, 40, 50, 60, 70, 80, 90, 100, 110};
//int temp2[] = {35, 45, 55, 65, 75, 85, 95, 105, 115};
//int temp3[] = {40, 50, 60, 70, 80, 90, 100, 110, 120};
int code_number = 0;
int humidity;
int dif = 3;
int time1[9];
int time2[9];
int time3[9];
int temp1[9];
int temp2[9];
int temp3[9];
//----------------------------------------------load-data-from-eeprom----------------------
void loadData(){
  int eepromSize = 213;
  int listSize = 36;
  int data;
  for (int i = 0; i < eepromSize; i+= 4){
    EEPROM.get(i,data);
    int listIndex = i/listSize;
    int elmentIndex = i % listSize;
    switch(listIndex){
      case 0:
        time1[elmentIndex/4] = data;
        break;
      case 1:
        time2[elmentIndex/4] = data;
        break;
      case 2:
        time3[elmentIndex/4] = data;
        break;
      case 3:
        temp1[elmentIndex/4] = data;
        break;
      case 4:
        temp2[elmentIndex/4] = data;
        break;
      case 5:
        temp3[elmentIndex/4] = data;
        break;
      default:
        break;
    }
  }
}
//------------------------------------save-data-to-eeprom------------------------
void savedata(){
  saveListToEEPROM(time1, 0);
  saveListToEEPROM(time2, 36);
  saveListToEEPROM(time3, 72);
  saveListToEEPROM(temp1, 108);
  saveListToEEPROM(temp2, 144);
  saveListToEEPROM(temp3, 180);
}
void saveListToEEPROM(int lists[], int startAddress){
  for (int i = 0; i< 33; i+= 4){
    EEPROM.put(startAddress + i, lists[i/4]);
    delay(10);
    EEPROM.commit();
  }
}
//---------------------------------------------save-password-to-eeprom------------------------------
void savePassToEEPROM(int password){
  EEPROM.put(passAddress, password);
  delay(10);
  EEPROM.commit();
}
//---------------------------------------------save-hunidity-to-eeprom------------------------------
void savehumidityToEEPROM(int humidity){
  EEPROM.put(humidityAddress, humidity);
  delay(10);
  EEPROM.commit();
}

void seeParameter(){
  unsigned long blynk_time = millis();
  unsigned long currenTime = millis() - start_time;
  int total_time = time1[code_number] + time2[code_number] + time3[code_number]; 
  int remainTime = total_time - currenTime/60000;
  
  while (millis() - infoPageTime < 5000){
    
    lcd.setCursor(0, 0);
    lcd.print("Remaining Time: " + String(remainTime));
    
    lcd.setCursor(0, 1);
    lcd.print("time1 " + String(time1[code_number]));
    lcd.setCursor(10, 1);
    lcd.print("temp1 " + String(temp1[code_number]));
    
    lcd.setCursor(0, 2);
    lcd.print("time2 " + String(time2[code_number]));
    lcd.setCursor(10, 2);
    lcd.print("temp2 " + String(temp2[code_number]));
    
    lcd.setCursor(0, 3);
    lcd.print("time3 " + String(time3[code_number]));
    lcd.setCursor(10, 3);
    lcd.print("temp3 " + String(temp3[code_number]));
    
    if (digitalRead(esc_button) == HIGH){
       button_cliked(esc_button);
       break;
    }
    if (timer == 1 and millis() - blynk_time >= 400 and device_status == true){
      lcd.setCursor(0, 1);
      lcd.print("                    ");
      delay(200);
      blynk_time = millis();
    }
    if (timer == 2 and millis() - blynk_time >= 400 and device_status == true){
      lcd.setCursor(0, 2);
      lcd.print("                    ");
      delay(200);
      blynk_time = millis();
    }
    if (timer == 3 and millis() - blynk_time >= 400 and device_status == true){
      lcd.setCursor(0, 3);
      lcd.print("                    ");
      delay(200);
      blynk_time = millis();
    }
  }
  infoPageTime = 0;
  lcd.clear();
}

void setup() {
  //-------------------------------initialize the lcd------------------------------------------
  lcd.init();
  lcd.backlight();
  lcd.setCursor(4, 0);
  lcd.print("Negin Sazan");
  lcd.setCursor(4, 2);
  lcd.print("09151601847");
  delay(2000);
  Serial.begin(9600);
  //-------------------------------input pin pinmode ---------------------------------------
  pinMode(enter_button, INPUT);
  pinMode(up_button, INPUT);
  pinMode(down_button, INPUT);
  pinMode(esc_button, INPUT);
  pinMode(start_stop_button, INPUT);
  pinMode(vent_button, INPUT);
  pinMode(lamp_button, INPUT);
  pinMode(infro_button, INPUT);
  pinMode(temp_sensor, INPUT);
  pinMode(humi_sensor, INPUT);
  //----------------------------------output pin pinmode ---------------------------------------
  pinMode(fan, OUTPUT);
  pinMode(mashhal, OUTPUT);
  pinMode(infrored, OUTPUT);
  pinMode(lamp, OUTPUT);
  pinMode(ventolation, OUTPUT);
  pinMode(bip, OUTPUT);
  pinMode(lampLED, OUTPUT);
  pinMode(startLED, OUTPUT);
  pinMode(infroLED, OUTPUT);
  pinMode(ventLED, OUTPUT);
  digitalWrite(fan, LOW);
  digitalWrite(mashhal, LOW);
//  if (! rtc.begin()) {
//    lcd.clear();
//    lcd.setCursor(0, 0);
//    lcd.print("Time Read Error");
//    lcd.setCursor(0, 1);
//    lcd.print("Restart Device");
//    while (1) delay(10);
//  }
  digitalWrite(bip, HIGH);
  delay(50);
  digitalWrite(bip, LOW);

  //  rtc.adjust(DateTime(2024, 1, 29, 13, 18, 50));
  lcd.clear();
  loadData();
//  EEPROM.put(passAddress, 111);
//  delay(10);
//  EEPROM.commit();
//  EEPROM.put(humidityAddress, 12);
//  delay(10);
//  EEPROM.commit();
//----------------------------------------------load-password-and-humidity-from-eeprom----------------------
  EEPROM.get(passAddress, passcode);
  EEPROM.get(humidityAddress, humidity);
  Serial.println("Setup Done!");
}

void loop() {
  run_program();
}
int inittempscan = 0, looprefresh = 0;
void run_program() {
  while (true) { delay(0);
    if (currenttime - inittempscan >= 5000){
      inittempscan = currenttime;
      Serial.println("Loop!");
      lcd.init();
    }
    if (currenttime - looprefresh >= 10000){
      looprefresh = currenttime;
      break;
    }
    currenttime = millis(); 
    if (currenttime - lasttempscan >= 1000){
      lasttempscan = currenttime;
      newtemp = analogRead(temp_sensor);
      float tem = newtemp * 3.226;
      current_temp = tem/10;
      lcd.setCursor(13, 2);
      lcd.print("       ");
    }

    if (currenttime - lasthumscan >= 1100){
      lasthumscan = currenttime;
      int newhum = analogRead(humi_sensor); 
      current_hum = map(newhum, 0, 780, 0, 100);
      lcd.setCursor(13, 3);
      lcd.print("       ");
    }
    if (digitalRead(start_stop_button) == HIGH) {
      delay(1000);
      if (digitalRead(start_stop_button) == HIGH){
        button_cliked(start_stop_button);
        device_status = !device_status;
        start_time = currenttime;
      }
    }
    if (digitalRead(esc_button) == HIGH){
       button_cliked(esc_button);
       infoPageTime = currenttime;
       lcd.clear();
       seeParameter();
    }
    if (digitalRead(vent_button) == HIGH){   
        button_cliked(vent_button);
        vent_state = !vent_state;
        digitalWrite(ventolation, vent_state); 
        digitalWrite(ventLED, vent_state); 
    }
    if (digitalRead(lamp_button) == HIGH){   
        button_cliked(lamp_button);
        lamp_stat = !lamp_stat;
        digitalWrite(lamp, lamp_stat);
        digitalWrite(lampLED, lamp_stat);  
    }
    if (digitalRead(infro_button) == HIGH){   
        button_cliked(infro_button);
        infro_stat = !infro_stat;
        digitalWrite(infrored, infro_stat);
        digitalWrite(infroLED, infro_stat);
    }
    if (digitalRead(enter_button) == HIGH) {
      button_cliked(enter_button);
      menu();
    }
    if (digitalRead(up_button) == HIGH && device_status == false) {
      button_cliked(up_button);
      if(code_number == 8) code_number = 0;
      else code_number ++;
    }
    if (digitalRead(down_button) == HIGH && device_status == false) {
      button_cliked(down_button);
      if(code_number == 0) code_number = 8;
      else code_number --;
    }
    if (currenttime - displyBlynk >= 3000){
      displyBlynk = currenttime;
      lcd.clear();
    }
    
    lcd.setCursor(5, 0);
    lcd.print("HOME PAGE");
    lcd.setCursor(1, 1);
    lcd.print("code M" + String(code_number) + "     " + device_stut);
    lcd.setCursor(1, 2);
    lcd.print("temprature: " + String(current_temp) + " C");
    lcd.setCursor(1, 3);
    lcd.print("humidity:   " + String(current_hum) + " %");

    if (device_status == true){
      int total_time = time1[code_number] + time2[code_number] + time3[code_number];
      if(currenttime - start_time >= total_time *60*1000){
        device_status = false; 
      }
      if (current_hum < humidity){
        device_status = false; 
      } 
    }
    chack_device(); 
  }
}
void chack_device(){ 
  if (device_status == true){
    chack_mashhal();
    digitalWrite(fan, HIGH);
    digitalWrite(startLED, HIGH);
    device_stut = " On"; 
  }
  else {
    digitalWrite(mashhal, LOW);
    digitalWrite(fan, LOW);
    digitalWrite(startLED, LOW);
    device_stut = "Off";
    start_time = currenttime;
  }
}

void chack_mashhal(){
  int totalTwoTemp = time1[code_number] + time2[code_number];
  if (currenttime - start_time <= time1[code_number]*60*1000){
    timer = 1;
    if (current_temp >= temp1[code_number] and ventState == false) {
      digitalWrite(ventolation, HIGH); 
      digitalWrite(ventLED, HIGH); 
      ventState = true;
    }
    if (current_temp > temp1[code_number]) digitalWrite(mashhal, LOW);   
    if (current_temp <= temp1[code_number]-2) digitalWrite(mashhal, HIGH);
  }
  else if (currenttime - start_time > time1[code_number]*60*1000 and currenttime - start_time < totalTwoTemp*60*1000){
    timer = 2;
    if (current_temp > temp2[code_number]) digitalWrite(mashhal, LOW);
    if (current_temp <= temp2[code_number]-2) digitalWrite(mashhal, HIGH);
  }
  else{
    timer = 3;
    if (current_temp > temp3[code_number]) digitalWrite(mashhal, LOW);
    if (current_temp <= temp3[code_number]-2) digitalWrite(mashhal, HIGH);
  }
}

void menu() {
  int corsor = 1;
  lcd.clear();
  lcd.setCursor(8, 0);
  lcd.print("MENU");
  lcd.setCursor(1, 1);
  lcd.print("cook command:");
  lcd.setCursor(1, 2);
  lcd.print("humidity setting:");
  lcd.setCursor(1, 3);
  lcd.print("password seting:");
  lcd.setCursor(0, corsor);
  lcd.blink();
  while (true) { delay(0);
    if (digitalRead(down_button) == HIGH) {
      button_cliked(down_button);
      if (corsor == 3) corsor = 1;
      else corsor ++;
      lcd.setCursor(0, corsor);
      lcd.blink();
    }

    if (digitalRead(up_button) == HIGH) {
      button_cliked(up_button);
      if (corsor == 1) corsor = 3;
      else corsor --;
      lcd.setCursor(0, corsor);
      lcd.blink();
    }

    if (digitalRead(esc_button) == HIGH) {
      button_cliked(esc_button);
      lcd.clear();
      run_program();
    }

    if (digitalRead(enter_button) == HIGH) {
      button_cliked(enter_button);
      lcd.clear();
      if (corsor == 1) chack_password("code_setting");
      if (corsor == 2) humidity_setting();
      if (corsor == 3) chack_password("set_pass");
    }
  }
}

void humidity_setting() {
  int y_corsor = 2;
  int delay_time = 200;
  bool bt_down_state = false;
  bool bt_up_state = false;
  unsigned long bt_time = 0;
  unsigned long diorition = 0;
  while (true) { delay(0);
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("SET HUMIDITY");
    lcd.setCursor(1, 2);
    lcd.print("humidity:   " + String(humidity) + '%');
    lcd.setCursor(1, 3);
    lcd.print("difrent:    " + String(dif));
    lcd.setCursor(0, y_corsor);
    lcd.blink();
    while (true){ 
      if (digitalRead(enter_button) == HIGH) {
        button_cliked(enter_button);
        if (y_corsor == 3) y_corsor = 2;
        else y_corsor = 3;
        break;
      }

      if (digitalRead(down_button) == HIGH && bt_down_state == false) {
        bt_down_state = true;
        bt_time = millis();
        analogWrite(bip, 255);
        delay(50);
        analogWrite(bip, 0);
      }
      if (digitalRead(down_button) == HIGH && bt_down_state == true) {
        diorition = millis() - bt_time;
        if (diorition >= 2000) delay_time = 10;
        else delay_time = 200;
        delay(delay_time);
        if (y_corsor == 3) {
          if (dif == 1) dif = 10;
          else dif --;
        }

        if (y_corsor == 2) {
          if (humidity == 1) humidity = 100;
          else humidity --;
        }
        break;
      }
      if (digitalRead(down_button) == LOW && bt_down_state == true){
        delay(20);
        bt_down_state = false;
      }

      if (digitalRead(up_button) == HIGH && bt_up_state == false) {
        bt_up_state = true;
        bt_time = millis();
        analogWrite(bip, 255);
        delay(50);
        analogWrite(bip, 0);
      }
      if (digitalRead(up_button) == HIGH && bt_up_state == true) {
        diorition = millis() - bt_time;
        if (diorition >= 2000) delay_time = 10;
        else delay_time = 200;
        delay(delay_time);
        if (y_corsor == 3) {
          if (dif == 10) dif = 1;
          else dif ++;
        }

        if (y_corsor == 2) {
          if (humidity == 100) humidity = 1;
          else humidity ++;
        }
        break;
      }
      if (digitalRead(up_button) == LOW && bt_up_state == true){
        delay(20);
        bt_up_state = false;
      }
      
      
      if (digitalRead(esc_button) == HIGH) {
        button_cliked(esc_button);
        lcd.clear();
        lcd.setCursor(6, 1);
        lcd.print("SAVED");
        lcd.noBlink();
        savehumidityToEEPROM(humidity);
        delay(1000);
        lcd.clear();
        menu();
      }
    }
  }
}



void code_setting() {
  int x_corsor = 0;
  int y_corsor = 0;
  int num = 1;
  int  time_1 = 120;
  int  time_2 = 130;
  int  time_3 = 140;
  int  temp_1 = 120;
  int  temp_2 = 130;
  int  temp_3 = 140;
  int delay_time = 200;
  bool bt_down_state = false;
  bool bt_up_state = false;
  unsigned long bt_time = 0;
  unsigned long diorition = 0;

  while (true) { delay(0);
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("code number: " + String(code_number));

    lcd.setCursor(1, 1);
    lcd.print("t1:  " + String(time1[code_number]));
    lcd.setCursor(13, 1);
    lcd.print("c1: " + String(temp1[code_number]));

    lcd.setCursor(1, 2);
    lcd.print("t2:  " + String(time2[code_number]));
    lcd.setCursor(13, 2);
    lcd.print("c2: " + String(temp2[code_number]));

    lcd.setCursor(1, 3);
    lcd.print("t3:  " + String(time3[code_number]));
    lcd.setCursor(13, 3);
    lcd.print("c3: " + String(temp3[code_number]));

    lcd.setCursor(x_corsor, y_corsor);
    lcd.blink();
    while (true) { delay(0);
      if (digitalRead(enter_button) == HIGH) {
        button_cliked(enter_button);
        if (num == 7) num = 1;
        else num ++;

        if (num == 1) {
          x_corsor = 0;
          y_corsor = 0;
        }

        if (num == 2) {
          x_corsor = 0;
          y_corsor = 1;
        }

        if (num == 3) {
          x_corsor = 12;
          y_corsor = 1;
        }

        if (num == 4) {
          x_corsor = 0;
          y_corsor = 2;
        }

        if (num == 5) {
          x_corsor = 12;
          y_corsor = 2;
        }

        if (num == 6) {
          x_corsor = 0;
          y_corsor = 3;
        }

        if (num == 7) {
          x_corsor = 12;
          y_corsor = 3;
        }
        break;
      }

      if (digitalRead(esc_button) == HIGH) {
        button_cliked(esc_button);
        lcd.clear();
        lcd.noBlink();
        lcd.setCursor(6, 1);
        lcd.print("SAVED");
        savedata();
        menu();
      }
      if (digitalRead(up_button) == HIGH && bt_up_state == false) {
        bt_up_state = true;
        bt_time = millis();
        analogWrite(bip, 255);
        delay(50);
        analogWrite(bip, 0);
      }
      if (digitalRead(up_button) == HIGH && bt_up_state == true) {
        diorition = millis() - bt_time;
        if (diorition >= 2000) delay_time = 10;
        else delay_time = 200;
        delay(delay_time);
        if (num == 1) {
          if (code_number == 8) code_number = 0;
          else code_number ++;
        }

        if (num == 2) {
          if (time1[code_number] >= 420) time1[code_number] = 1;
          else time1[code_number] ++;
        }

        if (num == 3) {
          if (temp1[code_number] >= 150) temp1[code_number] = 20;
          else temp1[code_number] ++;
        }

        if (num == 4) {
          if (time2[code_number] >= 420) time2[code_number] = 1;
          else time2[code_number] ++;
        }

        if (num == 5) {
          if (temp2[code_number] == 150) temp2[code_number] = 20;
          else temp2[code_number] ++;
        }

        if (num == 6) {
          if (time3[code_number] >= 420) time3[code_number] = 1;
          else time3[code_number] ++;
        }

        if (num == 7) {
          if (temp3[code_number] >= 150) temp3[code_number] = 20;
          else temp3[code_number] ++;
        }

        break;
      }
      if (digitalRead(up_button) == LOW && bt_up_state == true){
        delay(20);
        bt_up_state = false;
      }
      if (digitalRead(down_button) == HIGH && bt_down_state == false) {
        bt_down_state = true;
        bt_time = millis();
        analogWrite(bip, 255);
        delay(50);
        analogWrite(bip, 0);
      }
      if (digitalRead(down_button) == HIGH && bt_down_state == true) {
        diorition = millis() - bt_time;
        if (diorition >= 2000) delay_time = 10;
        else delay_time = 200;
        delay(delay_time);
        if (num == 1) {
          if (code_number == 0) code_number = 8;
          else code_number --;
        }

        if (num == 2) {
          if (time1[code_number] <= 1) time1[code_number] = 420;
          else time1[code_number] --;
        }

        if (num == 3) {
          if (temp1[code_number] <= 20) temp1[code_number] = 150;
          else temp1[code_number] --;
        }

        if (num == 4) {
          if (time2[code_number] <= 1) time2[code_number] = 420;
          else time2[code_number] --;
        }

        if (num == 5) {
          if (temp2[code_number] <= 20) temp2[code_number] = 150;
          else temp2[code_number] --;
        }

        if (num == 6) {
          if (time3[code_number] <= 1) time3[code_number] = 420;
          else time3[code_number] --;
        }

        if (num == 7) {
          if (temp3[code_number] <= 20) temp3[code_number] = 150;
          else temp3[code_number] --;
        }
        break;
      }
      if (digitalRead(down_button) == LOW && bt_down_state == true){
        delay(20);
        bt_down_state = false;
      }
    }
  }
}

void set_pass() {
  int x_corsor = 9;
  int old_password = passcode;
  String code = String(passcode);
  
  while (true) { delay(0);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ENTERT NEW PASSWORD");
    lcd.setCursor(9, 2);
    lcd.print(code);
    lcd.setCursor(x_corsor, 3);
    lcd.blink();
    while (true) { delay(0);
      if (digitalRead(up_button) == HIGH) {
        button_cliked(up_button);
        if (x_corsor == 9) {
          int pass = code[0] - '0';
          if (pass ==  9) pass = 1;
          else pass ++;
          code[0] = (char)(pass + '0');
        }

        if (x_corsor == 10) {
          int pass1 = code[1] - '0';
          if (pass1 ==  9) pass1 = 1;
          else pass1 ++;
          code[1] = (char)(pass1 + '0');
        }

        if (x_corsor == 11) {
          int pass2 = code[2] - '0';
          if (pass2 ==  9) pass2 = 1;
          else pass2 ++;
          code[2] = (char)(pass2 + '0');
        }
        break;
      }

      if (digitalRead(down_button) == HIGH) {
        button_cliked(down_button);
        if (x_corsor == 11) x_corsor = 9;
        else x_corsor ++;
        break;
      }

      if (digitalRead(enter_button) == HIGH) {
        button_cliked(enter_button);
        lcd.clear();
        lcd.noBlink();
        lcd.setCursor(6, 1);
        lcd.print("SAVED");
        delay(1000);
        passcode = code.toInt();
        savePassToEEPROM(passcode);
        menu();
      }

      if (digitalRead(esc_button) == HIGH) {
        error_bip(esc_button);
        passcode = old_password;
        lcd.clear();
        lcd.noBlink();
        lcd.setCursor(5, 1);
        lcd.print("NOT SAVED");
        delay(1000);
        menu();
      }
    }
  }
}

void error_page() {
  lcd.clear();
  lcd.noBlink();
  lcd.setCursor(1, 0);
  lcd.print("Incorrect Password");
  error_bip(enter_button);
  delay(1000);
  menu();
}

void chack_password(String fanction_name) {
  int x_corsor = 9;
  String password = "111";
  while (true) { delay(0);
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("ENTERT PASSWORD");
    lcd.setCursor(9, 2);
    lcd.print(password);
    lcd.setCursor(x_corsor, 3);
    lcd.blink();
    while (true) { delay(0);
      if (digitalRead(up_button) == HIGH) {
        button_cliked(up_button);

        if (x_corsor == 9) {
          int pass = password[0] - '0';
          if (pass ==  9) pass = 1;
          else pass ++;
          password[0] = (char)(pass + '0');
        }

        if (x_corsor == 10) {
          int pass1 = password[1] - '0';
          if (pass1 ==  9) pass1 = 1;
          else pass1 ++;
          password[1] = (char)(pass1 + '0');
        }

        if (x_corsor == 11) {
          int pass2 = password[2] - '0';
          if (pass2 ==  9) pass2 = 1;
          else pass2 ++;
          password[2] = (char)(pass2 + '0');
        }
        break;
      }

      if (digitalRead(down_button) == HIGH) {
        button_cliked(down_button);
        if (x_corsor == 11) x_corsor = 9;
        else x_corsor ++;
        break;
      }

      if (digitalRead(enter_button) == HIGH) {
        //Serial.print(passcode);
        password.trim();
        if (password == String(passcode)) {
          button_cliked(enter_button);
          if (fanction_name == "code_setting") code_setting();
          //          if (fanction_name == "date_time") date_time();
          if (fanction_name == "set_pass") set_pass();
        }
        else error_page();
      }

      if (digitalRead(esc_button) == HIGH) {
        error_bip(esc_button);
        password = "444";
        lcd.clear();
        lcd.setCursor(6, 1);
        menu();
      }
    }
  }
}
void button_cliked(int button_pin) {
  analogWrite(bip, 255);
  delay(50);
  analogWrite(bip, 0);
  while (digitalRead(button_pin) == HIGH) {
    lcd.noBlink();
  }
}
void error_bip(int button_pin){
  analogWrite(bip, 150);
  delay(50);
  analogWrite(bip, 0);
  delay(80);
  analogWrite(bip, 150);
  delay(100);
  analogWrite(bip, 0);
  while (digitalRead(button_pin) == HIGH) {
    lcd.noBlink();
  }
}
