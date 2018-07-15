#include "hw.h"

long getMicrotime(){
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);
	return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
}

int main(int argc, char const *argv[])
{
    int i = 0;
    int err;
    int count=0;
	int temp = 10;

	if (argc != 2)
    {
        printf("USAGE: ./floristApp filename.dat\n");
        exit(-1);
    }


	printf("Florist application initializing from file: %s\n",argv[1]);

	initiliazeQueues();
   	readfile(argv[1]);
 

    treadcreater();

    printf("---------------------------------------------------------\n");
    printf("Florist\t\t# of sales\t\tTotal time\n");
    printf("---------------------------------------------------------\n");
    printf("%s\t\t%d\t\t\t%d\n",florists[0].name, f1_res.sales, f1_res.totaltime);
    printf("%s\t\t%d\t\t\t%d\n",florists[1].name, f2_res.sales, f2_res.totaltime);
    printf("%s\t\t%d\t\t\t%d\n",florists[2].name, f3_res.sales, f3_res.totaltime);
    printf("----------------------------------------------------------\n");


    pthread_mutex_destroy(&mutex);
   	pthread_cond_destroy(&cond);
   	pthread_mutex_destroy(&mutexforQ);
   	pthread_cond_destroy(&condforQ);

   	pthread_mutex_destroy(&f1_mutex);
   	pthread_cond_destroy(&f1_cond);

	pthread_mutex_destroy(&f2_mutex);
   	pthread_cond_destroy(&f2_cond);

	pthread_mutex_destroy(&f3_mutex);
   	pthread_cond_destroy(&f3_cond);

	free(clients);
	free(ctid);
    return 0;
}

void* centralthread(void *arg)
{
	int i = 0;
	Client cl;
	int closest;



	while(i < NUMOFCLIENTS){

		while(isEmpty(CENTRALFLAG)){
			pthread_cond_wait(&condforQ, &mutexforQ);
		}

		pthread_mutex_trylock(&mutexforQ);

		cl = removeData(CENTRALFLAG);

		pthread_mutex_unlock(&mutexforQ);

		/*printf("size: %d,self: %d TID: %d name: %s coorx: %d coory: %d flower: %s\n",size(CENTRALFLAG),
			(int) pthread_self(), centQ.array[i].tid,
	    		centQ.array[i].name, centQ.array[i].coorx,
	    		 centQ.array[i].coory,centQ.array[i].flower);*/

		closest = closestflorist(cl);
		//printf("size: %d, %s florist \n",size(CENTRALFLAG),cl.name );
		//printf("closest: %d\n", closest);

		if (closest == 0)
		{
			++f1_delivsize;
			ret_mutex = pthread_mutex_lock(&f1_mutex);
			if (ret_mutex == -1)
			{
				printf("ERROR MUTEX LOCK EXIT(-1)\n");
				exit(-1);
			}

			insert(cl, FLORIST1);

			pthread_cond_signal(&f1_cond);

			pthread_mutex_unlock(&f1_mutex);
		}
		if (closest == 1)
		{
			++f2_delivsize;

			ret_mutex = pthread_mutex_lock(&f2_mutex);
			if (ret_mutex == -1)
			{
				printf("ERROR MUTEX LOCK EXIT(-1)\n");
				exit(-1);
			}
	
			insert(cl, FLORIST2);

			pthread_cond_signal(&f2_cond);

			pthread_mutex_unlock(&f2_mutex);

		}
		if (closest == 2)
		{
			++f3_delivsize;

			ret_mutex = pthread_mutex_lock(&f3_mutex);
			if (ret_mutex == -1)
			{
				printf("ERROR MUTEX LOCK EXIT(-1)\n");
				exit(-1);
			}

			insert(cl, FLORIST3);

			pthread_cond_signal(&f3_cond);

			pthread_mutex_unlock(&f3_mutex);
		}


		++i;

	}

	pthread_cond_broadcast(&f1_cond);
	OUTFLAG = true;
	pthread_cond_broadcast(&f2_cond);
	OUTFLAG = true;
	pthread_cond_broadcast(&f3_cond);
	OUTFLAG = true;

	pthread_exit(NULL);
}

