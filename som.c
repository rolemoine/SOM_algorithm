#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>


// definition des paramètres d entree

#define _GNU_SOURCE
#define TAILLE_MAX 1024
#define nb_l_map 10
#define nb_c_map 6
#define nb_epochs 2000
#define alpha_0 0.7



typedef struct data
// structure du set de data iris
{
  double slength;
  double swidth;
  double plength;
  double pwidth;
  double variety;

} data;



void printStructData(data * v, int size) {
  for (int i = 1; i < size; i++) {
    printf("%f | %f | %f | %f | %f\n", v[i].slength, v[i].swidth, v[i].plength, v[i].pwidth, v[i].variety);
  }
}


int countLineInFile(char * filename) {
  // fonction permettant de compter le nombre de donnees dans le dataset
  FILE * f = NULL;
  f = fopen(filename, "r");
  int line = 0;
  char c;
  while ((c = fgetc(f)) != EOF) {
    if (c == '\n') {
      line++;
    }
  }
  fseek(f, 0, SEEK_SET);
  return line;
}

double fleur (char * nomfleur) {
  // fontion permettant d associer une valeur aux labels
  double n ;
  if (strcmp(nomfleur,"Iris-setosa")==10) n = 0 ;
  else if (strcmp(nomfleur,"Iris-versicolor")==10) n = 1 ;
  else if (strcmp(nomfleur,"Iris-virginica")==10) n = 2 ;
  return(n);
}


struct data* parseFile(char * filename) {
  // chargement des data
  FILE * f = NULL;
  char buffer[TAILLE_MAX], *copy,*token;
  int lineInFile, index_struct, index_line = 0;
  f = fopen(filename, "r");
  lineInFile = countLineInFile(filename);
  data* v = malloc(lineInFile * sizeof *v);
  if (f) {
    while (fgets(buffer, TAILLE_MAX, f) != NULL) {
      copy = buffer;
      index_struct = 0;

      while ((token = strtok_r(copy, ",", &copy)) && index_line){
        if (index_struct==0) v[index_line].slength = atof(token);
        else if (index_struct==1) v[index_line].swidth = atof(token);
        else if (index_struct==2) v[index_line].pwidth = atof(token);
        else if (index_struct==3) v[index_line].plength = atof(token);
        else if (index_struct==4) v[index_line].variety = fleur(token);
        index_struct++;
      }
      index_line++;

    }

    fclose(f);
  } else {
    printf("Not open ! ");
  }
  return (v);
}



struct data* normalise(data * vecteur, int nb_l){
  // fonction permettant de normaliser le dataset en obtenant pour chaque colonne des
  // valeurs comprises entre 0 et 1
  double max1 = 0;
  double max2 =0 ;
  double max3 = 0 ;
  double max4 = 0;
  double min1 =10;
  double min2 =10;
  double min3 =10;
  double min4 =10;
  for (int i=1;i<nb_l;i++){
    if (max1<vecteur[i].slength) max1=vecteur[i].slength ;
    if (max2<vecteur[i].swidth) max2=vecteur[i].swidth ;
    if (max3<vecteur[i].plength) max3=vecteur[i].plength ;
    if (max4<vecteur[i].pwidth) max4=vecteur[i].pwidth ;
    if (min1>vecteur[i].slength) min1=vecteur[i].slength ;
    if (min2>vecteur[i].swidth) min2=vecteur[i].swidth ;
    if (min3>vecteur[i].plength) min3=vecteur[i].plength ;
    if (min4>vecteur[i].pwidth) min4=vecteur[i].pwidth ;
  }
   for (int i=1;i<nb_l;i++){
     vecteur[i].slength = (vecteur[i].slength-min1)/(max1-min1) ;
     vecteur[i].swidth = (vecteur[i].swidth-min2)/(max2-min2) ;
     vecteur[i].plength = (vecteur[i].plength-min3)/(max3-min3) ;
     vecteur[i].pwidth = (vecteur[i].pwidth-min4)/(max4-min4) ;
   }

  return vecteur;
}



void swap(int *a, int *b) {
  // fonction permettant d intervertir deux valeurs dans une liste
    int temp = *a;
    *a = *b;
    *b = temp;
}

void randomize(int arr[], int n) {
  // fonction permettant de melanger les valeurs d une liste
    srand(time(NULL));
    int i;
    for(i = n-1; i > 0; i--) {
        int j = rand() % (i+1);
        swap(&arr[i], &arr[j]);
    }
}


