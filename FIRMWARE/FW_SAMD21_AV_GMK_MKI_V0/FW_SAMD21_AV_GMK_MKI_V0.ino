#include <PCA9505_9506.h>
#include <Keyboard.h> 
#include <Adafruit_NeoPixel.h> 

//delays for the key actuation
#define INITIAL_DELAY 250
#define REPEAT_DELAY 5

//values for the min and max LED brightnesses, and the step
#define MIN_BRIGHTNESS 2  
#define MAX_BRIGHTNESS 255
#define BRIGHTNESS_STEP 2

// Color definitions
#define COLOR_MODIF strip.Color(255, 0, 0) 
#define COLOR_NUMBER strip.Color(0, 255, 0)
#define COLOR_LETTER strip.Color(0, 0, 255) 
#define COLOR_NUMPAD strip.Color(255, 255, 255) 
#define COLOR_SPECIAL strip.Color(255, 255, 0) 



//custom keys
#define KEY_FUNCTION 0xFB
#define KEY_CHEVRON_L 236
#define KEY_CHEVRON_R 236
#define KEY_SCREENSHOT 0xFA 

//customs keys for module control
#define KEY_MOD_1 0xF9  
#define KEY_MOD_2 0xF8  
#define KEY_MOD_3 0xF7  
#define KEY_MOD_4 0xF6  
#define KEY_MOD_5 0xF5  
#define KEY_MOD_P 0xFC  
#define KEY_MOD_M 0xFD



// Define the IO expander objects and the number of keys per expander
PCA9505_06 NUMPAD;
PCA9505_06 LEFT;
PCA9505_06 RIGHT;
const uint8_t numKeys = 40;


char keyMapLEFT[numKeys] = {
  KEY_ESC, '`', KEY_TAB, KEY_CAPS_LOCK, KEY_LEFT_SHIFT, KEY_LEFT_CTRL,
          '1', 'q', 'a', KEY_CHEVRON_L, KEY_LEFT_GUI,
  KEY_F1, '2', 'w', 's', 'z', KEY_LEFT_ALT,
  KEY_F2, '3', 'e', 'd', 'x', KEY_FUNCTION,
  KEY_F3, '4', 'r', 'f', 'c',
  KEY_F4, '5', 't', 'g', 'v',
  KEY_F5, '6', 'y', 'h', 'b',
  KEY_F6, '7'
};
char keyMapLEFTALT[numKeys] = {
  KEY_ESC, '`', KEY_TAB, KEY_CAPS_LOCK, KEY_LEFT_SHIFT, KEY_LEFT_CTRL,
  '1', 'q', 'a', KEY_CHEVRON_L, KEY_LEFT_GUI,
  KEY_F1, '2', 'w', 's', 'z', KEY_LEFT_ALT,
  KEY_F2, '3', 'e', 'd', 'x', KEY_FUNCTION,
  KEY_F3, '4', 'r', 'f', 'c',
  KEY_F4, '5', 't', 'g', 'v',
  KEY_F5, '6', 'y', 'h', 'b',
  KEY_F6, '7'
};
char keyMapRIGHT[numKeys] = {
              'u', 'j', 'n', ' ',
  KEY_F7, '8', 'i', 'k', 'm',
  KEY_F8, '9', 'o', 'l', ',',
  KEY_F9, '0', 'p', ';', '.',
  KEY_F10, '-', '[', '\'', '/', KEY_RIGHT_ALT,
  KEY_F11, '=', ']', '\\', KEY_RIGHT_SHIFT, KEY_MENU,
  KEY_F12, KEY_BACKSPACE, KEY_RETURN, KEY_RIGHT_CTRL,
  KEY_PRINT_SCREEN, KEY_NUM_LOCK, KEY_KP_7, KEY_KP_4, KEY_KP_1
};
char keyMapRIGHTALT[numKeys] = {
  'u', 'j', 'n', ' ',
  KEY_F7, '8', 'i', 'k', 'm',
  KEY_F8, '9', 'o', 'l', ',',
  KEY_F9, '0', 'p', ';', '.',
  KEY_F10, '-', '[', '\'', '/', KEY_RIGHT_ALT,
  KEY_F11, '=', ']', '\\', KEY_RIGHT_SHIFT, KEY_MENU,
  KEY_F12, KEY_BACKSPACE, KEY_RETURN, KEY_RIGHT_CTRL,
  KEY_SCREENSHOT, KEY_NUM_LOCK, KEY_KP_7, KEY_MOD_4, KEY_MOD_1

};
char keyMapNUMPAD[numKeys] = {
  KEY_KP_0,
  KEY_PAGE_UP, KEY_KP_SLASH, KEY_KP_8, KEY_KP_5, KEY_KP_2,
  KEY_PAGE_DOWN, KEY_KP_ASTERISK, KEY_KP_9, KEY_KP_6, KEY_KP_3, KEY_KP_DOT,
  KEY_KP_MINUS, KEY_KP_PLUS, KEY_KP_ENTER
};
char keyMapNUMPADALT[numKeys] = {
  KEY_KP_0,
  KEY_PAGE_UP, KEY_KP_SLASH, KEY_KP_8, KEY_MOD_5, KEY_MOD_2,
  KEY_HOME, KEY_KP_ASTERISK, KEY_KP_9, KEY_KP_6, KEY_MOD_3, KEY_KP_DOT,
  KEY_MOD_M, KEY_MOD_P, KEY_KP_ENTER
};


