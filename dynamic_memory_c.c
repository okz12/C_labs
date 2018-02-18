/* 
 * File:   main.cpp
 * Author: Osman
 *
 * Created on 21 January 2014, 10:26
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct{
    double x;
    double y;
} point;

int main(){
    int i, asize;
    point* pp;
    pp = malloc(sizeof(point));
    pp->y=0.5;
    pp->x=1;
    printf ("pp (%lf, %lf)\n", pp->x, pp->y);
    free(pp);
    printf("enter dynamic array size:\n");
    scanf("%d", &asize);
    
    pp=malloc(sizeof(*pp) * asize);
    
    for (i=0;i<asize;i++){
        printf("enter element %d:\n", i);
        scanf("%lf %lf", &pp[i].x, &pp[i].y);
    }
    
    for (i=0;i<asize;i++){
        printf("element %d: (%lf, %lf)\n", i, pp[i].x, pp[i].y);
    }
    
    free(pp);
    return 0;
}

