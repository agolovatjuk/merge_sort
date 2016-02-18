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
    
    return v1->size();   
}


void print_vbuff(std::vector <size_t> *v){

    for(auto n : *v){
        if (n == v->back()) // last element of my vector!
            cout << n << endl;
        else
            cout << n << ' ';
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
    sort_dataset();

    return 0;
}
