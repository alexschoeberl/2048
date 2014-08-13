/**
 * @file 2048-server.c
 * @author Alexander Schöberl 0942320
 * @date 27.05.2014
 *
 * @brief Server entry point.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include "interface.h"

/* Functions */
void usage_server(void);
void bail_out_server(char *message);

int read_cmd(void);
int write_st(int state, int id, int *values);
void close_con(void);

void create_pitch(int *values);
int game_over(int *values, int max);
int move_left(int *values);
int move_right(int *values);
int move_up(int *values);
int move_down(int *values);

/* Global Variables */
volatile sig_atomic_t want_quit = 0;

int max = 2048;
struct game {
	int id;
	int values[16];
	int state;
};
struct game* games = NULL;
int games_cntr = 0;

int sent_command = ST_OK;
int sent_game_id = 0;

/* Implementations */
void set_command(int tmp_id, int tmp_command) {
	sent_game_id = tmp_id;
	sent_command = tmp_command;
}

/**
 * @brief A game cell will be written into cell. 
 * @param cell: WIDTH+1 minimum length, must contain only '\0' chars
 * @param value: WIDTH-2 maximum chars to write this value, 0 and negative values become empty cells
 */
void add_game() {
	struct game new;
	new.id = 100+games_cntr+1;
	memset(new.values, 0, sizeof(int[16]));
	create_pitch(new.values);
	new.state = 0;
	
	games_cntr++;
	games[games_cntr-1] = new;
	games = realloc(games, (games_cntr+1)*sizeof(struct game));
	
	sent_game_id = new.id;
	sent_command = ST_OK;
}

/**
 * @brief A pitch will be written into pitch using the given values. 
 * @param pitch: (WIDTH*4-3)*9+9 minimum length, must contain only '\0' chars
 * @param values: 16 ints, 0 and negative values become empty cells, WIDTH-2 maximum chars to write each value
 */
