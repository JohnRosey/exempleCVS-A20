//#########################################################
//#
//# Titre : 	Utilitaires Liste Chainee et CVS LINUX Automne 20
//#				SIF-1015 - Système d'exploitation
//#				Université du Québec à Trois-Rivières
//#
//# Auteur : 	Francois Meunier
//#	Date :		Septembre 2020
//#
//# Langage : 	ANSI C on LINUX 
//#
//#######################################

#include "gestionListeChaineeCVS.h"

//Pointeur de tête de liste
extern struct noeudV* debutV;
//Pointeur de queue de liste pour ajout rapide
extern struct noeudV* finV;
sem_t semDebutV, semFinV;//semaphore debut et fin version
//#######################################
//#######################################
//#
//# Recherche un item dans la liste chaînée de VERSIONS
//#
//# RETOUR:	Un pointeur vers l'item recherché
//# 		
//# 		Retourne NULL dans le cas où l'item
//#			est introuvable
//#
struct noeudV * findItemV(const int no){
	//La liste est vide 
	if ((debutV==NULL)&&(finV==NULL)) return NULL;
	

	//Pointeur de navigation
	struct noeudV * ptr = debutV;

	if(ptr->noVersion==no) // premier noeud
	    return ptr;

	//Tant qu'un item suivant existe
	while (ptr->suivant!=NULL){

		//Déplacement du pointeur de navigation
		ptr=ptr->suivant;

		//Est-ce l'item recherché?
		if (ptr->noVersion==no){
			//On retourne un pointeur sur l'item
			return ptr;
		}
	}

	//On retourne un pointeur NULL
	return NULL;
}


//#######################################
//#
//# Recherche un item dans la liste chaînée de code d'une version
//#
//# RETOUR:	Un pointeur vers l'item recherché
//# 		
//# 		Retourne NULL dans le cas où l'item
//#			est introuvable
//#
struct noeudL * findItemL(const int noV, const int no){
  
  	struct noeudV * ptrV;
	
	ptrV = findItemV(noV);
	// Verifier si la version existe
	if (ptrV==NULL)
	  	return NULL;


	//La liste  de codes est vide 
	if ((ptrV->debutL==NULL)&&(ptrV->finL==NULL)) return NULL;

	//Pointeur du debut de la liste de codes
	struct noeudL * ptr = ptrV->debutL;

	if(ptr->ligne.noligne==no) // premier noeud
		return ptr;

	//Tant qu'un item suivant existe
	while (ptr->suivant!=NULL){

		//Déplacement du pointeur de navigation
		ptr=ptr->suivant;

		//Est-ce l'item recherché?
		if (ptr->ligne.noligne==no){
			//On retourne un pointeur sur l'item (toujours vérrouillé)
			return ptr;
		}
	}

	//On retourne un pointeur NULL
	return NULL;
}

//#######################################
//#
//# Recherche le PRÉDÉCESSEUR d'un item dans la liste chaînée de versions
//#
//# RETOUR:	Le pointeur vers le prédécesseur est retourné
//# 		
//#			
//# 		Retourne NULL dans le cas où l'item est introuvable
//#
struct noeudV * findPrevV(const int no){

		//La liste  de versions est vide 
	if ((debutV==NULL)&&(finV==NULL)) return NULL;

	//Pointeur du debut de la liste de versions
	struct noeudV * ptr = debutV;

	//Tant qu'un item suivant existe
	while (ptr->suivant!=NULL){

		//Est-ce le prédécesseur de l'item recherché?
		if (ptr->suivant->noVersion==no){
			//On retourne un pointeur sur l'item précédent
			return ptr;
		}

		//Déplacement du pointeur de navigation
		ptr=ptr->suivant;
	}

	//On retourne un pointeur NULL
	return NULL;
}
	
//#######################################
//#
//# Recherche le PRÉDÉCESSEUR d'un item dans la liste chaînée de codes
//#
//# RETOUR:	Le pointeur vers le prédécesseur est retourné
//# 		
//#			
//# 		Retourne NULL dans le cas où l'item est introuvable
//#
struct noeudL * findPrevL(const int noV, const int no){
  