double moyenne (data * vecteur, int nb_l,int num_colonne) {
  // fonction calculant la moyenne d une colonne
  double moy =  0;
  for (int i =1; i<nb_l+1; i++) {
    if (num_colonne == 0) moy = moy + vecteur[i].slength ;
    if (num_colonne == 1) moy = moy + vecteur[i].swidth ;
    if (num_colonne == 2) moy = moy + vecteur[i].plength ;
    if (num_colonne == 3) moy = moy + vecteur[i].pwidth ;
  }
  moy = moy / (nb_l-1);
  return (moy) ;
}


double *** fonction_allocation(int taille1, int taille2, int taille3){
  // fonction pour allouer de la memoire a un tableau et ainsi pouvoir creer un tableau de
  // doubles de dimension taille1 * taille2 * taille3 soit de dimension 3
  double ***ptr;
  int i,j;

  ptr = malloc(taille1 * sizeof(*ptr));
  if(ptr == NULL)
  return ptr;
  for(i=0 ; i < taille1 ; i++){
    ptr[i] = malloc(taille2 * sizeof(**ptr));
    if( ptr[i] == NULL)
    return ptr;
  }
  for(i=0 ; i < taille1 ; i++){
    for(j=0 ; j < taille2 ; j++){
      ptr[i][j] = malloc(taille3 * sizeof(***ptr));
      if(ptr[i][j] == NULL)
      return ptr;
    }
  }
  return ptr;
}

double *** map(data * vecteur ,int nb_l){
  // fonction qui va creer la map avec des valeur aleatoire comprises dans un intervalle
  // [moyenne-0.002, moyenne+0.005] la map sera dimension 3 et de taille definie dans les
  //parametres en debut de code
  double m1 = moyenne(vecteur, nb_l,0);
  double m2 = moyenne(vecteur, nb_l,1);
  double m3 = moyenne(vecteur, nb_l,2);
  double m4 = moyenne(vecteur, nb_l,3);
  double m[4] = {m1,m2,m3,m4} ;
  double ***matrice ;
  matrice = fonction_allocation(nb_l_map,nb_c_map,4);

  for(int i =0; i<nb_l_map; i++){
    for(int j =0; j<nb_c_map; j++){
      for(int k = 0; k<4;k++){
        double f= rand()/(RAND_MAX+1.0) ;
        matrice[i][j][k] = (m[k]-0.002) + (0.005+0.002)*f;
      }
    }
  }
  return matrice ;
}


double distance_euclidienne(double *** map , data * vecteur , int i, int j, int ind){
  // fonction permettant de calculer la distance euclidienne soit le neurone de position i,j et l'indice dans la liste du dataset normalisé ind
  double d_e = 0;
  d_e = pow((vecteur[ind].slength-map[i][j][0]),2) +  pow((vecteur[ind].swidth-map[i][j][1]),2) +  pow((vecteur[ind].plength-map[i][j][2]),2) + pow((vecteur[ind].pwidth-map[i][j][3]),2) ;
  d_e = pow(d_e,0.5) ;
  return(d_e) ;
}

typedef struct BMU_struct
{
  // structure du Best Match Unit qui se composera de 3 valeurs
  int idx_i;
  int idx_j;
  double BMU_value;
} BMU_struct;


struct BMU_struct* find_BMU(double *** map , data * data_shuffle, int nb_l, int ind){
  // fonction permettant de calculer le Best Match Unit
  // Ce dernier sera stocke dans une liste et la valeur des indices i,j contenant sa position
  // sera tire de facon aleatoire si il existe plusieurs BMU avec la meme valeur
  BMU_struct * bmu = malloc(60 * sizeof *bmu) ;
  bmu[0].idx_i = 0 ;
  bmu[0].idx_j = 0 ;
  bmu[0].BMU_value = distance_euclidienne(map , data_shuffle ,0,0, ind) ;
  int compteur = 0;
  for (int i=0;i<nb_l_map;i++){
    for(int j = 0 ;j<nb_c_map;j++){
      if (bmu[0].BMU_value > distance_euclidienne(map , data_shuffle ,i,j, ind) ){
        bmu[0].idx_i = i ;
        bmu[0].idx_j = j ;
        bmu[0].BMU_value = distance_euclidienne(map , data_shuffle ,i,j, ind) ;
        compteur = 0;

      }
      if (bmu[0].BMU_value == distance_euclidienne(map , data_shuffle ,i,j, ind) ){
        compteur = compteur + 1 ;
        bmu[compteur].idx_i = i ;
        bmu[compteur].idx_j = j ;
        bmu[compteur].BMU_value = distance_euclidienne(map , data_shuffle ,i,j, ind) ;

      }
    }
  }
  int alea = rand()% (compteur+1) ; // choisir un nombre aleatoire selon le nombre de BMU de meme valeur
  if (bmu[0].idx_i==0 && bmu[0].idx_j==0 ){alea = compteur;}
  BMU_struct * bmu_best = malloc(60 * sizeof *bmu_best);
  bmu_best[0].idx_i =  bmu[alea].idx_i ;
  bmu_best[0].idx_j =  bmu[alea].idx_j ;
  bmu_best[0].BMU_value = bmu[alea].BMU_value ;
  return(bmu_best);
}


