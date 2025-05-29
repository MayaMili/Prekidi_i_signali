#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

// funkcije za obradu signala, navedene ispod main-a
void obradi_dogadjaj(int isg);
void obradi_sigterm(int sig);
void obradi_sigint(int sig);
void obradi_sigill(int sig);
void obradi_sigabrt(int sig);
void obradi_sigsegv(int sig);

int nije_kraj = 1;
int prvi_prolaz = 1;
int T_P[5] = {0, 0, 0, 0, 0};
int K_Z[5] = {0, 0, 0, 0, 0};
int SuS[6] = {0, 0, 0, 0, 0, 0}; //sustavski stog
int PretHod[6] = {1, 0, 0, 0, 0, 0}; //stog koji biljezi prethodnike

int main(void) {
    struct sigaction act;

    //1. maskiranje signala SIGTERM, prioritet = 1
    act.sa_handler = obradi_sigterm;
    sigemptyset(&act.sa_mask);
    sigaction(SIGTERM, &act, NULL);

    //2. maskiranje signala SIGINT, prioritet = 2
    act.sa_handler = obradi_sigint;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGTERM);
    sigaction(SIGINT, &act, NULL);

    //3. maskiranje signala SIGILL, prioritet = 3
    act.sa_handler = obradi_sigill;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGTERM);
    sigaddset(&act.sa_mask, SIGINT);
    sigaction(SIGILL, &act, NULL);

    //4. maskiranje signala SIGABRT, prioritet = 4
    act.sa_handler = obradi_sigabrt;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGTERM);
    sigaddset(&act.sa_mask, SIGINT);
    sigaddset(&act.sa_mask, SIGILL);
    sigaction(SIGABRT, &act, NULL);

    //5. maskiranje signala SIGSEGV, prioritet = 5
    act.sa_handler = obradi_sigsegv;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGTERM);
    sigaddset(&act.sa_mask, SIGINT);
    sigaddset(&act.sa_mask, SIGILL);
    sigaddset(&act.sa_mask, SIGABRT);
    sigaction(SIGSEGV, &act, NULL);

    printf("Program s PID=%ld krenuo s radom\n", (long) getpid());
    //neki posao koji program radi; ovdje samo simulacija
    int i = 1;
    while(nije_kraj) {
        printf("Program: iteracija %d\n", i++);
        sleep(1);
    }
    int j;
    printf("K_Z = ");
    for(j = 0; j < 5; j++) {
        printf("%d", K_Z[j]);
    } 
    for (j = 0; j < 5; j++) {
        T_P[j] = 0;
    }
    printf(" T_P = ");
    for(j = 0; j < 5; j++) {
        printf("%d", T_P[j]);
    } 
    for (j = 0; j < 6; j++) {
        SuS[j] = 0;
    }
    printf(" S_Stog = ");
    for(j = 0; j < 6; j++) {
        printf("%d", SuS[j]);
    } 
    printf("\n");
    printf("Program s PID=%ld zavrsio s radom\n", (long) getpid());

    return 0;
}

void obradi_dogadjaj(int sig) {
    int j;
    printf("Pocetak obrade signala %d\n", sig);
    
    printf("K_Z = ");
    for(j = 0; j < 5; j++) {
        printf("%d", K_Z[j]);
    } 
    printf(" T_P = ");
    for(j = 0; j < 5; j++) {
        printf("%d", T_P[j]);
    } 

    printf(" S_Stog = ");
    for(j = 0; j < 6; j++) {
        printf("%d", SuS[j]);
    }
} 

void obradi_sigterm(int sig) {
    int pr = 1;
    int i, j;

    K_Z[pr - 1] = 1;

    if (prvi_prolaz == 0) {
        SuS[0] = 0;
    }

    obradi_dogadjaj(sig);

    K_Z[pr - 1] = 0;

    for (j = 0; j < 5; j++) {
        T_P[j] = 0;
    }
    T_P[pr - 1] = 1;
    
    printf("\n");
    
    for (j = 0; j < 6; j++) {
        SuS[j] = PretHod[j];
    }  
    PretHod[pr] = 1;

    for (i = 1; i <= 10; i++) {
        T_P[pr - 1] = 1;
        SuS[pr] = 0;
        
        printf("Obrada signala %d: %d/10\n", sig, i);
        printf("K_Z = ");

        for(j = 0; j < 5; j++) {
            printf("%d", K_Z[j]);
        } 
    
        printf(" T_P = ");
        for(j = 0; j < 5; j++) {
            printf("%d", T_P[j]);
        } 

        printf(" S_Stog = ");
        for(j = 0; j < 6; j++) {
            printf("%d", SuS[j]);
        }
        printf("\n");
        sleep(1);
    }

    K_Z [pr - 1] = 0;

    for (j = 0; j < 5; j++) {
        T_P[j] = 0;
    }
    printf("Primio signal SIGTERM, pospremam prije izlaska iz programa\n");

    PretHod[pr] = 0;
    SuS[pr] = PretHod[pr];
    prvi_prolaz = 0;
    nije_kraj = 0;
}

