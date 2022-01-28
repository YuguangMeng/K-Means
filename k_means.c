/*
 --------------------------------------------------------------------------
|  This k-means code for CS570 is my own work written without consulting   |
|  a tutor  or code written by other students - Yuguang Meng, Nov 1, 2015  |
 --------------------------------------------------------------------------
*/

#include "k_means.h"

int main(int argc, char ** argv)
{
  Node_data *dat=NULL;
  MinMax_data *MinMax=NULL;
  Centroid_data *Centroid=NULL;
  int attr_type[MAX_ATTR];
  FILE *fp=NULL;
  int k, class_attr, n_attr, n_tuple;

  if(argc!=4)
  {
    printf("Error: Usage: %s data_file k output_file!\n", argv[0]);
    exit(0);
  }
  if(atoi(argv[2])<1)
  {
    printf("Error: k must be more than 1!\n", argv[0]);
    exit(0);
  }

  printf("\n");
  printf(" ------------------------------------------------------------------------\n");
  printf("|  This k-means code for CS570 is my own work written without consulting |\n");
  printf("|  a tutor or code written by other students - Yuguang Meng, Nov 1, 2015 |\n");
  printf(" ------------------------------------------------------------------------\n");
  printf("\n");

  dat=(Node_data *)malloc(sizeof(Node_data)*MAX_TUPLE);
  n_tuple=Read_Data(dat, attr_type, argv[1], &n_attr, class_attr);
  k=atoi(argv[2]);
  Centroid=(Centroid_data *)calloc(k+1, sizeof(Centroid_data));

  //Print_data(dat, attr_type, n_tuple, n_attr);

  printf("This software can handle data with numeric and/or nominal attributes.\n");
  printf("The data file is %s.\n", argv[1]);
  printf("The cluster number is %s.\n", argv[2]);
  printf("Please enter the order of the class attribute (1-%d) or other numbers if no class: ", n_attr);
  scanf ("%d", &class_attr);
  class_attr--;

  free(dat);
  dat=(Node_data *)calloc(MAX_TUPLE, sizeof(Node_data));
  n_tuple=Read_Data(dat, attr_type, argv[1], &n_attr, class_attr);

  if(!(fp=fopen(argv[3],"w")))
  {
    printf("Cannot create output file %s!\n", argv[3]);
    exit(0);
  }

  Calc_MinMax(dat, &MinMax, attr_type, n_attr, n_tuple, class_attr);
  k_means(dat, MinMax, &Centroid, attr_type, n_attr, n_tuple, class_attr, k);
  Print_Centroids(fp, Centroid, attr_type, n_attr, class_attr, k);
  Calc_Class_Instance(fp, dat, n_tuple, k);
  Calculate_BCuded(fp, dat, Centroid, n_tuple, class_attr);
  Print_Class(fp, dat, Centroid, n_tuple, class_attr, n_attr);

  fclose(fp);

  printf("Results saved in %s.\n", argv[3]);

  return 0;
}

int Read_Data(Node_data *dat, int *attr_type, char *filename, int *n_attr, int class_attr)
{
  FILE *fp;
  char buf[256];
  int n_tuple=0;

  if((fp=fopen(filename,"r"))==NULL)
  {
    printf("File %s not exist!\n", filename);
    exit(0);
  }

  while(fgets (buf, 256, fp)!=NULL)
  {
    *n_attr=InsertData(dat, buf, attr_type, n_tuple, class_attr);
    n_tuple++;
  }

  fclose(fp);
  return n_tuple;
}

int InsertData(Node_data *dat, char *s, int *attr_type, int n_tuple, int class_attr)
{
  int i=0;
  char *p, *str, buf[256];
  const char *d = " \t\n";

  strcpy(buf,s);
  p = strtok(buf,d);
  while(p)
  {
    str=(char *)calloc(strlen(p)+1, sizeof(char));
    RemoveSpace(str, p);
    if(strlen(str)!=0)
    {
      if(!ExistLetter(str) && i!=class_attr)
      {
        dat[n_tuple].value.num[i]=atof(str);
        attr_type[i]=NUMERIC;
      }
      else
      {
        strcpy(dat[n_tuple].value.nom[i], str);
        attr_type[i]=NOMINAL;
        //printf("%s ", str);
      }
      i++;
    }
    free(str);
    p=strtok(NULL,d);
  }

  return i;
}

