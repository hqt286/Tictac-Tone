#include <stdlib.h>
#include "swtimer.h"
#include "maplogic.h"
#include "button.h"
#include "display.h"
#include "hwtimer.h"
#include "goertzel.h"
#include "microphone.h"
#include <LED_HAL.h>
#include "sound.h"

// This is the state definition for the top-level state machine,
// implemented in ProcessStep. As you build the game, you will
// have to define additional states to implement the actual game
// logic.
typedef enum {idle, GameMode1, GameMode2, GiveUp, GameOver, ButtonWait, ButtonWait1 , Dummy1} state_t;
unsigned glbListening = 0;
// This structure is a game state. It keeps track of the current
// playing field, the score achieved by the human, and the score
// achieved by the computer. The game state gets passed around
// during the game to allow different software components access
// to the game state.

typedef struct {
    tcellstate map[9];
    int computerscore;
    int humanscore;
} gamestate_t;

static Gtap  t697 = {0.8538689 * 2 * (1 << 8),0,0};
static Gtap  t770 = {0.8226405 * 2 * (1 << 8),0,0};
static Gtap  t852 = {0.7843435 * 2 * (1 << 8),0,0};
static Gtap t1209 = {0.5820520 * 2 * (1 << 8),0,0};
static Gtap t1336 = {0.4981851 * 2 * (1 << 8),0,0};
static Gtap t1477 = {0.3993092 * 2 * (1 << 8),0,0};


// This function implements the functionality of Tic Tac Tone during
// the idle state, i.e. when it is waiting for the player to provide
// a 'start game' command.
//
// This function is called every time the top-level FSM visits the
// idle state. The function has five parameters. You are welcome to
// add more, but know that the reference solution was implemented using
// only these 5 parameters.
//    b1   = 1 when button S1 is pressed, 0 otherwise
//    b2   = 1 when button S2 is pressed, 0 otherwise
//    sec  = 1 when the second-interval software timer elapses
//    ms50 = 1 when the 50ms-interval software timer elapses
//    G    = gamestate, as defined above. Pass by reference to make
//           sure that changes to G within this function will be
//           propagated out of the function.
//
// Note that this function RETURNS a state_t. This can be used to influence
// the state transition in the top-level FSM that will call processIdle.
// Currently, the ProcessIdle always returns idle (which means the top-level
// FSM will iterate in the idle state), but when the game is extended,
// the return state could change into something different (such as playing_circle
// or playing_cross, depending on whether S1 or S2 is pressed).


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// get the sample put them into the filer

