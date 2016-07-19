#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

static float const memberFunction_count=10;
static float const insertFunction_count=10;
static float const deleteFunction_count=10;

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

pthread_mutex_t mutexList=PTHREAD_MUTEX_INITIALIZER,mutexTotalOps=PTHREAD_MUTEX_INITIALIZER;
pthread_rwlockattr_t * rwlock;

int member_ratio=990;
int insert_ratio=5;
int delete_ratio=5;

void *threadOperations();
struct node * init();
bool insert_node(int val);
bool delete_node(int val);
struct node * member(int val);
void * executeThreads(void * rank);

int main()
{
    pthread_t * thread;
    pthread_mutex_init(&mutexList,NULL);
    //pthread_mutex_init(&totalThreadCount,NULL);


    memberFunctionCount=memberFunction_count;
    insertFunctionCount=insertFunction_count;
    deleteFunctionCount=deleteFunction_count;
    totalOps=memberFunction_count+insertFunction_count+deleteFunction_count;

    //pthread_rwlockattr_init(&rwlock);

    int i=0,numberOfThreads=2;

    init();


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

bool insert_node(int val)
{
    if(insertFunctionCount <= 0)
    {
        return false;
    }
    insertFunctionCount--;
    printf("insert\n");
    struct node * node=(struct node *)malloc(sizeof(struct node));
    node->value=val;
    node->next=head->next;
    head->next=node;
    return true;
}

bool delete_node(int val)
{
    if(deleteFunctionCount <= 0)
    {
        return false;
    }
    deleteFunctionCount--;
    printf("delete\n");
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
        return true;
    }

}

struct node *member(int val)
{

    if(memberFunctionCount <= 0)
    {
        return NULL;
    }
    memberFunctionCount--;
    printf("member\n");
    struct node * temp;

    if(head->next == NULL)
        return NULL;

    temp=head->next;
    while(temp->next != NULL)
    {
        if(temp->value == val)
            return temp;
        else
            temp=temp->next;
    }
    return NULL;
}

void * executeThreads(void *rank)
{
    clock_t start_time=clock(),end_time;
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
            pthread_mutex_lock(&mutexList);
            member(rand()%1500+1);
            pthread_mutex_unlock(&mutexList);
        }
        else if(rndVal <= insert_limit)
        {
            pthread_mutex_lock(&mutexList);
            insert_node(rand()%1000+1001);
            pthread_mutex_unlock(&mutexList);
        }
        else if(rndVal <= del_limit)
        {
            pthread_mutex_lock(&mutexList);
            delete_node(rand()%1500+1);
            pthread_mutex_unlock(&mutexList);
        }

    }
    end_time=clock();
    printf("Thread_id : %d -> Time : %f\n",(int)rank,(double)(end_time-start_time)/CLOCKS_PER_SEC);
    return NULL;
}
