#include "twitter.h"
/*Functions for Users*/
int print_User_Tree( struct user  *head,int tweetsTOO);
struct user *findUser(struct user *head,int uid);
struct user *deleteUser(struct user *root,int uid);
struct user * inOrderSuccessor(struct user *root, struct user *next);
struct user *minimumUser(struct user *users);
int is_user_exist(struct user *head,int uid);
int inOrder_Users(struct user *head);
int DELETE_EVERYTHING(struct user *root);
int maxuser=0;
/**********************************************/
/*********Functions for Followers**************/
int print_Followers_Tree(struct follower *head);
int Add_Tweet_to_Followers(struct follower *v,int uid,int tid,int timestamp);
struct follower *minFollower(struct follower *follow);
struct follower *deleteFollower(struct follower *follow,int uid);
int UnsubscribeAllFollowers(struct follower *follow,int uid);
int freeAllFollowers(struct follower *root);
/**********************************************/
/*********Functions for Tweets*****************/
int print_Wall_Tree(struct tweet *head);
struct tweet *InOrderTweet(struct tweet *v);
int update_leaforiented_tree(struct tweet *root);
int find_first_leaf(struct tweet *search);
struct tweet *deleteTweet(struct tweet *htweet,int timestamp);
int deleteAllTweets(struct tweet *root);
struct tweet *minTweet(struct tweet *tweets);
int CountTweets(struct tweet *tweets);
struct tweet *insertTweet(struct tweet* root, int uid,int tid,int timestamp);
struct tweet *prevtweet=NULL,*parent=NULL,*leaf=NULL,*foundTweet=NULL;
int flag=0;
/**********************************************/
/*********Functions for Hashtable**************/
void insertHashTweet(int uid,int tid,int timestamp);



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
    struct tweet_hashTable* new;
    users_p = NULL;
    users_s = (struct user *)malloc(sizeof(struct user));
    
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
    /*Creating a new tree user node and some helpful pointers*/
    struct user *head=users_p,*prev=NULL;
    struct user* new = (struct user *)malloc(sizeof(struct user));
    new->uid=uid;
    new->wall_head=NULL;
    new->followers=NULL;
    new->lc=users_s;
    new->rc=users_s;
    /*****************/
    
    /*Inserting a new user in the users tree*/
    if(users_p==NULL){
        users_p=new;
        users_p->par=NULL;
    }
    else{
        if(head->uid==uid) return 0;
        
        while(head!=users_s){
            prev=head;
            if(head->uid > uid) head=head->lc;
            else head=head->rc;
        }
        if(prev->uid > uid)  prev->lc=new;
        else  prev->rc=new;
        new->par=prev;
    }
    /*****************/

     if(maxuser<uid) maxuser=uid;

    /*Printing the users in a sorted way*/
    printf("    Users = ");
    print_User_Tree(users_p,0);
    printf("\nDONE\n");
    /*****************/
    
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
    /*Creating some helpful pointers*/
    struct user *find;
    struct follower *head,*prev;
    /*****************/
    
    /*Checking if subscribe is not possible*/
    if(users_p==NULL) return 0;
    if(uid1==uid2) return 0;
    if(!is_user_exist(users_p,uid1)) return 0;
    if(!is_user_exist(users_p,uid2)) return 0;
    /*****************/
    
    /*Creating a new follower for a user*/
    struct follower* new = (struct follower *)malloc(sizeof(struct follower));
    new->uid=uid2;
    new->lc=NULL;
    new->rc=NULL;
    /*****************/
    
    /*Inserting the new follwer into the followers tree*/
    find=findUser(users_p,uid1);
    head=find->followers;
    if(find->followers==NULL)
        find->followers=new;
    else{
        while(head!=NULL){
            prev=head;
            if(head->uid > uid2) head=head->lc;
            else head=head->rc;
        }
        if(prev->uid > uid2) prev->lc=new;
        else prev->rc=new;
    }
    /*****************/
    
    /*Printing all the user's followers*/
    printf("    Followers = ");
    print_Followers_Tree(find->followers);
    printf("\nDONE\n");
    /*****************/
    
    return 1;
}

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
int tweet(int uid, int tid, int timestamp)
{
    /*Creating some helpful pointers*/
    struct user *find;
    struct follower *follow;
    struct tweet *head,*prev;
    int i;
    /*****************/
    
    /*Checking if tweet is not possible*/
    if(users_p==NULL) return 0;
    if(!is_user_exist(users_p,uid)) return 0;
    /*****************/
    
    
    /*Inserting the new tweet into user's wall tree*/
    find=findUser(users_p,uid);
    
    find->wall_head=insertTweet(find->wall_head,uid,tid,timestamp);

    update_leaforiented_tree(find->wall_head);
    prevtweet=NULL;
    

    /*****************/
    
    /*Inserting the new tweet into the hashtable*/
    if(tweetHashTable == NULL){
        tweetHashTable = ( t_hashTable *)malloc(sizeof( t_hashTable));
        for(i=0;i<160;i++){
            if(primes_g[i] >= max_tweets_g ){
                max_tweets_g = primes_g[i];
                break;
            }
        }
        tweetHashTable = ( t_hashTable *)malloc(max_tweets_g *sizeof( t_hashTable));
        for(i=0;i<max_tweets_g;i++){
            tweetHashTable[i].timestamp=-1;
            tweetHashTable[i].uid=-1;
            tweetHashTable[i].tid=-1;
            tweetHashTable[i].remove=-1;
        }   
    }
    int key = HFUNC_1(timestamp);
    int d = HFUNC_2(timestamp)>0 ? HFUNC_2(timestamp):-HFUNC_2(timestamp);

    i=0;
    while(tweetHashTable[key].timestamp != -1 ){
        key +=  d ;
        if(key >= max_tweets_g) key -= max_tweets_g;
        if(i++ == max_tweets_g){
            printf("---------------ERROR---------------\n");
            return 0;
        }
    }
    tweetHashTable[key].tid = tid;
    tweetHashTable[key].uid = uid;
    tweetHashTable[key].timestamp = timestamp;
    tweetHashTable[key].remove = 0;
    /*****************/
    
    /*Inserting the new tweet into each follower wall tree*/
    follow=find->followers;
    Add_Tweet_to_Followers(follow,uid,tid,timestamp);
    /*****************/
    
    /*Printing all users tweets in their wall*/
    print_User_Tree(users_p,1);
    printf("\nDONE\n"); 
    /*****************/
    
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
    /*Creating some helpful pointers and variables*/
    struct user *find;
    int count=0,i;
    /***********************/

    /*Checking if unsubscribe is not possible*/
    if(uid1==uid2) return 0;
    if(!is_user_exist(users_p,uid1) || !is_user_exist(users_p,uid2)) return 0;
    /***********************/

    /*Counting all users tweets*/
    find=findUser(users_p,uid1);
    count=CountTweets(find->wall_head);
    /***********************/

    /*Delete every tweet with uid1*/
    find=findUser(users_p,uid2);
    for(i=0;i<count;i++)
       find->wall_head=deleteTweet(find->wall_head,uid1);
    
    update_leaforiented_tree(find->wall_head);
    prevtweet=NULL;
    /***********************/

    /*Delete the follower uid2 from uid1's followers tree*/
    find=findUser(users_p,uid1);
    find->followers=deleteFollower(find->followers,uid2);
    /***********************/

    /*Printing all the tweets from every user*/
    print_User_Tree(users_p,1);
    printf("\nDONE\n");
    /***********************/

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
    /*Creating a helpful pointer*/
    struct user *find;
    int i;
    /***********************/

    /*Checking if delete is not possible*/
    if(!is_user_exist(users_p,uid)) return 0;
	/***********************/

    /*Delete all the user's followers via unsubscribe*/
    find = findUser(users_p,uid);
    UnsubscribeAllFollowers(find->followers,uid);
    /***********************/

    /*Delete all the user's tweets */
    deleteAllTweets(find->wall_head);
    /***********************/
    
    /*Sets every tweet in the hashtable with uid removed*/
    for(i=0;i<max_tweets_g;i++){
         if(tweetHashTable[i].uid==uid)
             tweetHashTable[i].remove=1;
    } 
    /***********************/

    /*Deletes the user from user tree*/
    users_p=deleteUser(users_p,find->uid);
    /***********************/
	return 1;
}

