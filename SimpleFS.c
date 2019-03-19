#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXNAME	255
#define MAXH 255
#define MAXNODE 1024
#define COMMAND 10

#define WRONG -1
#define CREATE 1
#define CREATE_DIR 2
#define	READ 3
#define	WRITE 4
#define	DELETE 5
#define	DELETE_R 6
#define FIND 7

#define STR_BLOCK 20


typedef struct
{
  char type;
  char* name;
  char* content;
  int dim,charge,nc;
  struct table * child;
}node;

typedef struct table{
	char valid;
	char* name;
	char type;
	node* addr;	
}h_table;


int switch_num(char* str){
	if(strcmp(str,"create")==0)
		return CREATE;
	if(strcmp(str,"create_dir")==0)
		return CREATE_DIR;
	if(strcmp(str,"read")==0)
		return READ;
	if(strcmp(str,"write")==0)
		return WRITE;
	if(strcmp(str,"delete")==0)
		return DELETE;
	if(strcmp(str,"delete_r")==0)
		return DELETE_R;
	if(strcmp(str,"find")==0)
		return FIND;
	return WRONG;
}


char* verify(char* dir){	//VERIFICA CORRETTEZZA SINTATTICA DEL PERCORSO
	int i=0,j=0,err=0,dim=0;


	if(dir[0]!='/'){
		dir[0]='0';
		err=1;
	}
	while (err==0 && dir[i-1]=='/'){
		j=0;
		while(j<=255 && dir[j]!='\0'){
			if(dir[j]=='/'&& dir[j+1]=='/'){
				err=1;
			}
			j++;
			i++;
		}
	}
	return dir;
}


char *readline(char * string)	//LETTURA STRINGA DI TESTO
{	
	char tmp;
	int i=0;
	string[0]='\0';
	while (tmp!='"' && i<=100){
		tmp=getchar();
		i++;
	}
	i=0;
	do {
		string[i] = (char) getchar();
		i++;
	} while (string[i -1] != '"');

	string[i - 1] = '\0';
	return string;
}

void create_node (node* t,char type, char* name){	//CREAZIONE NUOVO NODO E ALLOCAZIONE DINAMICA MEMORIA DEI CAMPI CHE NE NECESSITANO
	int i;
	t->type=type;
	t->name=(char*)calloc((strlen(name)+1),sizeof(char));
	for(i=0;i<(strlen(name)+1);i++){
		t->name[i]=name[i];
	}
	t->content=(char*)calloc(1,sizeof(char));
	t->content[0]='\0';
	t->dim=64;
	t->charge=0;
	t->nc=0;
	if(type=='C'){
		t->child=(h_table *)calloc((t->dim),sizeof(h_table));
		for(i=0;i<(t->dim);i++){
			t->child[i].valid='E';  //E=empty  V=valid  T=tombstone
			t->child[i].addr=(void*)0;
		}
	}
	return;
}

int hash(char* str,int base){	//FUNZIONE DI HASHING SEMPLICE PESANDO IL NOME
	int i=0,sum=0;
	while(str[i]!='\0'){
		sum=sum+(int)str[i];
		i++;
	}
	sum=sum%(base);
	return sum;
}

