#include <PCA9505_9506.h>
#include <Keyboard.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>



//delays for the key actuation
#define INITIAL_DELAY 250
#define REPEAT_DELAY 5

//custom keys for mouse functions
#define MOUSE_LEFT_CLICK 0xFF
#define MOUSE_RIGHT_CLICK 0xFE
#define MOUSE_SCROLL_UP 0xFD
#define MOUSE_SCROLL_DOWN 0xFC

//custom keys
#define KEY_FUNCTION 0xFB
#define KEY_CHEVRON_L 236
#define KEY_CHEVRON_R 236
#define KEY_SCREENSHOT 0xFA
#define KEY_FUN 0xF9

//customs keys for module control
#define KEY_MOD_1 0xF8  //deply arm (servo) for mouse charge
#define KEY_MOD_2 0xF7  //retract arm for mouse charge
#define KEY_MOD_3 0xF6  //cycle between Clock, AV-RK MK2 logo, WPM/CPM count, and Heart rate monitor (smol OLED screen)/
#define KEY_MOD_4 0xF5  //cycle between compact display mode (one line for RTC time, one for wpm, one for temperature, one for humidity), normal display mode (just the temps and humidity) and fun mode (just displays random things "hollywood" style)
#define KEY_MOD_5 0xF4  //not assigned
#define KEY_MOD_6 0xF3  //not assigned







// Define the IO expander objects
PCA9505_06 NUMPAD;
PCA9505_06 LEFT;
PCA9505_06 RIGHT;
const int ledPin = 0;  // the number of the neopixel strip
const int numLeds = 95;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numLeds, ledPin, NEO_GRB + NEO_KHZ800);

const uint8_t numKeys = 40; // Assuming each expander has 40 pins

const int DEBOUNCE_DELAY = 200;



char keyMapLEFT[numKeys] = {
    KEY_ESC,          '`',  KEY_TAB,            KEY_CAPS_LOCK,      KEY_LEFT_SHIFT,             KEY_LEFT_CTRL,
                      '1',                    'q',                'a',                KEY_CHEVRON_L,              KEY_LEFT_GUI,
    KEY_F1,           '2',                    'w',                's',                'z',                        KEY_LEFT_ALT,
    KEY_F2,           '3',                    'e',                'd',                'x',                        KEY_FUNCTION,
    KEY_F3,           '4',                    'r',                'f',                'c',                        
    KEY_F4,           '5',                    't',                'g',                'v',            
    KEY_F5,           '6',                    'y',                'h',                'b',
    KEY_F6,           '7'                    
};


char keyMapLEFTALT[numKeys] = {
  KEY_ESC,          '`',  KEY_TAB,            KEY_CAPS_LOCK,      KEY_LEFT_SHIFT,             KEY_LEFT_CTRL,
                    '1',                    'q',                'a',                KEY_CHEVRON_L,              KEY_LEFT_GUI,
  KEY_F1,           '2',                    'w',                's',                'z',                        KEY_LEFT_ALT,
  KEY_F2,           '3',                    'e',                'd',                'x',                        KEY_FUNCTION,
  KEY_F3,           '4',                    'r',                'f',                'c',                        
  KEY_F4,           '5',                    't',                'g',                'v',            
  KEY_F5,           '6',                    'y',                'h',                'b',
  KEY_F6,           '7'                    
};



char keyMapNUMPAD[numKeys] = {
  KEY_PRINT_SCREEN, KEY_SCROLL_LOCK, KEY_PAUSE, KEY_NUM_LOCK, KEY_KP_SLASH, KEY_KP_ASTERISK, KEY_KP_MINUS, KEY_F13,
  KEY_INSERT, KEY_HOME, KEY_PAGE_UP, KEY_KP_7, KEY_KP_8, KEY_KP_9, KEY_KP_PLUS, KEY_F13,
  KEY_DELETE, KEY_END, KEY_PAGE_DOWN, KEY_KP_4, KEY_KP_5, KEY_KP_6, KEY_KP_PLUS, KEY_F13,
  KEY_F13, KEY_UP_ARROW, KEY_F13, KEY_KP_1, KEY_KP_2, KEY_KP_3, KEY_KP_ENTER, KEY_F13,
  KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_RIGHT_ARROW, KEY_KP_0, KEY_KP_0, KEY_KP_ENTER, KEY_KP_DOT, KEY_F13
};
char keyMapNUMPADALT[numKeys] = {
  KEY_PRINT_SCREEN, KEY_SCROLL_LOCK, KEY_PAUSE, KEY_NUM_LOCK, KEY_KP_SLASH, KEY_KP_ASTERISK, KEY_KP_MINUS, KEY_F13,
  KEY_INSERT, KEY_HOME, KEY_PAGE_UP, KEY_KP_7, KEY_KP_8, KEY_KP_9, KEY_KP_PLUS, KEY_F13,
  KEY_DELETE, KEY_END, KEY_PAGE_DOWN, KEY_MOD_4, KEY_MOD_5, KEY_MOD_6, KEY_KP_PLUS, KEY_F13,
  KEY_F13, KEY_UP_ARROW, KEY_F13, KEY_MOD_1, KEY_MOD_2, KEY_MOD_3, KEY_KP_ENTER, KEY_F13,
  KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_RIGHT_ARROW, KEY_KP_0, KEY_KP_0, KEY_KP_ENTER, KEY_KP_DOT, KEY_F13
}; 
char keyMapRIGHT[numKeys] = {
  '7', '8', '9', '0', '-', '=', '[', KEY_F13,
  KEY_F13, 'y', 'u', 'i', 'o', 'p', ']', KEY_F13,
  KEY_BACKSPACE, 'h', 'j', 'k', 'l', ';', '\'', KEY_F13,
  KEY_DELETE, 'b', 'n', 'm', ',', '.', '/', KEY_F13,
  KEY_RETURN, ' ', KEY_RIGHT_ALT, KEY_RIGHT_GUI, KEY_MENU, KEY_RIGHT_CTRL, '\\', KEY_F13

};
char keyMapRIGHTALT[numKeys] = {
  KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12, '[', KEY_F13,
  KEY_F13, 'y', 'u', KEY_UP_ARROW, 'o', 'p', ']', KEY_F13,
  KEY_BACKSPACE, 'h', KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_RIGHT_ARROW, ';', '\'', KEY_F13,
  KEY_DELETE, 'b', 'n', 'm', ',', '.', '/', KEY_F13,
  KEY_RETURN, ' ', KEY_RIGHT_ALT, KEY_F13, KEY_F13, KEY_F13, '\\', KEY_F13
};