/**
 * @brief Get the history of the user tweets
 *
 * @param uid The id of the user account
 * 
 * @return  1 on success
 *          0 on failure
 */
int history(int uid, int timestamp1, int timestamp2)
{
    /*Creating some helpful pointers*/
    struct user *find;
    struct tweet *head;
    /***********************/

    /*Checkinh if history is not possible*/
    if(!is_user_exist(users_p,uid)) return 0;
    if(timestamp2<=timestamp1) return 0;
    /***********************/

    /*Seaching for the user and the first leaf of his wall tree*/
    find = findUser(users_p,uid);
    find_first_leaf(find->wall_head);
    flag=0;
    /***********************/

    /*Searching for leaf in the list that is in history and prints it*/
    head=leaf;
    printf("  User =");
    while(head!=NULL){
       if(timestamp1<=head->timestamp && head->timestamp<=timestamp2){
         printf(" <%d:%d:%d> ",head->tid,head->uid,head->timestamp);
         if(head->next!=NULL)printf(",");
       }
       head=head->next;
    }
    printf("\nDONE\n");
    leaf=NULL;
    /***********************/

    return 1;
}

/**
 * @brief Lookup tweet the tweets hash table.
 * 
 * @param tid The tweet's id
 *
 * @return 1 on success
 *         0 on failure
 */
