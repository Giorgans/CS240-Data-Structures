#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "twitter.h"

#define BUFFER_SIZE 1024  /* Maximum length of a line in input file */

/* Uncomment the following line to enable debugging prints 
 * or comment to disable it */
/* #define DEBUG */

#ifdef DEBUG
#define DPRINT(...) fprintf(stderr, __VA_ARGS__);
#else  /* DEBUG */
#define DPRINT(...)
#endif /* DEBUG */

/*Checks if user exist in the users list*/
int isUserexists(struct user *head,int uid);
/*Pointer that points on the user we want to find*/
struct user *findUser(struct user *head,int uid);
/*Prints all the tweets on a user's wall*/
int printUsersTweets(struct tweet *head,int uid);

/**
 * @brief Optional function to initialize data structures that 
 *        need initialization
 *
 * @return 1 on success
 *         0 on failure
 */
int initialize()
{
	int i;
    /*Making sure that user list and all hashtags are null*/
	usersList = NULL;
    for(i=0;i<10;i++) Hashtags[i]=NULL;
	return 1;
}

/**
 * @brief Register user
 *
 * @param uid The user's id
 *
 * @return 1 on success
 *         0 on failure
 */
int register_user(int uid)
{
    /*Creating a new user node and some helpful pointers*/
    struct user* new=(struct user *)malloc(sizeof(struct user));
    struct user *last=usersList;
    new->uid=uid;
    new->next=NULL;
    new->followers=NULL;
    new->wall_head=NULL;
    new->wall_sentinel=NULL;
    /*********/

    /*Adding a new user in the userlist and sort the list*/
    if(usersList==NULL)  usersList=new;
    else{
      while(last!=NULL){
        if(last->next==NULL && last->uid<new->uid)  last->next=new;
        else if(last==usersList && last->uid>new->uid){
            new->next=usersList;
            usersList=new;
        }
        else if(last->uid<new->uid  &&  last->next->uid>new->uid){
          new->next=last->next;
          last->next=new;
        }
      last=last->next;
      }
    }
   /*********/
   
  /*Printing the updated users list*/
  last = usersList;
  printf("\nR %d\n",uid);
  printf("     Users =");
  while(last != NULL)
  {
    printf(" <%d> ", last->uid);
    if(last->next!=NULL) printf(",");
    last = last->next;
  }
  printf("\nDONE\n");
  /*********/
  return 1;
}

/**
 * @brief User uid1 is now following user uid2
 *
 * @param uid1 The 1st user's id
 * @param uid2 The 2nd user's id
 *
 * @return 1 on success
 *         0 on failure
 */
int subscribe(int uid1, int uid2)
{
    /*Creating a new follower node for uid1 and some helpful pointers*/
    struct follower* new=(struct follower *)malloc(sizeof(struct follower));
    struct follower *last;
    struct user *find=usersList;
    /*********/

    /*Checking if subscribe is not possible*/
    if(find==NULL)   return 0;
    if(uid1==uid2)   return 0;
    if (isUserexists(usersList,uid1)==0 || isUserexists(usersList,uid2)==0)
        return 0;
    /*********/
    
    /*Finding the uid1 user*/
    new->uid=uid2;
    new->next=NULL;
    while(find->uid!=uid1)  
        find=find->next;
    /*********/

    /*Adding a new follower uid2 to uid1 and sort the list*/
    last=find->followers;
    if(find->followers==NULL)  find->followers=new;
    else{
        while(last!=NULL){
            if(last->next==NULL && last->uid<new->uid)  last->next=new;
            else if(last==find->followers && last->uid>new->uid){
                new->next=find->followers;
                find->followers=new;
            }
            else if(last->uid<new->uid  &&  last->next->uid>new->uid){
               new->next=last->next;
               last->next=new;
            }
           last=last->next;
        }
    }
    /**********/

    /*Printing the updated followers list*/
    last=find->followers;
    printf("\nS %d %d\n",uid1,uid2);
    printf("     Followers =");
    while(last != NULL){
        printf(" <%d> ", last->uid);
        if(last->next!=NULL) printf(",");
        last = last->next;
    }
    printf("\nDONE\n");
    /**********/

    return 1;
}


