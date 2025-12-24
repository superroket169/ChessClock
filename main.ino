// main.ino
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

enum class SettingType
{
    HOUR = 1,
    MINUTE = 2,
    SECOND = 3,
    INCREAMENT = 4,
    MENU = 5
};

struct Time
{
    int8_t hour;
    int8_t min;
    int8_t sec;
};
// 1 saat = 3600 sn
// 1 dk = 60 sn

class SideClock
{
private:

    int32_t totalMillisec = 0;
    uint32_t beginTime = 0;

public:

    SideClock() = default;

    int8_t getHour() const
    {
        return ((totalMillisec - (millis() - beginTime)) / (3600L * 1000L)) % 24L;
    }

    int8_t getMin() const
    {
        return ((totalMillisec - (millis() - beginTime)) / (60L * 1000L)) % 60L;
    }

    int8_t getSec() const
    {
        return ((totalMillisec - (millis() - beginTime)) / 1000L) % 60L;
    }

    int32_t getTot() const
    {
        return totalMillisec;
    }

    void setClock(Time inpTime)
    {
        totalMillisec = (inpTime.hour * 3600000L) + (inpTime.min * 60000L) + (inpTime.sec * 1000L);
    }

    void setClockMill(uint32_t tmp)
    {
        totalMillisec = tmp;
    }

    // başlatır
    void startClock()
    {
        beginTime = millis();
    }

    // saati durdurur ve sıfırlar
    void stopClock()
    {
        beginTime = 0;
    }

    // yeniden başlatır. durdurmaz
    void restartClock()
    {
        beginTime = millis();
    }

};

class Timer
{
private:
    uint32_t beginTime = 0;

public:

    Timer() = default;

    void startTimer()
    {
        beginTime = millis();
    }

    int32_t getElapsedTimeMill()
    {
        return (millis() - beginTime);
    }

    void restartTimer()
    {
        beginTime = millis();
    }

};

char zaman[9]; // "HH:MM:SS"

const int buttonPins[5] = {4, 5, 6, 7, 8};
/*
    4 -> ayar değiştirme ve menüden çıkıp başlatma butonu. start button
    5 -> menüde saat/dakika/saniye ayarlamak için (+)
    6 -> menüde saat/dakika/saniye ayarlamak için (-)
    7 -> rakip kişiye geçme butonu 
    8 -> rakip kişiye geçme butonu
*/


const size_t numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);

bool inMenu = true; // 0-1
SettingType currSetType = SettingType::HOUR; 
Timer timer;
int8_t increaseAmount = 1;

SideClock whiteSide;
SideClock blackSide;

Time inputTime = {0, 0, 0};

void setup()
{
    lcd.init();
    lcd.backlight();

    for(int i = 0; i < numButtons; ++i)
        pinMode(buttonPins[i], INPUT_PULLUP);
}

void loop()
{
    sprintf(zaman, "%02d:%02d:%02d", whiteSide.getHour(), whiteSide.getMin(), whiteSide.getSec());
    lcd.setCursor(0, 0);
    lcd.print(zaman);

    if(inMenu)
    {
        if(buttonPins[1]) whiteSide.setClockMill(whiteSide.getTot() + increaseAmount);

        if(buttonPins[2]) whiteSide.setClockMill(whiteSide.getTot() - increaseAmount);

        if(buttonPins[0] == LOW && timer.getElapsedTimeMill() > 200)
        {
            if((int)currSetType <= 4) currSetType = (SettingType) (((int)currSetType) + 1);
            else inMenu = false;
            timer.restartTimer();
        }

        return;
    }
    lcd.clear();
}