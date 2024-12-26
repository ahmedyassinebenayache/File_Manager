#include <stdio.h>
#include <stdlib.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */
void compact(FILE *ms){
	BLOC_ch buffer;
	BLOC_ch temp;
	BLOC_co meta;
	int T[MAX];
	fread(T,MAX * sizeof(int),1,ms);
	for(int i = 0; i<10; i++){
    fread(meta, sizeof(BLOC_co),1,ms);
    for(int i = 0; i<MAX; i++){
    fread(temp, sizeof(BLOC_ch),1,ms);	
    	if(T[i]==0){
    	  	
		}
	}
	writeBloc;
}