/**
 * @brief New tweet event
 * 
 * @param uid            The user who posted the tweet
 * @param tid            The tweet's id
 * @param tag            The tweet's hashtag
 * @param timestamp      The time the tweet was posted
 * @param time_relevance How long the tweet was relevant/trending
 *
 * @return 1 on success
 *         0 on failure
 */
int tweet(int uid, int tid, int tag, int timestamp, int time_relevance)
{
    /*Creating a new tweet node for uid*/
    struct tweet* new=(struct tweet *)malloc(sizeof(struct tweet));
    new->uid=uid;
    new->tid=tid;
    new->next=NULL;
    /********/

    /*Creating a new tagged tweet for uid*/
    struct tagged_tweet* newTG=(struct tagged_tweet *)malloc(sizeof(struct tagged_tweet));
    newTG->uid=uid;
    newTG->tid=tid;
    newTG->timestamp=timestamp;
    newTG->time_relevance=time_relevance;
    newTG->next=NULL;
    newTG->prev=NULL; 
    /********/

    /*Creating some helpful pointers*/
    struct user *findU,*findF;
    struct tweet *head,*sentinel;
    struct follower *tempfollow;
    struct tagged_tweet *last;
    int headtime,nexttime,newtime;
    /********/

    /*Checking if tweet is not possible*/
    if (usersList==NULL) return 0;
    if (isUserexists(usersList,uid)==0) return 0;
    /********/

    /*Printing the tweet info*/
    printf("T %d %d %d %d %d",uid,tid,tag,timestamp,time_relevance);
    /********/

    /*Find the user*/
    findU=findUser(usersList,uid);
    /********/

    /*Insert the tweet into users wall*/
    if(findU->wall_head==NULL){
        findU->wall_head=new;
        findU->wall_sentinel=new->next;
    }
    else{
        new->next=findU->wall_head;
        findU->wall_head=new;
    }
    /********/

    /*Insert the tweet into every follower's wall*/
    tempfollow=findU->followers;
    while(tempfollow!=NULL){
        findF=findUser(usersList,tempfollow->uid);
        new=(struct tweet *)malloc(sizeof(struct tweet));
        new->uid=uid;
        new->tid=tid;
        /*Adding the new tweet into each user's follower */
        if(findF->wall_head==NULL){
             findF->wall_head=new;
             findF->wall_sentinel=new->next;
        }
        else{
             new->next=findF->wall_head;
             findF->wall_head=new;
        }
        tempfollow=tempfollow->next;
    }
    /********/

    /* Printing all tweets of the user and his followers */
    findU=usersList; 
    while(findU!=NULL){
        printUsersTweets(findU->wall_head,findU->uid); 
        findU=findU->next;
    }
    printf("\nDONE\n");
    /********/

    /*Insert the tagged tweet */
    last=Hashtags[tag];
    newtime=newTG->timestamp;
    if(Hashtags[tag]==NULL)  Hashtags[tag]=newTG; 
    else{
       while(last!=NULL){
           headtime=last->timestamp;
           /*Sorting the tweets on the hashtag base on the timestamp*/
           if(last->next!=NULL) nexttime=last->next->timestamp;
           if(headtime>newtime && last->prev==NULL){
               newTG->next=Hashtags[tag];
               newTG->prev=NULL;
               Hashtags[tag]->prev=newTG;
               Hashtags[tag]=newTG;
           }
           else if(headtime<newtime && last->next==NULL){
               last->next=newTG;
               newTG->prev=last;
           }
           else if(headtime<newtime && newtime<=nexttime){
                newTG->next=last->next;
                last->next=newTG;
                newTG->prev=last;
                newTG->next->prev=newTG;
           }
           /*******/
          last=last->next;
       }
    }
    /********/

	return 1;
}

/**
 * @brief User uid1 stopped following user uid2
 *
 * @param uid1 The 1st user's id
 * @param uid2 The 2nd user's id
 *
 * @return 1 on success
 *         0 on failure
 */
