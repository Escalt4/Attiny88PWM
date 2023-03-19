# Attiny88PWM

Библиотека для расширеной аппаратной генерации ШИМ на пинах 9 и 10 используя Timer1 (16 bit), имеется настройка режима, частоты, разрядности и скважности генерируемого ШИМ сигнала. Библиотека создана чтобы использовать функций с понятными названиями вместо работы с регистрами.

### Поддерживаемые платформы:
- Attiny88
- MH-Tiny (плата разработки на основе Attiny88)
### Обратная связь:
- telegram [@Escalt4](https://t.me/Escalt4)


## Пример использования библиотеки
Для запуска генерации шим обязательно нужно сделать следующее:
- сбросить старые настройки таймера
- назначить нужный пин как выход
- выбрать режим, задать предделитель, разрешение или частоту (смотри таблицу ниже) и установить заполение (оно же скважность) ШИМ

```cpp
#include <Attiny88PWM.h>

// В setup сделаем cброс и установку своих настроек таймера
void setup() {
  // Сброс текущих настроек таймера
  Attiny88PWM::resetTimer();
  
  // Установка режима, FAST_PWM (0) или CORRECT_PWM (1)
  Attiny88PWM::setMode(FAST_PWM);

  // Установка предделителя
  Attiny88PWM::setPrescaling(1);

  // Установка разрешения
  Attiny88PWM::setResolution(9);

  // Переключение порта на выход встроеной функцией
  Attiny88PWM::setPinModeOutput(9);
  // Либо стандартной arduino-функцией
  // pinMode(9, OUTPUT);
}

void loop() {
  // Установить заполнение 70.3% на 9 выводе
  Attiny88PWM::setDutyPercent(9, 70.3);

  // Установить заполнение 200 из 255 на 9 выводе
  // Аналогичен стандартной arduino-функцим analogWrite(pin, value)
  Attiny88PWM::setDuty8bit(9, 200);

  // Установить заполнение равное значению analogRead(pin) которая возвращает от 0 до 1023
  Attiny88PWM::setDuty10bit(9, analogRead(7));
  
  // Сменить разрешение и установить заполнение 55100 из 65535 на 9 выводе  
  Attiny88PWM::setResolution(16);
  Attiny88PWM::setDuty(9, 55100)
}
```

## Описание функций
```cpp

resetTimer();
restoreTimer();
pinDetach(uint8_t pin);
pinAttach(uint8_t pin);
setPinModeOutput(uint8_t pin);
setMode(bool correct);
setPrescaling(int prescaling);
setResolution(byte resolution);
setFrequency(float frequency);
setDutyRaw(byte pin, uint16_t duty);
setDuty(byte pin, uint16_t duty);
setDutyPercent(byte pin, float duty);
setDuty10bit(byte pin, uint16_t duty);

setMode_Attiny88PWM(bool correct);
// Задать режим генерации шим
// mode: 0 (FAST_PWM) или 1 (CORRECT_PWM), по умолчанию 0 (FAST_PWM)

setPrescaling_Attiny88PWM(int prescaling);
// Задать предделитель частоты таймера, по умолчанию 1
// prescaling: 1, 8, 64, 256 или 1024

setResolution_Attiny88PWM(byte resolution);
// Задать разрядность ШИМ, по умолчанию задана разрядность 8 бит 
// Частота ШИМ будет зависеть от режима, предделителя, частоты процессора
// resolution: от 1 до 16

setFrequency_Attiny88PWM(float frequency);
// Задать конкретную частоту шим 
// На практике частота будет немного отличатся от заданой 
// frequency: от 0.0 до частота_процессора разделить на 2 (4 для CORRECT_PWM)

setDutyRaw_Attiny88PWM(byte pin, uint16_t duty);
// Задать скважность ШИМ 
// pin: 9 или 10
// duty: значение от 0 до верхнего предела счетчика таймера

setDuty_Attiny88PWM(byte pin, uint16_t duty);
// Задать скважность ШИМ, делает приведение значения и вызывает setDutyRaw_Attiny88PWM
// pin: 9 или 10
// duty: значение из диапазона выбраной разрядности ШИМ или разрядности для выбраной частоты ШИМ

setDutyPercent_Attiny88PWM(byte pin, float duty);
// Задать скважность ШИМ, делает приведение значения и вызывает setDutyRaw_Attiny88PWM
// pin: 9 или 10
// duty: заполнение ШИМ в процентах от 0.0 до 100.0
   
setDuty8bit_Attiny88PWM(byte pin, uint16_t duty);
// Задать скважность ШИМ, делает приведение значения и вызывает setDutyRaw_Attiny88PWM
// Произойдет приведение к реальному доступному диапазону
// pin: 9 или 10
// duty: заполнение ШИМ, значения из диапазона 0-255 (8 бит)
    
setDuty10bit_Attiny88PWM(byte pin, uint16_t duty);
// Задать скважность ШИМ, делает приведение значения и вызывает setDutyRaw_Attiny88PWM
// Произойдет приведение к реальному доступному диапазону
// pin: 9 или 10
// duty: заполнение ШИМ, значения из диапазона 0-1023 (10 бит)
```

## 
####Задаем режим, делитель и разрядность
    Считаем что частота процессора 16 МГц (для MH-Tiny) тоесть F_CPU = 16000000
    Если будет 8 МГц то делим частоты в таблице на 2, если 1 МГц то на 16 и так далее
    Режим CORRECT_PWM дополнительно делит частоту на 2
    Предделитель (prescaling) еще дополнительно делит частоту процессора и следовательно частоту ШИМ

    Тоесть частота = частота_процессора / предделитель / (2 в степени разрядность) и еще на 2 если режим CORRECT_PWM
    Например 8000000 / 64 / 1024 / 2 = ~61.035 Гц

    Таблица для упрощения понимания
```cpp
    Разрядность  	max  	prescaling 1   	prescaling 1   	prescaling 64  	prescaling 64  	prescaling 1024	prescaling 1024
    бит          	duty 	Fast PWM       	Correct PWM    	Fast PWM       	Correct PWM    	Fast PWM
                            Частота Гц     	Частота Гц     	Частота Гц     	Частота Гц     	Частота Гц     	Частота Гц

    1            	1    	8000000        	5333333        	125000        	83333.3        	7812.5         	5208.33
    2            	3    	4000000        	2285714        	62500.0        	35714.2        	3906.25        	2232.14
    3            	7    	2000000        	1066666        	31250.0        	16666.6        	1953.12        	1041.66
    4            	15   	1000000        	516129        	15625.0        	8064.51        	976.562        	504.032
    5            	31   	500000        	253968        	7812.5         	3968.25        	488.281        	248.015
    6            	63   	250000        	125984        	3906.25        	1968.50        	244.140        	123.031
    7            	127  	125000        	62745.0        	1953.12        	980.392        	122.070        	61.2745
    8            	255  	62500.0        	31311.1        	976.562        	489.236        	61.0351        	30.5772
    9            	511  	31250.0        	15640.2        	488.281        	244.379        	30.5175        	15.2737
    10           	1023 	15625.0        	7816.31        	244.140        	122.129        	15.2587        	7.63312
    11           	2047 	7812.5         	3907.20        	122.070        	61.0500        	7.62939        	3.81562
    12           	4095 	3906.25        	1953.36        	61.0351        	30.5213        	3.81469        	1.90758
    13           	8191 	1953.12        	976.622        	30.5175        	15.2597        	1.90734        	0.95373
    14           	16383	976.562        	488.296        	15.2587        	7.62962        	0.95367        	0.47685
    15           	32767	488.281        	244.144        	7.62939        	3.81475        	0.47683        	0.23842
    16           	65535	244.140        	122.071        	3.81469        	1.90736        	0.23841        	0.11921
```
