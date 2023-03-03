# Attiny88PWM

- Библиотека для расширеной генерации ШИМ на Attiny88 (MH-Tiny ATtiny88) используя Timer1 
- Аппаратная генерация ШИМ на пинах 9 и 10 с настройкой режима, частоты и разрядности ШИМ

## Описание
```cpp
- Задать режим генерации шим:
    mode: 0 (FAST_PWM) или 1 (CORRECT_PWM), по умолчанию 0 (FAST_PWM)
    setMode_Attiny88PWM(byte mode);

- Задать предделитель частоты таймера (если нужна очень маленькая частота ШИМ)
    prescaling: 1, 8, 64, 256 или 1024, по умолчанию 1
    setPrescaling_Attiny88PWM(int prescaling);

- Задать частоту или разрядность ШИМ, по умолчанию задана разрядность 8 бит (частота будет зависеть от режима/предделителя/частоты процессора)
    Если нужна конкретная разрядность или частота важна лишь примерно то задаем разрядность ШИМ
    resolution: от 1 до 16
    setResolution_Attiny88PWM(byte resolution);

    Или если нужна конкретная частота то задаем частоту ШИМ
    frequency: доступный диапазон частот смотрим по таблице в зависимости от режима/предделителя/частоты процессора
    setFrequency_Attiny88PWM(uint32_t frequency);

- Задать скважность ШИМ на конкретном выводе, по умолчанию 0 на обоих выходах
    В процентах
    pin: ШИМ может быть на 9 и/или 10 выводе
    duty: заполнение в процентах от 0 до 10, например 0, 27.0, 85.117 или 100
    но точность ограниченна выбраным разрешением или максимальным разрешением для выбраной частоты
    setDutyPercent_Attiny88PWM(byte pin, float duty);

    Значением duty из диапазона 0-255 (8 бит), произойдет приведение к реальному доступному диапазону для выбраной частоты/разрешения
    pin: ШИМ может быть на 9 и/или 10 выводе
    duty: заполнение ШИМ, 0-255
    setDuty8bit_Attiny88PWM(byte pin, uint16_t duty);

    Значением duty из диапазона 0-1023 (10 бит), произойдет приведение к реальному доступному диапазону для выбраной частоты/разрешения
    pin: ШИМ может быть на 9 и/или 10 выводе
    duty: заполнение ШИМ,  0-1023
    setDuty10bit_Attiny88PWM(byte pin, uint16_t duty);

    Значением duty из реально доступного диапазона для выбраной частоты/разрешения
    pin: ШИМ может быть на 9 и/или 10 выводе
    duty: заполнение ШИМ
    Если задавали конкретное разрешение то диапазон заполнения равен 2 в степени разрешения, например шим 11 бит и duty будет от 0 до 2^11-1=2047
    setDuty_Attiny88PWM(byte pin, uint16_t duty);
    Если задавали конкретную частоту то есть два варианта
    Максимальное значение duty будет равно частота_процессора / предделитель / частота_ШИМ (для CORRECT_PWM делим еще на 2)
    Например 16000000 / 64 / 300 = 833
    setDuty_Attiny88PWM(byte pin, uint16_t duty);
    Или для удобства приводим диапазон к битности этого самого максимального значение duty
    16000000 / 1 / 30000 = 533, число 533 занимает 10 бит (0-1023) значит и диапазон duty будет 0-1023
    setDutyLead_Attiny88PWM(byte pin, uint16_t duty);

```

## Примеры
```cpp
```

## Немного теории
1) Задаем режим, делитель и разрядность
    Считаем что частота процессора 16 МГц (для MH-Tiny ATtiny88), F_CPU = 16000000
    Если будет 8 МГц то делим частоты в таблице на 2, если 1 МГц то на 16 и так далее
    Режим CORRECT_PWM дополнительно делит частоту на 2
    Предделитель (prescaling) еще дополнительно делит частоту процессора и следовательно частоту ШИМ

    Тоесть частота = частота_процессора/ предделитель / (2 в степени разрядность) и еще на 2 если режим CORRECT_PWM
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
2) Задаем режим, делитель и частоту
    Допустим нам нужна частота 13000 Гц
    Частота процессора 16 МГц, режим FAST_PWM и делитель 1,
    тогда при частоте 13000 Гц Timer1 должен сосчитать 16000000 / 13000 = 1230,77 раз
    но он может сделать это только целое число раз,
    тоесть либо 1230 либо 1231 и частота будет 16000000 / 1230 = 13008,13 либо  16000000 / 1231 = 12997,563
    а не как не 130000 - возникает погрешность задания частоты
    и чем выше нужна частота тем больше будет эта погрешность которую вы должны учитывать
  
```cpp  
    Частота   	Значение  	Частота   	
    процессора	таймера   	ШИМ Гц    	

    16 Мгц    	250       	64000     	
    16 Мгц    	251       	63745     	
    16 Мгц    	252       	63492     	
    16 Мгц    	253       	63241     	
    16 Мгц    	254       	62992     	
    16 Мгц    	255       	62745     	
    16 Мгц    	256       	62500     	
    16 Мгц    	257       	62257     	
    16 Мгц    	258       	62016     	
    16 Мгц    	259       	61776     	
    16 Мгц    	260       	61538     	
    16 Мгц    	261       	61303     	
    16 Мгц    	262       	61069     	
    16 Мгц    	263       	60837     	
    16 Мгц    	264       	60606     	
    16 Мгц    	265       	60377     	
    16 Мгц    	266       	60150     	
    16 Мгц    	267       	59925     	
    16 Мгц    	268       	59701     	
    16 Мгц    	269       	59480
```
    
    
    
