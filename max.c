#include<stdio.h>
#include<stdlib.h>
#define FGETS
#ifdef COMPARE
int comp(const void*a,const void*b)//用来做比较的函数。
{
    return *(int*)b-*(int*)a;
}
int main()
{
    int a[10] = {2,4,1,5,5,3,7,4,1,5};//乱序的数组。
    int i;
    qsort(a,10,sizeof(int),comp);//调用qsort排序
    for(i=0;i<10;i++)//输出排序后的数组
    {
        printf("%d\t",a[i]);
    }
    return 0;
}
#endif
#ifdef FGETS
int main(){
    FILE *fd=fopen("cc.c","r");
    char buf[100];
    fgets(buf,100,fd);
    printf("%s\n",buf);
    return 0;
}
#endif