int unsubscribe(int uid1, int uid2)
{
    /*Creating some helpful pointers*/
    struct follower *tempfollow;
    struct follower *Fhead,*Fback;
    struct user *findU,*findF;
    struct tweet *head,*back;
    /*********/

    /*Checking if unsubscribe is not possible */
    if(usersList==NULL) return 0;
    if (isUserexists(usersList,uid1)==0 || isUserexists(usersList,uid2)==0)
       return 0;
    if(uid1==uid2) return 0;
    findU=findUser(usersList,uid1);
    tempfollow=findU->followers;
    if(tempfollow==NULL) return 0;
    findF=findUser(usersList,uid2);
    head=findF->wall_head;
    if(head==NULL) return 0;
    /*********/

    /*Remove the user with uid2 from the followers of user uid1*/
    Fhead = findU->followers;
    if (Fhead!=NULL && Fhead->uid == uid2){
        findU->followers = Fhead->next;   
        free(Fhead);               
        Fhead=findU->followers;
    }
    else{
       while (Fhead!=NULL && Fhead->uid != uid2){
           Fback = Fhead;
           Fhead = Fhead->next;
       }
       if (Fhead == NULL) return 0;
       Fback->next = Fhead->next;
       free(Fhead);  
    }
    /*********/

    /*Removing all the tweets of uid1 from uid2's wall*/
    findF=findUser(usersList,uid2);
    head=findF->wall_head;
    while(head!=NULL && head->uid==uid1){
        findF->wall_head = head->next;
        free(head);
        head = findF->wall_head;
    }
    while(head!=NULL){
        while(head!=NULL && head->uid!=uid1){
             back=head;
             head=head->next;
        }
        if(head!=NULL && head->uid==uid1){
            back->next=head->next;
            free(head);
            head=back->next;
        }
    }
    /**********/

    /* Printing all tweets of the user and his followers */
    findU=usersList; 
    printf("U %d %d",uid1,uid2);
    while(findU!=NULL){
        printUsersTweets(findU->wall_head,findU->uid); 
        findU=findU->next;
    }
    printf("\nDONE\n");
    /*********/

	return 1;
}


/**
 * @brief Delete user account
 * 
 * @param uid The id of the user account to delete
 * 
 * @return 1 on success
 *         0 on failure
 */
int delete_user(int uid)
{
    /*Creating some helpful pointers*/
    struct user *head,*back,*find;
    struct tweet *headtw,*nexttw;
    struct follower *tempf;
    int i,x;
    /*********/

    /*Checking if users delete is not possible*/
    if(usersList==NULL) return 0;
    if(isUserexists(usersList,uid)==0) return 0;
    /*********/
    
    /*Remove every tweet in the user's wall*/
    find=findUser(usersList,uid);
    headtw=find->wall_head;
    while(headtw!=NULL){
        nexttw=headtw->next;
        free(headtw);
        headtw = nexttw;
    }
    find->wall_head=NULL;
    /*********/

    /*Remove via unsubscribe every tweet from the user that is goind to remove*/
    find=findUser(usersList,uid);
    if(find->followers!=NULL){
       tempf=find->followers;
       x=find->followers->uid;
       while(tempf->next!=NULL)
           tempf=tempf->next;
       for (i=x;i<=tempf->uid;i++)
           unsubscribe(uid,i);
    }
    /*********/

    /*Remove the user from users list*/
    head = usersList;
    if (head->uid == uid){
        usersList = head->next;   
        free(head);               
        head = usersList;
    }
    else{
       while (head->uid != uid){
           back = head;
           head = head->next;
       }
       if (head == NULL) return 0;
       back->next = head->next;
       free(head);  
    }
    /*********/

    /*Print the tweets of every user remained*/
    head = usersList;
    printf("D %d",uid);
    while(head!=NULL){
        printUsersTweets(head->wall_head,head->uid);
        head=head->next;
    }
    printf("\nDONE\n");
    /*********/

	return 1;
}

/**
 * @brief Lookup tweet in user's wall
 * 
 * @param uid The user's id
 * @param tid The tweet's id
 *
 * @return 1 on success
 *         0 on failure
 */
