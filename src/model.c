#include "fsm.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "time.h"

#define TIEMPOAMBAR 1000
#define TIEMPOVERDE 4000

typedef struct {
    int boton1 : 1;
    int boton2 : 1;
    int espira : 1;
} flags_t;

typedef struct {
    int sc1; // Semaforo principal coches
    int sc2; // Semaforo secundario coches
    int sp1; // Semaforo principal peatones
    int sp2; // Semaforo secundario peatones
} semaforos_t;

typedef struct {
    semaforos_t* semaforos;
    long int currentTime;
} fsm_data_t;

static pthread_mutex_t mutex;
static flags_t flags;

enum states {
    V1R2,
    A1R2,
    R1V2,
    R1A2
};

/* Guard functions */
static int
check_boton1_deadline(fsm_t* this){
    fsm_data_t* p_this = (fsm_data_t*) this->user_data;
     
    pthread_mutex_lock(&mutex);
    int res = (flags.boton1 && (p_this->currentTime <= clock()*1000/CLOCKS_PER_SEC));
    pthread_mutex_unlock(&mutex);

    return res;
}

static int
check_espira_deadline(fsm_t* this){
    fsm_data_t* p_this = (fsm_data_t*) this->user_data;
     
    pthread_mutex_lock(&mutex);
    int res = ((p_this->currentTime <= clock()*1000/CLOCKS_PER_SEC) && flags.espira);
    pthread_mutex_unlock(&mutex);

    return res;
}

static int
check_boton2_notdeadline(fsm_t* this){
    fsm_data_t* p_this = (fsm_data_t*) this->user_data;
     
    pthread_mutex_lock(&mutex);
    int res = (flags.boton2 && !(p_this->currentTime <= clock()*1000/CLOCKS_PER_SEC));
    pthread_mutex_unlock(&mutex);

    return res;
}

static int
check_deadline(fsm_t* this){
    fsm_data_t* p_this = (fsm_data_t*) this->user_data;
     
    pthread_mutex_lock(&mutex);
    int res = (p_this->currentTime <= clock()*1000/CLOCKS_PER_SEC);
    pthread_mutex_unlock(&mutex);

    return res;
}

static int
check_espira_notdeadline(fsm_t* this){
    fsm_data_t* p_this = (fsm_data_t*) this->user_data;
     
    pthread_mutex_lock(&mutex);
    int res = (flags.espira && !(p_this->currentTime <= clock()*1000/CLOCKS_PER_SEC));
    pthread_mutex_unlock(&mutex);

    return res;
}

static int
check_boton1_notdeadline(fsm_t* this){
    fsm_data_t* p_this = (fsm_data_t*) this->user_data;
     
    pthread_mutex_lock(&mutex);
    int res = (flags.boton1 && !(p_this->currentTime <= clock()*1000/CLOCKS_PER_SEC));
    pthread_mutex_unlock(&mutex);

    return res;
}

/* Transition functions */
static void
trans_A1R2_boton1_deadline (fsm_t* this){
    fsm_data_t* p_this = (fsm_data_t*) this->user_data;
    semaforos_t* semaforos = p_this->semaforos;

    p_this->currentTime = clock()*1000/CLOCKS_PER_SEC + TIEMPOAMBAR;

    pthread_mutex_lock(&mutex);
    flags.boton1 = 0;
    pthread_mutex_unlock(&mutex);

    // printf("Transición a A1R2 boton1 + deadline. Time: %ld\n", clock()*1000/CLOCKS_PER_SEC);
    printf("\nSemaforos: coches principal -> ambar; coches secundaria -> rojo; peatones principal -> rojo; peatones secundaria -> verde\n\n");

    semaforos->sc1 = 1;
    semaforos->sc2 = 0;
    semaforos->sp1 = 0;
    semaforos->sp2 = 2;
}

