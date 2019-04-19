#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> //for sleep
#include <time.h>
#include "init.h"

pthread_mutex_t telcount, account, transNo, timecount, seatPlan, monitorLock, randLock;
pthread_cond_t telCond = PTHREAD_COND_INITIALIZER;
int rc, income, counter;
int seatsTable[Nseat]; //plano thesewn
unsigned int seed; 	//sporos
int trans = 1;	//arithmisi synallagwn
int telAvailable = Ntel;
float sum, m;
int* seats;

struct timespec start, mid, finish;

void *call(void* threadId) {
	int *tid = (int*)threadId;
	int j, prob, sleeptime, numOfseats, cost;

	rc = pthread_mutex_lock(&telcount);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_lock(&telcount) is %d\n", rc);
		pthread_exit(&rc);
	}

	//xekinaei na metraei ton xrono
	rc = pthread_mutex_lock(&timecount);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_lock(&timecount) is %d\n", rc);
		pthread_exit(&rc);
	}

	clock_gettime(CLOCK_REALTIME, &start);

	rc = pthread_mutex_unlock(&timecount);	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_unlock(&timecount) is %d\n", rc);
		pthread_exit(&rc);
	}


	//an den yparxei diathesimos tilefonitis perimenei
	while(telAvailable == 0) {
		rc = pthread_cond_wait(&telCond, &telcount);
		if (rc != 0) {
			printf("ERROR: return code from pthread_cond_wait() is %d\n", rc);
			pthread_exit(&rc);
		}
	}
	
	telAvailable--;	//meiwnei ton metriti twn diathesimwn tifelwnitwn

	rc = pthread_mutex_unlock(&telcount);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_unlock(&telcount) is %d\n", rc);
		pthread_exit(&rc);
	}

	
	//ypologizei ton xrono anamonis
	rc = pthread_mutex_lock(&timecount);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_lock(&timecount) is %d\n", rc);
		pthread_exit(&rc);
	}
	
	clock_gettime(CLOCK_REALTIME, &mid);
	m += mid.tv_sec - start.tv_sec; 

	rc = pthread_mutex_unlock(&timecount);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_unlock(&timecount) is %d\n", rc);
		pthread_exit(&rc);
	}


	//epilegei enan tyxaio arithmo eisitiriwn (1-5) kai desmeyei xwro ston pinaka thesewn
	rc = pthread_mutex_lock(&randLock);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_lock(&randLock) is %d\n", rc);
		pthread_exit(&rc);
	}

	numOfseats = (rand_r(&seed)%Nseathigh) + Nseatlow;
	seats = (int*)malloc(numOfseats*sizeof(int));


	//epilegei ena tyxaio plithos deyteroleptwn (5-10)
	sleeptime = (rand_r(&seed)%(Tseathigh-Tseatlow+1)) + Tseatlow;

	rc = pthread_mutex_unlock(&randLock);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_unlock(&randLock) is %d\n", rc);
		pthread_exit(&rc);
	}

	sleep(sleeptime);

	rc = pthread_mutex_lock(&telcount);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_lock(&telcount) is %d\n", rc);
		pthread_exit(&rc);
	}

	//an einai foul
	if (counter == 250 ) {
		rc = pthread_mutex_lock(&monitorLock);
		if (rc != 0) {
			printf("ERROR: return code from pthread_mutex_lock(&monitorLock) is %d\n", rc);
			pthread_exit(&rc);
		}
		printf("Η κράτηση ματαιώθηκε γιατί το θέατρο είναι γεμάτο.\n");

		rc = pthread_mutex_unlock(&monitorLock);
		if (rc != 0) {
			printf("ERROR: return code from pthread_mutex_unlock(&monitorLock) is %d\n", rc);
			pthread_exit(&rc);
		}
	}
	else {
		//an den yparxoun arketes theseis
		if (counter + numOfseats > 250 ) {
			rc = pthread_mutex_lock(&monitorLock);
			if (rc != 0) {
				printf("ERROR: return code from pthread_mutex_lock(&monitorLock) is %d\n", rc);
				pthread_exit(&rc);
			}
			printf("Πελάτης: %d. Η κράτηση ματαιώθηκε γιατί δεν υπάρχουν αρκετές διαθέσιμες θέσεις.\n", *tid);

			rc = pthread_mutex_unlock(&monitorLock);
			if (rc != 0) {
				printf("ERROR: return code from pthread_mutex_unlock(&monitorLock) is %d\n", rc);
				pthread_exit(&rc);
			}
		} 
		//an yparxoun theseis
		else {

			//desmeyei tis theseis sto plano
			rc = pthread_mutex_lock(&seatPlan);
			if (rc != 0) {
				printf("ERROR: return code from pthread_mutex_lock(&seatPlan) is %d\n", rc);
				pthread_exit(&rc);
			}
			for(j=0; j < numOfseats; j++) {
				seatsTable[counter] = *tid;
				counter++;
				seats[j] = counter;
			}
			rc = pthread_mutex_unlock(&seatPlan);
			if (rc != 0) {
				printf("ERROR: return code from pthread_mutex_unlock(&seatPlan) is %d\n", rc);
				pthread_exit(&rc);
			}

			//plirwmi me pistwtiki
			rc = pthread_mutex_lock(&randLock);
			if (rc != 0) {
				printf("ERROR: return code from pthread_mutex_lock(&randLock) is %d\n", rc);
				pthread_exit(&rc);
			}
			prob = rand_r(&seed)%100;
			rc = pthread_mutex_unlock(&randLock);
			if (rc != 0) {
				printf("ERROR: return code from pthread_mutex_unlock(&randLock) is %d\n", rc);
				pthread_exit(&rc);
			}

			//an i plirwmi apotyxei
			if (prob > Pcardsuccess) {
				//apodesmeysi thesewn sto plano
				rc = pthread_mutex_lock(&seatPlan);
				if (rc != 0) {
					printf("ERROR: return code from pthread_mutex_lock(&seatPlan) is %d\n", rc);
					pthread_exit(&rc);
				}
				for (j=0; j<numOfseats; j++) {
					counter--;
					seatsTable[counter] = 0;
				}
				rc = pthread_mutex_unlock(&seatPlan);
				if (rc != 0) {
					printf("ERROR: return code from pthread_mutex_unlock(&seatPlan) is %d\n", rc);
					pthread_exit(&rc);
				}
				seats[0] = '\0';
				rc = pthread_mutex_lock(&monitorLock);
				if (rc != 0) {
					printf("ERROR: return code from pthread_mutex_lock(&monitorLock) is %d\n", rc);
					pthread_exit(&rc);
				}

				printf("Πελατης %d: Η κράτηση ματαιώθηκε γιατί η συναλλαγή με την πιστωτική κάρτα δεν έγινε αποδεκτή.\n", *tid);

				rc = pthread_mutex_unlock(&monitorLock);
				if (rc != 0) {
					printf("ERROR: return code from pthread_mutex_unlock(&monitorLock) is %d\n", rc);
					pthread_exit(&rc);
				}
			}
			//an i plirwmi ginetai apodekti
			else {
			//o pelatis xrewnetai Cseat ana thesi
			cost = numOfseats*Cseat;

			//ta xrimata metaferontai ston logariasmo tis etairias
			rc = pthread_mutex_lock(&account);
			if (rc != 0) {
				printf("ERROR: return code from pthread_mutex_lock(&account) is %d\n", rc);
				pthread_exit(&rc);
			}
			income += cost;

			rc = pthread_mutex_unlock(&account);
			if (rc != 0) {
				printf("ERROR: return code from pthread_mutex_unlock(&account) is %d\n", rc);
				pthread_exit(&rc);
			}

			rc = pthread_mutex_lock(&monitorLock);
			if (rc != 0) {
				printf("ERROR: return code from pthread_mutex_lock(&monitorLock) is %d\n", rc);
				pthread_exit(&rc);
			}

			printf("Πελάτης: %d. Η κράτηση ολοκληρώθηκε επιτυχώς. Ο αριθμός συναλλαγής είναι <%d>, οι θέσεις σας είναι οι <", *tid, trans);		
			for(j=0; j<numOfseats; j++){
				printf("%d", seats[j]);
				if(j+1<numOfseats){
					printf(",");
				}
			}
			printf("> και το κόστος συναλλαγής είναι %d ευρώ.\n", cost);

			rc = pthread_mutex_unlock(&monitorLock);
			if (rc != 0) {
				printf("ERROR: return code from pthread_mutex_unlock(&monitorLock) is %d\n", rc);
				pthread_exit(&rc);
			}

			//arithmitis synallagis
			rc = pthread_mutex_lock(&transNo);
			if (rc != 0) {
				printf("ERROR: return code from pthread_mutex_lock(&transNo) is %d\n", rc);
				pthread_exit(&rc);
			}
			trans++;
			rc = pthread_mutex_unlock(&transNo);
			if (rc != 0) {
				printf("ERROR: return code from pthread_mutex_unlock(&transNo) is %d\n", rc);
				pthread_exit(&rc);
			}
			}
		}
	}
	
	//ypologizei ton xrono exypiretisis
	rc = pthread_mutex_lock(&timecount);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_lock(&timecount) is %d\n", rc);
		pthread_exit(&rc);
	}
	clock_gettime(CLOCK_REALTIME, &finish);
	sum += finish.tv_sec - start.tv_sec; 
	rc = pthread_mutex_unlock(&timecount);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_unlock(&timecount) is %d\n", rc);
		pthread_exit(&rc);
	}

	telAvailable++;		//ayxanei ton metriti twn tilefwnitwn

	pthread_cond_signal(&telCond);
	if (rc != 0) {
		printf("ERROR: return code from pthread_cond_signal() is %d\n", rc);
		pthread_exit(&rc);
	}
	
	rc = pthread_mutex_unlock(&telcount);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_unlock(&telcount) is %d\n", rc);
		pthread_exit(&rc);
	}

	pthread_exit(tid);
}


