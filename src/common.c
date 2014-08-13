/**
 * @file common.c
 * @author Alexander Schöberl 0942320
 * @date 06.04.2014
 *
 * @brief Common functions program module.
 */

#include <stdio.h>
#include <stdlib.h>

/* Implementations */
/**
 * Mandatory usage function.
 * @brief This function writes helpful usage information about the program to stderr.
 */
void usage_client(void)
{
	char usage[] = "USAGE: 2048-client [-n | -u <id>]";
	char n[] = "-n:\tStart a new game";
	char i[] = "-u:\tConnect to existing game with the given id";
	(void) fprintf(stderr,"%s\n \t%s\n \t%s\n", usage, n, i);
	exit(EXIT_FAILURE);
}

/**
 * Mandatory usage function.
 * @brief This function writes helpful usage information about the program to stderr.
 */
void usage_server(void)
{
	char usage[] = "USAGE: 2048-server [-p power_of_two]";
	char p[] = "-p:\tPlay until 2ˆpower_of_two is reached (default: 11)";
	(void) fprintf(stderr,"%s\n \t%s\n", usage, p);
	exit(EXIT_FAILURE);
}

/**
 * Mandatory bail out function.
 * @brief This function writes helpful error information about the program to stderr.
 */
void bail_out_client(char *message)
{
	(void) fprintf(stderr, "ERROR: 2048-client %s\n", message);
	exit(EXIT_FAILURE);
}

/**
 * Mandatory bail out function.
 * @brief This function writes helpful error information about the program to stderr.
 */
void bail_out_server(char *message)
{
	(void) fprintf(stderr, "ERROR: 2048-server %s\n", message);
	exit(EXIT_FAILURE);
}
