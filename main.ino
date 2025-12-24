// main.ino
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

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

    void setClock(Time inpTime)
    {
        totalMillisec = 0;
        totalMillisec = (inpTime.hour * 3600000L) + (inpTime.min * 60000L) + (inpTime.sec * 1000L);
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

char zaman[9]; // "HH:MM:SS"

const int buttonPins[5] = {4, 5, 6, 7, 8};
/*
    4 -> ayar değiştirme ve menüden çıkıp başlatma butonu. start button
    6 -> menüde saat/dakika/saniye ayarlamak için (+)
    7 -> menüde saat/dakika/saniye ayarlamak için (-)
    8 -> rakip kişiye geçme butonu 
    9 -> rakip kişiye geçme butonu
*/


const size_t numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);

int inMenu = 1; // 0-1

SideClock whiteSide;
SideClock blackSide;

void setup()
{
    whiteSide.setClock((Time){0, 1, 5});
    whiteSide.startClock();

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

    if(buttonPins[0] == LOW) inMenu = 0;
    
    if(inMenu) return;

    lcd.clear();
}