bool keyStateNP[numKeys] = { 0 };
bool keyStateL[numKeys] = { 0 };
bool keyStateR[numKeys] = { 0 };

bool isShiftPressed = false;
bool isCapsLockOn = false;
bool initialDelayPassed = false;

unsigned long lastPressTime;
unsigned long lastToggleTime = 0;
uint8_t lastKeyPressed = 255;


uint32_t lastUpdate = 0;  // Last update of position
uint16_t j = 0;           // Position in the rainbow



//LED variables
const uint8_t ledPin = 0;         
const uint8_t numLeds = 95;     
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numLeds, ledPin, NEO_GRB + NEO_KHZ800);
uint8_t currentBrightness = 50; 
uint8_t currentKeyMod = 0;
uint8_t startLedIndexLeft = 0; 
uint8_t startLedIndexRight = 40;
uint8_t startLedIndexNumpad = 80;

const int potPin = A1; 
int lastVolume = 0; 
unsigned long lastTime = 0; 
const long interval = 100; 






// Prototypes
void checkKeys(PCA9505_06& expander, char* keyMap, char* funcKeyMap, bool* keyState, bool funcKeyPressed);
void rainbow(uint8_t wait);
uint32_t wheel(byte wheelPos);
bool isModifier(char key);
bool isNumber(char key);
bool isLetter(char key);
bool isNumpadCharacter(char key);
bool isSpecialCharacter(char key);
void applyKeymapLighting(char keyMap[], uint8_t numKeys, uint8_t startLedIndex);



void setup() {

  Serial.begin(9600); 
  // Initialize the LEDs
  strip.begin();
  strip.setBrightness(currentBrightness);


  // Initialize the IO expanders
  NUMPAD.begin(0x23);  // Adjust the address as needed
  LEFT.begin(0x22);    // Adjust the address as needed
  RIGHT.begin(0x21);   // Adjust the address as needed

  // Set all pins as inputs
  for (uint8_t i = 0; i < numKeys; i++) {
    NUMPAD.pinMode(i, INPUT);
    LEFT.pinMode(i, INPUT);
    RIGHT.pinMode(i, INPUT);
  } 
   pinMode(potPin, INPUT);
}

void loop() {
  // Assign the Function Key
  bool funcKeyPressed = ((LEFT.digitalRead(22) == 0));

  // Check all the expanders with their keymaps and keystates
  checkKeys(NUMPAD, keyMapNUMPAD, keyMapNUMPADALT, keyStateNP, funcKeyPressed);
  checkKeys(LEFT, keyMapLEFT, keyMapLEFTALT, keyStateL, funcKeyPressed);
  checkKeys(RIGHT, keyMapRIGHT, keyMapRIGHTALT, keyStateR, funcKeyPressed);


  // Do the rainbow wave if called
  
  if (currentKeyMod == 2) {
    rainbow(1); // Keep the rainbow effect running
  }
  else if(currentKeyMod == 1){
    applyKeymapLighting(keyMapLEFT, numKeys, startLedIndexLeft); 
    applyKeymapLighting(keyMapRIGHT, numKeys, startLedIndexRight); 
    applyKeymapLighting(keyMapNUMPAD, numKeys, startLedIndexNumpad); 


  } 
unsigned long currentTime = millis();
if (currentTime - lastTime >= interval) {
  int potValue = analogRead(potPin);
  int volumeLevel = map(potValue, 0, 1023, 0, 100); // plage 0–100 pour plus de stabilité

  if (abs(volumeLevel - lastVolume) > 2) { // pour éviter les petits changements
    if (volumeLevel > lastVolume) {
      Keyboard.press(KEY_F15);  // Volume up
      Keyboard.release(KEY_F15);
      Serial.print("up");
    } else {
      Keyboard.press(KEY_F16);  // Volume down
      Keyboard.release(KEY_F16);
      Serial.print("down");
    }

    lastVolume = volumeLevel;
    lastTime = currentTime;
  }
}

}