static void
trans_A1R2_espira_deadline (fsm_t* this){
    fsm_data_t* p_this = (fsm_data_t*) this->user_data;
    semaforos_t* semaforos = p_this->semaforos;

    p_this->currentTime = clock()*1000/CLOCKS_PER_SEC + TIEMPOAMBAR;
    
    pthread_mutex_lock(&mutex);
    flags.espira = 0;
    pthread_mutex_unlock(&mutex);

    // printf("Transición a A1R2 espira + deadline. Time: %ld\n", clock()*1000/CLOCKS_PER_SEC);
    printf("\nSemaforos: coches principal -> ambar; coches secundaria -> rojo; peatones principal -> rojo; peatones secundaria -> verde\n\n");

    semaforos->sc1 = 1;
    semaforos->sc2 = 0;
    semaforos->sp1 = 0;
    semaforos->sp2 = 2;
}

static void
trans_V1R2_boton2_notdeadline (fsm_t* this){
    fsm_data_t* p_this = (fsm_data_t*) this->user_data;
    semaforos_t* semaforos = p_this->semaforos;

    p_this->currentTime = clock()*1000/CLOCKS_PER_SEC + TIEMPOVERDE;

    pthread_mutex_lock(&mutex);
    flags.boton2 = 0;
    pthread_mutex_unlock(&mutex);

    // printf("Transición a V1R2 boton2 + not deadline. Time: %ld\n", clock()*1000/CLOCKS_PER_SEC);

    semaforos->sc1 = 2;
    semaforos->sc2 = 0;
    semaforos->sp1 = 0;
    semaforos->sp2 = 2;
}

static void
trans_R1V2_deadline (fsm_t* this){
    fsm_data_t* p_this = (fsm_data_t*) this->user_data;
    semaforos_t* semaforos = p_this->semaforos;

    p_this->currentTime = clock()*1000/CLOCKS_PER_SEC + TIEMPOVERDE;

    // printf("Transición a R1V2 deadline. Time: %ld\n", clock()*1000/CLOCKS_PER_SEC);
    printf("\nSemaforos: coches principal -> rojo; coches secundaria -> verde; peatones principal -> verde; peatones secundaria -> rojo\n\n");

    semaforos->sc1 = 0;
    semaforos->sc2 = 2;
    semaforos->sp1 = 2;
    semaforos->sp2 = 0;
}

static void
trans_A1R2_boton2_notdeadline (fsm_t* this){
    fsm_data_t* p_this = (fsm_data_t*) this->user_data;
    semaforos_t* semaforos = p_this->semaforos;

    p_this->currentTime = clock()*1000/CLOCKS_PER_SEC + TIEMPOAMBAR;

    pthread_mutex_lock(&mutex);
    flags.boton2 = 0;
    pthread_mutex_unlock(&mutex);

    // printf("Transición a A1R2 boton2 + not deadline. Time: %ld\n", clock()*1000/CLOCKS_PER_SEC);

    semaforos->sc1 = 1;
    semaforos->sc2 = 0;
    semaforos->sp1 = 0;
    semaforos->sp2 = 2;
}

static void
trans_A1R2_espira_notdeadline (fsm_t* this){
    fsm_data_t* p_this = (fsm_data_t*) this->user_data;
    semaforos_t* semaforos = p_this->semaforos;

    p_this->currentTime = clock()*1000/CLOCKS_PER_SEC + TIEMPOAMBAR;

    pthread_mutex_lock(&mutex);
    flags.espira = 0;
    pthread_mutex_unlock(&mutex);

    // printf("Transición a A1R2 espira + not deadline. Time: %ld\n", clock()*1000/CLOCKS_PER_SEC);

    semaforos->sc1 = 1;
    semaforos->sc2 = 0;
    semaforos->sp1 = 0;
    semaforos->sp2 = 2;
}

static void
trans_A1R2_boton1_notdeadline (fsm_t* this){
    fsm_data_t* p_this = (fsm_data_t*) this->user_data;
    semaforos_t* semaforos = p_this->semaforos;

    p_this->currentTime = clock()*1000/CLOCKS_PER_SEC + TIEMPOAMBAR;

    pthread_mutex_lock(&mutex);
    flags.boton1 = 0;
    pthread_mutex_unlock(&mutex);

    // printf("Transición a A1R2 boton1 + not deadline. Time: %ld\n", clock()*1000/CLOCKS_PER_SEC);

    semaforos->sc1 = 1;
    semaforos->sc2 = 0;
    semaforos->sp1 = 0;
    semaforos->sp2 = 2;
}

