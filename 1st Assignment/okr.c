#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> //for sleep
#include <string.h> //for strcat
#include <time.h>
#include "init.h"

pthread_mutex_t telcount, account, transNo, timecount, seatPlan, monitor;
pthread_cond_t telCond = PTHREAD_COND_INITIALIZER;
int i, rc;
int income;	//ta esoda
int counter;
int seatsTable[Nseat]; //plano thesewn
unsigned int seed; 	//sporos
int k = 1;	//arithmisi synallagwn
int telAvailable = Ntel;
float sum, m, cost;

/*typedef struct transaction {
	int id;
	char seats[50];
	int cost;
}transaction;
transaction book[250];*/

struct timespec start, mid, finish;

void *call(void* threadId) {
	int *tid = (int*)threadId;
	int  prob, sleeptime, numOfseats, v;
	char temp[3];
	int *seatsarray;

	rc = pthread_mutex_lock(&telcount);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
		pthread_exit(&rc);
	}

	rc = pthread_mutex_lock(&timecount);
	clock_gettime(CLOCK_REALTIME, &start);
	rc = pthread_mutex_unlock(&timecount);

	while(telAvailable == 0) {
		rc = pthread_cond_wait(&telCond, &telcount);
		if (rc != 0) {
			printf("ERROR: return code from pthread_cond_wait() is %d\n", rc);
			pthread_exit(&rc);
		}
	}
	rc = pthread_mutex_lock(&monitor);
	printf("Ο πελάτης %d θα εξυπηρετηθεί τώρα.\n", *tid);
	telAvailable--;
	
	rc = pthread_mutex_unlock(&telcount);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
		pthread_exit(&rc);
	}

	rc = pthread_mutex_lock(&timecount);	
	clock_gettime(CLOCK_REALTIME, &mid);
	m += mid.tv_sec - start.tv_sec; 
	rc = pthread_mutex_unlock(&timecount);

	//epilegei enan tyxaio arithmo eisitiriwn (1-5)
	numOfseats = (rand_r(&seed)%Nseathigh) + Nseatlow;
	printf("thread %d: seats = %d.\n", *tid, numOfseats);

	//xreaizetai ena tyxaio plithos deyteroleptwn (5-10)
	sleeptime = (rand_r(&seed)%(Tseathigh-Tseatlow+1)) + Tseatlow;
	printf("thread %d: will wait %d.\n", *tid, sleeptime);

	sleep(sleeptime);


	//an einai foul
	if (counter == 250 ) {
		printf("Η κράτηση ματαιώθηκε γιατί το θέατρο είναι γεμάτο.\n");
	}
	else {
		//an den yparxoun arketes theseis
		if (counter + numOfseats > 250 ) {
		printf("Πελάτης: %d. Η κράτηση ματαιώθηκε γιατί δεν υπάρχουν αρκετές διαθέσιμες θέσεις.\n", *tid);
		rc = pthread_mutex_unlock(&monitor);

		} 
		//an yparxoun theseis
		else {
			//desmeyei tis theseis sto plano
			rc = pthread_mutex_lock(&seatPlan);
			for (int j = 0; j < numOfseats; j++) {
				v=0;
				seatsTable[counter] = *tid;
				counter++;
				array[v]=book[k].seats;
				v++;
				sprintf(temp, "%d", counter);
				strcat(book[k].seats, temp);
				if(j+1 < numOfseats) {
					strcat(book[k].seats, ",");
				}
			}
			rc = pthread_mutex_unlock(&monitor);
			rc = pthread_mutex_unlock(&seatPlan);

			//plirwmi me pistwtiki
			prob = rand_r(&seed)%100;

			//an i plirwmi apotyxei
			if (prob > Pcardsuccess) {
				//apodesmeysi thesewn sto plano
				rc = pthread_mutex_lock(&seatPlan);
				for (int j=0; j<numOfseats; j++) {
					seatsTable[counter] = 0;
					counter--;
				}
				
				rc = pthread_mutex_unlock(&seatPlan);
				book[k].seats[0] = '\0';
				printf("Πελατης %d: Η κράτηση ματαιώθηκε γιατί η συναλλαγή με την πιστωτική κάρτα δεν έγινε αποδεκτή.\n", *tid);
			}
			//an i plirwmi ginetai apodekti
			else {
			cost = numOfseats*Cseat;
			/*book[k].id = *tid;

			//o pelatis xrewnetai Cseat ana thesi
			book[k].cost = numOfseats*Cseat;*/

			//ta xrimata metaferontai ston logariasmo tis etairias
			rc = pthread_mutex_lock(&account);
			//income += book[k].cost;
			income += cost;
			rc = pthread_mutex_unlock(&account);

			printf("Πελάτης: %d. Η κράτηση ολοκληρώθηκε επιτυχώς. Ο αριθμός συναλλαγής είναι <%d>, οι θέσεις σας είναι οι <%s> και το κόστος συναλλαγής είναι %d ευρώ.\n", book[k].id, k, array[k], book[k].cost);

			//arithmitis synallagis
			rc = pthread_mutex_lock(&transNo);
			k++;
			rc = pthread_mutex_unlock(&transNo);
			}
		}
	}

	rc = pthread_mutex_lock(&timecount);
	clock_gettime(CLOCK_REALTIME, &finish);
	sum += finish.tv_sec - start.tv_sec; 
	rc = pthread_mutex_unlock(&timecount);

	rc = pthread_mutex_lock(&telcount);

	telAvailable++;

	pthread_cond_signal(&telCond);
	if (rc != 0) {
		printf("ERROR: return code from pthread_cond_signal() is %d\n", rc);
		pthread_exit(&rc);
	}
	
	rc = pthread_mutex_unlock(&telcount);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
		pthread_exit(&rc);
	}

	pthread_exit(tid);
}