void* floriststhread(void *arg){

	Florist flor = *(Florist *)arg;
	int index = flor.queuevalue;
	Client cl;
	int i = 0;
	long t1,t2;
	long diff=0;

	if (index == 0)
	{
		i=0;
		while(1){
			t1 = getMicrotime();
			usleep(florists[0].click*1000); //deliver time
			usleep(12*1000); // preparing time

			while(isEmpty(FLORIST1) && OUTFLAG == false){
				pthread_cond_wait(&f1_cond, &f1_mutex);
			}
			if (OUTFLAG == true && i == f1_delivsize)
			{
				break;
			}

			pthread_mutex_trylock(&f1_mutex);
			cl = removeData(FLORIST1);
			t2 = getMicrotime();
			diff = t2-t1;
			f1_res.totaltime += diff;
			printf("Florist %s has delivered a %s to %s in %lums\n",flor.name, cl.flower, cl.name, diff );
			++i;
			pthread_mutex_unlock(&f1_mutex);

			
		}
		f1_res.sales = f1_delivsize;
	}
	if (index == 1)
	{
		i=0;
		while(1){
			t1 = getMicrotime();
			usleep(florists[1].click*1000);
			usleep(15*1000); // preparing time

			while(isEmpty(FLORIST2) && OUTFLAG == false){
				pthread_cond_wait(&f2_cond, &f2_mutex);
			}
			if (OUTFLAG == true && i == f2_delivsize)
			{
				break;
			}

			pthread_mutex_trylock(&f2_mutex);
			cl = removeData(FLORIST2);
			t2 = getMicrotime();
			diff = t2-t1;
			f2_res.totaltime += diff;
			printf("Florist %s has delivered a %s to %s in %lums\n",flor.name, cl.flower, cl.name, diff );
			++i;
			pthread_mutex_unlock(&f2_mutex);
			
		}
		f2_res.sales = f2_delivsize;
	}
	if (index == 2)
	{
		i = 0;
		while(1){
			t1 = getMicrotime();
			usleep(florists[2].click*1000);
			usleep(20*1000); // preparing time
			while(isEmpty(FLORIST3) && OUTFLAG == false){
				pthread_cond_wait(&f3_cond, &f3_mutex);
			}
			if (OUTFLAG == true && i == f3_delivsize)
			{
				break;
			}

			pthread_mutex_trylock(&f3_mutex);
			cl = removeData(FLORIST3);
			t2 = getMicrotime();
			diff = t2-t1;
			f3_res.totaltime += diff;
			printf("Florist %s has delivered a %s to %s in %lums\n",flor.name, cl.flower, cl.name, diff );
			++i;
			pthread_mutex_unlock(&f3_mutex);
			
		}
		f3_res.sales = f3_delivsize;
	}

	pthread_exit(NULL);

}

void* clientsthread(void* arg){

	Client client = *(Client*)arg;

	client.tid = (int) pthread_self();

	/*printf("self: %d TID: %d name: %s coorx: %d coory: %d flower: %s\n",(int) pthread_self(), client.tid,
	    		client.name, client.coorx, client.coory,client.flower);*/

	ret_mutex = pthread_mutex_lock(&mutex);
	if (ret_mutex == -1)
	{
		printf("ERROR MUTEX LOCK EXIT(-1)\n");
		exit(-1);
	}

	ret_mutex = pthread_mutex_lock(&mutexforQ);
	if (ret_mutex == -1)
	{
		printf("ERROR MUTEX LOCK EXIT(-1)\n");
		exit(-1);
	}

	insert(client, CENTRALFLAG);

	pthread_cond_signal(&condforQ);

	pthread_mutex_unlock(&mutexforQ);

	pthread_mutex_unlock(&mutex);


	pthread_exit(NULL);

}




