/**
 * @file send_receive.c
 * @author Alexander Schöberl 0942320
 * @date 27.05.2014
 *
 * @brief Communication with the Client.
 */

#include <stdio.h>
#include "interface.h"

/* Functions */
void bail_out_server(char *message);

void set_command(int tmp_id, int tmp_command);

/* Global Variables */
struct shm_game *shared = NULL;
int shm_id = 0;
int sem_cl_id = 0;
int sem_ser_id = 0;
int sem_ans_id = 0;
int processing = 0;

/* Implementations */
void close_con(void) {
	if (shmdt(shared) == -1)
		(void) fprintf(stderr, "WARNING: 2048-server shmdt not possible\n");
	if (shmctl(shm_id, IPC_RMID, 0) == -1)
		(void) fprintf(stderr, "WARNING: 2048-server shmctl RMID not possible\n");
	shm_id = 0;
	shared = NULL;
	if (semrm(sem_cl_id) == -1)
		(void) fprintf(stderr, "WARNING: 2048-server semrm client not possible\n");
	sem_cl_id = 0;
	if (semrm(sem_ser_id) == -1)
		(void) fprintf(stderr, "WARNING: 2048-server semrm server not possible\n");
	sem_ser_id = 0;
	if (semrm(sem_ans_id) == -1)
		(void) fprintf(stderr, "WARNING: 2048-server semrm answer not possible\n");
	sem_ans_id = 0;
}

/**
 * @brief A pitch will be written into pitch using the given values. 
 * @param pitch: (WIDTH*4-3)*9+9 minimum length, must contain only '\0' chars
 * @param values: 16 ints, 0 and negative values become empty cells, WIDTH-2 maximum chars to write each value
 */
void connect(void) {
	if (shared != NULL)
		return;
	
	key_t shm_key;
	shm_key = ftok(PATHNAME, MEMID);
	if (shm_key == -1)
		bail_out_server("ftok Error");
	shm_id = shmget(shm_key, sizeof(*shared), IPC_CREAT | IPC_EXCL | PERMISSION);
	if (shm_id == -1) {
		close_con();
		bail_out_server("shmget Error, Server already running?");
	}
	shared = shmat(shm_id, NULL, 0);
	if (shared == (void *)-1) {
		close_con();
		bail_out_server("shmat Error");
	}
	
	key_t sem_cl_key;
	sem_cl_key = ftok(PATHNAME, SE2ID);
	if (sem_cl_key == -1)
		bail_out_server("ftok Client Error");
	sem_cl_id = seminit(sem_cl_key, PERMISSION, 1);
	if (sem_cl_id == -1) {
		close_con();
		bail_out_server("seminit Client Error");
	}
	
	key_t sem_ser_key;
	sem_ser_key = ftok(PATHNAME, SE1ID);
	if (sem_ser_key == -1)
		bail_out_server("ftok Server Error");
	sem_ser_id = seminit(sem_ser_key, PERMISSION, 0);
	if (sem_ser_id == -1) {
		close_con();
		bail_out_server("seminit Server Error");
	}
	key_t sem_ans_key;
	sem_ans_key = ftok(PATHNAME, SE3ID);
	if (sem_ans_key == -1)
		bail_out_server("ftok Answer Error");
	sem_ans_id = seminit(sem_ans_key, PERMISSION, 0);
	if (sem_ans_id == -1) {
		close_con();
		bail_out_server("seminit Answer Error");
	}
}

int read_cmd(void) {
	processing = 0;
	connect();
	if (shared == NULL)
		return -1;
	P(sem_ser_id);
	
	if (shared->state < CMD_OK) {
		shared->state = ST_NOSUCHCMD;
		V(sem_ans_id);
		return -1;
	}
	processing = 1;
	set_command(shared->id, shared->state);
	return shared->id;
}

int write_st(int tmp_state, int tmp_id, int tmp_values[]) {
	if (shared == NULL)
		return -1;
	if (processing != 1)
		return -1;

	shared->state = tmp_state;
	shared->id = tmp_id;
	
	int i;
	for (i = 0; i < 16; i++) {
		shared->values[i] = tmp_values[i];
	}	
	V(sem_ans_id);
	processing = 0;
	return 0;
}