bool keyStateNP[numKeys] = { 0 };
bool keyStateL[numKeys] = { 0 };
bool keyStateR[numKeys] = { 0 };

bool isShiftPressed = false;
bool isCapsLockOn = false;
bool initialDelayPassed = false;
bool funTypingModeActive = false; 

unsigned long lastPressTime;
unsigned long lastToggleTime = 0;
uint8_t lastKeyPressed = 255;


uint32_t lastUpdate = 0; // Last update of position
uint16_t j = 0; // Position in the rainbow



void checkKeys(PCA9505_06& expander, char* keyMap, char* funcKeyMap, bool* keyState, bool funcKeyPressed);
void rainbow(uint8_t wait);
uint32_t Wheel(byte WheelPos);





void setup() {

  strip.begin();
  strip.setBrightness(255); 

  // Initialize serial communication
  Serial.begin(9600);
  
  // Initialize the IO expanders
  NUMPAD.begin(0x23); // Adjust the address as needed
  LEFT.begin(0x22);   // Adjust the address as needed
  RIGHT.begin(0x21);  // Adjust the address as needed

  // Set all pins as inputs
  for (uint8_t i = 0; i < numKeys; i++) {
    NUMPAD.pinMode(i, INPUT);
    LEFT.pinMode(i, INPUT);
    RIGHT.pinMode(i, INPUT);
  }
}

void loop() {  
  rainbow(10);
  bool funcKeyPressed = ((LEFT.digitalRead(35) == 0) || (RIGHT.digitalRead(8)) == 0);

  checkKeys(NUMPAD, keyMapNUMPAD, keyMapNUMPADALT, keyStateNP, funcKeyPressed);
  checkKeys(LEFT, keyMapLEFT, keyMapLEFTALT, keyStateL, funcKeyPressed);
  checkKeys(RIGHT, keyMapRIGHT, keyMapRIGHTALT, keyStateR, funcKeyPressed);
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
        case KEY_FUN:
          funTypingModeActive = !funTypingModeActive;
          break;
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
        case KEY_MOD_3:
          break;
        case KEY_MOD_4: 
          //trackball.setRGBW(255, 0, 0, 0);
          break;
        case KEY_MOD_5: 
          //trackball.setRGBW(0, 255, 255, 0);
          break;
        case KEY_MOD_6: 
          //trackball.setRGBW(0, 0, 0, 255);
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

  if (funTypingModeActive) {
    if (millis() - lastToggleTime > 150) {
      Keyboard.press(KEY_CAPS_LOCK);
      Keyboard.release(KEY_CAPS_LOCK);
      lastToggleTime = millis();
    }
  }

  //If you press a key once, it waits for INITIAL_DELAY, then if the key is still pressed, it spams it with a delay of REPEAT_DELAY in between each key press.
  if (lastKeyPressed != 255 && keyState[lastKeyPressed] && ((millis() - lastPressTime > INITIAL_DELAY && !initialDelayPassed) || (initialDelayPassed && millis() - lastPressTime > REPEAT_DELAY))) {
    char key = funcKeyPressed ? funcKeyMap[lastKeyPressed] : keyMap[lastKeyPressed];
    if (key != MOUSE_LEFT_CLICK && key != MOUSE_RIGHT_CLICK && key != MOUSE_SCROLL_UP && key != MOUSE_SCROLL_DOWN) {
      Keyboard.release(key);
      Keyboard.press(key);
      lastPressTime = millis();
      initialDelayPassed = true;
    }
  }
}

void rainbow(uint8_t wait) {
  uint32_t currentMillis = millis();
  if (currentMillis - lastUpdate > wait) {
    lastUpdate = currentMillis; // Save the last update time

    // Only update the LED strip if the specified 'wait' time has passed
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i * 1 + j) & 255));
    }
    strip.show();
    
    // Increment 'j', reset if it reaches 256
    if (++j >= 256) {
      j = 0;
    }
  }
}

uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}


