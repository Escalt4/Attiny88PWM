#pragma once

#define CORRECT_PWM 1 
#define FAST_PWM 0

namespace Attiny88PWM
{
    void resetTimer();
    void restoreTimer();
    void pinDetach(uint8_t pin);
    void pinAttach(uint8_t pin);
    void setPinModeOutput(uint8_t pin);
    void setMode(bool correct);
    void setPrescaling(int prescaling);
    void setResolution(byte resolution);
    void setFrequency(float frequency);
    void setDutyRaw(byte pin, uint16_t duty);
    void setDuty(byte pin, uint16_t duty);
    void setDutyPercent(byte pin, float duty);
    void setDuty8bit(byte pin, uint16_t duty);
    void setDuty10bit(byte pin, uint16_t duty);

    byte _modeDivider = 1;                         // дополнительный делитель итоговой частоты ШИМ
    int _prescaling = 1;                           // предделитель частоты таймера
    int _prescalingList[] = {1, 8, 64, 256, 1024}; // список предделителей
    byte _resolution = 8;                          // выбраное разрешение
    float _frequency = 0.0;                        // выбраная частота
    byte _tccr1a;                                  // прошлые настройки таймера
    byte _tccr1b;                                  // прошлые настройки таймера
};

// Бекап и сброс настроек таймера
void Attiny88PWM::resetTimer()
{
    Attiny88PWM::_tccr1a = TCCR1A;
    TCCR1A = 0;
    Attiny88PWM::_tccr1b = TCCR1B;
    TCCR1B = 0;
}

// Восстановление настроек таймера
void Attiny88PWM::restoreTimer()
{
    TCCR1A = _tccr1a;
    TCCR1B = _tccr1b;
}

// Отключение пина от таймера
void Attiny88PWM::pinDetach(uint8_t pin)
{
    switch (pin)
    {
    case 9:
        TCCR1A &= ~(1 << COM1A1);
        break;

    case 10:
        TCCR1A &= ~(1 << COM1B1);
        break;
    }
}

// Подключение пина к таймеру
void Attiny88PWM::pinAttach(uint8_t pin)
{
    switch (pin)
    {
    case 9:
        TCCR1A |= (1 << COM1A1);
        break;

    case 10:
        TCCR1A |= (1 << COM1B1);
        break;
    }
}

// Установка пина как выход
void Attiny88PWM::setPinModeOutput(uint8_t pin)
{
    switch (pin)
    {
    case 9:
        DDRB |= (1 << PB1);
        PORTB &= ~(1 << PB1);
        break;

    case 10:
        DDRB |= (1 << PB2);
        PORTB &= ~(1 << PB2);
        break;
    }
}

// Установка режима генерации ШИМ (Fast PWM или Correct PWM)
void Attiny88PWM::setMode(bool correct)
{
    if (correct)
    {
        // Задаем дополнительный делитель итоговой частоты ШИМ
        Attiny88PWM::_modeDivider = 2;

        // устанавливаем 10й режим работы таймера (по даташиту, PWM Phase Correct, сброс по ICR1)
        TCCR1A |= (1 << WGM11);
        TCCR1B |= (1 << WGM13);
    }
    else
    {
        // Задаем дополнительный делитель итоговой частоты ШИМ
        Attiny88PWM::_modeDivider = 1;

        // устанавливаем 14й режим работы таймера (по даташиту, Fast PWM, сброс по ICR1)
        TCCR1A |= (1 << WGM11);
        TCCR1B |= (1 << WGM12) | (1 << WGM13);
    }
}

// Установка предделителя частоты таймера
void Attiny88PWM::setPrescaling(int prescaling)
{
    switch (prescaling)
    {
    case 1:
        Attiny88PWM::_prescaling = 1;
        TCCR1B |= (1 << CS10);
        break;

    case 8:
        Attiny88PWM::_prescaling = 8;
        TCCR1B |= (1 << CS11);
        break;

    case 64:
        Attiny88PWM::_prescaling = 64;
        TCCR1B |= (1 << CS10) | (1 << CS11);
        break;

    case 256:
        Attiny88PWM::_prescaling = 256;
        TCCR1B |= (1 << CS12);
        break;

    case 1024:
        Attiny88PWM::_prescaling = 1024;
        TCCR1B |= (1 << CS10) | (1 << CS12);
        break;
    }
}

// Установка верхнего предела таймера исходя из заданого разрешения ШИМ
// Частота ШИМ будет зависеть от частоты процессора, режима и предделителя
void Attiny88PWM::setResolution(byte resolution)
{
    Attiny88PWM::_resolution = constrain(resolution, 1, 16);

    ICR1 = (1ul << Attiny88PWM::_resolution) - 1;
}

// Установка верхнего предела и предделителя таймера исходя из заданной частоты
// Не работает, TODO: исправить и дописать логику выбора предделителя и ICR1
void Attiny88PWM::setFrequency(float frequency)
{
    // _frequency = constrain(frequency, 0, 8000000);

    // uint32_t top;

    // // ищем верхний предел для счетчика таймера
    // // перебираем варианты предделителей чтобы выбрать наименьший
    // for (byte i = 0; i < 5; i++)
    // {
    //     top = F_CPU / _prescalingList[i] / (_frequency * _modeDivider);
    //     if (top <= 65535)
    //     {
    //         ICR1 = top;
    //         break;
    //     }
    // }

    // // определяем разрешение верхнего предела для счетчика таймера
    // for (_resolution = 0; top > 0; top >>= 1)
    // {
    //     _resolution++;
    // }
}

// Установка заполнения ШИМ значением от 0 до верхнего предела таймера (ICR1)
void Attiny88PWM::setDutyRaw(byte pin, uint16_t duty)
{
    if (duty)
    {
        pinAttach(pin);

        switch (pin)
        {
        case 9:
            OCR1A = duty;
            break;

        case 10:
            OCR1B = duty;
            break;
        }
    }
    else
    {
        pinDetach(pin);
    }
}

// Установка заполнения ШИМ значением от 0 до двух в степени выбраного разрешения
void Attiny88PWM::setDuty(byte pin, uint16_t duty)
{
    setDutyRaw(pin, map(constrain(duty, 0, (1ul << Attiny88PWM::_resolution) - 1), 0, (1ul << Attiny88PWM::_resolution) - 1, 0, ICR1));
}

// Установка заполнения ШИМ значением от 0 до 100
void Attiny88PWM::setDutyPercent(byte pin, float duty)
{
    setDutyRaw(pin, map(constrain(duty, 0, 100), 0, 100, 0, ICR1));
}

// Установка заполнения ШИМ значением от 0 до 255 (8 бит)
void Attiny88PWM::setDuty8bit(byte pin, uint16_t duty)
{
    setDutyRaw(pin, map(constrain(duty, 0, 255), 0, 255, 0, ICR1));
}

// Установка заполнения ШИМ значением от 0 до 1023 (10 бит)
void Attiny88PWM::setDuty10bit(byte pin, uint16_t duty)
{
    setDutyRaw(pin, map(constrain(duty, 0, 1023), 0, 1023, 0, ICR1));
}
