// main.ino
#include <Adafruit_LiquidCrystal.h>

Adafruit_LiquidCrystal lcd(0);

// buttonPins[0] -> Pin 6 (-)
// buttonPins[1] -> Pin 5 (+)
// buttonPins[2] -> Pin 4 (Start/Menu)
// buttonPins[3] -> taraf değişimi
// buttonPins[4] -> taraf değişimi
const int buttonPins[] = {6, 5, 4, 3, 2}; 
const size_t numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);

enum class SettingType { HOUR = 3, MINUTE = 2, SECOND = 1, DONE = 4};

struct Time
{
    int8_t hour;
    int8_t min;
    int8_t sec;
};

class SideClock
{
private:
    int32_t totalMillisec = 0;
    uint32_t beginTime = 0;

public:
    void setClockMill(int32_t tmp)
    {
        if(tmp < 0) tmp = 0;
        totalMillisec = tmp;
    }

    int32_t getTot() const
    {
        if (beginTime != 0) return totalMillisec - (millis() - beginTime);
        return totalMillisec;
    }

    int8_t getHour() const
    {
        int32_t tot = getTot();
        if(tot < 0) return 0; 
        return (tot / 3600000L) % 24L;
    }

    int8_t getMin() const
    {
        int32_t tot = getTot();
        if(tot < 0) return 0;
        return (tot / 60000L) % 60L;
    }

    int8_t getSec() const
    {
        int32_t tot = getTot();
        if(tot < 0) return 0;
        return (tot / 1000L) % 60L;
    }

    void startClock()
    {
        if(beginTime == 0) beginTime = millis();
    }

    void stopClock()
    {
        if(beginTime != 0)
        {
            totalMillisec -= (millis() - beginTime);
            beginTime = 0;
        }
    }
};

class Timer
{
private:
    uint32_t beginTime = 0;
public:
    void startTimer() { beginTime = millis(); }
    int32_t getElapsedTimeMill() { return (millis() - beginTime); }
    void restartTimer() { beginTime = millis(); }
};


char zaman[20]; 

bool inMenu = true;
Timer timer;
int32_t increaseAmount = 1000;
SettingType curSetTimer = SettingType::SECOND;

char settingSide = 'w';
char moveSide = 'w';

uint32_t lastWhiteTime;
uint32_t lastBlackTime;

SideClock whiteSide;
SideClock blackSide;

void setup()
{
    lcd.begin(16, 2);
    lcd.setBacklight(1); 

    for(int i = 0; i < numButtons; ++i)
        pinMode(buttonPins[i], INPUT_PULLUP);

    whiteSide.setClockMill(10 * 60000L);
    blackSide.setClockMill(10 * 60000L);
}

void loop()
{
    int h1 = whiteSide.getHour();
    int m1 = whiteSide.getMin();
    int s1 = whiteSide.getSec();

    int h2 = blackSide.getHour();
    int m2 = blackSide.getMin();
    int s2 = blackSide.getSec();
    
    lcd.setCursor(0, 0);
    // (int) ekleyerek sprintf hatasını önlüyoruz
    sprintf(zaman, "W %02d:%02d:%02d   ", (int)h1, (int)m1, (int)s1);
    lcd.print(zaman);

    // ALT SATIR: Siyah (B)
    lcd.setCursor(0, 1);
    sprintf(zaman, "B %02d:%02d:%02d   ", (int)h2, (int)m2, (int)s2);
    lcd.print(zaman);

    if(inMenu)
    {
        if(digitalRead(buttonPins[1]) == LOW && timer.getElapsedTimeMill() > 100)
        {
            if(settingSide == 'w') whiteSide.setClockMill(whiteSide.getTot() + increaseAmount);
            if(settingSide == 'b') blackSide.setClockMill(blackSide.getTot() + increaseAmount);
            timer.restartTimer();
        }

        if(digitalRead(buttonPins[0]) == LOW && timer.getElapsedTimeMill() > 100)
        {
            if(settingSide == 'w') whiteSide.setClockMill(whiteSide.getTot() - increaseAmount);
            if(settingSide == 'b') blackSide.setClockMill(blackSide.getTot() - increaseAmount);
            timer.restartTimer();
        }
        
        if(curSetTimer == SettingType::SECOND)
        {
            increaseAmount = 1000;
            lcd.setCursor(0,1);
            lcd.print("second   ");
        }
        else if(curSetTimer == SettingType::MINUTE)
        {
            increaseAmount = 60000;
            lcd.setCursor(0,1);
            lcd.print("minute   ");
        }
        else if(curSetTimer == SettingType::HOUR)
        {
            increaseAmount = 3600000;
            lcd.setCursor(0,1);
            lcd.print("hour     ");
        }

        if(digitalRead(buttonPins[2]) == LOW && timer.getElapsedTimeMill() > 150)
        {
            if(curSetTimer == SettingType::DONE)
            {
                if(settingSide == 'b')
                {
                    inMenu = false;
                    whiteSide.startClock();
                    lcd.clear();
                    lcd.setCursor(0,1);
                    lcd.print("Game Started");

                    //

                    lastWhiteTime = whiteSide.getTot();
                    lastBlackTime = blackSide.getTot();
                }
                if(settingSide == 'w')
                {
                    settingSide = 'b';
                    curSetTimer = SettingType::SECOND;
                }
            }
            else curSetTimer = (SettingType)(((int)curSetTimer) + 1);
            
        }
        return;
    }

    if(moveSide == 'w')
    {
        if(digitalRead(buttonPins[3]) == LOW && timer.getElapsedTimeMill() > 100)
        {
            blackSide.setClockMill(lastBlackTime);
            lastWhiteTime = whiteSide.getTot();
            timer.restartTimer();
            moveSide == 'b';
        }
    }
    else if(moveSide == 'b')
    {
        if(digitalRead(buttonPins[3]) == LOW && timer.getElapsedTimeMill() > 100)
        {
            whiteSide.setClockMill(lastWhiteTime);
            lastBlackTime = blackSide.getTot();
            timer.restartTimer();
            moveSide == 'w';
        }
    }
}