  	struct noeudV * ptrV;
	
	ptrV = findItemV(noV);
	
	// Verifier si la version existe
	if (ptrV==NULL)
	  	return NULL;

		//La liste  de codes est vide 
	if ((ptrV->debutL==NULL)&&(ptrV->finL==NULL)) return NULL;

	//Pointeur du debut de la liste de codes
	struct noeudL * ptr = ptrV->debutL;

	//Tant qu'un item suivant existe
	while (ptr->suivant!=NULL){

		//Est-ce le prédécesseur de l'item recherché?
		if (ptr->suivant->ligne.noligne==no){
			//On retourne un pointeur sur l'item précédent
			return ptr;
		}

		//Déplacement du pointeur de navigation
		ptr=ptr->suivant;
	}

	//On retourne un pointeur NULL
	return NULL;
}

//#######################################
//#
//# Ajoute un noeud (ligne de code) dans la liste chaînée de code d'une version
//#
void *addItemL(void *args){
	struct noeudV * ptrV;//pointeur
	struct paramAL* params = (struct paramAL*)args;

	// Initialisation des variables locales.
	char tl[100];
	int noVersion, nl;
noVersion =   param ->noVersion;
	nl = param->noligne;
	strcpy(tl,(const char*)param->tligne);
	free(param);
	

	// On libère le pointeur pour éviter un coulage de mémoire.
	free(params);
	
	ptrV = findItemV(noVersion);

	// Verifier si la version existe
	if (ptrV==NULL)
	  	return;

	//Création de l'enregistrement en mémoire
	struct noeudL* ni = (struct noeudL*)malloc(sizeof(struct noeudL));

	struct noeudL* ptrINS = findItemL(noVersion, nl);

	//Affectation des valeurs des champs
	ni->ligne.noligne = nl;
	strcpy(ni->ligne.ptrligne, tl);
	sem_init(&ni->semL, 0, 1);

	if((ptrINS == NULL) && (nl == 1)) // ajout au debut de la liste vide
	{
		// premiere ligne premier noeud 
		ni->suivant= NULL;
		sem_wait(&ptrV->semDebutL);		// Verrouille pointeur.
		sem_wait(&ptrV->semFinL);
		ptrV->finL = ptrV-> debutL = ni;
		sem_post(&ptrV->semFinL);
		sem_post(&ptrV->semDebutL);

	}
	else if ((ptrINS == NULL) && (nl > 1)) // ajout a la fin de la liste
	{
		sem_wait(&ptrV->semFinL);
		sem_wait(&ptrV->finL->semL);
		struct noeudL* tptr = ptrV->finL;
		ni->suivant= NULL;
		ptrV->finL = ni;
		tptr->suivant = ni;
		sem_post(&tptr->semL); 
		sem_post(&ptrV->semFinL);
	}
	else
	{
		struct noeudL* tptr = ptrINS;
		sem_wait(&ptrV->semDebutL);
		// Ajout a la tete de la liste.
		if(tptr == ptrV->debutL) {
			ptrV->debutL = ni;
			sem_wait(&ni->semL);
			sem_post(&ptrV->semDebutL);
		}
		else {
			struct noeudL* ptrPREV = findPrevL(noVersion, nl);
			ptrPREV->suivant = ni;
			sem_wait(&ni->semL);
			sem_post(&ptrPREV->semL);
		}
		ni->suivant = tptr;
		sem_post(&ni->semL);
		
		while (tptr!=NULL){
			//Est-ce le prédécesseur de l'item recherché?
			tptr->ligne.noligne++;
			
			//On retourne un pointeur sur l'item précédent
			struct noeudL* otptr = tptr;
			if(tptr->suivant != NULL)
			{
				sem_wait(&tptr->suivant->semL); // verrouiller le noeud suivant dans la liste de code
				//Déplacement du pointeur de navigation
				tptr=tptr->suivant;	
				sem_post(&otptr->semL); // deverrouiller le noeud courant dans la liste de code
			}
			else // tptr->suivant == NULL
			{
				//Déplacement du pointeur de navigation
				tptr=NULL;	
				sem_post(&otptr->semL);
			}
		}
	}
}