int closestflorist(Client cl){

	int f1, f2, f3, min,prev, closest = -1;
	bool flowerflag = false, flowerflag2 = false;
	int i = 0;

	f1 = calculateDistance(florists[0].coorx, florists[0].coory, cl.coorx, cl.coory);
	f2 = calculateDistance(florists[1].coorx, florists[1].coory, cl.coorx, cl.coory);
	f3 = calculateDistance(florists[2].coorx, florists[2].coory, cl.coorx, cl.coory);

	min = f1;
	closest = 0; //florist 1
	prev = 0;

	if (f2<min)
	{
		
		if (strcmp(cl.flower, florists[1].flower1)==0)
		{
			flowerflag = true;
		}
		if (strcmp(cl.flower, florists[1].flower2)==0)
		{
			flowerflag = true;
		}
		if (strcmp(cl.flower, florists[1].flower3)==0)
		{
			flowerflag = true;
		}
		if (flowerflag == true)
		{
			min = f2;
			closest = 1;//florist 2
		}
		

	}
	if (f3<min)
	{
		//printf("f1: %s f2: %s f3: %s\n", florists[2].flower1,florists[2].flower2,florists[2].flower3);
		if (strcmp(cl.flower, florists[2].flower1)==0)
		{
			flowerflag2 = true;
		}
		if (strcmp(cl.flower, florists[2].flower2) == 0)
		{
			flowerflag2 = true;
		}
		if (strcmp(cl.flower, florists[2].flower3)==0)
		{
			flowerflag2 = true;
		}
		if (flowerflag2 == true)
		{
			min = f3;
			closest = 2;//florist 3
		}
		
	}
	return closest;

}

//uzaklık hesaplar.
int calculateDistance(int x1, int y1, int x2, int y2)
{  
	int diffx = x1 - x2;
	int diffy = y1 - y2;
	int diffx_sqr = pow(diffx,2);
	int diffy_sqr = pow(diffy,2);
	int distance = sqrt(diffx_sqr + diffy_sqr);
	return distance;

}


void readfile(const char *filename){
	char line[50];
	int count=0, numberofclients=0, cl_index = 0;//client index;
	char *token;

	fp = fopen(filename,"r");
    if (fp == NULL)
    {
    	perror("fopen");
    	pthread_exit(NULL);
    }

    memset(line,0,0);
    while(fgets(line, 50, fp)!=NULL)
    {
    	++count;
    }

    fclose(fp);
    numberofclients = count-4;
    //Burada clinetlar üretilir.
    produceClients(numberofclients);

    fp = fopen("data.dat","r");
    if (fp == NULL)
    {
    	perror("fopen");
    	pthread_exit(NULL);
    }

    memset(line,0,0);
    count = 0;
    while(fgets(line, 50, fp)!=NULL )
    {
    	if (count <3) //çiçekçileri parse edilir.
    	{
    		
	     	token = strtok(line, " ;,:()");
	    	//printf("name: %s\n",token );
	    	strcpy(florists[count].name, token);

	    	token = strtok(NULL, " ;,:()");
	    	//printf("CoorX %d\n",atoi(token));
			florists[count].coorx = atoi(token);

	    	token = strtok(NULL, " ;,:()");
	    	//printf("CoorY %d\n", atoi(token));
	    	florists[count].coory = atoi(token);

	    	token = strtok(NULL, " ;,:()");
	    	//printf("Click %.1f\n",atof(token) );
	    	florists[count].click = atof(token);

	    	token = strtok(NULL, " ;,:()");
	    	//printf("flower1 %s\n", token );
	    	strcpy(florists[count].flower1, token);

	    	token = strtok(NULL, " ;,:()");
	    	//printf("flower2 %s\n", token );
	    	strcpy(florists[count].flower2, token);

	    	token = strtok(NULL, " ;,:()\n");
	    	//printf("flower3 %s\n", token );
	    	strcpy(florists[count].flower3, token);
	    	

	    	/*printf("name: %s coorx: %d coory: %d click: %.1f flower1: %s flower2: %s flower3: %s\n", 
	    		florists[count].name, florists[count].coorx, florists[count].coory, florists[count].click,
	    		florists[count].flower1, florists[count].flower2, florists[count].flower3);*/
    	}
    	if (count>=4)
    	{
    		cl_index = count - 4;
    		token = strtok(line, " ;,:()");
    		strcpy(clients[cl_index].name, token);

    		token = strtok(NULL, " ;,:()");
    		clients[cl_index].coorx = atoi(token);

    		token = strtok(NULL, " ;,:()");
    		clients[cl_index].coory = atoi(token);

    		token = strtok(NULL, " ;,:()\n");
    		strcpy(clients[cl_index].flower, token);

    		/*printf("name: %s coorx: %d coory: %d flower: %s\n", 
	    		clients[cl_index].name, clients[cl_index].coorx, clients[cl_index].coory,
	    		clients[cl_index].flower);*/

    	}

    	memset(line,0,0);
    	++count;
    }

    fclose(fp);

}

