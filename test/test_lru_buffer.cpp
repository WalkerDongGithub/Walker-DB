//
// Created by 董文阔 on 2022/7/16.
//
#include "../fs/Buffer/LRUBuffer.h"

#include <random>
int main() {
    std::default_random_engine dre(0);
    std::normal_distribution<double> uid(0, 3);
    LRUBuffer buffer;
    for (int i = 0; i < 1000; i++) {
        Id x = (Id)uid(dre);
        while (x < 0) x = (Id)uid(dre);
        Id replace_key, replace_value;
        bool miss;
        Id res = buffer.Get(x, replace_key, replace_value, miss);
        cout << res << " " << x << " " << replace_key << " " << replace_value << " " << miss << endl;

    }
    return 0;
}