int main(int argc, char** argv) {
	int i;

	if (argc != 3) {
		printf("Error. Provide two arguments.\n");
		return -1;
	}

	int Ncust = atoi(argv[1]);
	seed = atoi(argv[2]);

	if (Ncust<0) {
		printf("ERROR: the number of the thread to run should be a positive number. Current number given %d.\n", Ncust);
		exit(-1);
	}

	pthread_t* threads = malloc(Ncust * sizeof(pthread_t));
	int threadIds[Ncust];

	rc = pthread_mutex_init(&telcount, NULL);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_init(&telcount) is %d.\n", rc);
		exit(-1);
	}
	rc = pthread_mutex_init(&account, NULL);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_init(&acount) is %d.\n", rc);
		exit(-1);
	}
	rc = pthread_mutex_init(&transNo, NULL);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_init(&transNo) is %d.\n", rc);
		exit(-1);
	}
	rc = pthread_mutex_init(&timecount, NULL);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_init(&timecount) is %d.\n", rc);
		exit(-1);
	}
	rc = pthread_mutex_init(&seatPlan, NULL);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_init(&seatPlan) is %d.\n", rc);
		exit(-1);
	}
	rc = pthread_mutex_init(&monitorLock, NULL);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_init(&monitorLock) is %d.\n", rc);
		exit(-1);
	}
	rc = pthread_mutex_init(&randLock, NULL);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_init(&randLock) is %d.\n", rc);
		exit(-1);
	}
	
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

	printf("Τα συνολικά έσοδα από τις πωλήσεις είναι: %d ευρώ.\n", income);
	printf("Ο μέσος όρος αναμονής των πελατών είναι = %f δευτερόλεπτα.\n", m/(Ncust-Ntel));
	printf("Ο μέσος όρος εξυπηρέτησης των πελατών είναι = %f δευτερόλεπτα.\n", sum/Ncust);
	
	printf("Πλάνος θέσεων:\n");
	for (i = 0; i < Nseat; i++) {
		//gia na min emfanizei tis kenes theseis
		if(seatsTable[i] != 0) {
			printf("Θεση %d / Πελατης %d, ", i+1, seatsTable[i]);	
		}
	}

	rc = pthread_mutex_destroy(&telcount);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_destroy(&telcount) is %d.\n", rc);
		exit(-1);
	}
	rc = pthread_mutex_destroy(&account);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_destroy(&account) is %d.\n", rc);
		exit(-1);
	}
	rc = pthread_mutex_destroy(&transNo);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_destroy(&transNo) is %d.\n", rc);
		exit(-1);
	}
	rc = pthread_mutex_destroy(&timecount);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_destroy(&timecount) is %d.\n", rc);
		exit(-1);
	}
	rc = pthread_mutex_destroy(&seatPlan);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_destroy(&seatPlan) is %d.\n", rc);
		exit(-1);
	}
	rc = pthread_mutex_destroy(&monitorLock);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_destroy(&monitorLock) is %d.\n", rc);
		exit(-1);
	}
	rc = pthread_mutex_destroy(&randLock);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_destroy(&randLock) is %d.\n", rc);
		exit(-1);
	}

	rc = pthread_cond_destroy(&telCond);
	if (rc != 0) {
		printf("ERROR: return code from pthread_cond_destroy() is %d.\n", rc);
		exit(-1);
	}

	free(threads);
	free(seats);

 return 1;

}
