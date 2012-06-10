#include <Bounce.h>

#define FLASH_LENGTH 100

class LED {
  public:
    boolean ledOn;
 
  virtual void show()=0;
  virtual void hide()=0;  
  
  
};

class SingleLED : public LED {
  protected:
    int _onPin;
  
  public:
  
  SingleLED(int onPin)
  {
    _onPin = onPin;
    pinMode(_onPin, OUTPUT);
    digitalWrite(_onPin, LOW);
    ledOn = false;
  }
  
  void show()
  {
    digitalWrite(_onPin, HIGH);
    ledOn = true;
  }
  
  void hide()
  {
    digitalWrite(_onPin, LOW);
    ledOn = false;
  }
  
};


class RGBLED : public LED {
  protected:
    int _pins[3], _onPin;
  
  public:
  
  RGBLED(int redPin, int greenPin, int bluePin, int onPin)
  {
    ledOn = false;
    _pins[0] = redPin;
    _pins[1] = greenPin;
    _pins[2] = bluePin;
    for(int i = 0; i < 3; i++)
    {
      pinMode(_pins[i], OUTPUT);
    }
    _onPin = onPin;
    pinMode(_onPin, OUTPUT);
    digitalWrite(_onPin, LOW);
  }
  
  void show()
  {
    displayRandomColour();
    digitalWrite(_onPin, HIGH);
    ledOn = true;
  }
  
  void hide()
  {
    digitalWrite(_onPin, LOW);
    ledOn = false;
  }
  
  void displayRandomColour()
  {
  int currentColorValueGreen = random(64, 128);
  int currentColorValueBlue = 128;
  int currentColorValueRed = 0;
  
  int realRed = random(0, 3);
  int realBlue = random(0, 3);
  while (realBlue == realRed) 
    realBlue = random(0, 3);
  int realGreen = random(0, 3);
  while (realGreen == realRed || realGreen == realBlue)
    realGreen = random(0, 3);

  analogWrite(_pins[realRed], 255 - currentColorValueRed);
  analogWrite(_pins[realBlue],  255 - currentColorValueBlue);
  analogWrite(_pins[realGreen],  255 - currentColorValueGreen);
  }
  
};

class MultiLED : public LED {
  protected:
    LED **_leds;
    int _num;
  
  public:
  
  MultiLED(LED *led1, LED *led2, LED *led3, LED *led4)
  {
    _leds = (LED **) malloc(sizeof(LED *) * 4);
    _leds[0] = led1;
    _leds[1] = led2;
    _leds[2] = led3;
    _leds[3] = led4;
    _num = 4;
    ledOn = false;
  }
  
  MultiLED(LED *led1, LED *led2)
  {
    _leds = (LED **) malloc(sizeof(LED *) * 2);
    _leds[0] = led1;
    _leds[1] = led2;
    _num = 2;
    ledOn = false;
  }
  
  void show()
  {
    for (int i=0; i < _num; i++) 
      _leds[i]->show();
    ledOn = true;
  }
  
  void hide()
  {
    for (int i=0; i < _num; i++) 
      _leds[i]->hide();
    ledOn = false;
  }
  
};


class Flasher { 
  protected:
   int _speedPotPin;
   unsigned long _blinkSpeed;
   unsigned long _timeForNextEvent;
   LED *_led;
   
  public:
  
    Flasher(int speedPotPin, LED *led)
    {
      _speedPotPin = speedPotPin;  
      setTimeForNextFlash();
      _led = led;
    }

    long getBlinkSpeed()
    {
      return map(analogRead(_speedPotPin), 0, 1023, 25, 1000);
    }
    
    void setTimeForNextFlash()
    {
      _timeForNextEvent = millis() + getBlinkSpeed();        
    }
    
    void setTimeForEndOfFlash()
    {
      _timeForNextEvent = millis() + FLASH_LENGTH;
    }
    
    void tick()
    {
      if (millis() > _timeForNextEvent)
      {
        if (_led->ledOn)
        {
          _led->hide();
          setTimeForNextFlash();
        } 
        else
        {
          _led->show();
          setTimeForEndOfFlash(); 
        }
      }
    }
};
  

#define MODE_INDEPENDENT 0
#define MODE_ALL_SYNC 1
#define MODE_LEFT_RIGHT_SYNC 2
#define NUM_MODES 3

#define YELLOW_POT 3
#define GREEN_POT 2
#define BLUE_POT 1
#define RED_POT 0


#define BUTTON_PIN 12
RGBLED *triple_left, *triple_right;
SingleLED *single_right, *single_left;
MultiLED *multi1, *multi_left_leds, *multi_right_leds;
Flasher *flasher1, *flasher2, *flasher3, *flasher4, *flasher5, *flasher_left_leds, *flasher_right_leds;
Bounce bouncer = Bounce(BUTTON_PIN, 5);
int mode = MODE_INDEPENDENT;

void setup()
{
  Serial.begin(9600); 
  triple_left = new RGBLED(9,10,11,2);
  flasher1 = new Flasher(RED_POT, triple_left);
  
  triple_right = new RGBLED(3, 5, 6, 4);
  flasher2 = new Flasher(BLUE_POT, triple_right);
  
  single_right = new SingleLED(7);
  flasher3 = new Flasher(GREEN_POT, single_right);
  
  single_left = new SingleLED(8);
  flasher4 = new Flasher(YELLOW_POT, single_left);
  
  multi1 = new MultiLED(triple_left, triple_right, single_right, single_left);
  flasher5 = new Flasher(RED_POT, multi1);
  
  multi_left_leds = new MultiLED(triple_left, single_left);
  flasher_left_leds = new Flasher(YELLOW_POT, multi_left_leds);
  
  multi_right_leds = new MultiLED(triple_right, single_right);
  flasher_right_leds = new Flasher(GREEN_POT, multi_right_leds);
  
  pinMode(BUTTON_PIN, INPUT); 
  
  randomSeed(analogRead(0));
}

 
void loop()
{
  // Update the debouncer
  bouncer.update( );

  // Get the update value 
  if (bouncer.fallingEdge()) {
    mode++;
    mode = mode % NUM_MODES;
  }
  
  switch(mode) 
  {
    case MODE_INDEPENDENT:
      flasher1->tick();  
      flasher2->tick();
      flasher3->tick();
      flasher4->tick();
      break;
    case MODE_ALL_SYNC:
      flasher5->tick();
      break;  
    case MODE_LEFT_RIGHT_SYNC:
      flasher_left_leds->tick();
      flasher_right_leds->tick();
      break;
  }
}
