//
// Created by 董文阔 on 2022/7/16.
//

#include "../ds/hash_map.h"


int test_hash_map_insert() {
    Memory<ListNode<Entry<Id, Id>>, 5> mem;
    HashMap<Id, Id, 5, 11> hash(mem);
    cout << hash.Insert(1, 2) << endl;
    hash.Debug();
    cout << hash.Insert(1, 2) << endl;
    hash.Debug();
    cout << hash.Insert(2, 2) << endl;
    hash.Debug();
    cout << hash.Insert(4, 2) << endl;
    hash.Debug();
    cout << hash.Insert(5, 2) << endl;
    hash.Debug();
    cout << hash.Insert(13, 2) << endl;
    hash.Debug();
    cout << hash.Insert(6, 2) << endl;
    hash.Debug();
    cout << hash.Insert(1, 2) << endl;
    hash.Debug();
    return 0;
}
int test_hash_map_remove() {
    Memory<ListNode<Entry<Id, Id>>, 5> mem;
    HashMap<Id, Id, 5, 11> hash(mem);
    cout << hash.Insert(1, 2) << endl;
//    hash.Debug();
    cout << hash.Insert(2, 2) << endl;
//    hash.Debug();
    cout << hash.Insert(4, 2) << endl;
//    hash.Debug();
    cout << hash.Insert(5, 2) << endl;
//    hash.Debug();
    cout << hash.Remove(2) << endl;
    hash.Debug();
    return 0;

}
