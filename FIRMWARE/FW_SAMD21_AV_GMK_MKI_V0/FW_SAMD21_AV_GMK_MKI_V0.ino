/*-------------------------------------------------------------------------
FW_SAMD21_AV_GMK_MKI_V0.ino, the firmware of the AV-GMK MKI keyboard.
    Copyright (C) 2024    AVlabs (Avesta MOLAEI)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
-------------------------------------------------------------------------*/

//------------------LIBRARIES------------------
//PCA9505_9506.h: Lib for the NXP PCA9505 IO Expander through I2C
//Keyboard.h:     Classic Arduino Keyboard lib
//Neopixel.h:     Classic Adafruit WS2812B-like LED control
#include <PCA9505_9506.h>
#include <Keyboard.h> 
#include <Adafruit_NeoPixel.h>



//------------------DEFINES------------------------------------------------------
//delays for the key actuation
#define INITIAL_DELAY 250
#define REPEAT_DELAY 5

//values for the min and max LED brightnesses, and the step
#define MIN_BRIGHTNESS 2  
#define MAX_BRIGHTNESS 255
#define BRIGHTNESS_STEP 2

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

//Default and Alternative Keymaps for all 3 PCAs
char keyMapLEFT[numKeys] = {
  KEY_ESC,  '`', KEY_TAB, KEY_CAPS_LOCK, KEY_LEFT_SHIFT, KEY_LEFT_CTRL,
  '1', 'q', 'a', KEY_CHEVRON_L, KEY_LEFT_GUI,
  KEY_F1,   '2', 'w', 's', 'z', KEY_LEFT_ALT,
  KEY_F2,   '3', 'e', 'd', 'x', KEY_FUNCTION,
  KEY_F3,   '4', 'r', 'f', 'c',
  KEY_F4,   '5', 't', 'g', 'v',
  KEY_F5,   '6', 'y', 'h', 'b',
  KEY_F6,   '7'
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

//Keystate maps
bool keyStateNP[numKeys] = { 0 };
bool keyStateL[numKeys] = { 0 };
bool keyStateR[numKeys] = { 0 };

//General Keyboard variables
bool isShiftPressed = false;
bool isCapsLockOn = false;
//bool initialDelayPassed = false;
//unsigned long lastPressTime;
//unsigned long lastToggleTime = 0;
//uint8_t lastKeyPressed = 255;

//LED variables
const uint8_t ledPin = 0;         
const uint8_t numLeds = 95;     
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numLeds, ledPin, NEO_GRB + NEO_KHZ800);
uint8_t currentBrightness = 50; 
uint8_t currentKeyMod = 0;
uint32_t lastUpdate = 0;  // Last update of position
uint16_t j = 0;           // Position in the rainbow


//------------------PROTOTYPES------------------
void checkKeys(PCA9505_06& expander, char* keyMap, char* funcKeyMap, bool* keyState, bool funcKeyPressed);
void rainbow(uint8_t wait);
uint32_t Wheel(byte WheelPos);
//---------------------------------------------


//------------------FUNCTIONS------------------
/**
 * @brief void setup(), init function
*/
void setup() {
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
}

/**
 * @brief void loop(), infinite loop function
*/
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
            strip.setPixelColor(i, strip.Color(0, 255, 0)); // Set all pixels to green
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
            currentBrightness += BRIGHTNESS_STEP;
            if (currentBrightness > MAX_BRIGHTNESS) 
              currentBrightness = MAX_BRIGHTNESS; // Ensure no overflow
            strip.setBrightness(currentBrightness);
            strip.show(); // Apply the new brightness
            break;

        case KEY_MOD_M: // Decrease brightness
            currentBrightness -= BRIGHTNESS_STEP; 
            if (currentBrightness < MIN_BRIGHTNESS) 
              currentBrightness = MIN_BRIGHTNESS; // Ensure no underflow
            strip.setBrightness(currentBrightness);
            strip.show(); 
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

 //2025 EDIT: DO NOT IMPLEMENT THIS!!! THIS BEHAVIOR IS ALREADY IMPLEMENTED IN KEYBOARD.H, AND REDEFINING IT MAKES THE KEYBOARD UNEUSABLE INGAME. 
   //Comenting it out works. 
   /*
   //If you press a key once, it waits for INITIAL_DELAY, then if the key is still pressed, it spams it with a delay of REPEAT_DELAY in between each key press.
   if (lastKeyPressed != 255 && keyState[lastKeyPressed] && ((millis() - lastPressTime > INITIAL_DELAY && !initialDelayPassed) || (initialDelayPassed && millis() - lastPressTime > REPEAT_DELAY))) {
     char key = funcKeyPressed ? funcKeyMap[lastKeyPressed] : keyMap[lastKeyPressed];
     if (key != MOUSE_LEFT_CLICK && key != MOUSE_RIGHT_CLICK && key != MOUSE_SCROLL_UP && key != MOUSE_SCROLL_DOWN) {
       Keyboard.release(key);
       Keyboard.press(key);
       lastPressTime = millis();
       initialDelayPassed = true;
     }
   }*/

}

/**
 * @brief Generates a rainbow animation across the entire strip of LEDs.
 * 
 * This function creates a moving rainbow effect by cycling through all colors
 * across the strip's LEDs. The function calculates a color for each LED based
 * on its position and the current state of the animation cycle, creating a
 * continuous rainbow that moves along the strip. The animation speed can be
 * adjusted by changing the 'wait' parameter value.
 *
 * @param wait The time in milliseconds to wait before moving the rainbow to the
 *             next position. A smaller value results in a faster animation.
 * @note this function comes from mattnupen, https://codebender.cc/sketch:57804#NeoPixel%20Function%20Sample.ino
 */
void rainbow(uint8_t wait) {
  uint32_t currentMillis = millis();
  if (currentMillis - lastUpdate > wait) {
    lastUpdate = currentMillis;
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i * 1 + j) & 255));
    }
    strip.show();
    if (++j >= 256) {
      j = 0;
    }
  }
}

/**
 * @brief Calculates a color value given a position within a rainbow cycle.
 * 
 * This function generates a smooth gradient of colors transitioning from red to
 * green to blue and back to red, creating a rainbow effect. The position within
 * the cycle is specified by 'WheelPos', which ranges from 0 to 255. The
 * function divides this range into three equal parts, each corresponding to a
 * transition between two primary colors.
 *
 * @param WheelPos The position within the rainbow cycle, ranging from 0 to 255.
 *                 This value determines the color returned by the function,
 *                 creating a seamless transition across a set of colors.
 * @return The 32-bit color value in RGB format that corresponds to the given
 *         position within the rainbow cycle. This color can be used to set an
 *         LED's color on a strip.
* @note this function comes from mattnupen, https://codebender.cc/sketch:57804#NeoPixel%20Function%20Sample.ino
 */
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