void rehash(node * t){   //FUNZIONE DI REHASHING
	node* next;
	h_table*tmp;
	int i=0,index=0,old_n,h,tent,x;

	tmp=t->child;	//vecchia tabella
	t->dim=2*(t->dim);
	t->child=(h_table*)calloc(t->dim,sizeof(h_table));		//nuova tabella hash raddoppiata
	old_n=t->nc;
	t->nc=0;
	while(t->nc<old_n){		//spostamento valori vecchia tabella -> nuova tabella
		while(tmp[index].valid!='V'){
			index++;
		}
		next=tmp[index].addr;
		h=hash(next->name,t->dim);
		tent=0;
		x=h;
		while(t->child[x].valid=='V'){
			x=quadratic(h,(t->dim),tent);;
			tent++;
		}
		t->child[x].addr=tmp[index].addr;
		t->child[x].valid=tmp[index].valid;
		t->nc++;
		index++;
	}
	int k=0;
	while(k<t->dim){	//riempimento celle nuova tabella non indirizzate
		if(t->child[k].valid!='V'){
			t->child[k].addr=(void*)0;
			t->child[k].valid='E';
		}
		k++;
	}
	free(tmp);		//deallocazione vecchia tabella
	return;
}
int quadratic(int h, int dim,int i){	//FUNZIONE ISPEZIONE QUADRATICA
	int n,x;
	n=(h+((i+i*i)>>1));
	x=n%dim;
	return x;
}

void create(node* t,char* dir){		//CREAZIONE NUOVO FILE
	char tmp[MAXNAME+1];
	node * next;
	node * control;
	int i=0,j=0,k=0,h=0,exit=0,x=0;
	int hop=0;
	float charge=0.0;
	i=1;
	
	while(dir[i]!='\0' && hop<=255){	//cicli di scorrimento directory intermedie
		j=0;
		while(j<=255 && dir[i]!='/' && dir[i]!='\0'){
			tmp[j]=dir[i];
			i++;
			j++;
		}
		tmp[j]='\0';
		h=hash(tmp,t->dim);
		x=h;
		if (dir[i]=='\0'){	//fine percorso directory, valutazione creazione file
				exit=0;
				charge=(float)(t->nc)/(t->dim);
				if(charge>=0.75){	//condizione rehashing
					rehash(t);
					h=hash(tmp,t->dim);
					x=h;
				}
				k=1;
				x=h;
				while(k<t->dim &&(t->child[x].valid=='V'||t->child[x].valid=='T')){		//controllo duplicati
					if(t->child[x].valid=='V'){
						control=t->child[x].addr;
						if(strcmp(control->name,tmp)==0){
							exit=1;
						}
					}
					x=quadratic(h,(t->dim),k);
					k++;
				}
				if (exit==0){
					k=1;
					x=h;
					while(t->child[x].valid=='V' && k<t->dim){	//scorrimento fino ad una cella libera
						x=quadratic(h,(t->dim),k);
						k++;
					}
					if (k<t->dim){	//allocazione nuovo file
						node * new=(node*)calloc(1,sizeof(node));
						create_node(new,'F',tmp);
						t->child[x].valid='V';
						t->child[x].addr=new;
						t->nc++;
						printf("ok\n");
						return;
					}
				}
				printf("no\n");
				return;
		}
		
		next=t->child[h].addr;
		exit=0;
		
		//individuo directory successiva
		if(t->child[h].valid=='V' && strcmp(next->name,tmp)==0 && next->type=='C'){
			t=t->child[h].addr;
			exit=1;
			hop++;
		}else{
			k=1;
			x=h;
			while(k<t->dim && exit==0){
				if(t->child[x].valid=='V'){
					next=t->child[x].addr;
					if(strcmp(next->name,tmp)==0 && next->type=='C'){
						exit=1;
					}
				}
				if(exit==0){
					x=quadratic(h,(t->dim),k);
				}
				k++;
			}
			if(exit==1){
				t=t->child[x].addr;
				hop++;
			}
		}
		if(exit==0){
			printf("no\n");
			return;
		}
		if(dir[i]!='\0'){
			i++;
		}	
	}
	return;
}

