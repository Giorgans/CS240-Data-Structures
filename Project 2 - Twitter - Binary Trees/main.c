#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "twitter.h"

#define BUFFER_SIZE 1024  /* Maximum length of a line in input file */

/* Uncomment the following line to enable debugging prints 
 * or comment to disable it */
#define DEBUG
#ifdef DEBUG
#define DPRINT(...) fprintf(stderr, __VA_ARGS__);
#else  /* DEBUG */
#define DPRINT(...)
#endif /* DEBUG */

/* Global variable, pointer to the root of the users tree */
user_t *users_p;
 
/* Global variable, pointer to the sentinel node of the user tree */
user_t *users_s;

/* Global variable, tweets hashTable */
t_hashTable *tweetHashTable;

/* Global size of tweets hashTable */
int max_tweets_g;

/* Global variable, array of primes */
unsigned int primes_g[160] = {  5,   7,  11,  13,  17,  19,  23,  29,  31,  37,
                               41,  43,  47,  53,  59,  61,  67,  71,  73,  79,
                               83,  89,  97, 101, 103, 107, 109, 113, 127, 131,
                              137, 139, 149, 151, 157, 163, 167, 173, 179, 181,
                              191, 193, 197, 199, 211, 223, 227, 229, 233, 239,
                              241, 251, 257, 263, 269, 271, 277, 281, 283, 293,
                              307, 311, 313, 317, 331, 337, 347, 349, 353, 359,
                              367, 373, 379, 383, 389, 397, 401, 409, 419, 421,
                              431, 433, 439, 443, 449, 457, 461, 463, 467, 479,
                              487, 491, 499, 503, 509, 521, 523, 541, 547, 557,
                              563, 569, 571, 577, 587, 593, 599, 601, 607, 613,
                              617, 619, 631, 641, 643, 647, 653, 659, 661, 673,
                              677, 683, 691, 701, 709, 719, 727, 733, 739, 743,
                              751, 757, 761, 769, 773, 787, 797, 809, 811, 821,
                              823, 827, 829, 839, 853, 857, 859, 863, 877, 881,
                              883, 887, 907, 911, 919, 929, 937, 941, 947, 953};

/**
 * @brief The main function
 *
 * @param argc Number of arguments
 * @param argv Argument vector
 *
 * @return 0 on success
 *         1 on failure
 */
int main(int argc, char** argv)
{
    FILE *fin = NULL;
	char buff[BUFFER_SIZE], event;

	/* Check command buff arguments */
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <input_file> <>\n", argv[0]);
		return EXIT_FAILURE;
	}

	/* Open input file */
	if (( fin = fopen(argv[1], "r") ) == NULL ) {
		fprintf(stderr, "\n Could not open file: %s\n", argv[1]);
		perror("Opening test file\n");
		return EXIT_FAILURE;
	}

	/* Initializations */
	initialize();
	
    /* Read input file buff-by-buff and handle the events */
	while (fgets(buff, BUFFER_SIZE, fin)) {
        
		DPRINT("\n>>> Event: %s", buff);

		switch(buff[0]) {

			/* Comment */
			case '#':
				break;

		    /* Hash Table Size */	
            case 'Z':
                /* Parse the hashtable size */
                sscanf(buff, "%c %d", &event, &max_tweets_g);
                DPRINT("%c %d\n", event, max_tweets_g);
                break;

			/* Register user
			 * R <uid> */
			case 'R':
				{
					int uid;

					sscanf(buff, "%c %d", &event, &uid);
					DPRINT("%c %d\n", event, uid);

					if (register_user(uid)) {
						DPRINT("R %d succeeded\n", uid);
					} else {
						fprintf(stderr, "R %d failed\n", uid);
					}

					break;
				}

			/* User uid1 is now following user uid2
			 * S <uid1> <uid2> */
			case 'S':
				{
					int uid1, uid2;

					sscanf(buff, "%c %d %d", &event, &uid1, &uid2);
					DPRINT("%c %d %d\n", event, uid1, uid2);

					if (subscribe(uid1, uid2)) {
						DPRINT("%c %d %d succeeded\n", event, uid1, uid2);
					} else {
						fprintf(stderr, "%c %d %d failed\n", event, uid1, uid2);
					}

					break;
				}

			/* New tweet event
			 * T <uid> <tid> <hashtag> <timestamp> <time_relevance> */
			case 'T':
				{
					int uid, tid, timestamp;

					sscanf(buff, "%c %d %d %d", &event, &uid, &tid, &timestamp);
					DPRINT("%c %d %d %d", event, uid, tid, timestamp);

					if (tweet(uid, tid, timestamp)) {
						DPRINT("%c %d %d %d succeeded\n", event, uid, tid, timestamp);
					} else {
						fprintf(stderr, "%c %d %d %d failed\n", event, uid, tid, timestamp);
					}

					break;
				}

			/* User uid1 stopped following user uid2
			 * U <uid1> <uid2> */
			case 'U':
				{
					int uid1, uid2;

					sscanf(buff, "%c %d %d", &event, &uid1, &uid2);
					DPRINT("%c %d %d\n", event, uid1, uid2);

					if (unsubscribe(uid1, uid2)) {
						DPRINT("%c %d %d succeeded\n", event, uid1, uid2);
					} else {
						fprintf(stderr, "%c %d %d failed\n", event, uid1, uid2);
					}

					break;
				}

			/* Delete user account
			 * D <uid> */
			case 'D':
				{
					int uid;

					sscanf(buff, "%c %d", &event, &uid);
					DPRINT("%c %d\n", event, uid);

					if (delete_user(uid)) {
						DPRINT("%c %d succeeded\n", event, uid);
					} else {
						fprintf(stderr, "%c %d failed\n", event, uid);
					}

					break;
				}

                /* History of a user's tweets
                 * H <uid> <date1> <date2>
                 */
            case 'H':
                {
                    int uid, date1, date2;

                    sscanf(buff, "%c %d %d %d", &event, &uid, &date1, &date2);
					DPRINT("%c %d %d %d\n", event, uid, date1, date2);

                    if (history(uid, date1, date2)) {
                        DPRINT("%c %d %d %d succeded\n", event, uid, date1, date2);
                    } else {
                        fprintf(stderr, "%c %d %d %d failed\n", event, uid, date1, date2);
                    }

                    break;
                }

			/* Lookup tweet in user's wall
			 * L <tid> */
			case 'L':
				{
					int tid;

					sscanf(buff, "%c %d\n", &event, &tid);
					DPRINT("%c %d \n", event, tid);

					if (lookup(tid)) {
						DPRINT("%c %d succeeded\n", event, tid);
					} else {
						fprintf(stderr, "%c %d failed\n", event, tid);
					}

					break;
				}

			/* Print all active users
			 * X */
			case 'X':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (print_users()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

			/* Print all tagged tweets
			 * Y */
			case 'Y':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (print_tweets()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

			/* Empty line */
			case '\n':
				break;

			/* Ignore everything else */
			default:
				DPRINT("Ignoring buff: %s \n", buff);

				break;
		}
	}

	free_all();
	return (EXIT_SUCCESS);
}
