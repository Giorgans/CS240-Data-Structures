#ifndef __TWITTER_H_
#define __TWITTER_H_


struct user {
    int uid;
    struct follower *followers;
    struct tweet *wall_head;
    struct tweet *wall_sentinel;
    struct user *next;
};

struct tweet {
    int tid;
    int uid;
    struct tweet *next;
};

struct follower {
    int uid;
    struct follower *next;
};

struct tagged_tweet {
    int tid;
    int uid;
    int timestamp;
    int time_relevance;
    struct tagged_tweet *next;
    struct tagged_tweet *prev;
};

struct combat {
    int soldier_cnt;
    struct c_soldier *combat_s;
};


/* global variable, pointer to the head of the users list */
struct user *usersList;

/* global enum, used to specify the tweets hashtags */
typedef enum {
	sports      = 0,
	politics    = 1,
	economics   = 2,
	music       = 3,
	movies      = 4,
	nature      = 5,
	art         = 6,
	environment = 7,
	technology  = 8,
	weather     = 9
}hashtag;

/* global array, array of lists of tagged tweets */
struct tagged_tweet *Hashtags[10];


#endif /* __TWITTER_H_ */
