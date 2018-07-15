#ifndef __HW05__
#define __HW05__ 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include  <string.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <stdbool.h>
#include <sys/time.h>


//------------STRUCTURES---------------------------
#define MAX 10000
#define CENTRALFLAG 1
#define FLORIST1 2
#define FLORIST2 3
#define FLORIST3 4
bool OUTFLAG = false;

FILE *fp;
int NUMOFCLIENTS = 0;

typedef struct Client
{
	int tid;
	char name[11];
	int coorx;
	int coory;
	char flower[15];
}Client;

typedef struct Florist
{
	char name[11];
	int coorx;
	int coory;
	double click;
	char flower1[15];
	char flower2[15];
	char flower3[15];
	int queuevalue;
}Florist;

typedef struct Central
{
	Florist florists[3];
	Client Clients[100];

}Central; 

typedef struct Queue
{
	Client array[MAX];
	int rear;
	int front;
	int itemCount;

}Queue;

typedef struct Result
{
	int sales;
	int totaltime;
}Result;


//------------VARIABLES---------------------------

//structurs
Central central;
Florist florists[3];
Client *clients;
Queue centQ, f1Q, f2Q, f3Q;
Result f1_res, f2_res, f3_res;
int f1_delivsize, f2_delivsize,f3_delivsize;
int ret_mutex;



pthread_t centid, //central thread;
			ftid[3], //florist threads
			 *ctid; //clients threads
pthread_mutex_t mutex, mutexforQ;
pthread_cond_t cond, condforQ; 

pthread_mutex_t f1_mutex, f2_mutex, f3_mutex;
pthread_cond_t f1_cond, f2_cond, f3_cond;



//--------------METHODS-----------------------------

void produceClients(int numberofclients);
void readfile(const char *filename);
int calculateDistance(int x1, int y1, int x2, int y2);


void* centralthread(void *arg);
void* floriststhread(void *arg);
void* clientsthread(void* arg);

// QUEUE METHODS
//flag determine which queue is
void insert(Client data, int flag);
Client removeData(int flag);
int size(int flag);
bool isFull(int flag);
bool isEmpty(int flag);
Client peek(int flag);
int initiliazeQueues();
void treadcreater();
long getMicrotime();

#endif