int lookup(int tid)
{
  /*Searching the hash table for a non removed tweet with tid*/
  int i;
  for(i=0;i<max_tweets_g;i++){
    if(tweetHashTable[i].tid==tid && tweetHashTable[i].remove==0)
      printf("  Tweet = <%d:%d:%d>\n",tweetHashTable[i].tid,tweetHashTable[i].uid,tweetHashTable[i].timestamp);
  }  
  /***********************/
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
    /*Checking if print users is not possible*/
    if(users_p==NULL) return 0;
    /***********************/

    /*Printing all tweets and follower for each user*/
    inOrder_Users(users_p);
    printf("DONE\n");
    /***********************/
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
    int i,count=0;
    for(i=0;i<max_tweets_g;i++){
      if(tweetHashTable[i].timestamp!=-1 &&  tweetHashTable[i].remove==0){
        printf("  Tweet%d = <%d:%d:%d> \n",count,tweetHashTable[i].tid,tweetHashTable[i].uid,tweetHashTable[i].timestamp);
        count++;
      }
        
    }  
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
    //DELETE_EVERYTHING(users_p);
    return 1;
}

/**********************************************************************/
/**********************************************************************/
/*********************FUNCTIONS FOR USERS TREE*************************/

/*Prints all users from users tree in order
*USE FOR: REGISTER_USER,TWEET,UNSUBSCRIBE
*Returns 0 for failure , 1 for success */
int print_User_Tree(struct user *head,int tweetsTOO){
    if(head==users_s) return 0;
    struct user *find;
    print_User_Tree(head->lc,tweetsTOO);
    if(tweetsTOO==0){ 
        printf(" <%d> ", head->uid);
        if(head->uid!=maxuser) printf(",");
    }
    else{
        find=findUser(head,head->uid);
        printf("\n  User<%d> =", head->uid);
        print_Wall_Tree(find->wall_head);
    }
    print_User_Tree(head->rc,tweetsTOO);
    return 1;
}

/*Prints all user's tweets and followers for each user in order
*USE FOR: PRINT_USERS
*Returns 0 for failure , 1 for success */
int inOrder_Users(struct user *head){
    if (head==users_s) return 0;
    inOrder_Users(head->lc);
    printf("  User = %d\n    Followers: ",head->uid);
    print_Followers_Tree(head->followers);
    printf("\n    Tweets: ");
    print_Wall_Tree(head->wall_head);
    printf("\n");
    inOrder_Users(head->rc);
    return 1;
}

/*Checks if user exist or not
*USE FOR: REGISTER_USER,SUBSCRIBE,TWEET,UNSUBSCRIBE,DELETE,HISTORY
*Returns 0 for true , other numbers for false */
int is_user_exist(struct user *head,int uid){
    if(head==users_s) return 0;
    if(head->uid==uid) return 1;
    return is_user_exist(head->lc,uid) + is_user_exist(head->rc,uid);
}

/*Searching in order for a user and deletes him
*USE FOR: DELETE
* Returns the user in the deleted user's place*/
struct user *deleteUser(struct user *root,int uid){
   
    if(root==users_s) return root;
    if(uid < root->uid )
        root->lc=deleteUser(root->lc,uid);
    else if(uid > root->uid)
        root->rc=deleteUser(root->rc,uid);
    else{
        if(root->lc==users_s && root->rc==users_s){
           free(root);
           root=users_s;
           return root;
        }
        else if(root->lc==users_s){
            struct user *tempuser=root->rc;
            tempuser->par=root->par;
            free(root);
            return tempuser;

        }
        else if(root->rc==users_s){
            struct user *tempuser=root->lc;
            tempuser->par=root->par;
            free(root);
            return tempuser;
        }
           struct user* tempuser=minimumUser(root->rc);
           root->uid=tempuser->uid;
           root->rc=deleteUser(root->rc,tempuser->uid);
    }
    return root;
}

