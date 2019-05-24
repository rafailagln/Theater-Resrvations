#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> //for sleep
#include <time.h>
#include "t8160018-p3110178-res2.h"

pthread_mutex_t telcount, account, transNo, timecount, seatPlan, monitorLock, randLock, cashCount, takenCount;
pthread_cond_t telCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cashCond = PTHREAD_COND_INITIALIZER;
int *seatsTable[3]; //plano thesewn ana zwni
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
	int probZone, min, max, seat, cashsleep;
	int zone;
	int* seatsNum;
	rate = (int*)malloc(4*sizeof(int));
	seatsTable[0] = (int*)malloc(NzoneA*Nseat*sizeof(int)); //desmeuei dynamika tis theseis tis zonis A
	seatsTable[1] = (int*)malloc(NzoneB*Nseat*sizeof(int)); //desmeuei dynamika tis theseis tis zonis B
	seatsTable[2] = (int*)malloc(NzoneC*Nseat*sizeof(int)); //desmeuei dynamika tis theseis tis zonis C


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

	rc = pthread_mutex_unlock(&timecount);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_unlock(&timecount) is %d\n", rc);
		pthread_exit(&rc);
	}


	//an den yparxei diathesimos tilefonitis perimenei
	while(telAvailable == 0) {
		rc = pthread_cond_wait(&telCond, &telcount);
	
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

	//epilegei tyxaia mia zwni
	rc = pthread_mutex_lock(&randLock);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_lock(&randLock) is %d\n", rc);
		pthread_exit(&rc);
	}

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

	if (taken == 250 ) { //full
		rc = pthread_mutex_lock(&monitorLock);
		if (rc != 0) {
			printf("ERROR: return code from pthread_mutex_lock(&monitorLock) is %d\n", rc);
			pthread_exit(&rc);
		}

		printf("pelatis %d Η κράτηση ματαιώθηκε γιατί το θέατρο είναι γεμάτο.\n", *tid);

		rc = pthread_mutex_unlock(&monitorLock);
		if (rc != 0) {
			printf("ERROR: return code from pthread_mutex_unlock(&monitorLock) is %d\n", rc);
			pthread_exit(&rc);
		}
		rate[0]++;

		//ayxanei ton metriti twn tilefwnitwn
		telAvailable++;		

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
			counter = 0; //giati thelw na einai stin idia seira & metraei tis kenes theseis mexri na kalufthoun autes pou thelei o pelatis
			for (i = 0; i<Nseat; i++){ //kathe thesi tis seiras
				if (seatsTable[zone-1][seat] == 0){
					counter++;
				} else {
					counter = 0;
				}
				if(counter==numOfseats) break; //tis vrike thn mia dipla sthn allh ara vgainei apo to block
				seat++;
			}
			if(counter==numOfseats) break; //tis vrike thn mia dipla sthn allh ara vgainei apo to block
		}


		if (counter!=numOfseats) { //an den yparxoun aketes theseis
			rc = pthread_mutex_lock(&monitorLock);
			if (rc != 0) {
				printf("ERROR: return code from pthread_mutex_lock(&monitorLock) is %d\n", rc);
				pthread_exit(&rc);
			}

			printf("Πελάτης: %d. Η κράτηση ματαιώθηκε γιατί δεν υπάρχουν αρκετές συνεχόμενες θέσεις.\n", *tid);

			rc = pthread_mutex_unlock(&monitorLock);
			if (rc != 0) {
				printf("ERROR: return code from pthread_mutex_unlock(&monitorLock) is %d\n", rc);
				pthread_exit(&rc);
			}
			rate[1]++;

			//auxanei ton metriti twn tilefwnitwn
			telAvailable++;		

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
		} 
		//an yparxoun arketes theseis
		else { 
			//desmeyei tis theseis sto plano
			rc = pthread_mutex_lock(&seatPlan);
			if (rc != 0) {
				printf("ERROR: return code from pthread_mutex_lock(&seatPlan) is %d\n", rc);
				pthread_exit(&rc);
			}

			seat = seat - numOfseats +1; //gia na min apothikeyontai anapoda
			
			for(i=0; i < numOfseats; i++) {
				seatsTable[zone-1][seat] = *tid;
				seat++;
				seatsNum[i] = seat;
				
				rc = pthread_mutex_lock(&takenCount);
				if (rc != 0) {
					printf("ERROR: return code from pthread_mutex_lock(&takenCount) is %d\n", rc);
					pthread_exit(&rc);
				}
				taken++;
				rc = pthread_mutex_unlock(&takenCount);
				if (rc != 0) {
					printf("ERROR: return code from pthread_mutex_unlock(&takenCount) is %d\n", rc);
					pthread_exit(&rc);
				}
			}
			rc = pthread_mutex_unlock(&seatPlan);
			if (rc != 0) {
				printf("ERROR: return code from pthread_mutex_unlock(&seatPlan) is %d\n", rc);
				pthread_exit(&rc);
			}


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
		if (rc != 0) {
			printf("ERROR: return code from pthread_cond_signal() is %d\n", rc);
			pthread_exit(&rc);
		}
	
		rc = pthread_mutex_unlock(&telcount);
		if (rc != 0) {
			printf("ERROR: return code from pthread_mutex_unlock(&telcount) is %d\n", rc);
			pthread_exit(&rc);
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
	
		clock_gettime(CLOCK_REALTIME, &startover);

		rc = pthread_mutex_unlock(&timecount);	
		if (rc != 0) {
			printf("ERROR: return code from pthread_mutex_unlock(&timecount) is %d\n", rc);
			pthread_exit(&rc);
		}


		//an den yparxei diathesimos tamias perimenei
		while(cashAvailable == 0) {
			rc = pthread_cond_wait(&cashCond, &cashCount);
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
		
		clock_gettime(CLOCK_REALTIME, &midtwo);
		mtwo += midtwo.tv_sec - startover.tv_sec; 

		rc = pthread_mutex_unlock(&timecount);
		if (rc != 0) {
			printf("ERROR: return code from pthread_mutex_unlock(&timecount) is %d\n", rc);
			pthread_exit(&rc);
		}
	
		//epilegei ena tyxaio plithos deyteroleptwn (2-4)
		rc = pthread_mutex_lock(&randLock);
		if (rc != 0) {
			printf("ERROR: return code from pthread_mutex_lock(&randLock) is %d\n", rc);
			pthread_exit(&rc);
		}
	
		cashsleep = (rand_r(&seed)%(Tcashhigh-Tcashlow+1)) + Tcashlow;
		rc = pthread_mutex_unlock(&randLock);
		if (rc != 0) {
			printf("ERROR: return code from pthread_mutex_unlock(&randLock) is %d\n", rc);
			pthread_exit(&rc);
		}

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
	
		rc = pthread_mutex_lock(&cashCount);
		if (rc != 0) {
			printf("ERROR: return code from pthread_mutex_lock(&cashCount) is %d\n", rc);
			pthread_exit(&rc);
		}
	
		//an i plirwmi apotyxei
		if (prob > Pcardsuccess) {
			//apodesmeysi thesewn sto plano
			rc = pthread_mutex_lock(&seatPlan);

			for (i=0; i<numOfseats; i++) {
			seat--;
				seatsTable[zone-1][seat] = 0;
				
				rc = pthread_mutex_lock(&takenCount);
				if (rc != 0) {
					printf("ERROR: return code from pthread_mutex_lock(&takenCount) is %d\n", rc);
					pthread_exit(&rc);
				}
				taken--;
				rc = pthread_mutex_unlock(&takenCount);
				if (rc != 0) {
					printf("ERROR: return code from pthread_mutex_unlock(&takenCount) is %d\n", rc);
					pthread_exit(&rc);
				}
			}
			rc = pthread_mutex_unlock(&seatPlan);
			if (rc != 0) {
				printf("ERROR: return code from pthread_mutex_unlock(&seatPlan) is %d\n", rc);
				pthread_exit(&rc);
			}
			
			seatsNum[0] = '\0';
			rc = pthread_mutex_lock(&monitorLock);
			if (rc != 0) {
				printf("ERROR: return code from pthread_mutex_lock(&monitorLock) is %d\n", rc);
				pthread_exit(&rc);
			}

			printf("Πελατης %d: Η κράτηση ματαιώθηκε γιατί η συναλλαγή με την πιστωτικη κάρτα δεν έγινε αποδεκτή.\n", *tid);
			rc = pthread_mutex_unlock(&monitorLock);
			if (rc != 0) {
				printf("ERROR: return code from pthread_mutex_unlock(&monitorLock) is %d\n", rc);
				pthread_exit(&rc);
			}
			rate[2]++;
			
		}
		//an i pliwmi ginetai apodekti
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
			for(i=0; i<numOfseats; i++){
				printf("%d", seatsNum[i]);
				if(i+1<numOfseats){
					printf(",");
				}
			}
			printf("> στη Ζώνη <%d> και το κόστος συναλλαγής είναι %d ευρώ.\n", zone, cost);

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

	}//yparxoun theseis
	}//not full
	
	
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
		printf("ERROR: return code from pthread_mutex_init(&cashCount) is %d.\n", rc);
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
	printf("Το ποσοστό των συναλλαγών που το θέατρο ήταν γεμάτο είναι %.2f%%\n", (float)rate[0]*100/Ncust);
	printf("Το ποσοστό των συναλλαγών που δεν υπήρχαν αρκετές συνεχόμενες θέσεις είναι %.2f%%\n", (float)rate[1]*100/Ncust);
	printf("Το ποσοστό των συναλλαγών που η πιστωτική δεν έγινε αποδεκτή είναι %.2f%%\n", (float)rate[2]*100/Ncust);
	printf("Το ποσοστό των συναλλαγών που ολοκληρώθηκαν με επιτυχία είναι %.2f%%\n", (float)rate[3]*100/Ncust);
	printf("Ο μέσος όρος αναμονής των πελατών είναι = %.3f δευτερόλεπτα.\n", (m+mtwo)/Ncust);
	printf("Ο μέσος όρος εξυπηρέτησης των πελατών είναι = %.3f δευτερόλεπτα.\n", sum/Ncust);
	
	printf("Πλάνος θέσεων:\n");
	j=0;
	for (i = 0; i< 50; i++) {
		if(seatsTable[j][i] != 0) {
			printf("Ζώνη %d / Θεση %d / Πελατης %d, ", j+1, i+1, seatsTable[j][i]);	
		}
	}
	for (j = 1; j < 3; j++){
		for (i = 0; i < 100; i++) {
			//gia na min emfanizei tis kenes theseis
			if(seatsTable[j][i] != 0) {
				printf("Ζώνη %d / Θεση %d / Πελατης %d, ", j+1, i+1, seatsTable[j][i]);	
			}
		}
	}


	rc = pthread_mutex_destroy(&telcount);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_destroy(&telcount) is %d.\n", rc);
		exit(-1);
	}
	
	rc = pthread_mutex_destroy(&account);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_destroy(&acount) is %d.\n", rc);
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
	
	rc = pthread_mutex_destroy(&cashCount);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_destroy(&cashCount) is %d.\n", rc);
		exit(-1);
	}
	
	rc = pthread_mutex_destroy(&takenCount);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_destroy(&takenCount) is %d.\n", rc);
		exit(-1);
	}

	rc = pthread_cond_destroy(&telCond);
	if (rc != 0) {
		printf("ERROR: return code from pthread_cond_destroy() is %d.\n", rc);
		exit(-1);
	}
	rc = pthread_cond_destroy(&cashCond);
	if (rc != 0) {
		printf("ERROR: return code from pthread_cond_destroy() is %d.\n", rc);
		exit(-1);
	}
	

	free(threads);
	free(rate);
	for(j=0; j<3; j++) {
		free(seatsTable[j]);
	}

 return 1;

}
