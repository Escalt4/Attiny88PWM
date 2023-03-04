#pragma once

class Attiny88PWM
{
public:
    Attiny88PWM();
    void setMode(bool correct);
    void setPrescaling(int prescaling);
    void setResolution(byte resolution);
    void setDuty(byte pin, uint16_t duty);
    void setDutyPercent(byte pin, float duty);
    void setDuty8bit(byte pin, uint16_t duty);
    void setDuty10bit(byte pin, uint16_t duty);
    void setDutyLead(byte pin, uint16_t duty);

private:
    bool _correct = false;
    byte _resolution = 8;
};

Attiny88PWM::Attiny88PWM()
{
    // сбрасываем настройки таймера
    TCCR1B = 0;
    TCCR1A = 0;

    //
    TCCR1A |= (1 << COM1A1) | (1 << COM1B1);

    // выбираем делитель частоты 1
    TCCR1B |= (1 << CS10);

    // устанавливаем 14й режим работы таймера (по даташиту, Fast PWM, сброс по ICR1)
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM12) | (1 << WGM13);

    // установка нижнего предела счетчика таймера
    TCNT1 = 0;

    // установка верхнего предела счетчика таймера исходя из выбраного разрешения ШИМ
    ICR1 = (1 << _resolution) - 1;
}

void setMode(bool correct)
{
    _correct = correct;

    if (correct)
    {
        // устанавливаем 10й режим работы таймера (по даташиту, PWM Phase Correct, сброс по ICR1)
        TCCR1A |= (1 << WGM11);
        TCCR1B |= (1 << WGM13);
    }
    else
    { // устанавливаем 14й режим работы таймера (по даташиту, Fast PWM, сброс по ICR1)
        TCCR1A |= (1 << WGM11);
        TCCR1B |= (1 << WGM12) | (1 << WGM13);
        break;
    }
}

void setPrescaling(int prescaling)
{
    // устанавливаем предделитель
    switch (prescaling)
    {
    case 1:
        TCCR1B |= (1 << CS10);
        break;

    case 8:
        TCCR1B |= (1 << CS11);
        break;

    case 64:
        TCCR1B |= (1 << CS10) | (1 << CS11);
        break;

    case 256:
        TCCR1B |= (1 << CS12);
        break;

    case 1024:
        TCCR1B |= (1 << CS10) | (1 << CS12);
        break;

    default:
        break;
    }
}

void setResolution(byte resolution)
{
    _resolution = constrain(resolution, 1, 16);

    // установка верхнего предела счетчика таймера исходя из выбраного разрешения ШИМ
    ICR1 = (1ul << _resolution) - 1;
}

void setDuty(byte pin, uint16_t duty)
{
    switch (pin)
    {
    case 9:
        switch (duty)
        {
        case 0:
            // отключаем пин если скважность(заполнение) = 0
            DDRB &= ~(1 << PB1);
            break;

        default:
            // включаем пин
            DDRB |= (1 << PB1);

            // задаем скважность(заполнение)
            OCR1A = duty;
            break;
        }
        break;

    case 10:
        switch (duty)
        {
        case 0:
            DDRB &= ~(1 << PB2);
            break;

        default:
            DDRB |= (1 << PB2);
            OCR1B = duty;
            break;
        }
        break;

    default:
        break;
    }
}

void setDutyPercent(byte pin, float duty)
{
    setDuty(pin, map(constrain(duty, 0, 100), 0, 100, 0, ICR1));
}

void setDuty8bit(byte pin, uint16_t duty)
{
    setDuty(pin, map(constrain(duty, 0, 255), 0, 255, 0, ICR1));
}

void setDuty10bit(byte pin, uint16_t duty)
{
    setDuty(pin, map(constrain(duty, 0, 1023), 0, 1023, 0, ICR1));
}

void setDutyLead(byte pin, uint16_t duty)
{
    setDuty(pin, map(constrain(duty, 0, (1ul << _resolution) - 1), 0, (1ul << _resolution) - 1, 0, ICR1));
}
