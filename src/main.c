#include "fsm.h"
#include "time.h"
#include <stdlib.h>
#include <stdio.h>

#define MAX_TIME_REFRESH_VALUES 5000
#define FSM_FIRE_RATE 500

/* Inputs ISR */
void boton1_ISR(void);
void boton2_ISR(void);
void espira_ISR(void);
void deadline_ISR(void);

void newValues(void);
/* FSM */
fsm_t* fsm_new_cruceFSM ();

int runFSM;

int
main(void){
    fsm_t* cruceFSM = fsm_new_cruceFSM();

    long int lastTimeNew = clock()*1000/CLOCKS_PER_SEC;
    long int lastTimeFire = clock()*1000/CLOCKS_PER_SEC;
    long int actTime;

    srand(clock());

    while(1){
        actTime = clock()*1000/CLOCKS_PER_SEC;
        if(actTime-lastTimeNew >= (rand()%4 + 1)*MAX_TIME_REFRESH_VALUES/4){
            newValues();
            lastTimeNew = actTime;
        }

        if(actTime-lastTimeFire >= FSM_FIRE_RATE){
            fsm_fire(cruceFSM);
            lastTimeFire = actTime;
        }
    }

    return 0;
}

void
newValues(void){
    int num = rand() % 4;
    switch (num)
    {
    case 0:
        break;

    case 1:
        boton1_ISR();
        break;
    
    case 2:
        boton2_ISR();
        break;

    case 3:
        espira_ISR();
        break;

    default:
        break;
    }
}