void obradi_sigint(int sig) {
    int pr = 2;
    int i, j;

    K_Z[pr - 1] = 1;

    if (prvi_prolaz == 0) {
        SuS[0] = 0;
    }

    obradi_dogadjaj(sig);

    K_Z[pr - 1] = 0;

    for (j = 0; j < 5; j++) {
        T_P[j] = 0;
    }
    T_P[pr - 1] = 1;

    printf("\n");
    
    for (j = 0; j < 6; j++) {
        SuS[j] = PretHod[j];
    }  
    PretHod[pr] = 1;

    int m = 1;
    for (i = 1; i <= 10; i++) {

        //provjera postoji li signal nizeg prioriteta
    sigset_t pending_signali;
    sigpending(&pending_signali);
    
    if (sigismember(&pending_signali, SIGTERM)) {
        K_Z[0] = 1; // Signal SIGTERM ima prioritet 1
        m=0;
        K_Z[1] = m;
    }  

    T_P[pr - 1] = 1;
    SuS[pr] = 0;

    printf("Obrada signala %d: %d/10\n", sig, i);
    printf("K_Z = ");
    
    for(j = 0; j < 5; j++) {
       printf("%d", K_Z[j]);
    } 

    printf(" T_P = ");
    for(j = 0; j < 5; j++) {
        printf("%d", T_P[j]);
    } 

    printf(" S_Stog = ");
    for(j = 0; j < 6; j++) {
        printf("%d", SuS[j]);
    }

    printf("\n");
    sleep(1);

    }
    K_Z [pr - 1] = 0;

    for (j = 0; j < 5; j++) {
        T_P[j] = 0;
    }

    printf("Primio signal SIGINT, pospremam prije izlaska iz programa\n");

    PretHod[pr] = 0;
    SuS[pr] = PretHod[pr];
    prvi_prolaz = 0;
    nije_kraj = 0;
}

void obradi_sigill(int sig) {
    int pr = 3;
    int i, j;

    K_Z[pr - 1] = 1;

    if (prvi_prolaz == 0) {
        SuS[0] = 0;
    }

    obradi_dogadjaj(sig);

    K_Z[pr - 1] = 0;

    for (j = 0; j < 5; j++) {
        T_P[j] = 0;
    }
    T_P[pr - 1] = 1;

    printf("\n");
    
    for (j = 0; j < 6; j++) {
        SuS[j] = PretHod[j];
    }    
    PretHod[pr] = 1;

    int m = 1;
    for (i = 1; i <= 10; i++) {

        //provjera postoji li signal nizeg prioriteta
    sigset_t pending_signali;
    sigpending(&pending_signali);
    
    if (sigismember(&pending_signali, SIGTERM)) {
        K_Z[0] = 1; // Signal SIGTERM ima prioritet 1
        m = 0;
        K_Z[2] = m;
    } 
    if (sigismember(&pending_signali, SIGINT)) {
        K_Z[1] = 1; // Signal SIGINT ima prioritet 2
        m = 0;
        K_Z[2] = m;
    }
    T_P[pr - 1] = 1;
    SuS[pr] = 0;
    
    printf("Obrada signala %d: %d/10\n", sig, i);
        
    printf("K_Z = ");
    for(j = 0; j < 5; j++) {
        printf("%d", K_Z[j]);
    } 

    printf(" T_P = ");
    for(j = 0; j < 5; j++) {
        printf("%d", T_P[j]);
    } 

    printf(" S_Stog = ");
    for(j = 0; j < 6; j++) {
        printf("%d", SuS[j]);
    }

    printf("\n");
    sleep(1);

    }
    K_Z [pr - 1] = 0;

    for (j = 0; j < 5; j++) {
        T_P[j] = 0;
    }

    printf("Primio signal SIGILL, pospremam prije izlaska iz programa\n");

    PretHod[pr] = 0;
    SuS[pr] = PretHod[pr];
    prvi_prolaz = 0;
    nije_kraj = 0;
}