void produceClients(int numberofclients){

	NUMOFCLIENTS = numberofclients;
	clients = calloc(numberofclients, sizeof(Client));
	if (clients == NULL)
	{
		perror("calloc");
		exit(-1);
	}

	ctid = calloc(numberofclients, sizeof(pthread_t));
	if (ctid == NULL)
	{
		perror("calloc");
		exit(-1);
	}
}

// QUEUE METHODS
Client peek(int flag) {
	int fr = 0;

	if (flag == CENTRALFLAG)
	{
		fr = centQ.front;

		return centQ.array[fr];
	}else if (flag == FLORIST1){
		fr = f1Q.front;
		return f1Q.array[fr];
	}else if (flag == FLORIST2){
		fr = f2Q.front;
		return f2Q.array[fr];
	}
	else if (flag == FLORIST3){
		fr = f3Q.front;
		return f3Q.array[fr];
	}
}

bool isEmpty(int flag) {
	if (flag == CENTRALFLAG)
	{
		return centQ.itemCount == 0;
	}else if (flag == FLORIST1){
		return f1Q.itemCount == 0;
	}else if (flag == FLORIST2){
		return f2Q.itemCount == 0;
	}
	else if (flag == FLORIST3){
		return f3Q.itemCount == 0;
	}else return false;

}

bool isFull(int flag) {
	if (flag == CENTRALFLAG)
	{
		return centQ.itemCount == MAX;
	}else if (flag == FLORIST1){
		return f1Q.itemCount == MAX;
	}else if (flag == FLORIST2){
		return f2Q.itemCount == MAX;
	}
	else if (flag == FLORIST3){
		return f3Q.itemCount == MAX;
	}else return false;

}