void create_dir(node* t,char* dir){			//CREAZIONE NUOVA CARTELLA
	char tmp[MAXNAME+1];
	node * next;
	node * control;
	int i=0,j=0,k=0,h=0,exit=0,x=0;
	int hop=0;
	float charge=0.0;
	i=1;
	
	while(dir[i]!='\0' && hop<=255){	//cicli di scorrimento directory intermedie
		j=0;
		while(j<=255 && dir[i]!='/' && dir[i]!='\0'){
			tmp[j]=dir[i];
			i++;
			j++;
		}
		tmp[j]='\0';
		h=hash(tmp,t->dim);
		x=h;
		if (dir[i]=='\0'){	//fine percorso directory, valutazione creazione cartella
			exit=0;
			charge=(float)(t->nc)/(t->dim);
			if(charge>=0.75){	//condizione rehashing
					rehash(t);
					h=hash(tmp,t->dim);
					x=h;
			}
			k=1;
			x=h;
			while(k<t->dim &&(t->child[x].valid=='V'||t->child[x].valid=='T')){		//controllo duplicati
				if(t->child[x].valid=='V'){
					control=t->child[x].addr;
					if(strcmp(control->name,tmp)==0){
						exit=1;
					}
				}
				x=quadratic(h,(t->dim),k);
				k++;
			}
			if (exit==0){			
				k=1;
				x=h;
				while(t->child[x].valid=='V' && k<t->dim){	//scorrimento fino ad una cella libera
					x=quadratic(h,(t->dim),k);
					k++;
				}
				if (k<t->dim){		//allocazione nuova cartella
					node * new=(node*)calloc(1,sizeof(node));
					create_node(new,'C',tmp);
					t->child[x].valid='V';
					t->child[x].addr=new;
					t->nc++;
					printf("ok\n");
					return;
				}
			}
			printf("no\n");
			return;
		}
		
		next=t->child[h].addr;
		exit=0;
		
		//individuo directory successiva
		if(t->child[h].valid=='V' && strcmp(next->name,tmp)==0 && next->type=='C'){
			t=t->child[h].addr;
			exit=1;
			hop++;
		}else{
			k=1;
			x=h;
			while(k<t->dim && exit==0){
				if(t->child[x].valid=='V'){
					next=t->child[x].addr;
					if(strcmp(next->name,tmp)==0 && next->type=='C'){
						exit=1;
					}
				}
				if(exit==0){
					x=quadratic(h,(t->dim),k);
				}
				k++;
			}
			if(exit==1){
				t=t->child[x].addr;
				hop++;
			}
		}
		if(exit==0){
			printf("no\n");
			return;
		}
		if(dir[i]!='\0'){
			i++;
		}
	}
	return;
}

void write(node* t,char* dir, char* text){		//SCRITTURA FILE
	char tmp[MAXNAME+1];
	node * next;
	int i=0,j=0,k=0,h=0,x=0;
	int hop=0,exit=0;
	i=1;
	
	while(dir[i]!='\0' && hop<=255){	//cicli di scorrimento directory intermedie
		j=0;
		while(j<=255 && dir[i]!='/' && dir[i]!='\0'){
			tmp[j]=dir[i];
			i++;
			j++;
		}
		tmp[j]='\0';
		h=hash(tmp,t->dim);
		x=h;
		if (dir[i]=='\0'){	//fine percorso directory, valutazione scrittura file
			exit=0;
			k=1;
			x=h;
			next=t->child[x].addr;
			while(exit==0 && k<t->dim &&(t->child[x].valid=='V'||t->child[x].valid=='T')){	//ricerca file
				if(t->child[x].valid=='V' && strcmp(next->name,tmp)==0 && next->type=='F'){
					exit=1;
				}else{
					x=quadratic(h,(t->dim),k);
					next=t->child[x].addr;
					k++;
				}
			}
			if (exit==1){	//scrittura file
				next->content=realloc(next->content,(strlen(text)+1)*sizeof(char));
				strcpy(next->content,text);
				printf("ok %d\n",strlen(text));
				return;
			}
			printf("no\n");
			return;
		}
		
		next=t->child[h].addr;
		exit=0;
		//individuo directory successiva
		if(t->child[h].valid=='V' && strcmp(next->name,tmp)==0 && next->type=='C'){
			t=t->child[h].addr;
			exit=1;
			hop++;
		}else{
			k=1;
			x=h;
			while(k<t->dim && exit==0){
				if(t->child[x].valid=='V'){
					next=t->child[x].addr;
					if(strcmp(next->name,tmp)==0 && next->type=='C'){
						exit=1;
					}
				}
				if(exit==0){
					x=quadratic(h,(t->dim),k);
				}
				k++;
			}
			if(exit==1){
				t=t->child[x].addr;
				hop++;
			}
		}
		if(exit==0){
			printf("no\n");
			return;
		}
		if(dir[i]!='\0'){
			i++;
		}	
	}
	return;
}

