
int ai_pin = A0;
int field_pwm_pin = 3;
int field_max_on_time = 6666; //microseconds, 150 Hz
int field_on_time = 0;
int field_off_time = 0;
unsigned long field_time = micros();
bool field_pwm_high = false;
int ai_read = 0;
float scale = -0.000631893;
float offset = 0.616471;

int stator_pwm_pin = 4;
int stator_max_on_time = 10000; //microseconds, 100 Hz
int stator_on_time = 0;
int stator_off_time = 0;
unsigned long stator_time = micros();
bool stator_pwm_high = false;

bool startup_delay = true;

void setup() {
  
  pinMode(field_pwm_pin, OUTPUT);
  pinMode(stator_pwm_pin, OUTPUT);

}

void loop() {

  ai_read = analogRead(ai_pin);  // read throttle position
  
  if (ai_read > 200) {
    
    // 100% duty, run stator controller
    digitalWrite(stator_pwm_pin, LOW);

    if (startup_delay == false) {

      // field pwm control
      if (field_pwm_high == false) { 
        if ((micros() - field_time) > field_off_time) {
          digitalWrite(field_pwm_pin, HIGH);
          field_on_time = int(((ai_read * scale) + offset) * field_max_on_time);
          field_pwm_high = true;
          field_time = micros();
        }
      }
      else {
        if ((micros() - field_time) > field_on_time) {
          digitalWrite(field_pwm_pin, LOW);
          field_off_time = field_max_on_time - field_on_time;
          field_pwm_high = false;
          field_time = micros();
        }
      }
    }
    else {
      delay(1);
      startup_delay = false;
    }
  }
  else {

    startup_delay = true;
    
    // stop field controller
    digitalWrite(field_pwm_pin, LOW);

    // 4% duty, stop stator controller
    if (stator_pwm_high == false) { 
      if ((micros() - stator_time) > stator_off_time) {
        digitalWrite(stator_pwm_pin, HIGH);
        stator_on_time = int(0.96 * stator_max_on_time);
        stator_pwm_high = true;
        stator_time = micros();
      }
    }
    else {
      if ((micros() - stator_time) > stator_on_time) {
        digitalWrite(stator_pwm_pin, LOW);
        stator_off_time = stator_max_on_time - stator_on_time;
        stator_pwm_high = false;
        stator_time = micros();
      }
    }
    
  }

}