int lookup(int uid, int tid)
{
    /*Creating some useful pointers*/
    struct user *find;
    struct tweet *head;
    /**********/

    /*Checking if lookup is not possible*/
    if(usersList==NULL) return 0;
    if(isUserexists(usersList,uid)==0) return 0;
    /**********/

    /*Printing the tweet of user has the tweet on the wall*/
    find=findUser(usersList,uid);
    printf("L %d %d\n",uid,tid);
    head=find->wall_head;
    while(head!=NULL){
        if(head->tid==tid) printf("    Tweet = <%d:%d>",tid,head->uid);
        head=head->next;
    }
    printf("\nDONE\n");
    /**********/
    
	return 1;
}

/**
 * @brief Combine tweets that have following tags
 * 
 * @param tag1 The first tag
 * @param tag2 The second tag
 *
 * @return 1 on success
 *         0 on failure
 */
int merge(int tag1, int tag2)
{
    /*Creating some helpful pointers*/
    struct tagged_tweet *head1,*head2;
    /**********/
    head1=Hashtags[tag1];
    head2=Hashtags[tag2];
    /*Checking if merge is not possible*/
    if(tag1<0 || tag1>9) return 0;
    if(tag2<0 || tag2>9) return 0;
    if(head1==NULL && head2==NULL) return 0;
    /**********/

    /*Printing the merged list sorted by timestamp*/
    printf("M %d %d\n",tag1,tag2);
    printf("     Merged =");
    while(head1!=NULL || head2!=NULL){
      if(head1!=NULL && head2==NULL){
        printf(" <%d:%d:%d:%d> ",head1->tid,head1->uid,head1->timestamp,head1->time_relevance);
        head1=head1->next;
      }
      else if(head1==NULL && head2!=NULL){
        printf(" <%d:%d:%d:%d> ",head2->tid,head2->uid,head2->timestamp,head2->time_relevance);
        head2=head2->next;
      }
      else{
         if(head1->timestamp<=head2->timestamp){
           printf(" <%d:%d:%d:%d> ",head1->tid,head1->uid,head1->timestamp,head1->time_relevance);
           head1=head1->next;
         }
         else if(head1->timestamp>head2->timestamp){
           printf(" <%d:%d:%d:%d> ",head2->tid,head2->uid,head2->timestamp,head2->time_relevance);
           head2=head2->next;
         }
      }
      if(head1!=NULL || head2!=NULL) printf(",");
    }
    /**********/

    /*Printing the two seperated hashtags*/
    printf("\n     Tweets%d =",tag1);
    head1=Hashtags[tag1];
    while(head1!=NULL){
        printf(" <%d:%d:%d:%d> ",head1->tid,head1->uid,head1->timestamp,head1->time_relevance);
        head1=head1->next;
        if(head1!=NULL) printf(",");
    }
    printf("\n     Tweets%d =",tag2);
    head2=Hashtags[tag2];
    while(head2!=NULL){
        printf(" <%d:%d:%d:%d> ",head2->tid,head2->uid,head2->timestamp,head2->time_relevance);
        head2=head2->next;
        if(head2!=NULL) printf(",");
    }
    printf("\nDONE\n");
    /**********/

	return 1;

}

/**
 * @brief Find tweets of the same tag that were trending
 *        at the same time as the current tweet
 *
 * @param tid The tweet's id
 * @param tag The tweet's hashtag
 *
 * @return 1 on success
 *         0 on failure
 */