void RemoveSpace(char *s, char *p)
{
  char *p1=p;
  while(*p1!='\0')
  {
    if(IsLetter(*p1) || IsDigit(*p1) || *p1=='.' || *p1=='-')
    {
      *s=*p1;
      s++;
    }
    p1++;
  }
  *s='\0';
}

int IsLetter(char c) //Letter
{
  if(((c<='z')&&(c>='a'))||((c<='Z')&&(c>='A')))
    return 1;
  else
    return 0;
}

int IsDigit(char c) //Digit
{
  if(c>='0'&& c<='9')
    return 1;
  else
    return 0;
}

int ExistLetter(char *s)
{
  char *p=s;
  while(*p!='\0')
  {
    if(IsLetter(*p))
      return 1;
    p++;
  }
  return 0;
}

int allocate_class(Node_data *dat, MinMax_data *MinMax, Centroid_data *C, int *attr_type, int n_attr, int n_tuple, int class_attr, int k)
{
  int i, class_type;
  double min, t_min;

  min=calc_distance(&dat[0], &C[0], MinMax, attr_type, n_attr, class_attr);
  class_type=0;

  for(i=1; i<k; i++)
  {
    t_min=calc_distance(&dat[0], &C[i], MinMax, attr_type, n_attr, class_attr);
    if(t_min<min)
      class_type=i;
  }

  return class_type;
}

void k_means(Node_data *dat, MinMax_data *MinMax, Centroid_data **p_Centroid, int *attr_type, int n_attr, int n_tuple, int class_attr, int k)
{
  int i, j, n, iter, is_same;
  int *class_type=(int *)calloc(n_tuple+1, sizeof(int));
  Centroid_data *Centroid=*p_Centroid;

  for(i=0; i<n_tuple; i++)
    class_type[i]=0;

  while(Calc_class_num(class_type, n_tuple, k)<k)
  {
    iter=0;
    for(i=0; i<n_tuple; i++)
      class_type[i]=0;

    srand(time(NULL));
    for(i=0; i<k; i++)
    {
      j=rand() % (n_tuple-i) + i;
      for(n=0; n<n_attr; n++)
        if(attr_type[i]==NUMERIC)
          Centroid[i].value.num[n]=dat[j].value.num[n];
        else
          strcpy(Centroid[i].value.nom[n], dat[j].value.nom[n]);
    }

    do
    {
      for(i=0; i<n_tuple; i++)
        class_type[i]=allocate_class(&dat[i], MinMax, Centroid, attr_type, n_attr, n_tuple, class_attr, k);
      is_same=Is_Same_Class(dat, class_type, n_tuple);
      /*
      for(i=0; i<n_tuple; i++)
        printf("%d ", dat[i].class_type);
      printf("\n\n");
      
      for(i=0; i<k; i++)
        printf("%f ", Centroid[i].value.num[1]);
      printf("\n\n");*/
      
      Calc_Centroid(dat, &Centroid, class_type, attr_type, n_tuple, n_attr, class_attr, k);
      iter++;
    } while(!is_same && iter<MAX_ITERATION);
  }
  free(class_type);
}

int Calc_class_num(int *class_type, int n_tuple, int k)
{
  int i, j, n, is;
  int *K=(int *)calloc(k, sizeof(int));
  for(i=0; i<k; i++)
    K[i]=-1;
  for(i=0; i<n_tuple; i++)
    if(i==0)
    {
      n=0;
      K[0]=class_type[0];
    }
    else
    {
      is=0;
      for(j=0; j<=n; j++)
        if(K[j]==class_type[i])
        {
          is=1;
          break;
        }
      if(!is)
      {
        n++;
        K[n]=class_type[i];
      }
    }
  return n+1;
}