int main(int argc, char** argv) {

	if (argc != 3) {
		printf("Error.\n");
		return -1;
	}

	int Ncust = atoi(argv[1]);
	seed = atoi(argv[2]);


	if (Ncust<0) {
		printf("ERROR: the number of the thread to run should be a positive number. Current number given %d.\n", Ncust);
		exit(-1);
	}

	pthread_t threads[Ncust];
	int threadIds[Ncust];
	threads = malloc(maxNumberOfThreads * sizeof(pthread_t));
	if (threads == NULL) {
		printf("NOT ENOUGH MEMORY!\n");
		return -1;
	}
	counter = 0;

	rc = pthread_mutex_init(&telcount, NULL);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_init() is %d.\n", rc);
		exit(-1);
	}
	rc = pthread_mutex_init(&monitor, NULL);
	rc = pthread_mutex_init(&account, NULL);
	rc = pthread_mutex_init(&transNo, NULL);
	rc = pthread_mutex_init(&timecount, NULL);
	rc = pthread_mutex_init(&seatPlan, NULL);
	
	for (i = 0; i < Ncust; i++){
		threadIds[i] = i + 1;
		//creating thread
		rc = pthread_create(&threads[i], NULL, call, &threadIds[i]);
		if( rc!=0) {
			printf("ERROR: return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	void *status;
	for (i=0; i<Ncust; i++){
		//finish thread
		rc = pthread_join(threads[i], &status);
		if (rc!= 0) {
			printf("ERROR: return code from pthread_join is %d.\n", rc);
			exit(-1);
		}
	}
	printf("seats taken: %d.\n", counter);
	printf("Τα συνολικά έσοδα από τις πωλήσεις είναι: %d ευρώ.\n", income);
	printf("Ο μέσος όρος αναμονής των πελατών είναι = %f δευτερόλεπτα.\n", m/(Ncust-Ntel));
	printf("Ο μέσος όρος εξυπηρέτησης των πελατών είναι = %f δευτερόλεπτα.\n", sum/Ncust);
	
	printf("Πλάνος θέσεων:\n");
	for (i = 0; i < Nseat; i++) {
		//gia na min emfanizei tis kenes theseis
		if(seatsTable[i] != 0) {
			printf("Θεση %d / Πελατης %d", i+1, seatsTable[i]);	
		}
	}

	rc = pthread_mutex_destroy(&telcount);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_destroy() is %d.\n", rc);
		exit(-1);
	}
	rc = pthread_mutex_destroy(&account);
	rc = pthread_mutex_destroy(&transNo);
	rc = pthread_mutex_destroy(&timecount);
	rc = pthread_mutex_destroy(&seatPlan);
	rc = pthread_mutex_destroy(&monitor);

	rc = pthread_cond_destroy(&telCond);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_destroy() is %d.\n", rc);
		exit(-1);
	}

 free(threads);
 return 1;

}
