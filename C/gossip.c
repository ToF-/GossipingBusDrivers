#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXDRIVERS (256)
#define MAXMINUTES (480)
#define MAXLINE (1024)
char Line[MAXLINE];

int Knowledge[MAXDRIVERS][MAXDRIVERS];
int MaxDrivers;
int Meeting[MAXDRIVERS];
int MaxMeeting;

struct route {
    int stops[MAXMINUTES];
    int maxStops;
};

struct route Routes[MAXDRIVERS];

struct meeting {
    int driver;
    int stop;
};

struct meeting Meetings[MAXDRIVERS];
int MaxMeeting;

int Gossip[MAXDRIVERS];

int nextStop(struct route r, int minute) {
    return r.stops[minute % r.maxStops];
}

void initKnowledge() {
    for(int i=0; i<MaxDrivers; i++)
        for(int j=0; j<MaxDrivers; j++)
            Knowledge[i][j] = i == j ? 1 : 0;
}

void shareKnowledge(int start, int end) {
    if (start >= end)
        return;
    for(int i=0; i<MAXDRIVERS; i++) {
        Gossip[i]=0;
    }
    for(int m = start; m <= end; m++) {
        int driver = Meetings[m].driver;
        for(int d = 0; d < MaxDrivers; d++)
            Gossip[d] |= Knowledge[driver][d];
    }
    for(int m = start; m <= end; m++) {
        int driver = Meetings[m].driver;
        for(int d = 0; d < MaxDrivers; d++)
            Knowledge[driver][d] |= Gossip[d];
    }
}

int knowledgeComplete() {
    for(int i = 0; i<MaxDrivers; i++)
        for(int j = 0; j<MaxDrivers; j++)
            if(!Knowledge[i][j])
                return 0;
    return 1;
}

static int compare(void const *a, void const *b) {
    struct meeting *pa = (struct meeting *)a;
    struct meeting *pb = (struct meeting *)b;
    return pa->stop < pb->stop ? -1 : pa->stop > pb->stop ? +1 : pa->driver - pb->driver;
}
int timeToFullKnowledge(){
    for(int minute = 0; minute < 480; minute++) {
        MaxMeeting = 0;
        for(int driver = 0; driver<MaxDrivers; driver++) {
            Meetings[MaxMeeting].driver = driver;
            Meetings[MaxMeeting].stop = nextStop(Routes[driver], minute);
            MaxMeeting++;
        }
        qsort(Meetings, MaxMeeting, sizeof(struct meeting), compare);
        int start = 0;
        int currentStop = -1;
        for(int m=0; m<MaxMeeting; m++) {
            if(Meetings[m].stop != currentStop) {
                shareKnowledge(start, m-1);
                start = m;
            }
            currentStop = Meetings[m].stop;
        }
        shareKnowledge(start, MaxMeeting-1);
        if(knowledgeComplete())
            return minute+1;
    }
    return -1;
}

int get_int(char *line) {
    int n;
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &n);
    return n;
}

int get_ints(char *line, int *ints) {
    fgets(line, MAXLINE, stdin);
    char *strToken = strtok(line, " " );
    int n = 0;
    while(strToken != NULL) {
        sscanf(strToken, "%d", ints);
        n++;
        ints++;
        strToken = strtok(NULL, " ");
    }
    return n;
}

int main() {
    MaxDrivers = get_int(Line);
    for(int i=0; i<MaxDrivers; i++) {
        int maxStops = get_int(Line);
        Routes[i].maxStops = maxStops;
        get_ints(Line, Routes[i].stops);
    }
    initKnowledge();
    int result = timeToFullKnowledge();
    if(result == -1)
        printf("never\n");
    else
        printf("%d\n", result);
   return 0;
}
