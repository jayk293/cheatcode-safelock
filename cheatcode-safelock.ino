/*
Sketch that sets passwords using PS3 Controller
*/
#include <EEPROM.h>
#include <PS3USB.h>

USB Usb;
PS3USB PS3(&Usb); 

uint8_t storedPassword[16] = {1,2,3,4,5,6};
uint8_t enteredPassword[16];
uint8_t newPassword[16];
uint8_t resetArray[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint8_t lockedCounter = 0;
bool unlocked, setNewPassword;
uint8_t passwordState, rOne, rTwo, lOne, lTwo, Square, Cross, Triangle, Circle, Left, Right, Up, Down, rThree, lThree, passwordIndex;

void setup() {
  Serial.begin(115200);
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); //halt
  }  
  Serial.println(F("\r\nPS3 USB Library Started"));
  
  // declaring varibles to be used in sketch
  unlocked = false,
  setNewPassword = false,
  passwordState = 0, //0, no input gets read
                     //1, entering password
                     //2, set password
  
  passwordIndex = 0, //this is the index of the password
 
  rOne = 1,  //these reference the buttons on the controller and assign them to an integer to be saved to the EEPROM
  rTwo = 2,
  lOne = 3,
  lTwo = 4,
  Square = 5,
  Cross = 6,
  Triangle = 7,
  Circle = 8,
  Left = 9,
  Right = 10,
  Up = 11,
  Down = 12,
  rThree = 13,
  lThree = 14;
 
 pinMode(13,OUTPUT);
 digitalWrite(13,LOW);
  //method reads password from the EEPROM
  readFromEEPROM();
   for (uint8_t j = 0; j < 16; j++) {
    Serial.println(storedPassword[j]);
  }
}


void clearEEPROM(){
  for (uint8_t i = 0; i < 16; i++){
    EEPROM.write(i, 0);
  }
  Serial.println("EEPROM cleared");

}
void writeToEEPROM(){
    
    Serial.println("Writing to EEPROM");
    for (uint8_t i = 0; i < 16; i++){
      if (storedPassword[i] == 0) {
        break;
      }
      EEPROM.write(i, storedPassword[i]);
    }
    Serial.println("Successfully written to EEPROM");
}
            
void readFromEEPROM(){           
      
    Serial.println("Reading from EEPROM");
    for (uint8_t i = 0; i < 16; i++) {
      if (EEPROM.read(i) == 0){
        break;
      }
      storedPassword[i] = EEPROM.read(i);
    }
    Serial.println("Successfully Read from EEPROM");
}


boolean array_cmp(uint8_t *a, uint8_t *b){
     uint8_t n;
     //if their lengths are different, return false
     //if (len_a != len_b) return false;
     // test each element to be the same. if not, return false
     for (n=0;n<16;n++) if (a[n]!=b[n]) return false;
}