void obradi_sigabrt(int sig) {
    int pr = 4;
    int i, j;

    K_Z[pr - 1] = 1;

    if (prvi_prolaz == 0) {
        SuS[0] = 0;
    }

    obradi_dogadjaj(sig);

    K_Z[pr - 1] = 0;

    for (j = 0; j < 5; j++) {
        T_P[j] = 0;
    }
    T_P[pr - 1] = 1;

    printf("\n");

    for (j = 0; j < 6; j++) {
        SuS[j] = PretHod[j];
    }  
    PretHod[pr] = 1;

    int m = 1;
    for (i = 1; i <= 20; i++) {

        //provjera postoji li signal nizeg prioriteta
    sigset_t pending_signali;
    sigpending(&pending_signali);

    if (sigismember(&pending_signali, SIGTERM)) {
        K_Z[0] = 1; // Signal SIGTERM ima prioritet 1
        m = 0;
        K_Z[3] = m;
    } 
    if (sigismember(&pending_signali, SIGINT)) {
        K_Z[1] = 1; // Signal SIGINT ima prioritet 2
        m = 0;
        K_Z[3] = m;
    } 
    if (sigismember(&pending_signali, SIGILL)) {
        K_Z[2] = 1; // Signal SIGILL ima prioritet 3
        m = 0;
        K_Z[3] = m;
    }    
    
    T_P[pr - 1] = 1;
    SuS[pr] = 0;

    printf("Obrada signala %d: %d/20\n", sig, i);
        
    printf("K_Z = ");
    for(j = 0; j < 5; j++) {
        printf("%d", K_Z[j]);
    } 

    printf(" T_P = ");
    for(j = 0; j < 5; j++) {
       printf("%d", T_P[j]);
    } 
    
    printf(" S_Stog = ");
    for(j = 0; j < 6; j++) {
        printf("%d", SuS[j]);
    }
    
    printf("\n");
    sleep(1);

    }
    K_Z [pr - 1] = 0;

    for (j = 0; j < 5; j++) {
        T_P[j] = 0;
    }

    printf("Primio signal SIGABRT, pospremam prije izlaska iz programa\n");

    PretHod[pr] = 0;
    SuS[pr] = PretHod[pr];
    prvi_prolaz = 0;
    nije_kraj = 0;
}

void obradi_sigsegv(int sig) {
    int pr = 5;
    int i, j;

    K_Z[pr - 1] = 1;

    if (prvi_prolaz == 0) {
        SuS[0] = 0;
    }

    obradi_dogadjaj(sig);

    K_Z[pr - 1] = 0;
    
    for (j = 0; j < 5; j++) {
        T_P[j] = 0;
    }
    T_P[pr - 1] = 1;

    printf("\n");
    
    for (j = 0; j < 6; j++) {
        SuS[j] = PretHod[j];
    }  
    PretHod[pr] = 1;

    int m = 1;
    for (i = 1; i <= 10; i++) {

        //provjera postoji li signal nizeg prioriteta
    sigset_t pending_signali;
    sigpending(&pending_signali);
    
    if (sigismember(&pending_signali, SIGTERM)) {
        K_Z[0] = 1; // Signal SIGTERM ima prioritet 1
        m = 0;
        K_Z[4] = m;
    } 
    if (sigismember(&pending_signali, SIGINT)) {
        K_Z[1] = 1; // Signal SIGINT ima prioritet 2
        m = 0;
        K_Z[4] = m;
    } 
    if (sigismember(&pending_signali, SIGILL)) {
        K_Z[2] = 1; // Signal SIGILL ima prioritet 3
        m = 0;
        K_Z[4] = m;
    } 
    if (sigismember(&pending_signali, SIGABRT)) {
        K_Z[3] = 1; // Signal SIGABRT ima prioritet 4
        m = 0;
        K_Z[4] = m;
    } 
    
    T_P[pr - 1] = 1;
    SuS[pr] = 0;
        
    printf("Obrada signala %d: %d/10\n", sig, i);
        
    printf("K_Z = ");
    for(j = 0; j < 5; j++) {
        printf("%d", K_Z[j]);
    } 
        
    printf(" T_P = ");
    for(j = 0; j < 5; j++) {
        printf("%d", T_P[j]);
    } 

    printf(" S_Stog = ");
    for(j = 0; j < 6; j++) {
        printf("%d", SuS[j]);
    }
        
    printf("\n");
    sleep(1);

    }
    K_Z [pr - 1] = 0;

    for (j = 0; j < 5; j++) {
        T_P[j] = 0;
    }

    printf("Primio signal SIGSEGV, pospremam prije izlaska iz programa\n");
    
    PretHod[pr] = 0;
    SuS[pr] = PretHod[pr];
    prvi_prolaz = 0;
    nije_kraj = 0;
}