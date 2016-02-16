#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <string>
#include <ctime>
#include <chrono>

/*
 import random
 f = file("data.txt", "w")
 for i in range(1000000):f.write("%s " % random.randrange(1,1000000))
 f.close()
*/

using namespace std;

int *aux;


void *msort(int *a, int *b, int a_sz, int b_sz){

    int i=0, j=0;//, k=0;
    int *c = new int[a_sz+b_sz];

    for(int k = 0; k<a_sz+b_sz; k++){
        if (i >= a_sz)
            c[k] = b[j++];
        else if (j >= b_sz)
            c[k] = a[i++];
        else if(a[i]>b[j])
            c[k] = b[j++];
        else
            c[k] = a[i++];
//        cout << c[k] << endl;
    }
    return c;
}

void *msort2(int *c, int lo, int mid, int hi){

    static int cnt = 0;
    int i = lo, j = mid + 1;

    int *a = aux; //new int[hi + 1];
    
    for(int k = lo; k <= hi; k++)
        a[k] = c[k];

//    cout << "lo:" << lo << "_hi:" << hi << endl;
    for(int k = lo; k <= hi; k++){
        if (i > mid)
            c[k] = a[j++];
        else if (j > hi)
            c[k] = a[i++];
        else if(a[i] < a[j])
            c[k] = a[i++];
        else
            c[k] = a[j++];
//        cout <<"cnt:"<<cnt<<":k:"<< k<<":" <<c[k] << endl;
    }
    cnt++;
    return 0;
}

void *sort2(int *c, int lo, int hi){

    if (lo >= hi)
        return 0;

    int mid = lo + (hi - lo)/2;
    sort2(c, lo, mid);
    sort2(c, mid + 1, hi);
    msort2(c, lo, mid, hi);
    return 0;
}

void mergeBU(int *c, int n){
    for (int sz = 1; sz < n; sz += sz ){
    #pragma omp parallel for num_threads(32)
        for (int lo = 0; lo < n - sz; lo += 2*sz)
            msort2(c, lo, lo + sz - 1, min(lo + 2*sz - 1, n - 1));
    }
}

void sort_file(){

    char c[2];
    c[1] = '\0';
    std::vector <int> v1;
    std::string buff;

    FILE *f = fopen("data.txt", "r");
    while( (c[0] = fgetc(f)) != EOF){
        if (c[0] == ' ' && buff != ""){
            v1.push_back(atoll(buff.c_str()));
            buff = "";
        }
        else if (c[0] != ' ')
            buff.append(c);
    }
    fclose(f);

    int lo = 0;
    int hi = v1.size();
    aux = new int[hi];

    int *x = &v1[0];
    
    typedef std::chrono::high_resolution_clock Clock;
    auto t1 = Clock::now();

//    #pragma omp parallel num_threads(2)
    mergeBU(x, v1.size());
    
    auto t2 = Clock::now();
    std::cout << "Delta t2-t1: " 
          << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count()
          << " nanoseconds" << std::endl;

//    for (int i = 0; i < v1.size(); i++)
//        cout << x[i] << endl;

    delete (aux);
}

int main()
{
    cout << "Hello msort!" << endl;
//    int d[] = {6,  4};
//    int d[] = {6,1,  4};
//    int d[] = {6,1,  5,4,3};
//    int d[] = {66,17,16,11,33,97,83,41,47,59, 66,17,16,11,33,97,83,41,47,59, 66,17,16,11,33,97,83,41,47,59};
//    int d[] = {1,5,6,  4,8};

//    int lo = 0;
//    int hi = sizeof(d)/sizeof(int);
//    aux = new int[hi];
//    mergeBU(d, hi);
//    sort2(d, lo, hi - 1);
//    int mid = lo + (hi - lo - 1)/2;
//    e = (int *) msort2(d, lo, mid, hi - 1);
//    c = (int *) msort(a, b, sizeof(a)/sizeof(int), sizeof(b)/sizeof(int));

//    for(int k = 0; k < hi; k++)
//        cout << d[k] << endl;

    sort_file();

    return 0;
}