void read(node* t,char* dir){		//LETTURA FILE
	char tmp[MAXNAME+1];
	node * next;
	int i=0,j=0,k=0,h=0,x=0;
	int hop=0,exit=0;
	i=1;
	
	while(dir[i]!='\0' && hop<=255){	//cicli di scorrimento directory intermedie
		j=0;
		while(j<=255 && dir[i]!='/' && dir[i]!='\0'){
			tmp[j]=dir[i];
			i++;
			j++;
		}
		tmp[j]='\0';
		h=hash(tmp,t->dim);
		x=h;
		
		if (dir[i]=='\0'){	//fine percorso directory, valutazione lettura file
			k=1;
			x=h;
			exit=0;
			next=t->child[x].addr;
			while(exit==0 && k<t->dim &&(t->child[x].valid=='V'||t->child[x].valid=='T')){	//ricerca file
				if(t->child[x].valid=='V' && strcmp(next->name,tmp)==0 && next->type=='F'){
					exit=1;
				}else{
					x=quadratic(h,(t->dim),k);
					next=t->child[x].addr;
					k++;
				}
			}
			if (exit==1){	//stampa a video file
				printf("contenuto %s\n",next->content);
				return;
			}
			printf("no\n");
			return;
		}
		
		next=t->child[h].addr;
		exit=0;
		//individuo directory successiva
		if(t->child[h].valid=='V' && strcmp(next->name,tmp)==0 && next->type=='C'){
			t=t->child[h].addr;
			exit=1;
			hop++;
		}else{
			k=1;
			x=h;
			while(k<t->dim && exit==0){
				if(t->child[x].valid=='V'){
					next=t->child[x].addr;
					if(strcmp(next->name,tmp)==0 && next->type=='C'){
						exit=1;
					}
				}
				if(exit==0){
					x=quadratic(h,(t->dim),k);
				}
				k++;
			}
			if(exit==1){
				t=t->child[x].addr;
				hop++;
			}
		}
		if(exit==0){
			printf("no\n");
			return;
		}
		if(dir[i]!='\0'){
			i++;
		}	
	}
	return;
}