/*Search for the minimum user
*USE FOR: deleteUser
* Returns the most left user*/
struct user *minimumUser(struct user *users){
 struct user* current = users;
    while (current->lc != NULL) {
    current = current->lc;
  }
  return current;
}
 
/*Search for a user
*USE FOR: REGISTER_USER,SUBSCRIBE,TWEET,UNSUBSCRIBE,DELETE,HISTORY
*Returns the found user */
struct user *findUser(struct user *head,int uid){
    users_s->uid=uid;
    while(head->uid!=uid){
        if(head->uid > uid) head=head->lc;
        else head=head->rc;
    }
    if(head!=users_s) return head;
    else return NULL;
}

struct user * inOrderSuccessor(struct user *root, struct user *next)
{
  if( next->rc != NULL )
    return minimumUser(next->rc);
 
  // step 2 of the above algorithm
  struct user *parent = next->par;
  while(parent != NULL && next == parent->rc)
  {
     next = parent;
     parent = parent->par;
  }
  return parent;
}

int DELETE_EVERYTHING(struct user *root){
    if (root==NULL) return 0;
    DELETE_EVERYTHING(root->lc);
    deleteAllTweets(root->wall_head);
    freeAllFollowers(root->followers);
    free(root);
    DELETE_EVERYTHING(root->rc);
    return 1;
}
 
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

/**********************************************************************/
/*******************FUNCTIONS FOR FOLLOWERS TREE***********************/

/*Prints all users from users tree in order
*USE FOR: inOrder_Users, SUBSCRIBE
*Returns 0 for failure , 1 for success */
int print_Followers_Tree(struct follower *head){
    if(head==NULL) return 0;
    print_Followers_Tree(head->lc);
    printf(" <%d> ", head->uid);
    print_Followers_Tree(head->rc);
    return 1;
}

/*Search each follower in order,find the user with inorder successor
*adding the tweet on his wall and update the leaf oriented tree
*USE FOR: TWEET
*Returns 0 for failure , 1 for success */
int Add_Tweet_to_Followers(struct follower *v,int uid,int tid,int timestamp){
    struct user *find=NULL;
    if(v==NULL) return 0;
    
    Add_Tweet_to_Followers(v->lc,uid,tid,timestamp);

    find=findUser(users_p,v->uid);
    find->wall_head=insertTweet(find->wall_head,uid,tid,timestamp);
    update_leaforiented_tree(find->wall_head);
    prevtweet=NULL;
    
    Add_Tweet_to_Followers(v->rc,uid,tid,timestamp);
    
    return 1;
    
}

/*Searching in order for a follower and deletes him
*USE FOR: UNSUBSCRIBE
* Returns the follower in the deleted follower's place*/
struct follower *deleteFollower(struct follower *follow,int uid){
    if(follow==NULL) return follow;
    if(uid < follow->uid )
        follow->lc=deleteFollower(follow->lc,uid);
    else if(uid > follow->uid)
        follow->rc=deleteFollower(follow->rc,uid);
    else{

        if(follow->lc==NULL){
            struct follower *tempfollow=follow->rc;
            free(follow);
            return tempfollow;
        }
        else if(follow->rc==NULL){
            struct follower *tempfollow=follow->lc;
            free(follow);
            return tempfollow;
        }
        
           struct follower* tempfollow=minFollower(follow->rc);
           follow->uid=tempfollow->uid;
           follow->rc=deleteFollower(follow->rc,tempfollow->uid);
        
    }
    return follow;
}

/*Search for the minimum key number
*USE FOR: deleteFollower
* Returns the minimum follower uid*/
struct follower *minFollower(struct follower *follow){
    struct follower *head=follow;
    while(head->lc!=NULL)
        head=head->lc;

    return head;
    
}

/*Searching in order every follower and deletes him
*USE FOR: DELETE
*Returns 0 for failure , 1 for success */
int UnsubscribeAllFollowers(struct follower *follow,int uid){
    if(follow==NULL) return 0;
    UnsubscribeAllFollowers(follow->lc,uid);

    unsubscribe(uid,follow->uid);

    UnsubscribeAllFollowers(follow->rc,uid);
    return 1;
}

int freeAllFollowers(struct follower *root){
   if(root==NULL) return 0;
    freeAllFollowers(root->lc);
    free(root);
    freeAllFollowers(root->rc);
    return 1;
}
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/


/**********************************************************************/
/************************FUNCTIONS FOR TWEETS**************************/


