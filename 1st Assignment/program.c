#include <stdio.h> 
#include <pthread.h>
#include <unistd.h>
#include "init.h"

pthread_mutex_t lock; 
pthread_cond_t cond= PTHREAD_COND_INITIALIZER; 

int tilefonites = Ntel; //available telephon operators
int i, rc;
int income;	
int seatsleft;
int theater[Nseat]; //seat plan
unsigned int seed;
int ctrans= 0;	//count transactions
float p, sum, m;
struct timespec start, mid, finish;

struct transaction {
	int transid;
	char seats[50];
	int cost;
}transaction;
int bookings[250][250];



void *callfromcustomer(void *x){
	int id = (int *)x;
	int rc;
	int j, sleeptime, nseats;
	char temp[3];
	printf("Hello from pelati: %d\n",id);
	

	while (tilefonites == 0) {
		rc = pthread_mutex_lock(&lock);
		printf("O pelatis %d, den brike diathesimo tilefoniti.
		Blocked...\n", id);
		rc = pthread_cond_wait(&cond, &lock);
	}
	printf("O pelatis %d, eksipiretite.\n",id);
	tilefonites--;
	rc = pthread_mutex_unlock(&lock);
	//sleep(5); //kane kapoia douleia me ton tilefoniti
	rc = pthread_mutex_lock(&lock);
	printf("O pelatis %d eksipiretithike epitixos! \n", id);
	tilefonites++;
	rc = pthread_cond_signal(&cond);
	rc = pthread_mutex_unlock(&lock);

	//stavroula's code
	rc = pthread_mutex_lock(&timecount);	
	clock_gettime(CLOCK_REALTIME, &mid);
	m += mid.tv_sec - start.tv_sec; 
	rc = pthread_mutex_unlock(&timecount); 
	//stavroula's code

	nseats = (rand_r(&seed)%Nseathigh) + Nseatlow;
	sleeptime = (rand_r(&seed)%(Tseathigh-Tseatlow+1)) + Tseatlow;

	sleep(sleeptime);
	pthread_exit(NULL); //return

	
	 while seatsleft<250{
	 	rc = pthread_mutex_lock(&theater);
	 	if (seatsleft + numofseats > 250 ){
	 		printf("Transaction has been canceled. \n");
	 	}
	 	else{
			for (i = 0; i < numofseats; i++) {
					theater[seatsleft] = *tid;
					seatsleft++;
				}
				rc = pthread_mutex_unlock(&theater);
			}
	}
	if (seatsleft=>250){
		printf("Theater is full.\n");
	}

	p = rand_r(&seed)%100;
	//start of payment
	if (p> 90) {
		rc = pthread_mutex_lock(&seatPlan);
		for (j=0; j<numOfseats; j++) {
			seatsTable[counter] = 0;
			counter--;
		}
		rc = pthread_mutex_unlock(&theater);
		book[k].seats[0] = '\0';
		printf("Payment has been canceled \n");
	}
	else{

	}
	

int main(int argc, char *argv[]) {

	if (argc != 3) {
		printf("ERROR: the program should take two arguments, the number of customers and the seed for creating a random generator!\n");
		exit(-1);
	}
	int Ncust = atoi(argv[1]);
	seed = atoi(argv[2]);

	//stavroula's code
	if (Ncust<0) {
		printf("ERROR: the number of the thread to run should be a positive number. Current number given %d.\n", Ncust);
		exit(-1);
	}
	//stavroula's code

	pthread_t threads[Ncust];
	int allthreads[Ncust];
	counter = 0;

	rc = pthread_mutex_init(&telefonites, NULL);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_init() is %d.\n", rc);
		exit(-1);
	}
	rc = pthread_mutex_init(&account, NULL);
	rc = pthread_mutex_init(&trans, NULL);
	rc = pthread_mutex_init(&timecount, NULL);
	rc = pthread_mutex_init(&theater, NULL);

	//here all things should be print
	
	for (i = 0; i < Ncust; i++){
		allthreads[i] = i + 1;
		//creating thread
		rc = pthread_create(&allthreads[i], NULL, callfromcustomer, &allthreads[i]);
		if( rc!=0) {
			printf("ERROR: return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	rc = pthread_mutex_destroy(&account, NULL);
	rc = pthread_mutex_destroy(&trans, NULL);
	rc = pthread_mutex_destroy(&timecount, NULL);
	rc = pthread_mutex_destroy(&theater, NULL);
	for (i = 0; i < Ncust; i++){
		allthreads[i] = i + 1;
		rc = pthread_create(&threads[i], NULL, callfromcustomer, &allthreads[i]);
	if( rc!=0) {
		printf("ERROR: return code from pthread_create() is %d\n", rc);
		exit(-1);
	}
}
