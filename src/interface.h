#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sem182.h>

#define PERMISSION (0600)
#define PATHNAME "."
#define MEMID 'a'
#define SE1ID 'b'
#define SE2ID 'c'
#define SE3ID 'd'

#define ST_OK 0
#define ST_WAIT 1
#define ST_WON 2
#define ST_LOST 3
#define ST_NOSUCHGAME 4
#define ST_NOSUCHCMD 5
#define ST_ERROR -1
#define CMD_OK 100
#define CMD_NEW 101
#define CMD_QUIT 102
#define CMD_LEFT 103
#define CMD_RIGHT 104
#define CMD_UP 105
#define CMD_DOWN 106
#define CMD_ERROR -101

struct shm_game {
	int state;
	int id;
	int values[16];
};