void delete(node* t,char* dir){		//ELIMINAZIONE FILE-CARTELLA
	char tmp[MAXNAME+1];
	node * next;
	int i=0,j=0,k=0,h=0,l=0,x=0;
	int hop=0,exit=0;
	i=1;
	
	while(dir[i]!='\0' && hop<=255){	//cicli di scorrimento directory intermedie
		j=0;
		while(j<=255 && dir[i]!='/' && dir[i]!='\0'){
			tmp[j]=dir[i];
			i++;
			j++;
		}
		tmp[j]='\0';
		h=hash(tmp,t->dim);
		x=h;
		if (dir[i]=='\0'){	//fine percorso directory, valutazione cancellazione
			k=1;
			x=h;
			exit=0;
			next=t->child[x].addr;
			while(exit==0 && k<t->dim &&(t->child[x].valid=='V'||t->child[x].valid=='T')){	//cerco file-cartella
				if(t->child[x].valid=='V' && strcmp(next->name,tmp)==0){
					exit=1;
				}else{
					x=quadratic(h,(t->dim),k);
					next=t->child[x].addr;
					k++;
				}
			}
			if (exit==1 && next->type=='F'){	//cancellazione se file
				free(next->name);
				free(next->content);
				free(next->child);
				free(next);
				t->child[x].valid='T';
				t->child[x].addr=(void*)0;
				t->nc--;
				printf("ok\n");
				return;
			}
			if (exit==1 && next->type=='C'){	//cancellazione se cartella
				l=0;
				while(l<(next->dim) && exit==1){
					if(next->child[l].valid=='V'){	//controllo non abbia subdirectory
						exit=0;
					}
					l++;
				}
				if(exit!=0){  //non ci sono subdirectory o file nella cartella
					free(next->name);
					free(next->content);
					free(next->child);
					free(next);
					t->child[x].valid='T';
					t->child[x].addr=(void*)0;
					t->nc--;
					printf("ok\n");
					return;
				}
			}
			printf("no\n");
			return;
		}
		
		next=t->child[h].addr;
		exit=0;
		//individuo directory successiva
		if(t->child[h].valid=='V' && strcmp(next->name,tmp)==0 && next->type=='C'){
			t=t->child[h].addr;
			exit=1;
			hop++;
		}else{
			k=1;
			x=h;
			while(k<t->dim && exit==0){
				if(t->child[x].valid=='V'){
					next=t->child[x].addr;
					if(strcmp(next->name,tmp)==0 && next->type=='C'){
						exit=1;
					}
				}
				if(exit==0){
					x=quadratic(h,(t->dim),k);
				}
				k++;
			}
			if(exit==1){
				t=t->child[x].addr;
				hop++;
			}
		}
		if(exit==0){
			printf("no\n");
			return;
		}
		if(dir[i]!='\0'){
			i++;
		}		
	}
	return;
}

void delete_r(node* t,char* dir){		//ELIMINAZIONE FILE-CARTELLA RICORSIVA
	char tmp[MAXNAME+1];
	node * next;
	int i=0,j=0,k=0,h=0,l=0,x=0;
	int hop=0,exit=0,num=0;
	i=1;
	
	while(dir[i]!='\0' && hop<=255){	//cicli di scorrimento directory intermedie
		j=0;
		while(j<=255 && dir[i]!='/' && dir[i]!='\0'){
			tmp[j]=dir[i];
			i++;
			j++;
		}
		tmp[j]='\0';
		h=hash(tmp,t->dim);
		x=h;
		if (dir[i]=='\0'){	//fine percorso directory, valutazione eliminazione file-cartella
			k=1;
			x=h;
			exit=0;
			next=t->child[x].addr;
			while(exit==0 && k<t->dim &&(t->child[x].valid=='V'||t->child[x].valid=='T')){	//cerco file-cartella
				if(t->child[x].valid=='V' && strcmp(next->name,tmp)==0){
					exit=1;
				}else{
					x=quadratic(h,(t->dim),k);
					next=t->child[x].addr;
					k++;
				}
			}
			if (exit==1 && next->type=='F'){	//eliminazione semplice file
				free(next->name);
				free(next->content);
				free(next->child);
				free(next);
				t->child[x].valid='T';
				t->child[x].addr=(void*)0;
				t->nc--;
				printf("ok\n");
				return;
			}
			if (exit==1 && next->type=='C'){
				l=0;
				while(l<(next->dim) && exit==1){
					if(next->child[l].valid=='V'){
						num+=delete_all(next->child[l].addr);	//invocazione cancellazione ricorsiva
					}
					l++;
				}
				if(exit!=0){  //cancellazione cartella
					free(next->name);
					free(next->content);
					free(next->child);
					free(next);
					t->child[x].valid='T';
					t->child[x].addr=(void*)0;
					t->nc=(t->nc)-num-1;
					printf("ok\n");
					return;
				}
			}
			printf("no\n");
			return;
		}
		
		next=t->child[h].addr;
		exit=0;
		//individuo directory successiva
		if(t->child[h].valid=='V' && strcmp(next->name,tmp)==0 && next->type=='C'){
			t=t->child[h].addr;
			exit=1;
			hop++;
		}else{
			k=1;
			x=h;
			while(k<t->dim && exit==0){
				if(t->child[x].valid=='V'){
					next=t->child[x].addr;
					if(strcmp(next->name,tmp)==0 && next->type=='C'){
						exit=1;
					}
				}
				if(exit==0){
					x=quadratic(h,(t->dim),k);
				}
				k++;
			}
			if(exit==1){
				t=t->child[x].addr;
				hop++;
			}
		}
		if(exit==0){
			printf("no\n");
			return;
		}
		if(dir[i]!='\0'){
			i++;
		}
	}
	return;
}


