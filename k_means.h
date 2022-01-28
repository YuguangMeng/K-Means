/*
 --------------------------------------------------------------------------
|  This k-means code for CS570 is my own work written without consulting   |
|  a tutor  or code written by other students - Yuguang Meng, Nov 1, 2015  |
 --------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <math.h>

#define MAX_ATTR 128
#define MAX_TUPLE 8092
#define MAX_ITERATION 500//2000
#define ERROR_THRESHOLD 0.0001

using namespace std;

enum data_type
{
  NUMERIC,
  NOMINAL
};

typedef struct Node_data
{
  union
  {
    double num[MAX_ATTR];
    char nom[MAX_ATTR][32];
  } value;
  int class_type;
} Node_data;

typedef struct Centroid_data
{
  union
  {
    double num[MAX_ATTR];
    char nom[MAX_ATTR][32];
  } value;
}	Centroid_data;

typedef struct MinMax_data
{
  double Min;
  double Max;
}	MinMax_data;

int Read_Data(Node_data *, int *, char *, int *, int);
int InsertData(Node_data *, char *, int *, int, int);
void RemoveSpace(char *, char *);
int IsLetter(char);
int IsDigit(char);
int ExistLetter(char *);
int allocate_class(Node_data *, MinMax_data *, Centroid_data *, int *, int, int, int, int);
void Calc_MinMax(Node_data *, MinMax_data **, int *, int, int, int);
void k_means(Node_data *, MinMax_data *, Centroid_data **, int *, int, int, int, int);
int Calc_class_num(int *, int, int);
double calc_distance(Node_data *, Centroid_data *, MinMax_data *, int *, int, int);
void Calc_Centroid(Node_data *, Centroid_data **, int *, int *, int, int, int, int);
void GetMode(Node_data *, int *, int, int, int, char *);
int Is_Same_Class(Node_data *, int *, int);
void Print_Centroids(FILE *, Centroid_data *, int *, int, int, int);
void Calc_Class_Instance(FILE *, Node_data *, int, int);
void Print_Class(FILE *, Node_data *, Centroid_data *, int, int, int);
void Calculate_BCuded(FILE *, Node_data *, Centroid_data *, int, int);
void Print_data(Node_data *, int *, int, int);
