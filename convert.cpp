#include <iostream>
#include <fstream>
#include <queue>
#include <string>
using namespace std;

int main() {
    fstream         data, new_data;
    char            key[5];
    int             count = 0;
    queue<char*>    buffer_tmp;

    data.open("data.txt", fstream::in | fstream::binary);
    new_data.open("new_data.txt", fstream::out | fstream::binary);


    while(!data.eof())
    {
        string  offset = to_string(count);
        char*   buffer = new char[4092];

        data.read(key, 4);
        key[4] = ' ';

        new_data.write(key, 5);
        new_data.write(offset.c_str(), offset.size());
        for(int i = 0; i < 5 - offset.size(); i++)
            new_data.write(" ", 1);
        new_data.write("\n", 1);

        data.read(buffer, 4092);
        buffer_tmp.push(buffer);
        count++;
    }

    new_data.write("start\n", 6);
    while(!buffer_tmp.empty())
    {
        new_data.write(buffer_tmp.front(), 4092);
        delete [] buffer_tmp.front();
        buffer_tmp.pop();
    }

    data.close();
    new_data.close();

    return 0;
}