int delete_all(node* t){	//procedura cancellazione ricorsiva
	int i;
	int num=0;
	i=0;
	if (t->type=='F'){
		free(t->name);
		free(t->content);
		free(t->child);
		free(t);
		return 1;
	}
	if (t->type=='C'){
		while(i<(t->dim)){
			if(t->child[i].valid=='V'){
				num+=delete_all(t->child[i].addr);
			}
			i++;
		}
		free(t->name);
		free(t->content);
		free(t->child);
		free(t);
		num++;
		return num;
	}
	return num;
}


void find(node* t,char* name){		//RICERCA FILE-CARTELLE PER NOME (ROOT)
	node * next;
	h_table *v;
	v=(h_table *)calloc((t->nc),sizeof(h_table));	//creazione tabella hash di lavoro
	int i,j,k,l,h=0,num=0;
	
	k=0;
	h=0;
	l=0;
	next=t->child[h].addr;
	while(k<(t->dim) && l<(t->nc)){		//riempimento tabella hash
		if(t->child[h].valid=='V'){
			v[l].name=(char*)calloc((strlen(next->name)+1),sizeof(char));
			strcpy(v[l].name,next->name);
			v[l].type=next->type;
			v[l].valid='V';
			v[l].addr=t->child[h].addr;
			l++;
		}
		k++;
		h=(h+1)%(t->dim);
		next=t->child[h].addr;
	}
	char* tname;
	char ttype;
	char tvalid;
	node* taddr;
	//ORDINAMENTO TABELLA HASH
	for(i = 0; i<(t->nc); i++) {
 		for(k = 0; k<(t->nc)-1-i; k++) {
         		if(strcmp(v[k].name,v[k+1].name)>0){
				tname=v[k].name;
				ttype=v[k].type;
				tvalid=v[k].valid;
				taddr=v[k].addr;
				v[k].name=v[k+1].name;
				v[k].type=v[k+1].type;
				v[k].valid=v[k+1].valid;
				v[k].addr=v[k+1].addr;
				v[k+1].name=tname;
				v[k+1].type=ttype;
				v[k+1].valid=tvalid;
				v[k+1].addr=taddr;
         		}
 		}
	}
	k=0;
	while(k<(t->nc)){
		if(v[k].valid=='V'){
			if (v[k].type=='F' && strcmp(v[k].name,name)==0){	//stampa file ricercato a video
				printf("ok /%s\n",v[k].name);
				num++;
			}
			if(v[k].type=='C' && strcmp(v[k].name,name)==0){  //stampa cartella ricercata a video
				printf("ok /%s\n",v[k].name);
				num++;
			}
			if (v[k].type=='C'){	//invocazione funzione ricerca ricorsiva
				num+=find_r(v[k].addr,name,"");
			}
		}
		k++;
	}
	if(num==0){
		printf("no\n");
	}
	free(v);	//deallocazione tabella di lavoro
	return;
}