void DTMFAddSample(unsigned x)
{
    unsigned vx = ScaleSample(x);
    SampleGoertzel( &t697, vx);
    SampleGoertzel( &t770, vx);
    SampleGoertzel( &t852, vx);
    SampleGoertzel(&t1209, vx);
    SampleGoertzel(&t1336, vx);
    SampleGoertzel(&t1477, vx);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// reset the filter

void DTMFReset()
{
    ResetGoertzel( &t697);
    ResetGoertzel( &t770);
    ResetGoertzel( &t852);
    ResetGoertzel(&t1209);
    ResetGoertzel(&t1336);
    ResetGoertzel(&t1477);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// record the sound and put them into the sample

void T32_INT1_IRQHandler() {
    unsigned vx;
    static unsigned SamplesListened = 0;
    if (glbListening) {
        vx = GetSampleMicrophone();
        DTMFAddSample(vx);
        SamplesListened++;
        if (SamplesListened == 400) {
            glbListening = 0;
            SamplesListened = 0;
        }
    }
    Timer32_clearInterruptFlag(TIMER32_0_BASE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// game mode where user and machine takes turn

state_t GameModeM (int b1, int b2,gamestate_t *G, int *p697, int *p770, int *p852, int *p1209, int *p1336, int *p1477, int GameMode, int sec, int o5s)
{
    state_t ReturnValue = GameMode1;
    typedef enum {Idle, User, Machine, Winner, MachineThinking, Final, Dummy} GameState;
    static GameState CurrentState = Idle;
    int w,r = 0;
    switch (CurrentState)
    {
    case Idle:
        ClearMap(G->map); // clear the map every idle case
        DrawBoard(G->map);
        if (GameMode == 1) // machine go first if button 1 pressed
        {
        CurrentState = Machine;
        LCDSetFgColor(yellow);
        PrintString("Thinking !", 0,2);
        LCDSetFgColor(cyan);
        }
        else if (GameMode == 2) // user go first if the second button
        {
            ClearMap(G->map);
            DrawBoard(G->map); // clear out the screen and draw them
        CurrentState = User;
        LCDSetFgColor(yellow);
        PrintString("Listening ?", 0,2);
        LCDSetFgColor(cyan);
        }
        break;

    case Machine: // machine turn

        if (sec)
        {
        PlayMachineRandomAdd(G->map, cross);
        CurrentState = MachineThinking; // after machine think it will go to next state

        if (CrossWins(G->map) || Tie(G->map)) //check to see if anyone wins
        {
            CurrentState = Winner;
        }

        DrawBoard(G->map);
        if (b1 || b2)
            CurrentState = Idle;
        }
        break;

    case User: // user will go in this turn
        LCDSetFgColor(yellow);
        PrintString("Listening ?", 0,2);
        LCDSetFgColor(cyan);
        r = UpdateMap(G, SoundNumber(*p697, *p770, *p852,*p1209, *p1336, *p1477));

        if (r == 1) // if the move is legal then go to machine turn
       {
            LCDSetFgColor(yellow);
           CurrentState = Machine;
           PrintString("                  ", 0,0);
           PrintString("Thinking !", 0,2);
           LCDSetFgColor(cyan);
       }

        if (r == 2) // if the move is not legal then play the sounds then goback to  machine thinking
        {
            PrintString("                  ", 0,0);
            PlaySound(note_b5,100);
            PlaySound(note_c5, 100);
            PlaySound(note_silent,100);
            PrintString("Illegal Move !", 0,1);
            CurrentState = MachineThinking;
        }

        if (CircleWins(G->map) || Tie(G->map))
        {
            CurrentState = Winner;
        }
        DrawBoard(G->map);

        if (b1 || b2)
            CurrentState = Idle;

        break;

    case Winner:
        if (w = CircleWins(G->map))
        {
        UserWin();
        G->humanscore = G->humanscore + 1;
        DrawWinner(G->map, w, EMPHASISCOLOR);
        LCDSetFgColor(yellow);
        PrintString("                  ", 0,0);
        PrintString("YOU WIN !", 0,3);
        LCDSetFgColor(cyan);
        }
        if (w = CrossWins(G->map))
        {
        MachineWin ();
        G->computerscore = G->computerscore + 1;
        DrawWinner(G->map, w, EMPHASISCOLOR);
        LCDSetFgColor(yellow);
        PrintString("YOU LOST !", 0,3);
        LCDSetFgColor(cyan);
        }
        CurrentState = Final;
        break;

    case MachineThinking: // it will move to user
        if (sec)
        {
            CurrentState = User;
            PrintString("                  ", 0,0);
        }
        break;
    case Final: //final state will clear the map
        if (o5s)
        {
            PrintString("                  ", 0,0);
            ClearMap(G->map);
            DrawBoard(G->map);
            CurrentState = Dummy;
        }
        break;
    case Dummy: // it will wait for the map clear out then go back to idle
        if (sec)
        {
        ReturnValue = GameOver;
        CurrentState = Idle;
        }
    }
    return ReturnValue;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// this function will compare the sounds then give out the value should be input on the screen.
int SoundNumber(int p697, int p770, int p852, int p1209, int p1336, int p1477)
{
// it will return 1 if its a legal move
    // otherwise if it return 0 then it will be illegal move
    int ReturnValue = 0;
    if (p697 > 1000 && p1209 > 2000)
        ReturnValue = 1;
    else if (p697 > 3500 && p1336 > 300)
        ReturnValue = 2;
    else if (p697 > 10000 && p1477 > 1500)
        ReturnValue = 3;
    else if (p770 > 20000 && p1209 > 2500)
        ReturnValue = 4;
    else if (p770 > 20000 && p1336 > 800)
        ReturnValue = 5;
    else if (p770 > 2300 && p1477 > 815)
        ReturnValue = 6;
    else if (p852 > 28000 && p1209 > 2200)
        ReturnValue = 7;
    else if (p852 > 2500 && p1336 > 3600)
        ReturnValue = 8;
    else if (p852 > 3900 && p1477 > 3300)
        ReturnValue = 9;

    return ReturnValue;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// update the map based on the sound and threshold value
int UpdateMap(gamestate_t *G, int x)
{
    int ReturnValue = 0;
    if (x == 1 && G->map[0] == empty) // making sure if the map is empty
    {
        G->map[0] = circle;
        ReturnValue = 1;
    }
    else if (x == 2 && G->map[1] == empty)
    {
        G->map[1] = circle;
        ReturnValue = 1;
    }
    else if (x == 3 && G->map[2] == empty)
    {
        G->map[2] = circle;
        ReturnValue = 1;
    }
    else if (x == 4 && G->map[3] == empty)
    {
        G->map[3] = circle;
        ReturnValue = 1;
    }
    else if (x == 5 && G->map[4] == empty)
    {
        G->map[4] = circle;
        ReturnValue = 1;
    }
    else if (x == 6 && G->map[5] == empty)
    {
        G->map[5] = circle;
        ReturnValue = 1;
    }
    else if (x == 7 && G->map[6] == empty)
    {
        G->map[6] = circle;
        ReturnValue = 1;
    }
    else if (x == 8 && G->map[7] == empty)
    {
        G->map[7] = circle;
        ReturnValue = 1;
    }
    else if (x == 9 && G->map[8] == empty)
    {
        G->map[8] = circle;
        ReturnValue = 1;
    }
    // return 2 means the move is illegal
    else if  ((x == 1 && G->map[0] != empty) || (x == 2 && G->map[1] != empty) || (x == 3 && G->map[2] != empty)
            ||(x == 4 && G->map[3] != empty) || (x == 5 && G->map[4] != empty) || (x == 6 && G->map[5] != empty)
            ||(x == 7 && G->map[6] != empty) || (x == 8 && G->map[7] != empty) || (x == 9 && G->map[8] != empty))
    {
        ReturnValue = 2;
    }

    return ReturnValue;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Idle state



state_t ProcessIdle(int b1, int b2,
                    int sec, int ms50,
                    gamestate_t *G) {

    // These are the states of a _local_ FSM.
    // The state labels are prefixed with 'processidle' to
    // make sure they are distinct from the labels used for the
    // top-level FSM.
    //
    // The local FSM functionality plays a game of tic-tac-toe
    // against itself, using randomized moves. However, the
    // rules of tic-tac-toe are followed, including the game
    // map drawing and coloring over the reference solution.

    typedef enum {processidle_idle,
                  processidle_playingcircle,
                  processidle_playingcross,
                  processidle_winning} processidle_state_t;
    static processidle_state_t localstate = processidle_idle;

    unsigned w;

    // We will run this local state machine only once per second,
    // therefore, we only run it when sec is 1. sec is
    // a software-timer generated in the main function.
    //
    // To add more functionality, you can extend this function. For example,
    // to display a label every three seconds, you can add a counter that is
    // incremented for every sec, modulo-3. When the counter is two, it means
    // that the three-second timer mark is reached.
    //
    // A longer counter period (eg modulo-12 iso modulo-3) can be used to
    // display rotating messages.

    if (sec) {

        switch (localstate) {

            case processidle_idle:
              // Initially, just draw the playing field

              ClearMap(G->map);
              DrawBoard(G->map);
              localstate = processidle_playingcircle;
              break;

            case processidle_playingcircle:
              // This is circle who is playing. A circle is
              // added in a random (but valid) location. Next,
              // we check if the game ends, which happens when
              // circle or cross would win, or when there's a tie.

              // Decide what position to play
              RandomAdd(G->map, circle);

              // Next, it's cross' turn
              localstate = processidle_playingcross;

              // If we win or tie, go to winning state instead
              if (CircleWins(G->map) || Tie(G->map))
                  localstate = processidle_winning;

              // update game board status on display
              DrawBoard(G->map);
              break;

            case processidle_playingcross:;
              // This is cross who is playing. A cross is
              // added in a random (but valid) location. Next,
              // we check if the game ends, which happens when
              // circle or cross would win, or when there's a tie.

              // Decide what position to play
              RandomAdd(G->map, cross);

              // Next, it's circles' turn
              localstate = processidle_playingcircle;

              // If we win or tie, go to winning state instead
              if (CrossWins(G->map) || Tie(G->map))
                  localstate = processidle_winning;

              // update game board status on display
              DrawBoard(G->map);
              break;

            case processidle_winning:
              // This state is entered when there is a winner,
              // or it's a tie. In this state, we redraw the
              // winning combination in the emphasis color.
              // After that, we go for the next round.

              if (w = CircleWins(G->map))
                DrawWinner(G->map, w, EMPHASISCOLOR);

              if (w = CrossWins(G->map))
                DrawWinner(G->map, w, EMPHASISCOLOR);

              localstate = processidle_idle;
              break;

        }
    }

    return idle;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// This is the top-level FSM, which is called from within
// the cyclic executive. You will have to extend this FSM
// with the game logic. The FSM takes four inputs:
//
//    b1   = 1 when button S1 is pressed, 0 otherwise
//    b2   = 1 when button S2 is pressed, 0 otherwise
//    sec  = 1 when the second-interval software timer elapses
//    ms50 = 1 when the 50ms-interval software timer elapses

void ProcessStep(int b1, int b2, int sec, int ms50, int *p697, int *p770, int *p852, int *p1209, int *p1336, int *p1477, int o3s, int o5s) {
    static state_t S = idle;
    int i,w,m = 0;
    static gamestate_t G;
    state_t Mode;
    switch (S) {

    case idle:// in idle state, it will draw idle screen then wait for the button to be push
        DrawIdle (o3s,G.computerscore,G.humanscore);
        ProcessIdle(b1, b2, sec, ms50, &G);
        if (b1) // if button 1 pressed, go to game mode 1
        {
            ClearMap(G.map);
            DrawBoard(G.map);
            PrintString("                  ", 0,0);
            S = GameMode1;
        }
        else if (b2) // if button 2 pressed, go to game mode 2
        {
            PrintString("                  ", 0,0);
            S = GameMode2;
        }
        break;

    case GameMode1: // machine goes first
        m = 1;
        Mode = GameModeM (b1,b2,&G, p697, p770, p852, p1209, p1336, p1477,m,sec, o5s);
        if ( Mode == GameOver) // if the game is over then go to winning state
            S = idle;
        if (b1) // give the player give up, go to give up state
            S = GiveUp;
        break;
    case GameMode2:

        m = 2;
        Mode = GameModeM (b1,b2,&G, p697, p770, p852, p1209, p1336, p1477,m,sec, o5s);
        if ( Mode == GameOver)// if the game is over then go to winning state
             S = idle;
        if (b2)
            S = GiveUp;
        break;

    case GiveUp:// fill the map with x's
        PrintString("               ", 0,0);
        LCDSetFgColor(yellow);
        PrintString("YOU LOST !", 0,3);
        LCDSetFgColor(cyan);
        for (i = 0; i < 9; i++)
            if(G.map[i] == empty)
                G.map[i] = cross;

        DrawBoard(G.map);
        MachineWin ();
        if (w = CircleWins(G.map))
        {
            G.humanscore = G.humanscore + 1; // update the score for human if human win
            DrawWinner(G.map, w, EMPHASISCOLOR);
        }
        else if (w = CrossWins(G.map))
        {
            G.computerscore = G.computerscore + 1; // update the score for machine if machine with
               DrawWinner(G.map, w, EMPHASISCOLOR);
        }
               S = Dummy1;
               break;

    case Dummy1: // wait until the map clear out then go back to idle state
        if (sec)
        {
            PrintString("               ", 0,0);
            S = idle;
        }
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




int main(void) {

    // Device Initialization
    InitTimer();
    InitSound();
    InitDisplay();
    InitButtonS1();
    InitButtonS2();
    InitMicrophone();
    InitLEDs();
    Interrupt_enableInterrupt(INT_T32_INT1);
    Interrupt_enableMaster();
    int p697, p770, p852, p1209, p1336, p1477;
    // Software Timer - per second
    // Note that software timers MUST be tied to TIMER32_1_BASE;
    // TIMER32_1_BASE is configured in continuous mode
    // (TIMER32_0_BASE can then be used for periodic interrupts
    //  which will be needed to take samples from microphone)

    // Software Timer - per 50ms = 20Hz
    tSWTimer everySec;
    tSWTimer every50ms;
    tSWTimer OneShot3Sec;
    tSWTimer OneShot5Sec;
    tSWTimer OneShot2Sec;


        // initial all the timer
    InitSWTimer(&everySec, TIMER32_1_BASE, SYSTEMCLOCK);
    InitSWTimer(&OneShot3Sec, TIMER32_1_BASE, SYSTEMCLOCK*3);
    InitSWTimer(&every50ms, TIMER32_1_BASE, SYSTEMCLOCK/20);
    InitSWTimer(&OneShot5Sec, TIMER32_1_BASE, SYSTEMCLOCK*5);

    // start all the timer
    StartSWTimer(&everySec);
    StartSWTimer(&every50ms);
    StartSWTimer(&OneShot3Sec);
    StartSWTimer(&OneShot5Sec);

    glbListening = 1;

    while (1) {

        int sec   = SWTimerExpired(&everySec);
        int ms50  = SWTimerExpired(&every50ms);
        int o3s   = SWTimerExpired(&OneShot3Sec);
        int o5s  = SWTimerExpired(&OneShot5Sec);
        int b1 = ButtonS1Pressed();
        int b2 = ButtonS2Pressed();

        // listening and convert sound to power
        if (!glbListening)
              {
                 p697  = PowerGoertzel(  &t697);
                 p770  = PowerGoertzel(  &t770);
                 p852  = PowerGoertzel(  &t852);
                 p1209 = PowerGoertzel( &t1209);
                 p1336 = PowerGoertzel( &t1336);
                 p1477 = PowerGoertzel( &t1477);

                  DTMFReset();
                  glbListening = 1;

              }
       ProcessStep(b1, b2, sec, ms50, &p697, &p770, &p852, &p1209, &p1336, &p1477, o3s, o5s); // run the mother finite state machine
       // reset the power
       p697 = 0;
       p770 = 0;
       p852 = 0;
       p1209 = 0;
       p1336 = 0;
       p1477 = 0;
    }

}