/**
 * @brief Checks the state of each key connected to a specific PCA9505 chip, triggers key press/release actions, and handles mouse clicks.
 *
 * This function iterates over each key connected to the PCA9505 chip, represented by a PCA9505_06 object, and checks whether the state of the key has changed
 * (i.e., it has been pressed or released). It triggers the appropriate key press/release actions based on the current state of the keys and supports handling
 * mouse clicks. If a "function" key is pressed, the function switches to an alternative keymap layer defined by the function keymap.
 *
 * @param expander Reference to the PCA9505_06 object representing the PCA9505 chip to be checked.
 * @param keyMap Pointer to the array containing the primary keymap for the PCA9505 chip.
 * @param keyMapFunction Pointer to the array containing the function keymap for the PCA9505 chip.
 * @param keyState Pointer to the array containing the current state of each key connected to the PCA9505 chip.
 *
 * @note The function keymap is activated when the "function" key, represented by KEY_FUNCTION, is pressed.
 * @note MOUSE_LEFT_CLICK and MOUSE_RIGHT_CLICK are special constants used to represent mouse click actions.
 */
void checkKeys(PCA9505_06& expander, char* keyMap, char* funcKeyMap, bool* keyState, bool funcKeyPressed) {
  for (uint8_t i = 0; i < numKeys; i++) {
    unsigned long currentPressTime = millis();
    bool isPressed = (expander.digitalRead(i) == 0);
    char key = funcKeyPressed ? funcKeyMap[i] : keyMap[i];
    if (isPressed != keyState[i]) {
      keyState[i] = isPressed;
      switch (key) {
        case KEY_SCREENSHOT:
          if (isPressed) {
            Keyboard.press(KEY_LEFT_GUI);
            Keyboard.press(KEY_LEFT_SHIFT);
            Keyboard.press('s');
            delay(50);
            Keyboard.releaseAll();
            lastPressTime = millis();
            lastKeyPressed = i;
            initialDelayPassed = false;
          }
          break;
        case KEY_CHEVRON_L:
          if (isPressed) {
            if (isCapsLockOn || isShiftPressed) {
              Keyboard.press(KEY_LEFT_SHIFT);
              Keyboard.press(KEY_CHEVRON_L);
              Keyboard.releaseAll();
            } else {
              Keyboard.press(KEY_CHEVRON_R);
              Keyboard.release(KEY_CHEVRON_R);
            }
            lastPressTime = millis();
            lastKeyPressed = i;
            initialDelayPassed = false;
          }
          break;
        case KEY_MOD_1:
          currentKeyMod = 1;
          break;
        case KEY_MOD_2:
          currentKeyMod = 2;
          break;
        case KEY_MOD_3:
          for(int i = 0; i < strip.numPixels(); i++) {
            strip.setPixelColor(i, strip.Color(0, 255, 0)); // Set all pixels to 
          }
          
          strip.setBrightness(currentBrightness);
          strip.show();
          currentKeyMod = 3;
          break;
        case KEY_MOD_4:
          for(int i = 0; i < strip.numPixels(); i++) {
            strip.setPixelColor(i, strip.Color(255, 255, 255)); // Set all pixels to white
          }
          
          strip.setBrightness(currentBrightness);
          strip.show();
          break; 
          currentKeyMod = 4;
        case KEY_MOD_5:
          for(int i = 0; i < strip.numPixels(); i++) {
            strip.setPixelColor(i, strip.Color(0, 0, 0)); // Turn off all pixels
          }
          
          strip.setBrightness(currentBrightness);
          strip.show();
          currentKeyMod = 5;
          break;
        case KEY_MOD_P: // Increase brightness
            currentBrightness += BRIGHTNESS_STEP; // Adjust the step size as needed
            if (currentBrightness > MAX_BRIGHTNESS) 
              currentBrightness = MAX_BRIGHTNESS; // Ensure the brightness doesn't exceed MAX_BRIGHTNESS
            strip.setBrightness(currentBrightness);
            strip.show(); // Apply the new brightness
            break;

        case KEY_MOD_M: // Decrease brightness
            currentBrightness -= BRIGHTNESS_STEP; // Adjust the step size as needed
            if (currentBrightness < MIN_BRIGHTNESS) 
              currentBrightness = MIN_BRIGHTNESS; // Ensure the brightness doesn't drop below MIN_BRIGHTNESS
            strip.setBrightness(currentBrightness);
            strip.show(); // Apply the new brightness
            break;

        default:
          if (isPressed) {
            Keyboard.press(key);
            lastPressTime = millis();
            lastKeyPressed = i;
            initialDelayPassed = false;
          } else {  
            Keyboard.release(key);
            if (i == lastKeyPressed) lastKeyPressed = 255;
          }
          break;
      }
    }
  }
/* //2025 EDIT: DO NOT IMPLEMENT THIS FUNCTION!!!! AS A GOOD ELECTRICAL ENGINEERING I WANTED TO IMPLEMENT DEBOUNCING, BUT IT'S ALREADY BUILT IN KEYBOARD.h!!!!!
   //Commenting it out works. 
  //If you press a key once, it waits for INITIAL_DELAY, then if the key is still pressed, it spams it with a delay of REPEAT_DELAY in between each key press.
  if (lastKeyPressed != 255 && keyState[lastKeyPressed] && ((millis() - lastPressTime > INITIAL_DELAY && !initialDelayPassed) || (initialDelayPassed && millis() - lastPressTime > REPEAT_DELAY))) {
    char key = funcKeyPressed ? funcKeyMap[lastKeyPressed] : keyMap[lastKeyPressed];
      Keyboard.release(key);
      Keyboard.press(key);
      lastPressTime = millis();
      initialDelayPassed = true;
    }*/

}