//#######################################
//#
//# Ajoute un item (noeud version) a la fin de la liste chaînée de VERSIONS
//#
void addItemV(const int new, const int nv, const char* vl){

	//Création de l'enregistrement en mémoire
	struct noeudV* ni = (struct noeudV*)malloc(sizeof(struct noeudV));

	// Initialisation sémaphores pour les accès en exclusion mutuelle, lors de création d'un noeud.
	sem_init(&ni->semV, 0, 1);	
	sem_init(&ni->semDebutL, 0, 1);		
	sem_init(&ni->semFinL, 0, 1);		

	sem_destroy(&ni->semV);
	sem_destroy(&ni->semDebutL);
	sem_destroy(&ni->semFinL);

	//Affectation des valeurs des champs de la struct noeudV
	ni->noVersion	= nv;
	strcpy(ni->ptrNoVersion, vl);
	ni->debutL	= NULL;
	ni->finL	= NULL;
	
	if(new)
	{
	    ni->oldVersion	= false; // 0 old 1 new
	    ni->commited	= false; // never commited	  
	}
	else // old version
	{
	    ni->oldVersion	= true; // 0 old 1 new
	    ni->commited	= false; // never commited	  
	}
	
	if(finV == NULL) // ajout au debut de la liste vide
	{
		// premiere version au  premier noeud 
		ni->suivant= NULL;
		finV = debutV = ni;

	}
	else  // ajout a la fin de la liste
	{
	// ajout a la fin de la liste de versions
	    struct noeudV* tptr = finV;
	    ni->suivant= NULL;
	    finV = ni;
	    tptr->suivant = ni;	
	}

}

//#######################################
//#
//# Copier un item (noeud version) a la fin de la liste chaînée de VERSIONS (BRANCH)
//#
void copyItemV(const int new, const int nv){
  
	struct noeudV * ptrV = findItemV(nv); // ptr sur la version a copier 
	
	// Verifier si la version existe
	if (ptrV==NULL)
	  	return;

	//Création de l'enregistrement en mémoire
	struct noeudV* ni = (struct noeudV*)malloc(sizeof(struct noeudV));


	//Affectation des valeurs des champs de la struct noeudV
	ni->noVersion	= finV->noVersion+1;

	char nomVersion[200];
	sprintf(nomVersion,"V%d",ni->noVersion);
	strcpy(ni->ptrNoVersion, nomVersion); // copier le nouveau nom de version
	ni->debutL	= NULL;
	ni->finL	= NULL;
	
	if(new)
	{
	    ni->oldVersion	= false; // 0 old 1 new
	    ni->commited	= false; // never commited	  
	}
	else // old version
	{
	    ni->oldVersion	= true; // 0 old 1 new
	    ni->commited	= false; // never commited	  
	}
	
	// ajout a la fin de la liste de versions
	struct noeudV* tptr = finV;
	ni->suivant = NULL;
	finV = ni;
	tptr->suivant = ni;
	
	struct noeudL * ptrL = ptrV->debutL; // copier la liste de code de la version a la fin

	// Allocation de mémoire pour la structure que l'on va transférer à la fonction CONCURRENTE addItemL().
	struct paramAL *ptr = (struct paramAL*) malloc(sizeof(struct paramAL));
	ptr->noVersion = finV->noVersion;
	ptr->noLigne = ptrL->ligne.noligne;
	strcpy(ptr->tLigne, (const char *)ptrL->ligne.ptrligne);

	while (ptrL!=NULL){ 
		//Affectation des valeurs des champs
		// TODO: À REVOIR. (Threading)
		addItemL(ptr);
		ptrL = ptrL->suivant;
	}
}