int find_r(node* t,char* name,char* perc){//RICERCA FILE-CARTELLE PER NOME RICORSIVA (NO ROOT)
	node * next;
	char * perctmp;
	h_table *v;
	v=(h_table *)calloc((t->nc),sizeof(h_table));	//creazione tabella hash di lavoro
	int i,j,k,l,h=0,num=0;

	k=0;
	h=0;
	l=0;
	next=t->child[h].addr;
	while(k<(t->dim) && l<(t->nc)){		//riempimento tabella hash
		if(t->child[h].valid=='V'){
			v[l].name=(char*)calloc((strlen(next->name)+1),sizeof(char));
			strcpy(v[l].name,next->name);
			v[l].type=next->type;
			v[l].valid='V';
			v[l].addr=t->child[h].addr;
			l++;
		}
		k++;
		h=(h+1)%(t->dim);
		next=t->child[h].addr;
	}
	char* tname;
	char ttype;
	char tvalid;
	node* taddr;
	//ORDINAMENTO TABELLA HASH
	for(i = 0; i<(t->nc); i++) {
 		for(k = 0; k<(t->nc)-1-i; k++) {
         		if(strcmp(v[k].name,v[k+1].name)>0){
				tname=v[k].name;
				ttype=v[k].type;
				tvalid=v[k].valid;
				taddr=v[k].addr;
				v[k].name=v[k+1].name;
				v[k].type=v[k+1].type;
				v[k].valid=v[k+1].valid;
				v[k].addr=v[k+1].addr;
				v[k+1].name=tname;
				v[k+1].type=ttype;
				v[k+1].valid=tvalid;
				v[k+1].addr=taddr;
         		}
 		}
	}
	k=0;
	while(k<(t->nc)){
		if(v[k].valid=='V'){
			//CREAZIONE STRINGA PERCORSO EREDITARIO PER RICORSIONE
			perctmp=(char*)calloc((strlen(perc)+2+strlen(t->name)),sizeof(char));
			perctmp[0]='\0';
			strncat(perctmp,perc,strlen(perc));;
			strncat(perctmp,"/",1);
			strncat(perctmp,t->name,strlen(t->name));
			
			if (v[k].type=='F' && strcmp(v[k].name,name)==0){	//stampa file ricercato a video
				printf("ok %s/%s\n",perctmp,v[k].name);
				num++;
			}
			if(v[k].type=='C' && strcmp(v[k].name,name)==0){  //stampa cartella ricercata a video
				printf("ok %s/%s\n",perctmp,v[k].name);
				num++;
			}
			if (v[k].type=='C'){	//invocazione funzione ricerca ricorsiva
				num+=find_r(v[k].addr,name,perctmp);
			}
		}
		k++;
	}
	free(v);	//deallocazione tabella di lavoro
	if(num==0){
		return 0;
	}
	return num;

}

int main(){
	char com[COMMAND+1];
	char name[MAXNAME+1];
	char* dir=(char*)calloc(4000,sizeof(char));		//percorso directory sovrastimato
	char* text=(char*)calloc(1000,sizeof(char));	//testo per write
	dir[0]='\0';
	text[0]='\0';
	int i=0,j=0,err=0;
	node * root=(node*)calloc(1,sizeof(node));
	create_node(root,'C',"root");	
	while(1){
		err=0;
		scanf("%s",com);
		if(strcmp(com,"exit")==0){
			return 0;
		}
		if(switch_num(com)!=FIND){
			scanf("%s",dir);
			dir=verify(dir);
			if(dir[0]=='0'){
				err=1;
			}
		}else{
			scanf("%s",name);
			err=1;
		}
		if(err==0){
			switch(switch_num(com)){
				case CREATE:
					create(root,dir);
					break;
				case CREATE_DIR:
					create_dir(root,dir);
					break;
				case READ:
					read(root,dir);
					break;
				case WRITE:
					text=readline(text);
					write(root,dir,text);
					break;
				case DELETE:
					delete(root,dir);
					break;
				case DELETE_R:
					delete_r(root,dir);
					break;
				case WRONG:
					break;
			}
		}else if(switch_num(com)==FIND){
			find(root,name);
		}
	}
	return 0;	
}