int size(int flag) {
	if (flag == CENTRALFLAG)
	{
		return centQ.itemCount;
	}else if (flag == FLORIST1){
		return f1Q.itemCount;
	}else if (flag == FLORIST2){
		return f2Q.itemCount;
	}
	else if (flag == FLORIST3){
		return f3Q.itemCount;
	}else return -1;

}
void insert(Client data, int flag) {

	int index=0;


	if (flag == CENTRALFLAG)
	{
		if(!isFull(flag)) {
	
			if(centQ.rear == MAX-1) {
				printf("insert\n");
			 	centQ.rear = -1;            
			}   


			index = ++centQ.rear;

			centQ.array[index] = data;

			centQ.itemCount++;

		}
	}else if (flag == FLORIST1){
		if(!isFull(flag)) {
	
			if(f1Q.rear == MAX-1) {
			 	f1Q.rear = -1;            
			}
			
			index = ++f1Q.rear;
			f1Q.array[index] = data;
			f1Q.itemCount++;
		}
	}
	else if (flag == FLORIST2)
	{
		if(!isFull(flag)) {
	
			if(f2Q.rear == MAX-1) {
			 	f2Q.rear = -1;            
			}       
			
			index = ++f2Q.rear;

			f2Q.array[index] = data;

			f2Q.itemCount++;

		}
	}
	else if (flag == FLORIST3)
	{
		if(!isFull(flag)) {
	
			if(f3Q.rear == MAX-1) {
			 	f3Q.rear = -1;            
			}       

		
			index = ++f3Q.rear;

			f3Q.array[index] = data;

			f3Q.itemCount++;

		}
	}else printf("ERROR INSERT QUEUE\n");

   
}
Client removeData(int flag) {
   Client data;
	
	if (flag == CENTRALFLAG)
	{
		data = centQ.array[centQ.front++];
		
		if(centQ.front == MAX) {
			centQ.front = 0;
		}
		centQ.itemCount--;
		return data;
	}
	else if (flag == FLORIST1)
	{
		data = f1Q.array[f1Q.front++];

		if(f1Q.front == MAX) {
			f1Q.front = 0;
		}
		f1Q.itemCount--;
		return data;
	}
	else if (flag == FLORIST2)
	{
		data = f2Q.array[f2Q.front++];
	
		if(f2Q.front == MAX) {
			f2Q.front = 0;
		}
		f2Q.itemCount--;
		return data;
	}
	else if (flag == FLORIST3)
	{
		data = f3Q.array[f3Q.front++];
		if(f3Q.front == MAX) {
			f3Q.front = 0;
		}
		f3Q.itemCount--;
		return data;
	}else{ 
		printf("ERROR DELETE QUEUE\n");
		exit(-1);
	}
}
int initiliazeQueues(){
	centQ.front = 0;
	centQ.rear = -1;
	centQ.itemCount = 0;

	f1Q.front = 0;
	f1Q.rear = -1;
	f1Q.itemCount = 0;

	f2Q.front = 0;
	f2Q.rear = -1;
	f2Q.itemCount = 0;

	f3Q.front = 0;
	f3Q.rear = -1;
	f3Q.itemCount = 0;

	f1_delivsize = 0;
	f2_delivsize = 0;
	f3_delivsize = 0;

	 if (pthread_mutex_init(&mutex, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }

	if (pthread_cond_init(&cond, NULL) != 0) {                                    
		perror("pthread_cond_init() error");                                        
		exit(-1);
	} 

	if (pthread_mutex_init(&mutexforQ, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }

	if (pthread_cond_init(&condforQ, NULL) != 0) {                                    
		perror("pthread_cond_init() error");                                        
		exit(-1);
	} 

	if (pthread_mutex_init(&f1_mutex, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }

	if (pthread_cond_init(&f1_cond, NULL) != 0) {                                    
		perror("pthread_cond_init() error");                                        
		exit(-1);
	} 

	if (pthread_mutex_init(&f2_mutex, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }

	if (pthread_cond_init(&f2_cond, NULL) != 0) {                                    
		perror("pthread_cond_init() error");                                        
		exit(-1);
	} 


	if (pthread_mutex_init(&f3_mutex, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }

	if (pthread_cond_init(&f3_cond, NULL) != 0) {                                    
		perror("pthread_cond_init() error");                                        
		exit(-1);
	} 

}
//END QUEUE METHODS

void treadcreater(){
	int i = 0;
    int err;
    int count=0;
	int temp = 10; 
	//CREATE CENTRAL THREAD
   	err = pthread_create(&centid, NULL, &centralthread, NULL);
	if (err != 0){

	    printf("\ncan't create thread :[%s]\n", strerror(err));
	    exit(-1);
	}

	//CREATE FLORIST THREAD
	i = 0;
	while(i<3){

		florists[i].queuevalue = i;
		err = pthread_create(&ftid[i], NULL, &floriststhread, (void*)&florists[i]);
		if (err != 0){
		    perror("pthread_create");
		    exit(-1);
		}
		++i;
	}
	printf("3 florists have been created\n");

	//CREATE CLIENTS THREAD
	i=0;
	while(i<NUMOFCLIENTS){
		err = pthread_create(&ctid[i], NULL, &clientsthread, (void*)&clients[i]);
		if (err != 0){
		    perror("pthread_create");
		    exit(-1);
		}
		++i;

	}

	

	pthread_join(centid, NULL);
	for (i = 0; i < 3; ++i)
	{
		pthread_join(ftid[i], NULL);
	}

	for (i = 0; i < NUMOFCLIENTS; ++i)
	{
		pthread_join(ctid[i], NULL);
	}


}