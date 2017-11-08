/**
* @brief Funciones basicas para el manejo de semaforos
*
* Este modulo contiene los prototipos de las funciones que tratan con los 
* semaforos, incluyendo su creacion, borrado, inicializacion y modificacion.
* 
* @file semaforos.h
* @author Jesus Miguel Alvarez Dominguez, jesusm.alvarez@estudiante.uam.es
* @author Miguel Fernandez de Alarcon Gervas, miguel.fernandezdealarcon@estudiante.uam.es
* @date 14-04-2016
*/

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define ERROR -1
#define OK 0


/**
 * @brief Inicializa los semaforos indicados
 *
 * @param semid entero identificador del semaforo.
 * @param array lista de valores iniciales para los semaforos.
 * @return OK si todo fue correcto, ERROR en caso de error.
 */
int initialize_Semaphore(int semid, unsigned short *array);


/**
 * @brief Borra un semaforo.
 *
 * @param semid entero identificador del semaforo.
 * @return OK si todo fue correcto, ERROR en caso de error.
 */
int erase_Semaphore(int semid);


/**
 * @brief Crea un semaforo con la clave y el tamano especificado. Lo inicializa a 0.
 *
 * Modifica los valores del parametro semid con el identificador del semaforo.
 * 
 * @param key Clave precompartida del semaforo.
 * @param size Tamano del semaforo.
 * @return 0 si el semaforo se ha creado, 1 si ya existia y ERROR en caso de error.
 */
int create_Semaphore(key_t key, int size, int *semid);


/**
 * @brief Baja el semaforo indicado
 *
 * @param semid entero identificador del semaforo.
 * @param num_sem Semaforo dentro del array.
 * @param undo Flag de modo persistente pese a finalizacion abrupta,
 * SEM_UNDO para activarlo y 0 para que no se reestablezca el valor en caso de 
 * que se termine abruptamente.
 * @return OK si todo fue correcto, ERROR en caso de error.
 */
int down_Semaphore(int id, int num_sem, int undo);


/**
 * @brief Baja todos los semaforos del array indicado por active.
 *
 * @param semid entero identificador del semaforo.
 * @param num_sem Semaforo dentro del array.
 * @param undo Flag de modo persistente pese a finalizacion abrupta,
 * SEM_UNDO para activarlo y 0 para que no se reestablezca el valor en caso de 
 * que se termine abruptamente.
 * @param active Lista con los semaforos involucrados.
 * @return OK si todo fue correcto, ERROR en caso de error.
 */
int downMultiple_Semaphore(int id, int size, int undo, int *active);

/**
 * @brief Sube el semaforo indicado
 *
 * @param semid entero identificador del semaforo.
 * @param num_sem Semaforo dentro del array.
 * @param undo Flag de modo persistente pese a finalizacion abrupta,
 * SEM_UNDO para activarlo y 0 para que no se reestablezca el valor en caso de 
 * que se termine abruptamente.
 * @return OK si todo fue correcto, ERROR en caso de error.
 */
int up_Semaphore(int id, int num_sem, int undo);

/**
 * @brief Sube todos los semaforos del array indicado por active.
 *
 * @param semid entero identificador del semaforo.
 * @param num_sem Semaforo dentro del array.
 * @param undo Flag de modo persistente pese a finalizacion abrupta,
 * SEM_UNDO para activarlo y 0 para que no se reestablezca el valor en caso de 
 * que se termine abruptamente.
 * @param active Lista con los semaforos involucrados.
 * @return OK si todo fue correcto, ERROR en caso de error.
 */
int upMultiple_Semaphore(int id, int size, int undo, int *active);


#endif /* SEMAPHORE_H */
