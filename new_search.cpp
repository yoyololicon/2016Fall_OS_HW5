#include<stdlib.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<wait.h>
#include<sys/resource.h>
#include <fstream>
#include <iostream>
using namespace std;

#define IDX_SIZE 11
#define RECORD_SIZE 4092
int search(char* addr , string &keyword, int start, int end, size_t start_dis)
{
    int low=0,  high = (start_dis/IDX_SIZE)-1, ret;
    static char buffer[4100];
    static char tmp[5], idx[6];
    tmp[4] = '\0';
    size_t data_pos = start_dis + 6;

    while (low <= high){
        int mid = (low + high) / 2;
        strncpy(tmp,(addr+mid*IDX_SIZE),4);
        ret = strcmp(tmp, keyword.c_str());
        if (ret == 0)
        {
            strncpy(idx, (addr+mid*IDX_SIZE+5), 6);
            for(int i = 0; i < 6; i++)
            {
                if(idx[i] == ' ' | idx[i] == '\n')
                {
                    idx[i] = 0;
                    break;
                }
            }
            int real_idx = atoi(idx);
            memset(buffer, '\0', sizeof(buffer));
            strncpy(buffer, addr + data_pos + RECORD_SIZE * real_idx + start,  (size_t)end - start + 1);
            cout << "key " << keyword << " found : " << string(buffer) << endl;
            return mid;
        }
        else if (ret > 0)
            high = mid - 1;
        else
            low = mid + 1;
    }
    cout << "key " << keyword << " not found" << endl;
    return -1;
}
void print_max_rss(){
    struct rusage r_stat;
    getrusage(RUSAGE_SELF, &r_stat);
    cout << "max rss = " << r_stat.ru_maxrss << " KB" << endl;
}

int main(int argc, char* argv[]){
    char *addr;
    int dataset_fd;
    struct stat sb;
    int start, end;
    size_t N;
    fstream testcase ;
    string key;

    dataset_fd = open("new_data.txt", O_RDWR);
    testcase.open(argv[1], fstream::in);
    fstat(dataset_fd, &sb);
    N = (size_t )sb.st_size;
    addr =  (char*)mmap(NULL, N , PROT_READ| PROT_WRITE , MAP_PRIVATE ,dataset_fd , 0);

    //find true data position
    char* pch = strstr(addr, "start");
    size_t dis = pch - addr;

    while(testcase >> key >> start >> end)
    {
        search(addr, key, start, end, dis);
    }
    munmap(addr, N);
    print_max_rss();
    close(dataset_fd);
    testcase.close();
    return 0;
}
