#pragma once

bool _correct = false;
byte _modeDivider = 1;
int _prescaling = 1;
int _prescalingList[] = {1, 8, 64, 256, 1024};
byte _resolution = 8;
float _frequency = 0.0;

void init_Attiny88PWM()
{
    // сбрасываем настройки таймера
    TCCR1B = 0;
    TCCR1A = 0;

    // при переполнении счетчика таймера на выводах OC1A и OC1B устанавливается 1
    // при совпадении счетчика таймера с OCR1A и OCR1B на выводах OC1A и OC1B будет установлен 0
    TCCR1A |= (1 << COM1A1) | (1 << COM1B1);

    // выбираем делитель частоты 1
    TCCR1B |= (1 << CS10);

    // устанавливаем 14й режим работы таймера (по даташиту, Fast PWM, сброс по ICR1)
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM12) | (1 << WGM13);

    // установка нижнего предела счетчика таймера
    TCNT1 = 0;

    // установка верхнего предела счетчика таймера исходя из выбраного разрешения ШИМ
    ICR1 = 255;
}

void setMode_Attiny88PWM(bool correct)
{
    _correct = correct;

    if (correct)
    {
        _modeDivider = 2;

        // устанавливаем 10й режим работы таймера (по даташиту, PWM Phase Correct, сброс по ICR1)
        TCCR1A |= (1 << WGM11);
        TCCR1B |= (1 << WGM13);
    }
    else
    {
        _modeDivider = 1;

        // устанавливаем 14й режим работы таймера (по даташиту, Fast PWM, сброс по ICR1)
        TCCR1A |= (1 << WGM11);
        TCCR1B |= (1 << WGM12) | (1 << WGM13);
    }
}

void setPrescaling_Attiny88PWM(int prescaling)
{
    // устанавливаем предделитель
    switch (prescaling)
    {
    case 1:
        _prescaling = 1;
        TCCR1B |= (1 << CS10);
        break;

    case 8:
        _prescaling = 8;
        TCCR1B |= (1 << CS11);
        break;

    case 64:
        _prescaling = 64;
        TCCR1B |= (1 << CS10) | (1 << CS11);
        break;

    case 256:
        _prescaling = 256;
        TCCR1B |= (1 << CS12);
        break;

    case 1024:
        _prescaling = 1024;
        TCCR1B |= (1 << CS10) | (1 << CS12);
        break;

    default:
        break;
    }
}

void setResolution_Attiny88PWM(byte resolution)
{
    _resolution = constrain(resolution, 1, 16);

    // установка верхнего предела счетчика таймера исходя из выбраного разрешения ШИМ
    ICR1 = (1ul << _resolution) - 1;
}

void setFrequency_Attiny88PWM(float frequency)
{
    _frequency = constrain(frequency, 0, 8000000);

    uint32_t top;

    // ищем верхний предел для счетчика таймера
    // перебираем варианты предделителей чтобы выбрать наименьший
    for (byte i = 0; i < 5; i++)
    {
        top = F_CPU / _prescalingList[i] / (_frequency * _modeDivider);
        if (top <= 65535)
        {
            ICR1 = top;
            break;
        }
    }

    // определяем разрешение верхнего предела для счетчика таймера
    for (_resolution = 0; top > 0; top >>= 1)
    {
        _resolution++;
    }
}

void setDutyRaw_Attiny88PWM(byte pin, uint16_t duty)
{
    switch (pin)
    {
    case 9:
        if (duty)
        {
            // включаем пин
            DDRB |= (1 << PB1);

            // задаем скважность(заполнение)
            OCR1A = duty;
        }
        else
        {
            // отключаем пин если скважность(заполнение) = 0
            DDRB &= ~(1 << PB1);
        }
        break;

    case 10:
        if (duty)
        {
            // включаем пин
            DDRB |= (1 << PB2);

            // задаем скважность(заполнение)
            OCR1B = duty;
        }
        else
        {
            // отключаем пин если скважность(заполнение) = 0
            DDRB &= ~(1 << PB2);
        }
        break;

    default:
        break;
    }
}

void setDuty_Attiny88PWM(byte pin, uint16_t duty)
{
    setDutyRaw_Attiny88PWM(pin, map(constrain(duty, 0, (1ul << _resolution) - 1), 0, (1ul << _resolution) - 1, 0, ICR1));
}

void setDutyPercent_Attiny88PWM(byte pin, float duty)
{
    setDutyRaw_Attiny88PWM(pin, map(constrain(duty, 0, 100), 0, 100, 0, ICR1));
}

void setDuty8bit_Attiny88PWM(byte pin, uint16_t duty)
{
    setDutyRaw_Attiny88PWM(pin, map(constrain(duty, 0, 255), 0, 255, 0, ICR1));
}

void setDuty10bit_Attiny88PWM(byte pin, uint16_t duty)
{
    setDutyRaw_Attiny88PWM(pin, map(constrain(duty, 0, 1023), 0, 1023, 0, ICR1));
}
