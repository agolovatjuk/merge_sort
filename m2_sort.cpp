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
 * TODO 
 * make class MSort
 * create my own Allocator instead std::vector<int>v1 for read data from file
 * refactor code..
 */

using namespace std;

size_t *auxArr;

void *msort(size_t *c, size_t lo, size_t mid, size_t hi){

    static int cnt = 0;
    size_t i = lo, j = mid + 1;

    size_t *a = auxArr;
    
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


void sort(size_t *c, size_t lo, size_t hi){

    if (lo >= hi)
        return;

    size_t mid = lo + (hi - lo)/2;
    sort(c, lo, mid);
    sort(c, mid + 1, hi);
    msort(c, lo, mid, hi);
}


void msortBU(size_t *c, size_t n){

    #pragma omp parallel num_threads(32)
    for (size_t sz = 1; sz < n; sz += sz ){
    #pragma omp for //parallel for num_threads(32)
        for (size_t lo = 0; lo < n - sz; lo += 2*sz)
            msort(c, lo, lo + sz - 1, min(lo + 2*sz - 1, n - 1));
    }
}


ssize_t read_dataset(const char *fname, std::vector <size_t> *v1) {

    size_t buff;

    ifstream f_in(fname, ios_base::in);

    if (!f_in) {
        fprintf(stderr, "Error open: %s\n", fname);
        return -1;
    }

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


void sort_dataset(const char *fname){

    std::vector <size_t> vbuff;

    size_t cnt = read_dataset(fname, &vbuff);
    if (cnt == -1){
        return;
    }
    auxArr = new size_t[vbuff.size()];

    cout << vbuff.size() << " data from file was read, sorting.." << endl;
    
    typedef std::chrono::high_resolution_clock Clock;
    auto tm_bg = Clock::now();

    size_t *x = &vbuff[0];
    msortBU(x, vbuff.size());
//    sort(x, 0, vbuff.size() - 1); //recursive variant

//    cout.precision(7);    
    auto tm_en = Clock::now();
    double tm_df = std::chrono::duration_cast<std::chrono::nanoseconds>(tm_en - tm_bg).count();
    std::cout << "Delta t2-t1: " << tm_df  << " nanoseconds" << std::endl;

//    print_vbuff(&vbuff);

    delete (auxArr);
}

int main(int argc, char **argv)
{
//    int d[] = {1,5,6,  4,8};
//    int lo = 0;
//    int hi = sizeof(d)/sizeof(int);
//    auxArr = new int[hi];
//    mergeBU(d, hi);
//    sort(d, lo, hi - 1);

    int opt;
//    char *fn_dflt = (char *) "data.txt";
    std::string file_name = "";
    
    while ((opt = getopt(argc, argv, "fh:")) != -1) {
        switch(opt) {
            case 'f':
                file_name = argv[optind];
                break;
            case 'h':
                break;
            default:
                fprintf(stderr, "Usage: %s [-f] file_name\n",argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    
    sort_dataset(file_name.c_str());

    return 0;
}