//#######################################
//#
//# Retire un item de la liste chaînée de versions
//#
void removeItemV(const int noV){

	struct noeudV * ptrV;
	struct noeudV * tptrV;
	struct noeudV * optrV;
	struct noeudL * ptrL;
	struct noeudL * ptrLSuivant;
	
	char texteVersion[100];


	//Vérification sommaire (no>0 et liste non vide)
	
	if ((noV<1)||((debutV==NULL)&&(finV==NULL)))
		return;

	//Pointeur de recherche
	if(noV==1){
		ptrV = debutV; // suppression du premier element de la liste de versions
	}
	else{
		ptrV = findPrevV(noV);
	}


	//L'item a été trouvé
	if (ptrV!=NULL){

		// Memorisation du pointeur de l'item en cours de suppression
		// Ajustement des pointeurs
		if((debutV == ptrV) && (noV==1)) // suppression de l'element de tete
		{
			if(debutV==finV) // un seul element dans la liste des version
			{
			    ptrL = ptrV->debutL; // supprimer la liste de code de la version
			    while (ptrL!=NULL){ 
					ptrLSuivant = ptrL->suivant;
					free(ptrL); // suppression du noeud de la liste de code 
					ptrL=ptrLSuivant;
			    }

			    free(ptrV);
			    finV = debutV = NULL;
			    return;
			}
			tptrV = ptrV->suivant;
			debutV = tptrV;
			ptrL = ptrV->debutL; // supprimer la liste de code de la version
			while (ptrL!=NULL){ 
			    ptrLSuivant = ptrL->suivant;
			    free(ptrL); // suppression du noeud de la liste de code 
			    ptrL=ptrLSuivant;
			}

			free(ptrV);
		}
		else if (finV==ptrV->suivant) // suppression de l'element de queue
		{
			finV=ptrV;
			ptrL = ptrV->suivant->debutL; // supprimer la liste de code de la version a la fin
			while (ptrL!=NULL){ 
				ptrLSuivant = ptrL->suivant;
				free(ptrL); // suppression du noeud de la liste de code 
				ptrL=ptrLSuivant;
			}

			free(ptrV->suivant);
			ptrV->suivant=NULL;
			return;
		}
		else // suppression d'un element dans la liste
		{
			optrV = ptrV->suivant;
			ptrL = ptrV->suivant->debutL; // supprimer la liste de code de la version a la fin
			while (ptrL!=NULL){ 
				ptrLSuivant = ptrL->suivant;
				free(ptrL); // suppression du noeud de la liste de code 
				ptrL=ptrLSuivant;
			}

			ptrV->suivant = ptrV->suivant->suivant;
			tptrV = ptrV->suivant;
			free(optrV);
		}
		
		
		while (tptrV!=NULL){ // ajustement des numeros de version
			//Est-ce le prédécesseur de l'item recherché?
			tptrV->noVersion--;
			//On retourne un pointeur sur l'item précédent
			
			sprintf(texteVersion, "V%d",tptrV->noVersion);
			strcpy(tptrV->ptrNoVersion, texteVersion);

			//Déplacement du pointeur de navigation
			tptrV=tptrV->suivant;
		}
	}
}



//#######################################
//#
//# Retire un item de la liste chaînée de code d'une version noVersion
//#
void removeItemL(const int noVersion, const int noline){

	struct noeudL * ptrL;
	struct noeudL * tptrL;
	struct noeudL * optrL;
	struct noeudV * ptrV;
	
	ptrV = findItemV(noVersion);
	
	// Verifier si la version existe
	if (ptrV==NULL)
	  return;

	//Vérification sommaire (no>0 et liste non vide)
	
	if ((noline<1)||((ptrV->debutL==NULL)&&(ptrV->finL==NULL)))
		return;

	//Pointeur de recherche
	if(noline==1){
		ptrL = ptrV->debutL; // suppression du premier element de la liste
	}
	else{
		ptrL = findPrevL(noVersion, noline);
	}
	//L'item a été trouvé
	if (ptrL!=NULL){

		// Memorisation du pointeur de l'item en cours de suppression
		// Ajustement des pointeurs
		if((ptrV->debutL  == ptrL) && (noline==1)) // suppression de l'element de tete
		{
			if(ptrV->debutL == ptrV->finL) // un seul element dans la liste de code
			{
				free(ptrL);
				ptrV->debutL = ptrV->finL = NULL;
				return;
			}

			tptrL = ptrL->suivant;
			ptrV->debutL = tptrL;
			free(ptrL);
		}
		else if (ptrV->finL == ptrL->suivant) // suppression de l'element de queue
		{
			ptrV->finL = ptrL;
			free(ptrL->suivant);
			ptrL->suivant=NULL;
			return;
		}
		else // suppression d'un element dans la liste de code
		{
			optrL = ptrL->suivant;	
			ptrL->suivant = ptrL->suivant->suivant;
			tptrL = ptrL->suivant;
			free(optrL);
		}
		
		while (tptrL!=NULL){ // ajustement des numeros de ligne
			//Est-ce le prédécesseur de l'item recherché?
			tptrL->ligne.noligne--;
			//On retourne un pointeur sur l'item précédent	

			//Déplacement du pointeur de navigation
			tptrL=tptrL->suivant;
		}
	}
}

