#include"unit3.h"
#include"unit3_data.h"

#include<stdio.h>

/*
 * this might be a trawback of C, Data3_1_t is a private data, only have declaration in .h file
 * and other .c file should not know the implement detail of Data3_1_t
int func3_1(Data3_1_t *data){
    printf("%d\n", data->num);
    return 0;
}
*/

/*
 * Data3_2_t is public data, have defination in .h file
 * other .c file could use it's inner data
 */
int func3_2(Data3_2_t *data){
    printf("%d\n", data->num);
    return 0;
}
