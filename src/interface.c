/**
 * @file interface.c
 * @author Alexander Schöberl 0942320
 * @date 27.05.2014
 *
 * @brief Communication with the Server.
 */

#include <stdio.h>
#include <string.h>
#include "interface.h"

/* Functions */
void bail_out_client(char *message);

void set_game_id(int tmp_game_id);
void set_state(int tmp_state);
void set_values(int *tmp_values);

/* Global Variables */
struct shm_game *shared = NULL;
int shm_id = 0;
int sem_cl_id = 0;
int sem_ser_id = 0;
int sem_ans_id = 0;

/* Implementations */
void close_con(void) {
	if (shmdt(shared) == -1)
		(void) fprintf(stderr, "WARNING: 2048-client shmdt not possible\n");
	shm_id = 0;
	shared = NULL;
	sem_cl_id = 0;
	sem_ser_id = 0;
	sem_ans_id = 0;
}

/**
 * @brief Establish connection. 
 * @param cell: WIDTH+1 minimum length, must contain only '\0' chars
 * @param value: WIDTH-2 maximum chars to write this value, 0 and negative values become empty cells
 */
void connect(void) {
	if (shared != NULL)
		return;
	
	key_t shm_key;
	shm_key = ftok(PATHNAME, MEMID);
	if (shm_key == -1)
		bail_out_client("ftok Error");
	shm_id = shmget(shm_key, sizeof(*shared), PERMISSION);
	if (shm_id == -1) {
		close_con();
		bail_out_client("shmget Error");
	}
	shared = shmat(shm_id, NULL, 0);
	if (shared == (void *)-1) {
		close_con();
		bail_out_client("shmat Error");
	}
	
	key_t sem_cl_key;
	sem_cl_key = ftok(PATHNAME, SE2ID);
	if (sem_cl_key == -1)
		bail_out_client("ftok Client Error");
	sem_cl_id = semgrab(sem_cl_key);
	if (sem_cl_id == -1) {
		close_con();
		bail_out_client("semgrab Client Error");
	}
	
	key_t sem_ser_key;
	sem_ser_key = ftok(PATHNAME, SE1ID);
	if (sem_ser_key == -1)
		bail_out_client("ftok Server Error");
	sem_ser_id = semgrab(sem_ser_key);
	if (sem_ser_id == -1) {
		close_con();
		bail_out_client("semgrab Server Error");
	}
	key_t sem_ans_key;
	sem_ans_key = ftok(PATHNAME, SE3ID);
	if (sem_ans_key == -1)
		bail_out_client("ftok Answer Error");
	sem_ans_id = semgrab(sem_ans_key);
	if (sem_ans_id == -1) {
		close_con();
		bail_out_client("semgrab Answer Error");
	}
}

int create_game(void) {
	connect();
	if (shared == NULL)
		return -1;
	P(sem_cl_id);
	
	if (shared->id != 0 || shared->state != ST_OK) {
		V(sem_cl_id);
		return -1;
	}
	shared->state = CMD_NEW;
	shared->id = 0;
	memset(shared->values, 0, sizeof(shared->values));
	V(sem_ser_id);
	P(sem_ans_id);
	
	if (shared->state != ST_WAIT || shared->id <= 0) {
		V(sem_cl_id);
		return -1;
	}
	int id = shared->id;
	set_game_id(id);
	set_state(shared->state);
	set_values(shared->values);
	
	shared->state = ST_OK;
	shared->id = 0;
	memset(shared->values, 0, sizeof(shared->values));
	V(sem_cl_id);
	
	return id;
}

int load_game(int game_id) {
	connect();
	if (shared == NULL)
		return -1;
	P(sem_cl_id);
	
	if (shared->id != 0 || shared->state != ST_OK) {
		V(sem_cl_id);
		return -1;
	}
	shared->state = CMD_OK;
	shared->id = game_id;
	memset(shared->values, 0, sizeof(shared->values));
	V(sem_ser_id);
	P(sem_ans_id);
	
	if (shared->state != ST_WAIT || shared->id != game_id) {
		V(sem_cl_id);
		return -1;
	}
	int id = shared->id;
	set_game_id(id);
	set_state(shared->state);
	set_values(shared->values);
	
	shared->state = ST_OK;
	shared->id = 0;
	memset(shared->values, 0, sizeof(shared->values));
	V(sem_cl_id);
	
	return id;
}

int end_game(int game_id) {
	connect();
	if (shared == NULL)
		return -1;
	P(sem_cl_id);
	
	if (shared->id != 0 || shared->state != ST_OK) {
		V(sem_cl_id);
		return -1;
	}
	shared->state = CMD_QUIT;
	shared->id = game_id;
	memset(shared->values, 0, sizeof(shared->values));
	V(sem_ser_id);
	P(sem_ans_id);
	
	if (shared->state != ST_LOST || shared->id != game_id) {
		V(sem_cl_id);
		return -1;
	}
	int id = shared->id;
	
	shared->state = ST_OK;
	shared->id = 0;
	memset(shared->values, 0, sizeof(shared->values));
	V(sem_cl_id);
	
	close_con();
	return id;
}

int send_cmd(int game_id, int command) {
	if (command < CMD_LEFT || command > CMD_DOWN)
		return -1;
	connect();
	if (shared == NULL)
		return -1;
	P(sem_cl_id);
	
	if (shared->id != 0 || shared->state != ST_OK) {
		V(sem_cl_id);
		return -1;
	}
	shared->state = command;
	shared->id = game_id;
	
	memset(shared->values, 0, sizeof(shared->values));
	V(sem_ser_id);
	P(sem_ans_id);
	
	if ((shared->state != ST_WAIT && shared->state != ST_WON && shared->state != ST_LOST) || shared->id != game_id) {
		V(sem_cl_id);
		return -1;
	}
	int id = shared->id;
	set_game_id(shared->id);
	set_state(shared->state);
	set_values(shared->values);
	
	shared->state = ST_OK;
	shared->id = 0;
	memset(shared->values, 0, sizeof(shared->values));
	V(sem_cl_id);
	
	return id;
}
