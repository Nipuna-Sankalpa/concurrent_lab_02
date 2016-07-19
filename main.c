#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include "timer.h"

static float const memberFunction_count=9900;
static float const insertFunction_count=50;
static float const deleteFunction_count=50;

struct node
{
    int value;
    struct node *next;
};
struct node * head=NULL;

int memberFunctionCount;
int insertFunctionCount;
int deleteFunctionCount;
int totalOps;

pthread_mutex_t mutexList,mutexTotalOps;
pthread_rwlockattr_t rwlock;

int member_ratio=990;
int insert_ratio=5;
int delete_ratio=5;

void *threadOperations();
struct node * init();
bool insert_node(int val,int thread_id);
bool delete_node(int val,int thread_id);
struct node * member(int val,int thread_id);
void * executeThreads(void * rank);

int main()
{
    double start, finish;
    pthread_t * thread;
    //pthread_mutex_init(&mutexList,NULL);
    pthread_mutex_init(&mutexTotalOps,NULL);
    pthread_rwlock_init(&rwlock,NULL);

    memberFunctionCount=memberFunction_count;
    insertFunctionCount=insertFunction_count;
    deleteFunctionCount=deleteFunction_count;
    totalOps=memberFunction_count+insertFunction_count+deleteFunction_count;

    int i=0,numberOfThreads=4;

    init();
    GET_TIME(start);

    thread=malloc(numberOfThreads * sizeof(pthread_t));

    for(i=0; i<numberOfThreads; i++)
    {
        pthread_create(&thread[i],NULL,executeThreads,(void *) i);
    }

    printf("threads created\n");

    for(i=0; i<numberOfThreads; i++)
    {
        pthread_join(thread[i],NULL);
    }

    GET_TIME(finish);
    printf("Time Taken to Complete Task: %f\n",finish - start);


    free(thread);
    return 0;
}

struct node * init()
{
    int i=0;
    head=(struct node *)malloc(sizeof(struct node));
    head->next=NULL;
    head->value=NULL;
    struct node * node;
    for(i=0; i<1000; i++)
    {
        node=(struct node *)malloc(sizeof(struct node));
        node->value=i;
        node->next=head->next;
        head->next=node;
    }
    return head;
}

bool insert_node(int val,int thread_id)
{
    if(insertFunctionCount <= 0)
    {
        return false;
    }
    insertFunctionCount--;
    printf("insert|Thread_id: %d\n",thread_id);
    struct node * node=(struct node *)malloc(sizeof(struct node));
    node->value=val;
    node->next=head->next;
    head->next=node;

    printf("Thread_id: %d inserted Value : %d\n",thread_id,val);
    return true;
}

bool delete_node(int val,int thread_id)
{
    if(deleteFunctionCount <= 0)
    {
        return false;
    }
    deleteFunctionCount--;
    printf("delete|Thread_id: %d\n",thread_id);
    struct node * temp;
    struct node * nodePrev;
    if(head->next == NULL)
    {
        return false;
    }
    else
    {
        temp=head->next;
        nodePrev=head;
    }

    while(temp != NULL && temp->value != val)
    {
        nodePrev=temp;
        temp=temp->next;
    }

    if(temp == NULL)
    {
        return false;
    }
    else
    {
        nodePrev->next=temp->next;
        free(temp);
        printf("Thread_id: %d deleted Value : %d\n",thread_id,val);
        return true;
    }

}

struct node *member(int val,int thread_id)
{

    if(memberFunctionCount <= 0)
    {
        return NULL;
    }
    memberFunctionCount--;
    printf("member|Thread_id: %d\n",thread_id);
    struct node * temp;

    if(head->next == NULL)
        return NULL;

    temp=head->next;
    while(temp->next != NULL)
    {
        if(temp->value == val)
        {
            printf("Thread_id: %d Searched Value : %d\n",thread_id,val);
            return temp;
        }
        else
            temp=temp->next;
    }
    return NULL;
}

void * executeThreads(void *rank)
{
    float mem_limit=member_ratio,insert_limit=member_ratio+insert_ratio,del_limit=insert_limit+delete_ratio;
    srand(time(NULL));
    while(totalOps > 0)
    {
        pthread_mutex_lock(&mutexTotalOps);
        totalOps--;
        pthread_mutex_unlock(&mutexTotalOps);

        if (insertFunctionCount == 0 && memberFunctionCount == 0 && deleteFunctionCount == 0)
        {
            break;
        }

        int rndVal=rand()%1000+1;

        if(memberFunctionCount == 0)
        {
            mem_limit=0;
            if (insertFunctionCount == 0)
            {
                insert_limit=0;
            }
            else if(deleteFunctionCount == 0)
            {
                insert_limit=del_limit;
            }
            else
            {
                insert_limit=1000*(insertFunction_count/(insertFunction_count+deleteFunction_count));
            }

        }
        else if (insertFunctionCount == 0)
        {
            if (memberFunctionCount == 0)
            {
                insert_limit=0;
                mem_limit=0;
            }
            else if(deleteFunctionCount == 0)
            {
                insert_limit=del_limit;
                mem_limit=del_limit;
            }
            else
            {
                mem_limit=1000*(memberFunction_count/(memberFunction_count+deleteFunction_count));
            }

        }
        else if(deleteFunctionCount == 0)
        {
            if (memberFunctionCount == 0)
            {
                insert_limit=del_limit;
                mem_limit=0;
            }
            else if(insertFunctionCount == 0)
            {
                mem_limit=del_limit;
            }
            else
            {
                insert_limit=del_limit;
                mem_limit=1000*(memberFunction_count/(memberFunction_count+insertFunction_count));
            }
        }

        if(rndVal <= mem_limit)
        {
            pthread_rwlock_rdlock(&rwlock);
            //pthread_mutex_lock(&mutexList);
            member(rand()%1500+1,(int)rank);
            //pthread_mutex_unlock(&mutexList);
            pthread_rwlock_unlock(&rwlock);
        }
        else if(rndVal <= insert_limit)
        {
            //pthread_mutex_lock(&mutexList);
            pthread_rwlock_wrlock(&rwlock);
            insert_node(rand()%1000+1001,(int)rank);
            //pthread_mutex_unlock(&mutexList);
            pthread_rwlock_unlock(&rwlock);
        }
        else if(rndVal <= del_limit)
        {
            pthread_rwlock_wrlock(&rwlock);
            //pthread_mutex_lock(&mutexList);
            delete_node(rand()%1500+1,(int)rank);
            //pthread_mutex_unlock(&mutexList);
            pthread_rwlock_unlock(&rwlock);
        }
    }
    return NULL;
}
