/*Alvaro García García
  DNI:45138930G
*/

#include "dispersion.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//SEGURAMENTE LA DE LA SEGUNDA CONVOCATORIA
int modificarReg(char *fichero, char *dni, char *provincia){

	
	FILE *fHas;
	tipoCubo cubo;
	int i,numCubo,numCuboMod,j;
	tipoAlumno reg;

	
	fHas=fopen(fichero,"r+b");
	
	if(fHas==NULL){
	printf("No se pudo abrir el fichero HASH\n");
	return -1;
	}
	
	numCuboMod=buscaReg(fHas,&reg,dni);
	if(numCuboMod==-1){
	return -1;
	}	
	
	//Ahora apuntamos al cubo en cuestion y lo leemos
	fseek(fHas,numCuboMod*sizeof(tipoCubo),SEEK_SET);
	fread(&cubo,sizeof(tipoCubo),1,fHas);
	
	for(i=0; i<cubo.numRegAsignados; i++){
		
		if(atoi(dni)==atoi(cubo.reg[i].dni)){
		strcpy(cubo.reg[i].provincia,provincia);
		}
	}
	
	//Nos colocamos en el cubo correspondiente
	fseek(fHas,numCuboMod*sizeof(tipoCubo),SEEK_SET);
	fwrite(&cubo,sizeof(tipoCubo),1,fHas);		


fclose(fHas);
return numCuboMod;
}




//LA DE LA PRIMERA CONVOCATORIA
int buscaReg(FILE *fHas, tipoAlumno *reg, char *dni){

	
	tipoCubo cubo,cuboDesborde;
	int numCubo=0,i,numCuboDesborde;

	
	//Obtenemos en que cubo se debe encontrar a través del dni (dni es la clave)
	numCubo=((atoi(dni))%CUBOS);


	//Nos colocamos en el fichero hash sobre el cubo en cuestion
	fseek(fHas,numCubo*sizeof(tipoCubo),SEEK_SET);
	fread(&cubo,sizeof(cubo),1,fHas);

							//Con i<5 esto nos aseguramos que no revise los de desborde
	for(i=0; i<cubo.numRegAsignados && i<5; i++){
		
		if(atoi(cubo.reg[i].dni)==atoi(dni)){
		*reg=cubo.reg[i];
		return numCubo;
		}
	}
	
	numCuboDesborde=CUBOS;
	fseek(fHas,numCuboDesborde*sizeof(tipoCubo),SEEK_SET);
	fread(&cuboDesborde,sizeof(tipoCubo),1,fHas);
		
	while(numCuboDesborde<(CUBOS+CUBOSDESBORDE)){
		for(i=0; i<cuboDesborde.numRegAsignados && i<5; i++){
			
			if(atoi(dni)==atoi(cuboDesborde.reg[i].dni)){
			*reg=cuboDesborde.reg[i];
			return numCuboDesborde;
			}
		}
	numCuboDesborde++;
	fseek(fHas,numCuboDesborde*sizeof(tipoCubo),SEEK_SET);
	fread(&cuboDesborde,sizeof(tipoCubo),1,fHas);
	}
		
//No existe alumno con ese dni
return -1;
}


int leeHash(char *fichHash)
{ FILE *f;
  tipoCubo cubo;
  int j,i=0;
  size_t numLee;

   f = fopen(fichHash,"rb");
   rewind(f);
	//Almacena lo que lee en el tipoCubo
   fread(&cubo,sizeof(cubo),1,f);
   while (!feof(f)){
	//Va mostrando los cubos de uno en uno
	for (j=0;j<C;j++) {
        if (j==0)    	printf("Cubo %2d (%2d reg. ASIGNADOS)",i,cubo.numRegAsignados);
        else  	printf("\t\t\t");
	if (j < cubo.numRegAsignados) 
		    printf("\t%s %s %s %s %s\n",
	    		cubo.reg[j].dni,
			cubo.reg[j].nombre,
			cubo.reg[j].ape1,
		  	cubo.reg[j].ape2,
  	                cubo.reg[j].provincia);
	else printf ("\n");
        }
		//Cuando ya ha mostrado todo un cubo, carga el siguiente cubo a mostrar
       i++;
       fread(&cubo,sizeof(cubo),1,f);
   }
   fclose(f);
   return i;
}

// Crea un fichero hash inicialmente vacio seg�n los criterios especificados en la pr�ctica
// Primera tarea a realizar para  crear un fichero organizado mediante DISPERSI�N
void creaHvacio(char *fichHash)
{ FILE *fHash; //Puntero a file
  tipoCubo cubo;
  int j;		//24 cubos
  int numCubos =CUBOS+CUBOSDESBORDE;

  memset(&cubo,0,sizeof(cubo));


  //Fichhash nombre del fichero
  fHash = fopen(fichHash,"wb"); //Insertamos los cubos que tendra el hash en el fichero
  for (j=0;j<numCubos;j++) fwrite(&cubo,sizeof(cubo),1,fHash);
  fclose(fHash);
}