int time_relevant(int tid, int tag)
{
    /*Creating some helpful pointers and variables*/
    struct tagged_tweet *head=NULL,*next,*back,*temp;
    int tstamp,trel;
    /***********/

    /*Checking if time relevant is not possible*/
    if(usersList==NULL) return 0;
    if(Hashtags[tag]==NULL) return 0;
    temp=Hashtags[tag];
    while(temp!=NULL){
        if(temp->tid==tid)
            head=temp;
        
        temp=temp->next;
    }
    if(head==NULL) return 0;
    /***********/
    
    /*Printing the relevant tweets of the hashtag*/
    printf("F %d %d",tid,tag);
    printf("\n    Relevant Tweets = ");
    back=head->prev;
    next=head->next;
    tstamp=head->timestamp;
    trel=head->time_relevance;
    if(back==NULL && next==NULL)
         printf(" <%d:%d:%d:%d> ",head->tid,head->uid,head->timestamp,head->time_relevance);
    while(back!=NULL || next!=NULL){
       
       if(back!=NULL && (tstamp-trel)<=back->timestamp && back->timestamp<=(tstamp+trel))
         printf(" <%d:%d:%d:%d> ",back->tid,back->uid,back->timestamp,back->time_relevance);
       
       if((back!=NULL && (tstamp-trel)<=back->timestamp && back->timestamp<=(tstamp+trel))&&
          (next!=NULL && (tstamp-trel)<=next->timestamp && next->timestamp<=(tstamp+trel))) 
          printf(","); 

       if(next!=NULL && (tstamp-trel)<=next->timestamp && next->timestamp<=(tstamp+trel))
          printf(" <%d:%d:%d:%d> ",next->tid,next->uid,next->timestamp,next->time_relevance);
       
       if(back!=NULL) back=back->prev;
       if(next!=NULL) next=next->next;

       if((back!=NULL && (tstamp-trel)<=back->timestamp && back->timestamp<=(tstamp+trel))||
          (next!=NULL && (tstamp-trel)<=next->timestamp && next->timestamp<=(tstamp+trel))) 
          printf(","); 

    }
    printf("\nDONE\n");
    /***********/

	return 1;
}

/**
 * @brief Print all active users
 *
 * @return 1 on success
 *         0 on failure
 */
int print_users(void)
{
    /*Creating some helpful pointers*/
    struct user *find;
    struct follower *tempF;
    struct tweet *head;
    /**********/

    /*Checking if users print is not possible*/
    if(usersList==NULL) return 0;
    /**********/

    /*Printing all information for every user*/
    find=usersList;
    printf("X");
    while(find!=NULL){
        printf("\n  User = %d\n",find->uid);
        /*Printing all the followers of the user*/
        printf("   Followers : ");
        tempF=find->followers;
        while(tempF!=NULL){
           printf(" <%d> ",tempF->uid);
           if(tempF->next!=NULL) printf(",");
           tempF=tempF->next;
        }
        /*****/
        /*Printing all the tweets on the user's wall*/
        printf("\n   Tweets : ");
        head=find->wall_head;
        while(head!=NULL){
           printf(" <%d:%d> ",head->tid,head->uid);
           if(head->next!=NULL) printf(",");
           head=head->next;
        }
        /*****/
        find=find->next;
    }
    printf("\nDONE\n");
    /**********/

	return 1;
}

/**
 * @brief Print all tagged tweets
 *
 * @return 1 on success
 *         0 on failure
 */
int print_tweets(void)
{
    /*Creating some helpful pointers*/
    struct tagged_tweet *head;
    /********/

    /*Printing all the tagged tweets from every hashtag*/
    int i;
    printf("Y");
    for(i=0;i<10;i++){
        printf("\n  Hashtag%d = ",i);
        head=Hashtags[i];
        while(head!=NULL){
          printf(" <%d:%d:%d:%d> ",head->tid,head->uid,head->timestamp,head->time_relevance);
          if(head->next!=NULL) printf(",");
          head=head->next;
        }
    }
    printf("\nDONE\n");
    /********/

	return 1;
}

/**
 * @brief Free resources
 *
 * @return 1 on success
 *         0 on failure
 */
int free_all(void)
{
    /*Creating some helpful pointers*/
    struct user *find=usersList,*tempu;
    struct follower *headf,*tempf;
    struct tweet *thead,*tempt;
    struct tagged_tweet *tghead,*temptg;
    int i;
    /**********/
    
    /*Free all users and for every user all his followers and tweets*/
    while(find!=NULL){
        headf=find->followers;
        while(headf!=NULL){
            tempf=headf->next;
            free(headf);
            headf=tempf;
        }

        thead=find->wall_head;
        while(thead!=NULL){
           tempt=thead->next;
           free(tempt);
           thead=tempt;
        }
        tempu=find->next;
        free(find);
        find=tempu;
    }
    /**********/

    /*Free all tagged tweets from all hashtags*/
    for(i=0;i<10;i++){
        tghead=Hashtags[i];
        while(tghead!=NULL){
            temptg=tghead->next;
            free(tghead);
            tghead=temptg;
        }
    }
    /**********/
	return 1;
}