void loop() {
  Usb.Task();
  if (PS3.PS3Connected || PS3.PS3NavigationConnected) {

    if(unlocked){
      digitalWrite(13,HIGH);
    } else {
      digitalWrite(13,LOW);
    }

    //PRESS START TO ENTER PASSWORD
    if (PS3.getButtonClick(START)) {
        if(unlocked){
          unlocked=false;
          Serial.println("Locking Item");
        } else {
          if (passwordState < 2) {
              if (passwordState == 1) {
                   for (uint8_t k = 0; k < 16; k++) {
                   Serial.println(enteredPassword[k]);
                   }
                   
                  if (array_cmp(storedPassword, enteredPassword)) {
                    Serial.println(F("Correct"));
                    passwordState = 0;
                    passwordIndex = 0;
                    memcpy(enteredPassword, resetArray, 16);
                    unlocked = true;
                    PS3.setLedOff();
                    PS3.setLedOn(LED4);
                    Serial.println("Item Unlocked");
        
                  } else {
                    Serial.println(F("incorrect"));
                    passwordState = 0;
                    passwordIndex = 0;
                    memcpy(enteredPassword, resetArray, 16);
                    unlocked = false;
                  }
              } else {
                Serial.println(F("Enter Password"));
                passwordState = 1;
                passwordIndex = 0;
              }
          } else {
            Serial.println(F("Please press select to complete password"));
          }
        }
    }

    //PRESS SELECT TO SET NEW PASSWORD
    if (PS3.getButtonClick(SELECT)) {
        if (!(passwordState == 1) && passwordState == 2) {
            if (array_cmp(storedPassword, enteredPassword)) {
                if (setNewPassword) {
                  passwordState = 0;
                  memcpy(storedPassword, newPassword, 16);
                  memcpy(enteredPassword, resetArray, 16);
                  memcpy(newPassword, resetArray, 16);
                  writeToEEPROM();
                  passwordIndex = 0;
                  setNewPassword = false; 
                  Serial.println("Changed Password");
      
                } else {
                  Serial.println(F("Correct"));
                  Serial.println(F("Set New Password"));
                  setNewPassword = true;
                  passwordIndex = 0;
                }
            } else {
              Serial.println(F("Set - incorrect"));
              passwordState = 0;
              passwordIndex = 0;
              memcpy(enteredPassword, resetArray, 16);
              unlocked = false;
            }
        } else {
          Serial.println(F("Please enter current password, then press select"));
          passwordState = 2;
        }
    }
    // just for debug purpose
    // delete this code when prod ready
    if (PS3.getButtonClick(PS)) {
     clearEEPROM(); 
    }  
    
    if (PS3.getButtonClick(L2) && passwordState) {
      Serial.println("L2");
      if (setNewPassword) {
        newPassword[passwordIndex] = lTwo;
        passwordIndex++;
      } else {
        enteredPassword[passwordIndex] = lTwo;
        passwordIndex++;
      }
    }
    if (PS3.getButtonClick(R2) && passwordState) {
      Serial.println("R2");
      if (setNewPassword) {
        newPassword[passwordIndex] = rTwo;
        passwordIndex++;
      } else {
        enteredPassword[passwordIndex] = rTwo;
        passwordIndex++;
      }
    }
    if (PS3.getButtonClick(TRIANGLE) && passwordState) {
      Serial.println("TRIANGLE");
      if (setNewPassword) {
        newPassword[passwordIndex] = Triangle;
        passwordIndex++;
      } else {
        enteredPassword[passwordIndex] = Triangle;
        passwordIndex++;
      }
    }
    if (PS3.getButtonClick(CIRCLE) && passwordState) {
      Serial.println("CIRCLE");
      if (setNewPassword) {
        newPassword[passwordIndex] = Circle;
        passwordIndex++;
      } else {
        enteredPassword[passwordIndex] = Circle;
        passwordIndex++;
      }
    }
    if (PS3.getButtonClick(CROSS) && passwordState) {
      Serial.println("CROSS");
      if (setNewPassword) {
        newPassword[passwordIndex] = Cross;
        passwordIndex++;
      } else {
        enteredPassword[passwordIndex] = Cross;
        passwordIndex++;
      }
    }
    if (PS3.getButtonClick(SQUARE) && passwordState) {
      Serial.println("SQUARE");
      if (setNewPassword) {
        newPassword[passwordIndex] = Square;
        passwordIndex++;
      } else {
        enteredPassword[passwordIndex] = Square;
        passwordIndex++;
      }
    }
    if (PS3.getButtonClick(UP) && passwordState) {
      Serial.println("UP");
      if (setNewPassword) {
        newPassword[passwordIndex] = Up;
        passwordIndex++;
      } else {
        enteredPassword[passwordIndex] = Up;
        passwordIndex++;
      }
    }
    if (PS3.getButtonClick(RIGHT) && passwordState) {
      Serial.println("RIGHT");
      if (setNewPassword) {
        newPassword[passwordIndex] = Right;
        passwordIndex++;
      } else {
        enteredPassword[passwordIndex] = Right;
        passwordIndex++;
      }
    }
    if (PS3.getButtonClick(DOWN) && passwordState) {
     Serial.println("DOWN");
     if (setNewPassword) {
        newPassword[passwordIndex] = Down;
        passwordIndex++;
      } else {
        enteredPassword[passwordIndex] = Down;
        passwordIndex++;
      }
    }
    if (PS3.getButtonClick(LEFT) && passwordState) {
      Serial.println("LEFT");
      if (setNewPassword) {
        newPassword[passwordIndex] = Left;
        passwordIndex++;
      } else {
        enteredPassword[passwordIndex] = Left;
        passwordIndex++;
      }
    }
    if (PS3.getButtonClick(L1) && passwordState) {
      Serial.println("L1");
      if (setNewPassword) {
        newPassword[passwordIndex] = lOne;
        passwordIndex++;
      } else {
        enteredPassword[passwordIndex] = lOne;
        passwordIndex++;
      }
    }
    if (PS3.getButtonClick(L3) && passwordState) {
      Serial.println("L3");
      if (setNewPassword) {
        newPassword[passwordIndex] = lThree;
        passwordIndex++;
      } else {
        enteredPassword[passwordIndex] = lThree;
        passwordIndex++;
      }
    }
    if (PS3.getButtonClick(R1) && passwordState) {
        Serial.println("R1");      
      if (setNewPassword) {
        newPassword[passwordIndex] = rOne;
        passwordIndex++;
        Serial.println("R1");
      } else {
        enteredPassword[passwordIndex] = rOne;
        passwordIndex++;
      }
    }
    if (PS3.getButtonClick(R3) && passwordState) {
      Serial.println("R3");
      if (setNewPassword) {
        newPassword[passwordIndex] = rThree;
        passwordIndex++;
      } else {
        enteredPassword[passwordIndex] = rThree;
        passwordIndex++;
      }
    }
  }
}
