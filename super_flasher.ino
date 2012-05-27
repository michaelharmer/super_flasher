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
  



RGBLED *triple1, *triple2;
SingleLED *single1, *single2;
Flasher *flasher1, *flasher2, *flasher3, *flasher4;
 
void setup()
{
  Serial.begin(9600); 
  triple1 = new RGBLED(9,10,11,2);
  flasher1 = new Flasher(0, triple1);
  
  triple2 = new RGBLED(3, 5, 6, 4);
  flasher2 = new Flasher(1, triple2);
  
  single1 = new SingleLED(7);
  flasher3 = new Flasher(2, single1);
  
  single2 = new SingleLED(8);
  flasher4 = new Flasher(3, single2);
  
  randomSeed(analogRead(0));
}

 
void loop()
{
  flasher1->tick();  
  flasher2->tick();
  flasher3->tick();
  flasher4->tick();
}
