#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int saat = 0;
int dak  = 1;
int san  = 5;

char zaman[9]; // "HH:MM:SS"

const int buttonPins[5] = {4, 5, 6, 7, 8};
/*
    4 -> menüden çıkıp başlatma butonu. start button
    6 -> menüde dakika/saniye ayarlamak için (+)
    7 -> menüde dakika/saniye ayarlamak için (-)
    8 -> rakip kişiye geçme butonu 
    9 -> rakip kişiye geçme butonu
*/


const size_t numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);

int inMenu = 1; // 0-1 

void setup()
{
    lcd.begin();
    lcd.backlight();

    for(int i = 0; i < numButtons; ++i)
        pinMode(buttonPins[0], INPUT_PULLUP);
}

void loop()
{
    sprintf(zaman, "%02d:%02d:%02d", saat, dak, san);
    lcd.setCursor(0, 0);
    lcd.print(zaman);

    if(buttonPins[0] == HIGH) inMenu = 0;
    
    if(inMenu) return;
    
    // main loop:

    delay(1000);
    lcd.clear();
    
    if(san > 0) san--;
    else
    {
        san = 59;
        if(dak > 0) dak--;
        else
        {
            dak = 59;
            if (saat > 0) saat--;
        }
    }

    // bitiş kontrolü
    if(saat == 0 && dak == 0 && san == 0)
    {
        while(1) // errory finish
        {
            lcd.print(zaman);
            delay(1000);
            lcd.clear();
            delay(500);
        }
    }

}