static void
trans_R1A2_deadline (fsm_t* this){
    fsm_data_t* p_this = (fsm_data_t*) this->user_data;
    semaforos_t* semaforos = p_this->semaforos;

    p_this->currentTime = clock()*1000/CLOCKS_PER_SEC + TIEMPOAMBAR;

    // printf("Transición a R1A2 deadline. Time: %ld\n", clock()*1000/CLOCKS_PER_SEC);
    printf("\nSemaforos: coches principal -> rojo; coches secundaria -> ambar; peatones principal -> verde; peatones secundaria -> rojo\n\n");

    semaforos->sc1 = 0;
    semaforos->sc2 = 1;
    semaforos->sp1 = 2;
    semaforos->sp2 = 0;
}

static void
trans_R1V2_espira_notdeadline (fsm_t* this){
    fsm_data_t* p_this = (fsm_data_t*) this->user_data;
    semaforos_t* semaforos = p_this->semaforos;

    p_this->currentTime = clock()*1000/CLOCKS_PER_SEC + TIEMPOVERDE;

    pthread_mutex_lock(&mutex);
    flags.espira = 0;
    pthread_mutex_unlock(&mutex);

    // printf("Transición a R1V2 espira + not deadline. Time: %ld\n", clock()*1000/CLOCKS_PER_SEC);

    semaforos->sc1 = 0;
    semaforos->sc2 = 2;
    semaforos->sp1 = 2;
    semaforos->sp2 = 0;
}

static void
trans_R1V2_boton1_notdeadline (fsm_t* this){
    fsm_data_t* p_this = (fsm_data_t*) this->user_data;
    semaforos_t* semaforos = p_this->semaforos;

    p_this->currentTime = clock()*1000/CLOCKS_PER_SEC + TIEMPOVERDE;

    pthread_mutex_lock(&mutex);
    flags.boton1 = 0;
    pthread_mutex_unlock(&mutex);

    // printf("Transición a R1V2 boton1 + not deadline. Time: %ld\n", clock()*1000/CLOCKS_PER_SEC);

    semaforos->sc1 = 0;
    semaforos->sc2 = 2;
    semaforos->sp1 = 2;
    semaforos->sp2 = 0;
}

static void
trans_V1R2_deadline (fsm_t* this){
    fsm_data_t* p_this = (fsm_data_t*) this->user_data;
    semaforos_t* semaforos = p_this->semaforos;

    p_this->currentTime = clock()*1000/CLOCKS_PER_SEC + TIEMPOVERDE;

    // printf("Transición a V1R2 deadline. Time: %ld\n", clock()*1000/CLOCKS_PER_SEC);
    printf("\nSemaforos: coches principal -> verde; coches secundaria -> rojo; peatones principal -> rojo; peatones secundaria -> verde\n\n");

    semaforos->sc1 = 2;
    semaforos->sc2 = 0;
    semaforos->sp1 = 0;
    semaforos->sp2 = 2;
}

static void
trans_R1A2_boton2_notdeadline (fsm_t* this){
    fsm_data_t* p_this = (fsm_data_t*) this->user_data;
    semaforos_t* semaforos = p_this->semaforos;

    p_this->currentTime = clock()*1000/CLOCKS_PER_SEC + TIEMPOAMBAR;

    pthread_mutex_lock(&mutex);
    flags.boton2 = 0;
    pthread_mutex_unlock(&mutex);

    // printf("Transición a R1A2 boton2 + not deadline. Time: %ld\n", clock()*1000/CLOCKS_PER_SEC);

    semaforos->sc1 = 0;
    semaforos->sc2 = 1;
    semaforos->sp1 = 2;
    semaforos->sp2 = 0;
}

