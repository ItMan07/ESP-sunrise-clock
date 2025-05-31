#include <Arduino.h>

uint32_t startMS;     // переменная для хранения стартового количства мс
int dayOfWeekNow = 2; // настройка текущего дня недели (сейчас вторник)

void setup()
{
  // настраиваем пин 3 на выход
  pinMode(3, OUTPUT);

  // включаем сериал порт для отладки
  Serial.begin(9600);

  // разбиваем текущее время с компьютера на части
  const char now[] = __TIME__;
  uint32_t hour = atoi(&now[0]);
  uint32_t minute = atoi(&now[3]);
  uint32_t seconds = atoi(&now[6]);

  // переводим текущее время в мс
  startMS = (millis() / 1000ul) + (hour * 3600) + (minute * 60) + seconds + 30;
}

void loop()
{
  // считаем время
  uint32_t sec = (millis() / 1000ul) + startMS;
  int timeDaysWeek = (dayOfWeekNow + (sec / 86400)) % 7; // дни недели
  int timeHours = (sec / 3600ul) % 24ul;                 // часы
  int timeMins = (sec % 3600ul) / 60ul;                  // минуты
  int timeSecs = (sec % 3600ul) % 60ul;                  // секунды

  // проверяем, если рабочий день
  if (timeDaysWeek <= 5)
  {
    // время с 6:30 до 7:10 - нужно включить лампочку
    if (timeHours == 6 && timeMins == 30 && timeSecs == 0)
    {
      digitalWrite(3, 1);
    }
    // в иных случаях - выключаем
    if (timeHours == 7 && timeMins == 10 && timeSecs >= 0)
    {
      digitalWrite(3, 0);
    }
  }

  // выводим время и состояние лампочки в сериал порт
  static uint32_t timer;
  if (millis() - timer >= 1000)
  {
    timer = millis();
    Serial.print(millis());
    Serial.print(" | ");
    Serial.print(timeDaysWeek);
    Serial.print(" | ");
    Serial.print(sec);
    Serial.print(" | ");
    Serial.print(timeHours);
    Serial.print(" : ");
    Serial.print(timeMins);
    Serial.print(" : ");
    Serial.println(timeSecs);
  }
}