double calc_distance(Node_data *dat, Centroid_data *Centroid, MinMax_data *MinMax, int *attr_type, int n_attr, int class_attr)
{
  int i;
  double d=0;

  for(i=0; i<n_attr; i++)
    if(i!=class_attr)
      if(attr_type[i]==NUMERIC)
          d+=pow(fabs((dat->value.num[i]-Centroid->value.num[i])/(MinMax[i].Max-MinMax[i].Min)), 2.0);
        //d+=pow(fabs(dat->value.num[i]-Centroid->value.num[i]), 2.0);
      else
        d+=pow(!(!(strcmp(dat->value.nom[i], Centroid->value.nom[i]))), 2.0);

  return sqrt(d);
}

int Is_Same_Class(Node_data *dat, int *class_type, int n_tuple)
{
  int i;
  double unmatched=0;
  for(i=0; i<n_tuple; i++)
  {
    if(dat[i].class_type!=class_type[i])
      unmatched=unmatched+1.0/(double)n_tuple;
    dat[i].class_type=class_type[i];
  }

  if(unmatched<ERROR_THRESHOLD)
    return 1;
  else
    return 0;
}

void Print_Class(FILE *fp, Node_data *dat, Centroid_data *Centroid, int n_tuple, int class_attr, int n_attr)
{
  int i;

  fprintf(fp, "The calculated class in data file is:\n");
  for(i=0; i<n_tuple; i++)
    if(class_attr>=0 && class_attr<=n_attr-1)
      fprintf(fp, "%s ", Centroid[dat[i].class_type].value.nom[class_attr]);
    else
      fprintf(fp, "%d ", dat[i].class_type);

  if(class_attr>=0 && class_attr<=n_attr-1)
  {
    fprintf(fp, "\n\nThe true class in data file is:\n");
    for(i=0; i<n_tuple; i++)
      fprintf(fp, "%s ", dat[i].value.nom[class_attr]);
  }

}

void Calc_Centroid(Node_data *dat, Centroid_data **p_Centroid, int *class_type, int *attr_type, int n_tuple, int n_attr, int class_attr, int k)
{
  int i,j;
  char *s=NULL;
  Centroid_data *Centroid=*p_Centroid;

  double *count=(double *)calloc(k+1, sizeof(double));
  for(i=0; i<k; i++)
    count[i]=0.0;
    
  for(i=0; i<n_tuple; i++)
      count[class_type[i]]++;

  for(i=0; i<k; i++)
    for(j=0; j<n_attr; j++)
      if(j!=class_attr && attr_type[j]==NUMERIC)
        Centroid[i].value.num[j]=0.0;

  for(i=0; i<n_tuple; i++)
    for(j=0; j<n_attr; j++)
      if(j!=class_attr && attr_type[j]==NUMERIC)
        Centroid[class_type[i]].value.num[j]+=dat[i].value.num[j];

  for(i=0; i<k; i++)
    for(j=0; j<n_attr; j++)
      if(j!=class_attr && attr_type[j]==NUMERIC)
        Centroid[i].value.num[j]/=count[i];

  free(count);

  for(i=0; i<k; i++)
    for(j=0; j<n_attr; j++)
      if(attr_type[j]==NOMINAL)
      {
        s=(char *)malloc(sizeof(char)*32);
        GetMode(dat, class_type, n_tuple, j, i, s);
        strcpy(Centroid[i].value.nom[j], s);
        free(s);
      }
}

void Calc_MinMax(Node_data *dat, MinMax_data **MinMax, int *attr_type, int n_attr, int n_tuple, int class_attr)
{
  int i, j;

  *MinMax=(MinMax_data *)calloc(n_attr+1, sizeof(MinMax_data));
  for(i=0; i<n_attr; i++)
    if(i!=class_attr && attr_type[i]==NUMERIC)
      (*MinMax)[i].Min=(*MinMax)[i].Max=dat[0].value.num[i];

  for(i=0; i<n_attr; i++)
    if(i!=class_attr && attr_type[i]==NUMERIC)
      for(j=0; j<n_tuple; j++)
      {
        if(dat[j].value.num[i]<(*MinMax)[i].Min)
          (*MinMax)[i].Min=dat[j].value.num[i];
        if(dat[j].value.num[i]>(*MinMax)[i].Max)
          (*MinMax)[i].Max=dat[j].value.num[i];
      }
}

