#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
sem_t semDebutV, semFinV;//semaphore debut et fin version
//#######################################
struct infoligne{						
	int	noligne;
	char ptrligne[100];
};

struct noeudL{
	struct infoligne ligne;
	struct noeudL *suivant;
	sem_t semL;
};
	
struct noeudV{
	int noVersion;
	char ptrNoVersion[100];
	int oldVersion;
	int commited;
	struct noeudL *debutL;
	struct noeudL *finL;
	struct noeudV *suivant;
	sem_t semV, semDebutL, semFinL;
};
struct paramAL{
	int noVersion;
    int noligne;
	char tligne[100];
	};
struct paramAl param;

void cls(void);
void error(const int exitcode, const char * message);

struct noeudL* findItemL(const int noV, const int no);
struct noeudL* findPrevL(const int noV, const int no);
struct noeudV* findItemV(const int no);
struct noeudV* findPrevV(const int no);

void addItemV(const int new, const int nv, const char* vl);
void addItemL(struct paramAL* param);
void copyItemV(const int new, const int nv);
void removeItemV(const int noV);
void removeItemL(const int noVersion, const int noline);
void modifyItemL(const int noVersion, const int noline, const char* tline);
void listItemsV(const int start, const int end);
void listItemsL(const int noVersion, const int start, const int end);

void saveItemsV();
void saveItemsL(const int noVersion);
void executeFile(char* sourcefname);
void listFileC();
void loadVersions();
void loadVersion(const int noV);
void* readTrans(char* nomFichier);