/** @brief Checks if user exist in the users list
 *  @return 1 if user exist
 *          0 if user does not exist
 */
int isUserexists(struct user *head,int uid){
   /*Creating some helpful pointers and variables*/
   struct user *findu=head;
   int flag=0;
   /*******/

   /*Searching for the user*/
   while(findu!=NULL){
       if(findu->uid==uid){
           flag=1;
       }
       findu=findu->next;
   }
   /*******/
   return flag;
}

/** @brief Prints all the tweets on a user's wall
 *  @return 1 on success
 *          0 on failure
 */
int printUsersTweets(struct tweet *head,int uid){
    /*Prints all the tweets on a user's wall*/    
    printf("\n   User%d = ",uid);
    while(head!=NULL){
        printf("<%d:%d>",head->tid,head->uid);
        if(head->next!=NULL) printf(" , ");
        head=head->next;
    }   
    /*******/

    return 1;
}

/** @brief Pointer that points on the user we want to find 
 *  @returns a pointer for a user
 */
struct user *findUser(struct user *head,int uid){
   /*Creating a helpful pointer*/
   struct user *search=NULL;
   /***********/

   /*Searching for the user in the users list*/
   while(head!=NULL){
       if(head->uid==uid) search=head;
       head=head->next;
   }
   /***********/
   
   return search;
}

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
		fprintf(stderr, "Usage: %s <input_file> \n", argv[0]);
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
					int uid, tid, tag, timestamp, time_relevance;

					sscanf(buff, "%c %d %d %d %d %d", &event, &uid, &tid, &tag, &timestamp, &time_relevance);
					DPRINT("%c %d %d %d %d %d", event, uid, tid, tag, timestamp, time_relevance);

					if (tweet(uid, tid, tag, timestamp, time_relevance)) {
						DPRINT("%c %d %d %d %d %d succeeded\n", event, uid, tid, tag, timestamp, time_relevance);
					} else {
						fprintf(stderr, "%c %d %d %d %d %d failed\n", event, uid, tid, tag, timestamp, time_relevance);
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

			/* Lookup tweet in user's wall
			 * L <uid> <tid> */
			case 'L':
				{
					int uid, tid;

					sscanf(buff, "%c %d %d\n", &event, &uid, &tid);
					DPRINT("%c %d %d\n", event, uid, tid);

					if (lookup(uid, tid)) {
						DPRINT("%c %d %d succeeded\n", event, uid, tid);
					} else {
						fprintf(stderr, "%c %d %d failed\n", event, uid, tid);
					}

					break;
				}

			/* Combine tweets that have following tags
			 * M <hashtag1> <hashtag2> */
			case 'M':
				{
					int tag1, tag2;

					sscanf(buff, "%c %d %d", &event, &tag1, &tag2);
					DPRINT("%c %d %d\n", event, tag1, tag2);

					if (merge(tag1, tag2)) {
						DPRINT("%c %d %d succeeded\n", event, tag1, tag2);
					} else {
						fprintf(stderr, "%c %d %d failed\n", event, tag1, tag2);
					}

					break;
				}

			/* Find tweets of the same tag that were trending
			 * at the same time as the current tweet
			 * F <tid> <hashtag> */
			case 'F':
				{
					int tid, tag;

					sscanf(buff, "%c %d %d", &event, &tid, &tag);
					DPRINT("%c %d %d\n", event, tid, tag);

					if (time_relevant(tid, tag)) {
						DPRINT("%c %d %d succeeded\n", event, tid, tag);
					} else {
						fprintf(stderr, "%c %d %d failed\n", event, tid, tag);
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