//#######################################
//#
//# Modifie un item de la liste chaînée
//#
void modifyItemL(const int noVersion, const int noline, const char* tline){
	struct noeudV * ptrV;

	ptrV = findItemV(noVersion);
	
	// Verifier si la version existe
	if (ptrV==NULL)
	  return;
	//Vérification sommaire (no>0 et liste non vide)

	if ((noline<1)||((ptrV->debutL==NULL)&&(ptrV->finL==NULL)))
		return;


	//Recherche de l'élément à modifier
	struct noeudL * ptrL = findItemL(noVersion, noline);

	//L'élément à été trouvé
	if (ptrL!=NULL){

		//Modification des champs de l'élément trouvé
		strcpy(ptrL->ligne.ptrligne, tline);

	}
}

//#######################################
//#
//# Affiche les items dont le numéro séquentiel est compris dans une plage de numero de version
//#
void listItemsV(const int start, const int end){

	//Affichage des entêtes de colonnes
	printf("noVersion  Nom Version                                  \n");
	printf("======= ================================================\n");

	struct noeudV * ptr = debutV;			//premier element de la liste des version

	while (ptr!=NULL){

		//L'item a un numéro séquentiel dans l'interval défini
		if ((ptr->noVersion>=start)&&(ptr->noVersion<=end)){
			printf("%d \t %s\n", ptr->noVersion, ptr->ptrNoVersion);
		}
		if (ptr->noVersion>end){
			//L'ensemble des items potentiels sont maintenant passés
			//Déplacement immédiatement à la FIN de la liste des versions
			//Notez que le pointeur optr est toujours valide
			ptr=NULL;
		}
		else{
			ptr = ptr->suivant;
		}
	}

	//Affichage des pieds de colonnes
	printf("======= ===============================================\n\n");
}
	
	
//#######################################
//#
//# Affiche les items dont le numéro séquentiel est compris dans une plage de numero de ligne de code
//#
void listItemsL(const int noVersion, const int start, const int end){
  
	struct noeudV * ptrV;
	
	ptrV = findItemV(noVersion);
	
	// Verifier si la version existe
	if (ptrV==NULL)
	  return;

	//Affichage des entêtes de colonnes
	printf("noVersion noligne	texte                                          \n");
	printf("=======   ===========   ==================================\n");

	struct noeudL * ptr = ptrV->debutL;			//premier element de la liste de code d'une version


	while (ptr!=NULL){

		//L'item a un numéro séquentiel dans l'interval défini
		if ((ptr->ligne.noligne>=start)&&(ptr->ligne.noligne<=end)){
			printf("%d \t %d \t %s\n", ptrV->noVersion, ptr->ligne.noligne, ptr->ligne.ptrligne);
		}
		if (ptr->ligne.noligne>end){
			//L'ensemble des items potentiels sont maintenant passés
			//Déplacement immédiatement à la FIN de la liste
			//Notez que le pointeur optr est toujours valide
			ptr=NULL;
		}
		else{
			ptr = ptr->suivant;
		}
	}

	//Affichage des pieds de colonnes
	printf("=========================================================\n\n");
}