void GetMode(Node_data *dat, int *class_type, int n_tuple, int n, int class_i, char *s)
{
  int *ipRepetition = new int[n_tuple];
  int *Tuple = new int[n_tuple];
  int i=0, ii, iMaxRepeat = 0;

  for (ii = 0; ii < n_tuple; ++ii)
    if(dat[ii].class_type==class_i)
    {
      ipRepetition[i] = 0;
      Tuple[i]=ii;
      int j = 0;
      while ((j < i) && strcmp(dat[ii].value.nom[n], dat[j].value.nom[n])!=0)  //(daArray[i] != daArray[j])
      {
        if (strcmp(dat[ii].value.nom[n], dat[j].value.nom[n])!=0)
          ++j;
      }
      ++(ipRepetition[j]);
      i++;
    }

  for (i = 1; i < n_tuple; ++i)
    if (ipRepetition[i] > ipRepetition[iMaxRepeat])
      iMaxRepeat = i;

  delete [] ipRepetition;
  delete [] Tuple;
  strcpy(s, dat[Tuple[iMaxRepeat]].value.nom[n]);
}

void Calculate_BCuded(FILE *fp, Node_data *dat, Centroid_data *Centroid, int n_tuple, int class_attr)
{
  int i,j;
  double s, s1, s2;
  
  s=0;
  for(i=0; i<n_tuple; i++)
  {
  	  s1=s2=0;
      for(j=0; j<n_tuple; j++)
         if(i!=j && dat[i].class_type==dat[j].class_type)
         	{
         	  s2++;
         	  if(strcmp(dat[i].value.nom[class_attr], dat[j].value.nom[class_attr])==0)
         	  	 s1++;
         	}
     s+=s1/s2;    	  	 
  }
  s/=(double)n_tuple;
  fprintf(fp, "The recall BCuded is %.1f\n", s);
  
  s=0;
  for(i=0; i<n_tuple; i++)
  {
  	  s1=s2=0;
      for(j=0; j<n_tuple; j++)
         if(i!=j && strcmp(dat[i].value.nom[class_attr], dat[j].value.nom[class_attr])==0)
         	{
         	  s2++;
         	  if(dat[i].class_type==dat[j].class_type)
         	  	 s1++;
         	}
     s+=s1/s2;    	  	 
  }
  s/=(double)n_tuple;
  fprintf(fp, "The Precision BCuded is %.1f\n\n", s);
}

void Print_Centroids(FILE *fp, Centroid_data *Centroid, int *attr_type, int n_attr, int class_attr, int k)
{
  int i,j;
  fprintf(fp, "Cluster Centroids:\n");
  fprintf(fp, "Attribute\t");
  for(i=0; i<k; i++)
    fprintf(fp, "\tCluster %d\t", i);
  fprintf(fp, "\n");
  for(i=0; i<n_attr; i++)
  {
    fprintf(fp, "%d\t", i+1);
    if(attr_type[i]==NUMERIC)
      for(j=0; j<k; j++)
        fprintf(fp, "\t%-24f", Centroid[j].value.num[i]);
    else
      for(j=0; j<k; j++)
        fprintf(fp, "\t%-24s", Centroid[j].value.nom[i]);
    fprintf(fp, "\n");
  }
  fprintf(fp, "\n");
}

void Calc_Class_Instance(FILE *fp, Node_data *dat, int n_tuple, int k)
{
  int i,sum=0;
  int *Class_num = new int[k];

  for(i=0; i<k; i++)
    Class_num[i]=0;

  for(i=0; i<n_tuple; i++)
    Class_num[dat[i].class_type]++;
  for(i=0; i<k; i++)
    sum+=Class_num[i];

  fprintf(fp, "Clustered Instances\n");
  for(i=0; i<k; i++)
    fprintf(fp, "%d\t%d (%.0f%%)\n", i, Class_num[i], 100.0*(double)Class_num[i]/(double)sum);
  fprintf(fp, "\n");

  delete [] Class_num;
}

void Print_data(Node_data *dat, int *attr_type, int n_tuple, int n_attr)
{
  for(int i=0; i<n_tuple; i++)
  {
    for(int j=0; j<n_attr; j++)
      if(attr_type[j]==NUMERIC)
        printf("%f  ", dat[i].value.num[j]);
      else
        printf("%s  ", dat[i].value.nom[j]);
    exit(0);    
    printf("\n");
  }
  printf("\n");
}