int creaHash(char *fichEntrada,char *fichHash)
{
  FILE *fEntrada,*fHash;
  tipoAlumno reg;
  tipoCubo cubo,cuboDesborde;
  int i,k=0,numCubo,numDesb=0;

					
  creaHvacio(fichHash);
  fEntrada = fopen(fichEntrada,"rb");
  if(NULL == fEntrada) 
  {
    printf("ERROR. No se puede abrir el fichero %s\n",fichEntrada);
    return -1;
  }
	
  fHash = fopen(fichHash,"r+b");
  if(NULL == fHash)
   {											
    printf("ERROR. No se puede abrir el fichero %s\n",fichHash);
    return -1;
  }


  fread(&reg,sizeof(tipoAlumno),1,fEntrada);

  while(!feof(fEntrada))
  {	
	numCubo = ((atoi(reg.dni))%CUBOS); 
	fseek(fHash,numCubo*sizeof(cubo),SEEK_SET);
	fread(&cubo,sizeof(cubo),1,fHash);
	i=cubo.numRegAsignados;
	if(i>=C)
	{	
  		numDesb++;
		cubo.numRegAsignados++;
		fseek(fHash,(CUBOS)*sizeof(tipoCubo),SEEK_SET);
		fread(&cuboDesborde,sizeof(tipoCubo),1,fHash); 
		i=cuboDesborde.numRegAsignados;
		k=0;
		if(i>=C){
			while(i>=C){
				cuboDesborde.numRegAsignados++;
				fseek(fHash,(CUBOS+k)*sizeof(cubo),SEEK_SET);
				fwrite(&cuboDesborde,sizeof(cubo),1,fHash);
		     	fread(&cuboDesborde,sizeof(cubo),1,fHash);
		     	i=cuboDesborde.numRegAsignados;
				 	
		     	k++;
			}
			cuboDesborde.reg[i]=reg;
			cuboDesborde.numRegAsignados++;
		}else{
			cuboDesborde.reg[i]=reg;
			cuboDesborde.numRegAsignados++;
		}
		
		fseek(fHash,(CUBOS+k)*sizeof(cubo),SEEK_SET);
		fwrite(&cuboDesborde,sizeof(cubo),1,fHash);
	}
	else
	{
 	  cubo.reg[i]=reg;
	  cubo.numRegAsignados++;
	}
	fseek(fHash,numCubo*sizeof(tipoCubo),SEEK_SET);
	fwrite(&cubo,sizeof(tipoCubo),1,fHash);

	fread(&reg,sizeof(reg),1,fEntrada);
  }
  fclose(fEntrada);
  fclose(fHash);
  return numDesb;
}

//A PARTIR DE AQUI, LAS FUNCIONES ESTÁN REPETIDAS PARA PRACTICAR


/*

int modificarProvincia(char *fichero, char *dni, char *provincia){

	FILE *fHash;
	tipoCubo cubo;
	tipoAlumno reg;
	int numCubo;
	

	fHash=fopen(fichero,"r+b");	
	
	numCubo=buscarReg(fHash,&reg,dni);
	if(numCubo==-1){
	return -1;
	}	

	fseek(fHash,numCubo*sizeof(tipoCubo),SEEK_SET);
	fread(&cubo,sizeof(tipoCubo,1,fHash));

	for(i=0; i<5; i++){
		if(atoi(dni)==atoi(cubo.reg[i].dni)){
			for(j=0; j<11; j++){
			cubo.reg[i].provincia[j]=provincia[j];
			}
		}
	}

	fseek(fHash,numCubo*sizeof(tipoCubo),SEEK_SET);
	fwrite(&cubo,sizeof(tipoCubo),1,fHash);

fclose(fHash);
return numCubo;
}


*/
/*
int creaHash(char *fichEntrada,char *fichHash){

	FILE *fHash, *fDat;
	tipoAlumno reg;
	int nDesb=0,k=0,i;
	int nCubo;
	tipoCubo cubo;
	
	fDat=fopen(fichEntrada,"r");
	fHash=fopen(fichHash, "r+b");
	
	
	fread(&reg,sizeof(tipoAlumno),1,fDat);
	
	while(!feof(fDat)){
	k=0;
	nCubo=((atoi(reg.dni)%CUBOS));
	printf("1\n");
	fseek(fHash,nCubo*sizeof(tipoCubo),SEEK_SET);
	fread(&cubo,sizeof(tipoCubo),1,fHash);
	i=cubo.numRegAsignados;
	printf("2\n");
	if(i>=5){
	cubo.numRegAsignados++;
	fseek(fHash,nCubo*sizeof(tipoCubo),SEEK_SET);
	fwrite(&cubo,sizeof(tipoCubo),1,fHash);
		while(i>=5){
		nDesb++;
		nCubo=CUBOS+k;
		fseek(fHash,(CUBOS+k)*sizeof(tipoCubo),SEEK_SET);
		fread(&cubo,sizeof(tipoCubo),1,fHash);
		i=cubo.numRegAsignados;
		k++;
		}
	cubo.numRegAsignados++;
	i=cubo.numRegAsignados;
	cubo.reg[i]=reg;
	fseek(fHash,nCubo*sizeof(tipoCubo),SEEK_SET);
	fwrite(&cubo,sizeof(tipoCubo),1,fHash);
	}
	ubo.reg[i]=reg;
	cubo.numRegAsignados++;
	printf("3\n");
	cubo.reg[i]=reg;
	cubo.numRegAsignados++;
	printf("4\n");
	fseek(fHash,nCubo*sizeof(tipoCubo),SEEK_SET);
	printf("4\n");
	fwrite(&cubo,sizeof(tipoCubo),1,fHash);
	printf("4\n");
	}

printf("4\n");

fclose(fHash);
fclose(fDat);
return nDesb;
}









*/


