/*Prints all tweets from wakk tree in order
*USE FOR: inOrder_Users, print_User_Tree
*Returns 0 for failure , 1 for success */
int print_Wall_Tree(struct tweet *root){
   if(root==NULL) return 0;
   struct tweet *head;
   find_first_leaf(root);
   flag=0;
   head=leaf;
   while(head!=NULL){
       printf(" <%d:%d> ",head->tid,head->uid);
       if(head->next!=NULL) printf(",");
       head=head->next;
   }
   leaf=NULL;
    return 1;
}

/*Updates the leaf oriented tree
*USE FOR: TWEET,UNSUBSCRIBE,InOrder_Followers
*Returns 0 for failure , 1 for success */
int update_leaforiented_tree(struct tweet *root){
        if (root==NULL)
            return 0;

        update_leaforiented_tree(root->lc);
        if (root->lc==NULL && root->rc==NULL){
             if (prevtweet!=NULL)
                  prevtweet->next=root;
             prevtweet=root;
        }
        else{
            root->next=NULL;
        }
        update_leaforiented_tree(root->rc);
        return 1;
}

/*Searching for the wall tweet tree first tree
*USE FOR: HISTORY */
int find_first_leaf(struct tweet *search){
    if(search==NULL) return 0;
    find_first_leaf(search->lc);

     if(search->lc==NULL && search->rc==NULL && flag==0){
         leaf=search;
         flag=1;
     }
     find_first_leaf(search->rc);
     return 1;
}

/*Counts all the tweet nodes of a tree
*USE FOR: UNSUBSCRIBE
* Returns the total nodes */
int CountTweets(struct tweet *tweets){
   int c=1;
   if(tweets==NULL) return 0;
   else{
       c+=CountTweets(tweets->lc);
       c+=CountTweets(tweets->rc);
       return c;
   }
}  

/*Searching in order for a tweet and deletes him
*USE FOR: UNSUBSCRIBE
* Returns the tweet in the deleted wall's place*/
struct tweet *deleteTweet(struct tweet *root,int uid){
    if (root == NULL) return root;

    root->lc = deleteTweet(root->lc, uid);

    if(root->uid==uid)
    {
        if(root->lc == NULL && root->rc == NULL){
            root=NULL;
            return root;
        }
        else if (root->lc == NULL)
        {
            struct tweet *temp = root->rc;
            free(root);
            return temp;
        }
        else if (root->rc == NULL)
        {
            struct tweet *temp = root->lc;
            free(root);
            return temp;
        }
        else {
            struct tweet* temp = minTweet(root->rc);
            root->timestamp = temp->timestamp;
            root->uid = temp->uid;
            root->tid = temp->tid;
            free(temp);
        }
    }
    root->rc = deleteTweet(root->rc, uid);
    return root;
    
}

/*Search for the minimum tweet
* USE FOR: deleteTweet
* Returns the most left tweet*/
struct tweet *minTweet(struct tweet *tweets){
    struct tweet *head=tweets;
    while(head->lc!=NULL)
        head=head->lc;

    return head;
    
}

/*Free all the tweets
* USE FOR: DELETE */
int deleteAllTweets(struct tweet *root){
    if(root==NULL) return 0;
    deleteAllTweets(root->lc);
    free(root);
    deleteAllTweets(root->rc);
    return 1;
}

/*Inserts a new tweet into tweets wall leaf oriented tree
* USE FOR: TWEET,InOrder_Followers
* Returns the inserted tweet*/
struct tweet *insertTweet(struct tweet* v, int uid,int tid,int timestamp)
{

    if(v==NULL){
        struct tweet* new = (struct tweet *)malloc(sizeof(struct tweet));
        new->tid=tid;
        new->uid=uid;
        new->timestamp=timestamp;
        new->lc=NULL;
        new->rc=NULL;
        new->next=NULL;
        return new;
    }

     if (timestamp <= v->timestamp){
        if(v->lc==NULL && v->rc==NULL){
          v->rc=insertTweet(v->rc,v->uid,v->tid,v->timestamp);
        }
           
        v->lc  = insertTweet(v->lc,uid,tid,timestamp);
    }
    else if (timestamp > v->timestamp){
        if(v->lc==NULL && v->rc==NULL){
          v->lc=insertTweet(v->lc,v->uid,v->tid,v->timestamp);
        }
        v->rc = insertTweet(v->rc,uid,tid,timestamp);   
    } 

    return v;
}
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

/**********************************************************************/
/*********************FUNCTIONS FOR HASHTABLE**************************/

void insertHashTweet(int uid,int tid,int timestamp){

}
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

