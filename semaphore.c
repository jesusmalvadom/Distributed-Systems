#include "semaphore.h"

/*************************************************************** 
Nombre:
    initialize_Semaphore.
Descripcion:
    Inicializa los semaforos indicados.
Entrada:
    int semid: Identificador del semaforo.
    unsigned short *array: Valores iniciales.
Salida:
    int: OK si todo fue correcto, 
         ERROR en caso de error. 
************************************************************/
int initialize_Semaphore(int semid, unsigned short *array){
    /* Para realizar operaciones de up y down sobre semaforos */
    union semun {
        int val; 
        struct semid_ds *semstat;
        unsigned short *array;
    } vals;

    int tamano = sizeof(array) / sizeof(array[0]);
    vals.array = array;
    if (semctl(semid, tamano, SETALL, vals) == -1) {
        return ERROR;
    }
    return OK;
}

/*************************************************************** 
Nombre: 
    erase_Semaphore.
Descripcion: 
    Borra un semaforo.
Entrada:
    int semid: Identificador del semaforo.
Salida:
    int: OK si todo fue correcto, 
         ERROR en caso de error. 
***************************************************************/
int erase_Semaphore(int semid) {
    if (semctl(semid, 0, IPC_RMID) == -1) {
        return ERROR;
    }
    return OK;
}

/*************************************************************** 
Nombre: 
    create_Semaphore.
Descripcion: 
    Crea un semaforo con la clave y el tamano especificado. Lo inicializa a 0.
Entrada:
    key_t key: Clave precompartida del semaforo.
    int size: Tamano del semaforo.
Salida:
    int *semid: Identificador del semaforo.
    int: ERROR en caso de error,
         0 si ha creado el semaforo,
         1 si ya estaba creado. 
**************************************************************/
int create_Semaphore(key_t key, int size, int *semid){
    if ((*semid = semget(key, 0, 0)) == -1) { 
    /* El semaforo no existe asi que debemos crearlo */
        *semid = semget(key, size, IPC_CREAT | 0600);

        if ((*semid == -1) && (errno == EEXIST)) {
            *semid = semget(key, size, SHM_R | SHM_W);
        }

        if ((semget(key, 0, 0) == -1) || (*semid == -1)) {
            return ERROR;
        }
    } else {
        /* El semaforo ya existia */
        return 1;
    }

    return OK;
}

/************************************************************** 
Nombre:
    down_Semaphore
Descripcion:
    Baja el semaforo indicado
Entrada:
    int semid: Identificador del semaforo.
    int num_sem: Semaforo dentro del array.
    int undo: Flag de modo persistente pese a finalizacion abrupta.
Salida:
    int: OK si todo fue correcto, 
         ERROR en caso de error.
***************************************************************/ 
int down_Semaphore(int id, int num_sem, int undo) {
    struct sembuf sem_oper;

    sem_oper.sem_num = num_sem;
    sem_oper.sem_op = -1; /* Para decrementar en 1 */ 
    sem_oper.sem_flg = undo;

    if (semop(id, &sem_oper, 1) == -1){
        return ERROR;
    }
    return OK;
}

/***************************************************************
Nombre: 
    downMultiple_Semaphore
Descripcion:    
    Baja todos los semaforos del array indicado por active.
Entrada:
    int semid: Identificador del semaforo.
    int size: Numero de semaforos del array.
    int undo: Flag de modo persistente pese a finalización abrupta.
    int *active: Semaforos involucrados.
Salida:
    int: OK si todo fue correcto, 
         ERROR en caso de error. 
***************************************************************/
int downMultiple_Semaphore(int id, int size, int undo, int *active) {
    int i, j;
    for (i = 0; i < size; i++){
        for(j = 0; j < size; j++){
            if (i == active[j]) {
                if (down_Semaphore(id, i, undo) == -1) {
                    return ERROR;
                }
            }
        }
    }

    return OK;
}

/************************************************************** 
Nombre:
    Up_Semaphore
Descripcion: 
    Sube el semaforo indicado
Entrada:
    int semid: Identificador del semaforo.
    int num_sem: Semaforo dentro del array.
    int undo: Flag de modo persistente pese a finalizacion abupta.
Salida:
    int: OK si todo fue correcto, 
         ERROR en caso de error.
***************************************************************/ 
int up_Semaphore(int id, int num_sem, int undo) {
    struct sembuf sem_oper;

    sem_oper.sem_num = num_sem;
    sem_oper.sem_op = 1; /* Para aumentar en 1 */ 
    sem_oper.sem_flg = /*(short)*/ undo;
    if (semop(id, &sem_oper, 1) == -1){
        return ERROR;
    }
    return OK;
}

/*************************************************************** 
Nombre: 
    UpMultiple_Semaphore
Descripcion: 
    Sube todos los semaforos del array indicado por active.
Entrada:
    int semid: Identificador del semaforo.
    int size: Numero de semaforos del array.
    int undo: Flag de modo persistente pese a finalizacion abrupta.
    int *active: Semaforos involucrados.
Salida:
    int: OK si todo fue correcto, 
         ERROR en caso de error.
***************************************************************/ 
int upMultiple_Semaphore(int id, int size, int undo, int *active) {
    int i, j;

    for (i = 0; i < size; i++){
        for(j = 0; j < size; j++){
            if (i == active[j]) {
                if (up_Semaphore(id, i, undo) == -1) {
                    return ERROR;
                }
            }
        }
    }

    return OK;
}
