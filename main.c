#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include<time.h>

struct node
{
    int value;
    struct node *next;
};
struct node * head=NULL;
int memberFunctionCount=10;
int insertFunctionCount=10;
int deleteFunctionCount=10;
pthread_mutex_t * mutex;

int member_ratio=3;
int insert_ratio=3;
int delete_ratio=3;

void *threadOperations();
struct node * init();
bool insert_node(int val);
bool delete_node(struct node * node,struct node * nodePrev);
struct node * member(int val);
void executeThreads(void * rank);

int main()
{
    executeThreads("3");
    return 0;
}

struct node * init()
{
    head=(struct node *)malloc(sizeof(struct node));
    head->next=NULL;
    head->value=NULL;
    return head;
}

bool insert_node(int val)
{
    struct node * node=(struct node *)malloc(sizeof(struct node));
    node->value=val;
    node->next=head->next;
    head->next=node;
    return true;
}

bool delete_node(struct node * node,struct node * nodePrev)
{
    if(node == NULL && nodePrev == NULL)
        return false;

    nodePrev->next=node->next;
    free(node);
    return true;
}

struct node *member(int val)
{
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
    int mem_limit=member_ratio,insert_limit=member_ratio+insert_ratio,del_limit=insert_limit+delete_ratio;

    if (insertFunctionCount == 0 && memberFunctionCount == 0 && deleteFunctionCount == 0)
    {
        return NULL;
    }

    srand(time(NULL));
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
            insert_limit=1000*(insertFunctionCount/(insertFunctionCount+deleteFunctionCount));
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
            mem_limit=1000*(memberFunctionCount/(memberFunctionCount+deleteFunctionCount));
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
            mem_limit=1000*(memberFunctionCount/(memberFunctionCount+insertFunctionCount));
        }
    }



    if(rndVal <= mem_limit)
    {
        member(20);
    }
    else if(rndVal <= insert_limit)
    {
        insert_node(20);
    }
    else if(rndVal <= del_limit)
    {
        delete_node();
    }

}
