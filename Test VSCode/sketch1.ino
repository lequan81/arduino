#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup()
{
	lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("Hello, world!");
  lcd.setCursor(2, 1);
  lcd.print("VS Code Arduino!");
  lcd.setCursor(0, 2);
  lcd.print("Arduino LCD I2C 20x4");
  lcd.setCursor(3, 3);
  lcd.print("Make by LeQuan");
}

void loop()
{
	
}
