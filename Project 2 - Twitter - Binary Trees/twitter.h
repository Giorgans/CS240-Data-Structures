#ifndef __TWITTER_H_
#define __TWITTER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_TWEETS 1000                             /* Maximum tweets in the hashTable   */
#define HFUNC_1(X) ((X) % (max_tweets_g))           /* First hash function               */
#define HFUNC_2(X) (((X)*(X)) % (max_tweets_g) + 1) /* Second hash function              */

/**
 * Structure defining a node of the user double linked binary search
 * tree
 */
typedef struct user {
    int uid;                                    /* User identifier                   */
    struct follower *followers;                 /* Pointer to the node's followers   */
    struct tweet *wall_head;                    /* Pointer to the node's tweet wall  */
    struct user *par;                           /* Pointer to the node's parent      */
    struct user *lc;                            /* Pointer to the node's left child  */
    struct user *rc;                            /* Pointer to the node's right child */
}user_t;

/**
 * Structure defining a node of the tweet leaf-oriented binary search tree
 */
typedef struct tweet {
    int tid;                                    /* Tweet identifier                  */   
    int uid;                                    /* User identifier                   */
    int timestamp;                              /* Publication date (form: YYYYMMDD) */
    struct tweet *next;                         /* Pointer to the next node's leaf   */
    struct tweet *lc;                           /* Pointer to the node's left child  */
    struct tweet *rc;                           /* Pointer to the node's right child */
}tweet_t;

/**
 * Structure defining a node of the follower binary search tree
 */
typedef struct follower {
    int uid;                                    /* User identifier                   */
    struct follower *lc;                        /* Pointer to the node's left child  */
    struct follower *rc;                        /* Pointer to the node's right child */
}follower_t;

/**
 * Structure defining the contents of the tweets hash tables
 */
typedef struct tweet_hashTable {
    int tid;                                    /* Tweet identifier                  */
    int uid;                                    /* User identifier                   */
    int timestamp;                              /* Publication date (form: YYYYMMDD) */
    char remove;                                /* Delete tweet flag                 */
}t_hashTable;

/* Global variable, array of primes */
extern unsigned int primes_g[160]; 

/* Global variable, pointer to the root of the users tree */
extern user_t *users_p;
 
/* Global variable, pointer to the sentinel node of the user tree */
extern user_t *users_s;

/* Global variable, tweets hashTable */
extern t_hashTable *tweetHashTable;

/* Global size of tweets hashTable */
extern int max_tweets_g;

/**
 * @brief Optional function to initialize data structures that 
 *        need initialization
 *
 * @return 1 on success
 *         0 on failure
 */
int initialize();

/**
 * @brief Register user
 *
 * @param uid The user's id
 *
 * @return 1 on success
 *         0 on failure
 */
int register_user(int uid);

/**
 * @brief User uid1 is now following user uid2
 *
 * @param uid1 The 1st user's id
 * @param uid2 The 2nd user's id
 *
 * @return 1 on success
 *         0 on failure
 */
int subscribe(int uid1, int uid2);

/**
 * @brief New tweet event
 * 
 * @param uid       The user who posted the tweet
 * @param tid       The tweet's id
 * @param timestamp The time the tweet was posted
 *
 * @return 1 on success
 *         0 on failure
 */
int tweet(int uid, int tid, int timestamp);

/**
 * @brief User uid1 stopped following user uid2
 *
 * @param uid1 The 1st user's id
 * @param uid2 The 2nd user's id
 *
 * @return 1 on success
 *         0 on failure
 */
int unsubscribe(int uid1, int uid2);

/**
 * @brief Delete user account
 * 
 * @param uid The id of the user account to delete
 * 
 * @return 1 on success
 *         0 on failure
 */
int delete_user(int uid);

/**
 * @brief Get the history of the user tweets
 *
 * @param uid   The id of the user account
 * @param date1 Start date
 * @param date2 End date
 * 
 * @return  1 on success
 *          0 on failure
 */
int history(int uid, int date1, int date2);

/**
 * @brief Lookup tweet in the tweets hash table
 * 
 * @param tid The tweet's id
 *
 * @return 1 on success
 *         0 on failure
 */
int lookup(int tid);

/**
 * @brief Print all active users
 *
 * @return 1 on success
 *         0 on failure
 */
int print_users(void);

/**
 * @brief Print all tagged tweets
 *
 * @return 1 on success
 *         0 on failure
 */
int print_tweets(void);

/**
 * @brief Free resources
 *
 * @return 1 on success
 *         0 on failure
 */
int free_all(void);

#endif /* __TWITTER_H_ */