void rainbow(uint8_t wait) {
  uint32_t currentMillis = millis();
  if (currentMillis - lastUpdate > wait) {
    lastUpdate = currentMillis;  // Save the last update time

    // Only update the LED strip if the specified 'wait' time has passed
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, wheel((i * 1 + j) & 255));
    }
    strip.show();

    // Increment 'j', reset if it reaches 256
    j+= 5;
    if (j >= 256) {
      j = 0;
    }
  }
}

uint32_t wheel(byte wheelPos) {
  if (wheelPos < 85) {
    return strip.Color(wheelPos * 3, 255 - wheelPos * 3, 0);
  } else if (wheelPos < 170) {
    wheelPos -= 85;
    return strip.Color(255 - wheelPos * 3, 0, wheelPos * 3);
  } else {
    wheelPos -= 170;
    return strip.Color(0, wheelPos * 3, 255 - wheelPos * 3);
  }
}

void applyKeymapLighting(char keyMap[], uint8_t numKeys, uint8_t startLedIndex) {
    for (uint8_t i = 0; i < numKeys; i++) {
        char key = keyMap[i];
        uint32_t color = 0;
        
        if (isModifier(key)) {
            color = COLOR_MODIF;
        } else if (isNumber(key)) {
            color = COLOR_NUMBER;
        } else if (isLetter(key)) {
            color = COLOR_LETTER;
        } else if (isNumpadCharacter(key)) {
            color = COLOR_NUMPAD;
        } else if (isSpecialCharacter(key)) {
            color = COLOR_SPECIAL;
        }

        strip.setPixelColor(i + startLedIndex, color);
    }
    strip.show();
}


bool isModifier(char key) {
    return key == KEY_LEFT_CTRL || key == KEY_LEFT_SHIFT || key == KEY_LEFT_ALT || 
           key == KEY_RIGHT_CTRL || key == KEY_RIGHT_SHIFT || key == KEY_RIGHT_ALT ||
           key == KEY_FUNCTION || key == KEY_LEFT_GUI || key == KEY_RIGHT_GUI || key == KEY_TAB  
           || key == KEY_CAPS_LOCK || key == KEY_MENU  || key == KEY_RETURN || key == KEY_BACKSPACE ||

           key == KEY_F1 || key == KEY_F2 || key == KEY_F3 || key == KEY_F4 || 
           key == KEY_F5 || key == KEY_F6 || key == KEY_F7 || key == KEY_F8 || 
           key == KEY_F9 || key == KEY_F10 || key == KEY_F11 || key == KEY_F12 ||
           
           key == KEY_PAGE_UP || key == KEY_PAGE_DOWN || key == KEY_PRINT_SCREEN || key == KEY_NUM_LOCK ||
           key == KEY_ESC;
}

bool isNumber(char key) {
    return key >= '0' && key <= '9' || key == '`' || key =='=' || key =='-';
}

bool isLetter(char key) {
    return (key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z') || key == ' ';
}
bool isSpecialCharacter(char key) {
    return    key == 'm'|| key == ';' || key == ',' || key == '.' || key == '/'  || key == '\\' || key == '\''
    || key == '[' || key == ']' || key ==KEY_CHEVRON_L;
}

bool isNumpadCharacter(char key) {
    return key == KEY_KP_0 || key == KEY_KP_1 || key == KEY_KP_2 || 
           key == KEY_KP_3 || key == KEY_KP_4 || key == KEY_KP_5 || 
           key == KEY_KP_6 || key == KEY_KP_7 || key == KEY_KP_8 || 
           key == KEY_KP_9 || key == KEY_KP_PLUS || key == KEY_KP_MINUS || 
           key == KEY_KP_SLASH || key == KEY_KP_ASTERISK || key == KEY_KP_ENTER || 
           key == KEY_KP_DOT;
}


