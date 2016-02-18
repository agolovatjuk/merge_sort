#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <string>
#include <ctime>
#include <chrono>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

/*
 import random
 f = file("data.txt", "w")
 for i in range(1000000):f.write("%s " % random.randrange(1,1000000))
 f.close()
*/

/*
 http://www.viva64.com/ru/a/0054/
 * 
 * В последнем учебном модуле домашнее задание одно, предельно простое и дает сразу 100 баллов :)
Используя OpenMP, Intel TBB или MPI (на ваш выбор) сделайте 
 * (у себя, локально) параллельную сортировку слиянием и отсортируйте 
 * полученный набор данных.
 */

/*
 * TODO create my own Allocator instead std::vector<int>v1 for read data from file
 * rewrite reading file with dataset with for(...) and use threads
 * refactor code..
 */

using namespace std;

size_t *aux;


void *msort_2part(int *a, int *b, int a_sz, int b_sz){

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

void *msort(size_t *c, size_t lo, size_t mid, size_t hi){

    static int cnt = 0;
    size_t i = lo, j = mid + 1;

    size_t *a = aux;
    
    for(size_t k = lo; k <= hi; k++)
        a[k] = c[k];

//    cout << "lo:" << lo << "_hi:" << hi << endl;
    for(size_t k = lo; k <= hi; k++){
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

void *sort2(size_t *c, size_t lo, size_t hi){

    if (lo >= hi)
        return 0;

    size_t mid = lo + (hi - lo)/2;
    sort2(c, lo, mid);
    sort2(c, mid + 1, hi);
    msort(c, lo, mid, hi);
    return 0;
}

void mergeBU(size_t *c, size_t n){
    #pragma omp parallel num_threads(32)
    for (size_t sz = 1; sz < n; sz += sz ){
    #pragma omp for //parallel for num_threads(32)
        for (size_t lo = 0; lo < n - sz; lo += 2*sz)
            msort(c, lo, lo + sz - 1, min(lo + 2*sz - 1, n - 1));
    }
}

ssize_t read_dataset(char *fname, std::vector <size_t> *v1) {

    size_t buff;

    ifstream f_in(fname, ios_base::in);
    while (f_in >> buff) 
        v1->push_back(buff);
    f_in.close();
    
//    char c[2];
//    c[1] = '\0';
//    std::string buff;
//    FILE *f = fopen(fname, "r");
//    do {
//        c[0] = fgetc(f);
//        if ((isspace(c[0]) || c[0] == EOF) && buff != ""){
////            v1->push_back(atoll(buff.c_str()));
//            v1->push_back(strtoull(buff.c_str(),0,10));
//            buff.clear();
//        }
//        else if (! isspace(c[0])){
//            buff.append(c);
//        }
//    } while (c[0] != EOF);
//    
//    fclose(f);
    
    return v1->size();   
}

void print_vbuff(std::vector <size_t> * v){

//    size_t &x_iter = *v->begin(); // x_iter++
//    vector <size_t>::iterator iter = v->begin(); // *(iter++)

    for (size_t i = 0; i < v->size(); i++){
        if(i < v->size() - 1)
            cout << v->at(i) << " ";
        else
            cout << v->at(i) << endl;
    }
}

void sort_dataset(){

    std::vector <size_t> vbuff;

    read_dataset((char *) "data.txt", &vbuff);
    aux = new size_t[vbuff.size()];

    cout << vbuff.size() << " File was read, sorting.." << endl;
    
    typedef std::chrono::high_resolution_clock Clock;
    auto t1 = Clock::now();

    size_t *x = &vbuff[0];
    mergeBU(x, vbuff.size());

//    cout.precision(7);    
    auto t2 = Clock::now();
    double ff = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
    std::cout << "Delta t2-t1: " << ff  << " nanoseconds" << std::endl;

/*    std::cout << "Delta t2-t1: " 
          << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count()
          << " nanoseconds" << std::endl;
*/

    print_vbuff(&vbuff);

    delete (aux);
}

int main()
{
//    cout << "Hello msort!" << endl;
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

    sort_dataset();

    return 0;
}