struct BMU_struct* find_BMU2(double *** map , data * data_shuffle, int nb_l, int ind){
  // fonction permettant de retourner tous les BMU
  BMU_struct * bmu = malloc(60 * sizeof *bmu) ;
  bmu[0].idx_i = 0 ;
  bmu[0].idx_j = 0 ;
  bmu[0].BMU_value = distance_euclidienne(map , data_shuffle ,0,0, ind) ;
  int compteur = 1 ;
  for (int i=0;i<nb_l_map;i++){
    for(int j = 0 ;j<nb_c_map;j++){
      // printf("%f\n", bmu[0].BMU_value);
      if (bmu[0].BMU_value > distance_euclidienne(map , data_shuffle ,i,j, ind) ){
        bmu[0].idx_i = i ;
        bmu[0].idx_j = j ;
        bmu[0].BMU_value = distance_euclidienne(map , data_shuffle ,i,j, ind) ;
        compteur = 1 ;
      }
      if (bmu[0].BMU_value == distance_euclidienne(map , data_shuffle ,i,j, ind) ){
        compteur = compteur + 1;
        bmu[compteur].idx_i = i ;
        bmu[compteur].idx_j = j ;
        bmu[compteur].BMU_value = distance_euclidienne(map , data_shuffle ,i,j, ind) ;
      }
    }
  }
  // printf("%d \n", compteur );
  return(bmu);
}



double *** new_map(double *** map, data * vecteur, BMU_struct * bmu , int nb_l , int ind,  int nb_iteration, double rayon){
  // mise a jour des poids pour chaque noeud pour les noeuds situes dans un rayon de vosinnqge rayon
 int i_bmu = bmu[0].idx_i ;
 int j_bmu = bmu[0].idx_j ;
 double alpha = alpha_0 * (1-nb_iteration/nb_epochs);
 for (int i = 0 ; i < nb_l_map ; i++){
   for (int j = 0 ; j < nb_c_map ; j++){
     double dist_neuronne = pow((pow((i_bmu-i),2)+pow((j_bmu-j),2)),0.5) ;
     if (dist_neuronne<=rayon){
         map[i][j][0] = map[i][j][0] + alpha  * (vecteur[ind].slength-map[i][j][0]);
         map[i][j][1] = map[i][j][1] + alpha  * (vecteur[ind].swidth-map[i][j][1]);
         map[i][j][2] = map[i][j][2] + alpha  * (vecteur[ind].plength-map[i][j][2]);
         map[i][j][3] = map[i][j][3] + alpha  * (vecteur[ind].pwidth-map[i][j][3]);
       }
     }
 }
 return map ;
}


double *** final_map(double *** map, data * vecteur, int nb_l ,int t,int index[], double rayon){
  // generation de la map pour l epoch t en considerant un rayon de voisinage rayon
  // les indices seront melanges et les poids de la map seront mise a jour pour chaque indice
  //pris aleatoirement selon la position du BMU trouvee
  BMU_struct* bmu_it ;
  randomize(index,nb_l) ;
    for (int ind=1; ind<nb_l ; ind ++){
      bmu_it =  find_BMU(map , vecteur , nb_l, index[ind]);
      map = new_map( map, vecteur, bmu_it ,nb_l ,index[ind], t, rayon);
      free(bmu_it);
    }
  return(map) ;
}

