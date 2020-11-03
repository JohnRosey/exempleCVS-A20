#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

struct infoligne{						
	int		noligne;				
	char	ptrligne[100];							
	};								 

struct noeud{			
	struct infoligne	ligne;		
	struct noeud		*suivant;	
	};	
	
void cls(void);
void error(const int exitcode, const char * message);

struct noeud * findItem(const int no);
struct noeud * findPrev(const int no);

void addItem(const int noline, const char* tline);
void removeItem(const int noline);
void modifyItem(const int noline, const char* tline);
void listItems(const int start, const int end);
void saveItems(const char* sourcefname);
void executeFile(const char* sourcefname);

void loadFich(const char* sourcefname);
void* readTrans(char* nomFichier);
