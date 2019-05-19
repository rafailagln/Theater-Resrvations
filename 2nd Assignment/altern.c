#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> //for sleep
#include <time.h>
#include "second.h"

pthread_mutex_t telcount, account, transNo, timecount, seatPlan, monitorLock, randLock, cashCount, takenCount;
pthread_cond_t telCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cashCond = PTHREAD_COND_INITIALIZER;
int seatsTable[3][100]; //plano thesewn
unsigned int seed; 	//sporos
int trans = 1;	//arithmisi synallagwn
int telAvailable = Ntel;
int cashAvailable = Ncash;
double sum, m, mtwo; 
int rc, income, i, j, taken;
int *rate;


struct timespec start, startover, mid, midtwo, finish;

void *call(void* threadId) {
	int *tid = (int*)threadId;
	int row, prob, sleeptime, numOfseats, cost, counter;
	int probZone, min, max, seat, cashsleep, n;
	int zone;
	int* seatsNum;
	rate = (int*)malloc(4*sizeof(int));
//	seatsTable[0] = (int*)malloc(NzoneA*Nseat*sizeof(int));
//	seatsTable[1] = (int*)malloc(NzoneB*Nseat*sizeof(int));
//	seatsTable[2] = (int*)malloc(NzoneC*Nseat*sizeof(int));


	rc = pthread_mutex_lock(&telcount);

	//xekinaei na metraei ton xrono
	rc = pthread_mutex_lock(&timecount);
	
	clock_gettime(CLOCK_REALTIME, &start);

	rc = pthread_mutex_unlock(&timecount);	


	//an den yparxei diathesimos tilefonitis perimenei
	while(telAvailable == 0) {
		rc = pthread_cond_wait(&telCond, &telcount);
	
	}
	
	telAvailable--;	//meiwnei ton metriti twn diathesimwn tifelwnitwn

	rc = pthread_mutex_unlock(&telcount);
	
	
	//ypologizei ton xrono anamonis
	rc = pthread_mutex_lock(&timecount);
	
	clock_gettime(CLOCK_REALTIME, &mid);
	m += mid.tv_sec - start.tv_sec; 

	rc = pthread_mutex_unlock(&timecount);

	//epilegei tyxaia mia zwni
	rc = pthread_mutex_lock(&randLock);

	probZone = rand_r(&seed)%100;
	

	if(probZone <= PzoneA) {
		zone = 1;
	} else if (probZone <= PzoneB) {
		zone = 2;
	} else {
		zone = 3;
	}


	//epilegei enan tyxaio arithmo eisitiriwn (1-5) kai desmeyei xwro ston pinaka thesewn
	numOfseats = (rand_r(&seed)%Nseathigh) + Nseatlow;
	seatsNum = (int*)malloc(numOfseats*sizeof(int));

	printf("pelatis %d zone %d numOfseats %d\n", *tid, zone, numOfseats);

	//epilegei ena tyxaio plithos deyteroleptwn (5-10)
	sleeptime = (rand_r(&seed)%(Tseathigh-Tseatlow+1)) + Tseatlow;

	rc = pthread_mutex_unlock(&randLock);
	

	sleep(sleeptime);

	rc = pthread_mutex_lock(&telcount);

	if (taken >= 250 ) { //full
		rc = pthread_mutex_lock(&monitorLock);

		printf("pelatis %d Η κράτηση ματαιώθηκε γιατί το θέατρο είναι γεμάτο.\n", *tid);

		rc = pthread_mutex_unlock(&monitorLock);
		rate[0]++;

		//ayxanei ton metriti twn tilefwnitwn
		telAvailable++;		

		pthread_cond_signal(&telCond);
	
		rc = pthread_mutex_unlock(&telcount);
	} 
	else { //not full


		if(zone==1){
			min = 0; max = NzoneA;
		} else if (zone == 2) {
			min = NzoneA; max = NzoneA+NzoneB;
		} else if (zone == 3) {
			min = NzoneA+NzoneB; max = NzoneA+NzoneB+NzoneC;
		}
	
		seat = 0;
		for(row = min; row < max; row++){ //kathe seira tis katigorias
			counter = 0; //giati thelw na einai stin idia seira
			for (i = 0; i<Nseat; i++){ //kathe thesi tis seiras
				if (seatsTable[zone-1][seat] == 0){
					counter++;
				} else {
					counter = 0;
				}
				if(counter==numOfseats) break;
				seat++;
			}
			if(counter==numOfseats) break;
		}


		if (counter!=numOfseats) { //am den yparxoun aketes theseis
			rc = pthread_mutex_lock(&monitorLock);

			printf("Πελάτης: %d. Η κράτηση ματαιώθηκε γιατί δεν υπάρχουν αρκετές συνεχόμενες θέσεις.\n", *tid);

			rc = pthread_mutex_unlock(&monitorLock);
			rate[1]++;

			//ayxanei ton metriti twn tilefwnitwn
			telAvailable++;		

			pthread_cond_signal(&telCond);
	
			rc = pthread_mutex_unlock(&telcount);
		} 
		//an yparxoun arketes theseis
		else { 
			//desmeyei tis theseis sto plano
			rc = pthread_mutex_lock(&seatPlan);

			seat = seat - numOfseats +1; //gia na min apothikeyontai anapoda
			
			for(i=0; i < numOfseats; i++) {
				seatsTable[zone-1][seat] = *tid;
				seat++;
				seatsNum[i] = seat;
				taken++;
			}
			rc = pthread_mutex_unlock(&seatPlan);


			//ypologizetai to synoliko kostos
			if (zone==1) {
				cost = numOfseats*CzoneA;
			}else if(zone==2){
				cost = numOfseats*CzoneB;
			}else if(zone==3){
				cost = numOfseats*CzoneC;
			}		



		//ayxanei ton metriti twn tilefwnitwn
		telAvailable++;		

		pthread_cond_signal(&telCond);
	
		rc = pthread_mutex_unlock(&telcount);


		//syndesi me tamia
		rc = pthread_mutex_lock(&cashCount);

		//xekinaei na metraei ksana ton xrono
		rc = pthread_mutex_lock(&timecount);
	
		clock_gettime(CLOCK_REALTIME, &startover);

		rc = pthread_mutex_unlock(&timecount);	


		//an den yparxei diathesimos tamias perimenei
		while(cashAvailable == 0) {
			rc = pthread_cond_wait(&cashCond, &cashCount);
		}
	
		cashAvailable--;//meiwnei ton metriti twn tamiwn

		rc = pthread_mutex_unlock(&cashCount);

		//ypologizei ton xrono anamonis
		rc = pthread_mutex_lock(&timecount);
		
		clock_gettime(CLOCK_REALTIME, &midtwo);
		mtwo += midtwo.tv_sec - startover.tv_sec; 

		rc = pthread_mutex_unlock(&timecount);
	
		//epilegei ena tyxaio plithos deyteroleptwn (2-4)
		rc = pthread_mutex_lock(&randLock);
	
		cashsleep = (rand_r(&seed)%(Tcashhigh-Tcashlow+1)) + Tcashlow;
		rc = pthread_mutex_unlock(&randLock);

		sleep(cashsleep);

		//plirwmi me pistwtiki
		rc = pthread_mutex_lock(&randLock);
			
		prob = rand_r(&seed)%100;
		rc = pthread_mutex_unlock(&randLock);
	
		rc = pthread_mutex_lock(&cashCount);
	
		//an i plirwmi apotyxei
		if (prob > Pcardsuccess) {
			//apodesmeysi thesewn sto plano
			rc = pthread_mutex_lock(&seatPlan);

			for (i=0; i<numOfseats; i++) {
			seat--;
				seatsTable[zone-1][seat] = 0;
				taken--;
			}
			rc = pthread_mutex_unlock(&seatPlan);
			
			seatsNum[0] = '\0';
			rc = pthread_mutex_lock(&monitorLock);

			printf("Πελατης %d: Η κράτηση ματαιώθηκε γιατί η συναλλαγή με την πιστωτικη κάρτα δεν έγινε αποδεκτή.\n", *tid);
			rc = pthread_mutex_unlock(&monitorLock);
			rate[2]++;
			
		}
		//an i pliwmi ginetai apodekti
		else {
			//ta xrimata metaferontai ston logariasmo tis etairias
			rc = pthread_mutex_lock(&account);
	
			income += cost;
	
			rc = pthread_mutex_unlock(&account);
	
			rc = pthread_mutex_lock(&monitorLock);
		
			printf("Πελάτης: %d. Η κράτηση ολοκληρώθηκε επιτυχώς. Ο αριθμός συναλλαγής είναι <%d>, οι θέσεις σας είναι οι <", *tid, trans);		
			for(i=0; i<numOfseats; i++){
				printf("%d", seatsNum[i]);
				if(i+1<numOfseats){
					printf(",");
				}
			}
			printf("> στη Ζώνη <%d> και το κόστος συναλλαγής είναι %d ευρώ.\n", zone, cost);

			rc = pthread_mutex_unlock(&monitorLock);
			rate[3]++;
	
			//arithmitis synallagis
			rc = pthread_mutex_lock(&transNo);
			
			trans++;
			rc = pthread_mutex_unlock(&transNo);
		
		}
		cashAvailable++; //ayxanei ton metriti twn tamiwn
		rc = pthread_cond_signal(&cashCond);
	
		rc = pthread_mutex_unlock(&cashCount);

	}//yparxoun theseis
	}//not full
	
	
	//ypologizei ton xrono exypiretisis
	rc = pthread_mutex_lock(&timecount);
	
	clock_gettime(CLOCK_REALTIME, &finish);
	sum += finish.tv_sec - start.tv_sec; 
	rc = pthread_mutex_unlock(&timecount);
	
	
	free(seatsNum);
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
	
	rc = pthread_mutex_init(&account, NULL);
	
	rc = pthread_mutex_init(&transNo, NULL);
	
	rc = pthread_mutex_init(&timecount, NULL);
	
	rc = pthread_mutex_init(&seatPlan, NULL);
	
	rc = pthread_mutex_init(&monitorLock, NULL);
	
	rc = pthread_mutex_init(&randLock, NULL);

	rc = pthread_mutex_init(&cashCount, NULL);

	rc = pthread_mutex_init(&takenCount, NULL);
	
	
	for (i = 0; i < Ncust; i++){
		threadIds[i] = i + 1;
		//creating thread
		rc = pthread_create(&threads[i], NULL, call, &threadIds[i]);
		
	}

	void *status;
	for (i=0; i<Ncust; i++){
		//finish thread
		rc = pthread_join(threads[i], &status);
		
	}

	printf("Τα συνολικά έσοδα από τις πωλήσεις είναι: %d ευρώ.\n", income);
	printf("Το ποσοστό των συναλλαγών που το θέατρο ήταν γεμάτο είναι %.2f%%\n", (float)rate[0]*100/Ncust);
	printf("Το ποσοστό των συναλλαγών που δεν υπήρχαν αρκετές συνεχόμενες θέσεις είναι %.2f%%\n", (float)rate[1]*100/Ncust);
	printf("Το ποσοστό των συναλλαγών που η πιστωτική δεν έγινε αποδεκτή είναι %.2f%%\n", (float)rate[2]*100/Ncust);
	printf("Το ποσοστό των συναλλαγών που ολοκληρώθηκαν με επιτυχία είναι %.2f%%\n", (float)rate[3]*100/Ncust);
	printf("Ο μέσος όρος αναμονής των πελατών είναι = %.3f δευτερόλεπτα.\n", (m+mtwo)/Ncust);
	printf("Ο μέσος όρος εξυπηρέτησης των πελατών είναι = %.3f δευτερόλεπτα.\n", sum/Ncust);
	
	printf("Πλάνος θέσεων:\n");
	for (j = 0; j < 3; j++){
		for (i = 0; i < 100; i++) {
		//gia na min emfanizei tis kenes theseis
		if(seatsTable[j][i] != 0) {
			printf("Ζώνη %d / Θεση %d / Πελατης %d, ", j+1, i+1, seatsTable[j][i]);	
		}
		}
	}


	rc = pthread_mutex_destroy(&telcount);
	
	rc = pthread_mutex_destroy(&account);
	
	rc = pthread_mutex_destroy(&transNo);
	
	rc = pthread_mutex_destroy(&timecount);
	
	rc = pthread_mutex_destroy(&seatPlan);
	
	rc = pthread_mutex_destroy(&monitorLock);
	
	rc = pthread_mutex_destroy(&randLock);

	rc = pthread_mutex_destroy(&cashCount);

	rc = pthread_mutex_destroy(&takenCount);


	rc = pthread_cond_destroy(&telCond);
	rc = pthread_cond_destroy(&cashCond);
	

	free(threads);
	free(rate);

 return 1;

}
