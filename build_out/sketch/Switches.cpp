#line 1 "/home/runner/work/ESP32Marauder_xiaomiao/ESP32Marauder_xiaomiao/esp32_marauder/Switches.cpp"
#include "Switches.h"

Switches::Switches() {
	this->pin = 0;
	this->pin = false;
	this->pressed = false;
	this->hold_lim = 2000;
	this->cur_hold = 0;
	this->isheld = false;
	
	pinMode(this->pin, INPUT);
	
	return;
}

Switches::Switches(int pin, uint32_t hold_lim, bool pullup) {
	this->pin = pin;
	this->pullup = pullup;
	this->pressed = false;
	this->hold_lim = hold_lim;
	this->cur_hold = 0;
	this->isheld = false;

  // ESP32 GPIO 34/35/36/39 are input-only pads with NO internal pull-up/pull-down
  // resistors. INPUT_PULLUP is silently ignored on them, leaving the pin floating,
  // so digitalRead returns noise and the button reads as randomly pressed/released.
  // Boards that wire a button to one of these pins (e.g. XiaoMiao C_BTN=34, R_BTN=35)
  // must provide an EXTERNAL pull-up; in that case use plain INPUT. The active-low
  // read in getButtonState() still works because the external pull-up holds the pin
  // HIGH and the button pulls it LOW. (pin 38 is not an input-only pad on ESP32.)
  if (pullup && (pin == 34 || pin == 35 || pin == 36 || pin == 39))
    pinMode(this->pin, INPUT);
  else if (pullup)
    pinMode(this->pin, INPUT_PULLUP);
  else
    pinMode(this->pin, INPUT_PULLDOWN);

	return;
}

int Switches::getPin() {
	return this->pin;
}

bool Switches::getPullup() {
	return this->pullup;
}

bool Switches::isHeld() {
	return this->isheld;
}

bool Switches::getButtonState() {
	int buttonState = digitalRead(this->pin);
	
	if ((this->pullup) && (buttonState == LOW))
		return true;
	else if ((!this->pullup) && (buttonState == HIGH))
		return true;
	else
		return false;
}

bool Switches::justPressed() {
	bool btn_state = this->getButtonState();
	
	// Button was JUST pressed
	if (btn_state && !this->pressed) {
		this->hold_init = millis();
		this->pressed = btn_state;
		return true;
	}
	else if (btn_state) { // Button is STILL pressed
		// Check if button is held
		//Serial.println("cur_hold: " + (String)this->cur_hold);
		if ((millis() - this->hold_init) < this->hold_lim) {
			this->isheld = false;
		}
		else {
			this->isheld = true;
		}
		
		this->pressed = btn_state;
		return false;
	}
	else { // Button is not pressed
		this->pressed = btn_state;
		this->isheld = false;
		return false;
	}
}

bool Switches::justReleased() {
	bool btn_state = this->getButtonState();
	
	// Button was JUST released
	if (!btn_state && this->pressed) {
		this->isheld = false;
		this->pressed = btn_state;
		return true;
	}
	else { // Button is STILL released
		this->pressed = btn_state;
		return false;
	}
	
}