#include "sound.h"
#include "hwtimer.h"
#include "swtimer.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#define Duration 100
// This configuration sets a 50% duty cycle configuration
// for CCR4. You will have to figure out what Timer_A module
// drives the buzzer, i.e. what pin TAx.4 is driving the
// buzzer. The Timer_A module then is x.

Timer_A_PWMConfig pwmConfig = {
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_1,
        (int) (SYSTEMCLOCK / 523.25),         // C5
        TIMER_A_CAPTURECOMPARE_REGISTER_4,
        TIMER_A_OUTPUTMODE_RESET_SET,
        (int) ((SYSTEMCLOCK/2) / 523.25)
};


void InitSound() {

    // This function switches the IC pin connected to
    // the buzzer from GPIO functionality to Timer_A functionality
    // so that we can drive it with PWM.

    GPIO_setAsPeripheralModuleFunctionOutputPin(
            GPIO_PORT_P2,
            GPIO_PIN7,
            GPIO_PRIMARY_MODULE_FUNCTION);
}




void PlaySound(tnote n, unsigned ms) {

    //=============================================================
    // TO BE COMPLETED BY YOU

    // Play note n for ms milliseconds.

    // You have to use the PWM setting of the Timer_A
    // peripheral that drives the buzzer to sound it

    // The delay ms is generated using a software timer
    // (different from Timer_A!)

    // PlaySound can be implemented as a blocking function.
    // That means that the function turns on the PWM
    // generation, then waits for ms milliseconds, then
    // turns the PWM generation off again.

    if (n == note_g4)
    {
        pwmConfig.timerPeriod = SYSTEMCLOCK/392;
        pwmConfig.dutyCycle = (SYSTEMCLOCK*0.5)/392;
    }

    else if (n == note_c5)
    {
        pwmConfig.timerPeriod = SYSTEMCLOCK/523.25;
        pwmConfig.dutyCycle = (SYSTEMCLOCK*0.5)/523.25;
    }

    else if (n == note_e5)
    {
        pwmConfig.timerPeriod = SYSTEMCLOCK/659.25;
        pwmConfig.dutyCycle = (SYSTEMCLOCK*0.5)/659.25;
    }

    else if (n == note_f5)
    {
        pwmConfig.timerPeriod = SYSTEMCLOCK/ 698.46;
        pwmConfig.dutyCycle = (SYSTEMCLOCK*0.5)/ 698.46;
    }

    else if (n == note_fs5)
    {
        pwmConfig.timerPeriod = SYSTEMCLOCK/ 739.99;
        pwmConfig.dutyCycle = (SYSTEMCLOCK*0.5)/ 739.99;
    }

    else if (n == note_g5)
    {
        pwmConfig.timerPeriod = SYSTEMCLOCK/ 783.99;
        pwmConfig.dutyCycle = (SYSTEMCLOCK*0.5)/ 783.99;
    }

    else if (n == note_a4)
    {
        pwmConfig.timerPeriod = SYSTEMCLOCK/ 440;
        pwmConfig.dutyCycle = (SYSTEMCLOCK*0.5)/ 440;
    }

    else if (n == note_b4)
    {
        pwmConfig.timerPeriod = SYSTEMCLOCK/ 493.88;
        pwmConfig.dutyCycle = (SYSTEMCLOCK*0.5)/ 493.88;
    }

    else if (n == note_silent)
    {
        pwmConfig.dutyCycle = 0;
    }

    else if (n == note_c6)
    {
        pwmConfig.timerPeriod = SYSTEMCLOCK/ 1046.50;
        pwmConfig.dutyCycle = (SYSTEMCLOCK*0.5)/ 1046.50;
    }
    else if (n == note_d5)
    {
        pwmConfig.timerPeriod = SYSTEMCLOCK/ 587.33;
        pwmConfig.dutyCycle = (SYSTEMCLOCK*0.5)/ 587.33;
    }

    else if (n == note_a5)
    {
        pwmConfig.timerPeriod = SYSTEMCLOCK/ 880.00;
        pwmConfig.dutyCycle = (SYSTEMCLOCK*0.5)/ 880.00;
    }
    else if (n == note_b5)
    {
        pwmConfig.timerPeriod = SYSTEMCLOCK/ 987.77;
        pwmConfig.dutyCycle = (SYSTEMCLOCK*0.5)/ 987.77;
    }
    //=============================================================
    tSWTimer T;

    int ClockNumber = 16000*ms;
    InitSWTimer( &T, TIMER32_1_BASE, ClockNumber); // not using timer 0 hardware because its for the buzzer
    StartSWTimer(&T);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    while (!SWTimerOneShotExpired(&T))
        pwmConfig.dutyCycle = 0;
}


void MachineSound(int i)
{
              if (i == 0)
              {
                  PlaySound(note_c5, Duration);
                  PlaySound(note_g5, Duration);
                  PlaySound(note_silent,Duration);
              }
              else if (i == 1)
              {
                  PlaySound(note_c5, Duration);
                  PlaySound(note_a5, Duration);
                  PlaySound(note_silent,Duration);
              }
              else if (i == 2)
               {
                   PlaySound(note_c5, 100);
                   PlaySound(note_b5, 100);
                   PlaySound(note_silent,100);
               }
              else if (i == 3)
               {
                   PlaySound(note_d5, 100);
                   PlaySound(note_g5, 100);
                   PlaySound(note_silent,100);
               }
              else if (i == 4)
               {
                   PlaySound(note_d5, 100);
                   PlaySound(note_a5, 100);
                   PlaySound(note_silent,100);
               }
              else if (i == 5)
               {
                   PlaySound(note_d5, 100);
                   PlaySound(note_b5, 100);
                   PlaySound(note_silent,100);
               }
              else if (i == 6)
               {
                   PlaySound(note_e5, 100);
                   PlaySound(note_g5, 100);
                   PlaySound(note_silent,100);
               }
              else if (i == 7)
               {
                   PlaySound(note_e5, 100);
                   PlaySound(note_a5, 100);
                   PlaySound(note_silent,100);
               }
              else if (i == 8)
               {
                   PlaySound(note_e5, 100);
                   PlaySound(note_b5, 100);
                   PlaySound(note_silent,100);
               }
}

void MachineWin ()
{
    PlaySound(note_e5, 100);
    PlaySound(note_b5, 100);
    PlaySound(note_c5, 100);
    PlaySound(note_a5, 100);
    PlaySound(note_a5, 500);
    PlaySound(note_c5, 100);
    PlaySound(note_b5, 100);
    PlaySound(note_silent,100);
}

void UserWin()
{
    PlaySound(note_a5, 100);
    PlaySound(note_f5, 100);
    PlaySound(note_g5, 100);
    PlaySound(note_d5, 100);
    PlaySound(note_a5, 500);
    PlaySound(note_b5, 100);
    PlaySound(note_c5, 100);
    PlaySound(note_silent,100);
}