double *** create_stat_map(double *** map, data * vecteur ,int nb_l){
  // A la fin de l apprentissage une carte comprenant le nombre de fois qu'un
  // neurone a ete active en repassant une derniere fois sur le dataset
 // va etre generer par cette fonction
  double *** m = fonction_allocation(nb_l_map, nb_c_map, 3) ;
  BMU_struct* bmu_fin ;

  for (int i = 1 ; i<nb_l ; i++){
    bmu_fin = find_BMU2(map , vecteur, nb_l, i);
    int i_fin = bmu_fin[0].idx_i ;
    int j_fin = bmu_fin[0].idx_j ;
    int i_fin1 = bmu_fin[1].idx_i ;
    int j_fin1 = bmu_fin[1].idx_j ;
    if (vecteur[i].variety == 0.0) m[i_fin][j_fin][0] = m[i_fin][j_fin][0] + 1 ;
    if (vecteur[i].variety == 1.0) m[i_fin][j_fin][1] = m[i_fin][j_fin][1] + 1 ;
    if (vecteur[i].variety == 2.0) m[i_fin][j_fin][2] = m[i_fin][j_fin][2] + 1 ;
    if (vecteur[i].variety == 0.0) m[i_fin1][j_fin1][0] = m[i_fin][j_fin1][0] + 1 ;
    if (vecteur[i].variety == 1.0) m[i_fin1][j_fin1][1] = m[i_fin][j_fin1][1] + 1 ;
    if (vecteur[i].variety == 2.0) m[i_fin1][j_fin1][2] = m[i_fin][j_fin1][2] + 1 ;
  }
  return(m);
}


void repartition_map (double *** map) {
  // cette fonction va permettre d afficher la map finale pour voir la repartition
  // des clusters qui decoulera de la carte_stat_map ou le neurone prendra la
  // valeur de la variete qui a ete le plus activee
  for(int i = 0 ; i<nb_l_map ; i++){
    for (int j = 0 ; j<nb_c_map ; j++){
      if (i!=0 && j!=0){
      if ((map[i][j][0] > map[i][j][1]) && (map[i][j][0] > map[i][j][2])) printf(" S " ) ;
      if ((map[i][j][1] > map[i][j][0]) && (map[i][j][1] > map[i][j][2])) printf(" C " ) ;
      if ((map[i][j][2] > map[i][j][1]) && (map[i][j][2] > map[i][j][0])) printf(" V " )  ;
      if ((map[i][j][0] == map[i][j][1] && map[i][j][0]!=0) || (map[i][j][0] == map[i][j][2] && map[i][j][0]!=0)  || (map[i][j][1] == map[i][j][2] && map[i][j][1]!=0) ) printf(" X " )  ;
      if (map[i][j][2]==0 && map[i][j][1] ==0 && map[i][j][0]==0) printf(" O " )  ;
    }}
    printf("\n");
  }
}



int main() {

  int nb_l = (countLineInFile("iris_csv.csv")); //nombre de data
  struct data* v2 = parseFile("iris_csv.csv");  // generation des data
  v2 = normalise(v2,nb_l) ; // normalisation des data

 printStructData(v2,nb_l);

  double *** my_map = map(v2,nb_l) ; // generation de la map


  int index[nb_l]; // generation d une liste contenant tous les indices es data pour pouvoir par la suite la melanger
  for (int i = 1 ; i<nb_l;i++ ){
    index[i] = i ;
  }

 // initialisation de la phase 1 pour un quart du nombre total d nb_epochs
 // le rayon de voisinage est egal a 1/4 du perimetre
  double rayon_phase1 = ((nb_l_map + nb_c_map)/4) ;

  for (int epoch = 1 ; epoch<(nb_epochs/4) ; epoch++){
 my_map = final_map(my_map,v2, nb_l ,epoch,index,rayon_phase1); // mise a jour de la map pour chaque epoque
}

//initialisation de la phase 2 pour 3/4 du nombre total d epochs
//le rayon de voisinnage est reduit dans cette phase
  double rayon_phase2 = 1 ;
  for (int epoch = (nb_epochs/4) ; epoch<nb_epochs ; epoch++){
    my_map = final_map(my_map,v2, nb_l ,epoch,index,rayon_phase2);
  }


// phase finale ou on repertorie les neurones actives pour chaque indice
// du dataset en regardant la localisation de son BMU
 double *** map_s = create_stat_map(my_map,v2,nb_l);

// création de la carte de repartition ou il sera observe quels neurones
// ont ete active par quel vartete de fleur apres apprentissage
// des clusters pourront etre observes
printf("\nS : setosa \n");
printf("C : versicolor \n");
printf("V : virginica \n");
printf("O : neurone non active \n");
printf("X : conflit deux varietes ont actives ce neurone le meme nombre de fois \n");
repartition_map (map_s);


}