void manage_games() {
	games = calloc(1, sizeof(struct game));
	
	(void) fprintf(stdout, "Server started correctly.\n");
	(void) fprintf(stdout, "Shutdown Server with CTRL + C.\n");
	(void) fprintf(stdout, "Waiting for Client Connection.\n");
	while (!want_quit) {
		int r;
		int state = 0;
		if (read_cmd() < 0)
			bail_out_server("Client return Error");
		switch (sent_command) {
			case CMD_OK:
				if (sent_game_id > 100 && sent_game_id <= 100+games_cntr && games[sent_game_id-101].state == 0) {
					r = write_st(ST_WAIT, sent_game_id, games[sent_game_id-101].values);
					(void) fprintf(stdout, "Load Game %d: %d\n", sent_game_id, r);
				} else {
					int zeros[16] = {0};
					r = write_st(ST_NOSUCHGAME, sent_game_id, zeros);
					(void) fprintf(stdout, "Load No Such Game %d: %d\n", sent_game_id, r);
				}
				sent_game_id = 0;
				sent_command = ST_OK;
				break;
			case CMD_NEW:
				add_game();
				r = write_st(ST_WAIT, games[games_cntr-1].id, games[games_cntr-1].values);
				(void) fprintf(stdout, "New Game %d: %d\n", games[games_cntr-1].id, r);
				sent_game_id = 0;
				sent_command = ST_OK;
				break;
			case CMD_QUIT:
				if (sent_game_id > 100 && sent_game_id <= 100+games_cntr && games[sent_game_id-101].state == 0) {
						games[sent_game_id-101].state = -1;
						r = write_st(ST_LOST, sent_game_id, games[sent_game_id-101].values);
						(void) fprintf(stdout, "Game %d Lost: %d\n", sent_game_id, r);
				} else {
					int zeros[16] = {0};
					r = write_st(ST_NOSUCHGAME, sent_game_id, zeros);
					(void) fprintf(stdout, "No Such Game %d Lost: %d\n", sent_game_id, r);
				}
				sent_game_id = 0;
				sent_command = ST_OK;
				break;
			case CMD_LEFT:
				state = 0;
				if (sent_game_id > 100 && sent_game_id <= 100+games_cntr) {
					if (games[sent_game_id-101].state == 0) {
						move_left(games[sent_game_id-101].values);
						state = game_over(games[sent_game_id-101].values, max);
						if (state == 0) {
							r = write_st(ST_WAIT, sent_game_id, games[sent_game_id-101].values);
							(void) fprintf(stdout, "Game %d Left: %d\n", sent_game_id, r);
						} else if (state == 1) {
							games[sent_game_id-101].state = 1;
							r = write_st(ST_WON, sent_game_id, games[sent_game_id-101].values);
							(void) fprintf(stdout, "Game %d Won: %d\n", sent_game_id, r);
						} else {
							games[sent_game_id-101].state = -1;
							r = write_st(ST_LOST, sent_game_id, games[sent_game_id-101].values);
							(void) fprintf(stdout, "Game %d Lost: %d\n", sent_game_id, r);
						}
					}
				} else {
					int zeros[16] = {0};
					r = write_st(ST_NOSUCHGAME, sent_game_id, zeros);
					(void) fprintf(stdout, "No Such Game %d Left: %d\n", sent_game_id, r);
				}
				sent_game_id = 0;
				sent_command = ST_OK;
				break;
			case CMD_RIGHT:
				state = 0;
				if (sent_game_id > 100 && sent_game_id <= 100+games_cntr) {
					if (games[sent_game_id-101].state == 0) {
						move_right(games[sent_game_id-101].values);
						state = game_over(games[sent_game_id-101].values, max);
						if (state == 0) {
							r = write_st(ST_WAIT, sent_game_id, games[sent_game_id-101].values);
							(void) fprintf(stdout, "Game %d Right: %d\n", sent_game_id, r);
						} else if (state == 1) {
							games[sent_game_id-101].state = 1;
							r = write_st(ST_WON, sent_game_id, games[sent_game_id-101].values);
							(void) fprintf(stdout, "Game %d Won: %d\n", sent_game_id, r);
						} else {
							games[sent_game_id-101].state = -1;
							r = write_st(ST_LOST, sent_game_id, games[sent_game_id-101].values);
							(void) fprintf(stdout, "Game %d Lost: %d\n", sent_game_id, r);
						}
					}
				} else {
					int zeros[16] = {0};
					r = write_st(ST_NOSUCHGAME, sent_game_id, zeros);
					(void) fprintf(stdout, "No Such Game %d Right: %d\n", sent_game_id, r);
				}
				sent_game_id = 0;
				sent_command = ST_OK;
				break;
			case CMD_UP:
				state = 0;
				if (sent_game_id > 100 && sent_game_id <= 100+games_cntr) {
					if (games[sent_game_id-101].state == 0) {
						move_up(games[sent_game_id-101].values);
						state = game_over(games[sent_game_id-101].values, max);
						if (state == 0) {
							r = write_st(ST_WAIT, sent_game_id, games[sent_game_id-101].values);
							(void) fprintf(stdout, "Game %d Up: %d\n", sent_game_id, r);
						} else if (state == 1) {
							games[sent_game_id-101].state = 1;
							r = write_st(ST_WON, sent_game_id, games[sent_game_id-101].values);
							(void) fprintf(stdout, "Game %d Won: %d\n", sent_game_id, r);
						} else {
							games[sent_game_id-101].state = -1;
							r = write_st(ST_LOST, sent_game_id, games[sent_game_id-101].values);
							(void) fprintf(stdout, "Game %d Lost: %d\n", sent_game_id, r);
						}
					}
				} else {
					int zeros[16] = {0};
					r = write_st(ST_NOSUCHGAME, sent_game_id, zeros);
					(void) fprintf(stdout, "No Such Game %d Up: %d\n", sent_game_id, r);
				}
				sent_game_id = 0;
				sent_command = ST_OK;
				break;
			case CMD_DOWN:
				state = 0;
				if (sent_game_id > 100 && sent_game_id <= 100+games_cntr) {
					if (games[sent_game_id-101].state == 0) {
						move_down(games[sent_game_id-101].values);
						state = game_over(games[sent_game_id-101].values, max);
						if (state == 0) {
							r = write_st(ST_WAIT, sent_game_id, games[sent_game_id-101].values);
							(void) fprintf(stdout, "Game %d Down: %d\n", sent_game_id, r);
						} else if (state == 1) {
							games[sent_game_id-101].state = 1;
							r = write_st(ST_WON, sent_game_id, games[sent_game_id-101].values);
							(void) fprintf(stdout, "Game %d Won: %d\n", sent_game_id, r);
						} else {
							games[sent_game_id-101].state = -1;
							r = write_st(ST_LOST, sent_game_id, games[sent_game_id-101].values);
							(void) fprintf(stdout, "Game %d Lost: %d\n", sent_game_id, r);
						}
					}
				} else {
					int zeros[16] = {0};
					r = write_st(ST_NOSUCHGAME, sent_game_id, zeros);
					(void) fprintf(stdout, "No Such Game %d Down: %d\n", sent_game_id, r);
				}
				sent_game_id = 0;
				sent_command = ST_OK;
				break;
			default: {
				int zeros[16] = {0};
				r = write_st(ST_NOSUCHCMD, sent_game_id, zeros);
				(void) fprintf(stdout, "No Such Command %d: %d\n", sent_game_id, r);
				sent_game_id = 0;
				sent_command = ST_OK;
				break;
			}
		}
	}
}

void signal_handler(int sig) {
	want_quit = 1;
	close_con();
	(void) fprintf(stdout, "Signal Termination\n");
	exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
	char *max_str;
	int p_cntr = 0;
	char c;
	while ((c = getopt(argc, argv, "p:")) != EOF) {
		switch (c) {
			case 'p':
				if (p_cntr != 0)
					usage_server();
				p_cntr++;
				max_str = optarg;
				break;
			case '?':
				usage_server();
				break;
			default :
				usage_server();
				break;
		}
	}
	
	if (p_cntr == 1) {
		int i = (int)strtol(max_str, NULL, 10);
		if (i <= 0 || i > 23)
			usage_server();
		else
			max = (int) pow(2.0, (double)i);
	}
	if (signal(SIGINT, signal_handler) == SIG_ERR || signal(SIGTERM, signal_handler) == SIG_ERR)
		bail_out_server("Signal Error");
	manage_games();
	exit(EXIT_FAILURE);
}