static void
trans_R1A2_boton1_notdeadline (fsm_t* this){
    fsm_data_t* p_this = (fsm_data_t*) this->user_data;
    semaforos_t* semaforos = p_this->semaforos;

    p_this->currentTime = clock()*1000/CLOCKS_PER_SEC + TIEMPOAMBAR;

    pthread_mutex_lock(&mutex);
    flags.boton1 = 0;
    pthread_mutex_unlock(&mutex);

    // printf("Transición a R1A2 boton1 + not deadline. Time: %ld\n", clock()*1000/CLOCKS_PER_SEC);

    semaforos->sc1 = 0;
    semaforos->sc2 = 1;
    semaforos->sp1 = 2;
    semaforos->sp2 = 0;
}

static void
trans_R1A2_espira_notdeadline (fsm_t* this){
    fsm_data_t* p_this = (fsm_data_t*) this->user_data;
    semaforos_t* semaforos = p_this->semaforos;

    p_this->currentTime = clock()*1000/CLOCKS_PER_SEC + TIEMPOAMBAR;

    pthread_mutex_lock(&mutex);
    flags.espira = 0;
    pthread_mutex_unlock(&mutex);

    // printf("Transición a R1A2 espira + not deadline. Time: %ld\n", clock()*1000/CLOCKS_PER_SEC);

    semaforos->sc1 = 0;
    semaforos->sc2 = 1;
    semaforos->sp1 = 2;
    semaforos->sp2 = 0;
}

/* Generate new fsm */

fsm_t* fsm_new_cruceFSM ()
{
	static fsm_trans_t cruceFSM_tt[] = {

		{ V1R2, check_boton1_deadline, A1R2, trans_A1R2_boton1_deadline},
        { V1R2, check_espira_deadline, A1R2, trans_A1R2_espira_deadline},
        { V1R2, check_boton2_notdeadline, V1R2, trans_V1R2_boton2_notdeadline},
        { A1R2, check_deadline, R1V2, trans_R1V2_deadline},
        { A1R2, check_boton2_notdeadline, A1R2, trans_A1R2_boton2_notdeadline},
        { A1R2, check_espira_notdeadline, A1R2, trans_A1R2_espira_notdeadline},
        { A1R2, check_boton1_notdeadline, A1R2, trans_A1R2_boton1_notdeadline},
        { R1V2, check_deadline, R1A2, trans_R1A2_deadline},
        { R1V2, check_espira_notdeadline, R1V2, trans_R1V2_espira_notdeadline},
        { R1V2, check_boton1_notdeadline, R1V2, trans_R1V2_boton1_notdeadline},
        { R1A2, check_deadline, V1R2, trans_V1R2_deadline},
        { R1A2, check_boton2_notdeadline, R1A2, trans_R1A2_boton2_notdeadline},
        { R1A2, check_boton1_notdeadline, R1A2, trans_R1A2_boton1_notdeadline},
        { R1A2, check_espira_notdeadline, R1A2, trans_R1A2_espira_notdeadline},
		{ -1, NULL, -1, NULL},
	};

    pthread_mutex_init(&mutex, NULL);

    semaforos_t* semaforos = malloc(sizeof(flags_t));

    long int actTime = clock()*1000/CLOCKS_PER_SEC + TIEMPOVERDE;

    fsm_data_t* fsm_data = malloc(sizeof(fsm_data_t));
    fsm_data->currentTime = actTime;
    fsm_data->semaforos = semaforos;

    printf("\nSemaforos: coches principal -> verde; coches secundaria -> rojo; peatones principal -> rojo; peatones secundaria -> verde\n\n");

	return fsm_new (V1R2, cruceFSM_tt, fsm_data);
}

/* Inputs ISR */
void
boton1_ISR(void){
    pthread_mutex_lock(&mutex);
    flags.boton1 = 1;
    pthread_mutex_unlock(&mutex);

    printf("Se ha pulsado el boton de la carretera principal\n");    
}

void
boton2_ISR(void){
    pthread_mutex_lock(&mutex);
    flags.boton2 = 1;
    pthread_mutex_unlock(&mutex);  

    printf("Se ha pulsado el boton de la carretera secundaria\n");   
}

void
espira_ISR(void){
    pthread_mutex_lock(&mutex);
    flags.espira = 1;
    pthread_mutex_unlock(&mutex);

    // printf("Se ha activado la espira\n");     
    printf("Hay un coche en la carretera secundaria\n");
}