#include "t8160018-p3110178-p3160186-res2.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> //for sleep
#include <time.h>

pthread_mutex_t telcount, account, transNo, timecount, seatPlan, monitorLock, randLock, cashCount, takenCount;
pthread_cond_t telCond = PTHREAD_COND_INITIALIZER;
int rc, income, takenseats;
int seatsTable[NzoneA+NzoneB+NzoneC][Nseat]; //plano thesewn
unsigned int seed; 	//sporos
int trans = 1;	//arithmisi synallagwn
int telAvailable = Ntel;
int cashAvailable = Ncash;
float sum, m;
//int* seats;
int *rate;

struct timespec start1, start2, mid1, mid2, finish;

void *call(void* threadId) {
	int *tid = (int*)threadId;
	int j, row, prob, sleeptime, numOfseats, cost, counter;
	int probz;
	char zone;
	int* seatsNum;
	rate = (int*)malloc(4*sizeof(int));

	rc = pthread_mutex_lock(&telcount);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_lock(&telcount) is %d\n", rc);
		pthread_exit(&rc);
	}

	//start counting time
	rc = pthread_mutex_lock(&timecount);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_lock(&timecount) is %d\n", rc);
		pthread_exit(&rc);
	}

	clock_gettime(CLOCK_REALTIME, &start1);

	rc = pthread_mutex_unlock(&timecount);	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_unlock(&timecount) is %d\n", rc);
		pthread_exit(&rc);
	}


	//if there's no available waits
	while(telAvailable == 0) {
		rc = pthread_cond_wait(&telCond, &telcount);
		if (rc != 0) {
			printf("ERROR: return code from pthread_cond_wait() is %d\n", rc);
			pthread_exit(&rc);
		}
	}
	telAvailable--;	

	rc = pthread_mutex_unlock(&telcount);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_unlock(&telcount) is %d\n", rc);
		pthread_exit(&rc);
	}

	
	//hold on time
	rc = pthread_mutex_lock(&timecount);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_lock(&timecount) is %d\n", rc);
		pthread_exit(&rc);
	}
	
	clock_gettime(CLOCK_REALTIME, &mid1);
	m += mid1.tv_sec - start1.tv_sec; 

	rc = pthread_mutex_unlock(&timecount);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_unlock(&timecount) is %d\n", rc);
		pthread_exit(&rc);
	}

	//randomly selects a zone
	rc = pthread_mutex_lock(&randLock);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_lock(&randLock) is %d\n", rc);
		pthread_exit(&rc);
	}

	probz = rand_r(&seed)%100;
	

	if(probz <= PzoneA) {
		zone = 'A';
	} else if (probz <= PzoneB) {
		zone = 'B';
	} else {
		zone = 'C';
	}

	//epilegei enan tyxaio arithmo eisitiriwn (1-5) kai desmeyei xwro ston pinaka thesewn
	rc = pthread_mutex_lock(&randLock);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_lock(&randLock) is %d\n", rc);
		pthread_exit(&rc);
	}

	numOfseats = (rand_r(&seed)%Nseathigh) + Nseatlow;
	seats = (int*)malloc(numOfseats*sizeof(int));


	//select random secs(5-10)
	sleeptime = (rand_r(&seed)%(tseathigh-tseatlow+1)) + tseatlow;

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

	//if theater's full
	if (counter == 250 ) {
		rc = pthread_mutex_lock(&monitorLock);
		if (rc != 0) {
			printf("ERROR: return code from pthread_mutex_lock(&monitorLock) is %d\n", rc);
			pthread_exit(&rc);
		}
		printf("Η κράτηση ματαιώθηκε γιατί το θέατρο είναι γεμάτο.\n");
		rate[0]++;

		rc = pthread_mutex_unlock(&monitorLock);
		if (rc != 0) {
			printf("ERROR: return code from pthread_mutex_unlock(&monitorLock) is %d\n", rc);
			pthread_exit(&rc);
		}
	}
	else { //if theater's not full 
		
		if(zone=='A'){
			min = 0; max = NzoneA;
		} else if (zone == 'B') {
			min = NzoneA; max = NzoneA+NzoneB;
		} else if (zone == 'C') {
			min = NzoneA+NzoneB; max = NzoneA+NzoneB+NzoneC;
		}


		//DO NOT KNOW WHY
		for(row = min; row < max; row++){ //kathe seira tis katigorias
			counter = 0; //giati thelw na einai stin idia seira
			for (seat = 0; seat<Nseat; seat++){ //kathe thesi tis seiras
				if (seatsTable[row][seat] == 0){
					counter++;
				} else {
					counter = 0;
				}
				if(counter==numOfseats) break;
			}
			if(counter==numOfseats) break;
		}
		if (counter + numOfseats > 250 ) {
			rc = pthread_mutex_lock(&monitorLock);
			if (rc != 0) {
				printf("ERROR: return code from pthread_mutex_lock(&monitorLock) is %d\n", rc);
				pthread_exit(&rc);
			}
			printf("Πελάτης: %d. Η κράτηση ματαιώθηκε γιατί δεν υπάρχουν αρκετές διαθέσιμες θέσεις.\n", *tid);
			rate[1]++;
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
				seatsTable[row][seat] = *tid;
				seat++;
				seatsNum[i] = seat;
				takenseats++;
			}
			}
			rc = pthread_mutex_unlock(&seatPlan);
			if (rc != 0) {
				printf("ERROR: return code from pthread_mutex_unlock(&seatPlan) is %d\n", rc);
				pthread_exit(&rc);
			}

			//ypologizetai to synoliko kostos
			if (zone=='A') {
				cost = numOfseats*CzoneA;
			}else if(zone=='B'){
				cost = numOfseats*CzoneB;
			}else if(zone=='C'){
				cost = numOfseats*CzoneC;
			}		
		//syndesi me tamia
		rc = pthread_mutex_lock(&cashCount);
		if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_lock(&cashCount) is %d\n", rc);
		pthread_exit(&rc);
	}

		//xekinaei na metraei ksana ton xrono
		rc = pthread_mutex_lock(&timecount);
		if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_lock(&timecount) is %d\n", rc);
		pthread_exit(&rc);
	}

		clock_gettime(CLOCK_REALTIME, &start2);

		rc = pthread_mutex_unlock(&timecount);	


		//an den yparxei diathesimos tamias perimenei
		while(cashAvailable == 0) {
			rc = pthread_cond_wait(&cashCond, &cashCount);
			if (rc != 0) {
				printf("ERROR: return code from pthread_cond_wait() is %d\n", rc);
				pthread_exit(&rc);
			}
		}
	
		cashAvailable--;//meiwnei ton metriti twn tamiwn

		rc = pthread_mutex_unlock(&cashCount);
			if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_unlock(&cashCount) is %d\n", rc);
		pthread_exit(&rc);
	}


		//ypologizei ton xrono anamonis
		rc = pthread_mutex_lock(&timecount);
		if (rc != 0) {
			printf("ERROR: return code from pthread_mutex_lock(&timecount) is %d\n", rc);
			pthread_exit(&rc);
		}
		
		clock_gettime(CLOCK_REALTIME, &mid2);
		m2 += mid2.tv_sec - start2.tv_sec; 

		rc = pthread_mutex_unlock(&timecount);
		if (rc != 0) {
			printf("ERROR: return code from pthread_mutex_unlock(&timecount) is %d\n", rc);
			pthread_exit(&rc);
		}
	
		//randomly choose again sec [2,4]
		rc = pthread_mutex_lock(&randLock);
		if (rc != 0) {
			printf("ERROR: return code from pthread_mutex_lock(&randLock) is %d\n", rc);
			pthread_exit(&rc);
		}
	
		cashsleep = (rand_r(&seed)%(Tcashhigh-Tcashlow+1)) + Tcashlow;
		rc = pthread_mutex_unlock(&randLock);

		sleep(cashsleep);
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
					seat--;
					seatsTable[row][seat]= 0;
					takenseats--;
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
				rate[2]++;
			}
			//an i plirwmi ginetai apodekti
			else {
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
			printf("> στη Ζώνη <%c> και το κόστος συναλλαγής είναι %d ευρώ.\n", zone, cost);

			rc = pthread_mutex_unlock(&monitorLock);
			if (rc != 0) {
				printf("ERROR: return code from pthread_mutex_unlock(&monitorLock) is %d\n", rc);
				pthread_exit(&rc);
			}
			rate[3]++;

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
			cashAvailable++; //ayxanei ton metriti twn tamiwn
			rc = pthread_cond_signal(&cashCond);
			if (rc != 0) {
				printf("ERROR: return code from pthread_cond_signal() is %d\n", rc);
				pthread_exit(&rc);
			}
		
			rc = pthread_mutex_unlock(&cashCount);
			if (rc != 0) {
				printf("ERROR: return code from pthread_mutex_unlock(&cashCount) is %d\n", rc);
				pthread_exit(&rc);
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
	sum += finish.tv_sec - start1.tv_sec; 
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
	
	rc = pthread_mutex_init(&cashCount, NULL);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_init(&cashCoun) is %d.\n", rc);
		exit(-1);
	}

	rc = pthread_mutex_init(&takenCount, NULL);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_init(&takenCount) is %d.\n", rc);
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
	printf("Τα συνολικά έσοδα από τις πωλήσεις είναι: %d ευρώ.\n", income);
	printf("pososto  %.2f%%\n", (float)rate[0]*100/Ncust);;
	printf("pososto completed%.2f%%\n", (float)rate[3]*100/Ncust);
	printf("Ο μέσος όρος αναμονής των πελατών είναι = %.3f δευτερόλεπτα.\n", (m+m2)/Ncust);
	printf("Ο μέσος όρος εξυπηρέτησης των πελατών είναι = %.3f δευτερόλεπτα.\n", sum/Ncust);
	
	
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

	rc = pthread_cond_destroy(&takenCount);
	if (rc != 0) {
		printf("ERROR: return code from pthread_cond_destroy() is %d.\n", rc);
		exit(-1);
	}

	rc = pthread_cond_destroy(&cashCount);
	if (rc != 0) {
		printf("ERROR: return code from pthread_cond_destroy() is %d.\n", rc);
		exit(-1);
	}

	free(threads);
	free(seats);

 return 1;

}