#include "vfs/walker_vfs_implement.h"
#include "vm/walker_vm.h"
#include "lru/lru.h"

#include <time.h>
#include <stdlib.h>

/**
 * 祝我成为一名优秀的工程师。
 */
void test_vm() {
    walker_vm vm;
    walker_vm_init(&vm);
    walker_byte buffer[1024 * 16];
    for (walker_uint64 i = 0; i < 1024 * 16; i++) {
        buffer[i] = (walker_byte) i % 256;
    }
    walker_page_id id = walker_vm_new_table(&vm.file);
    walker_vm_write_all(&vm.file, id, buffer, 1024 * 16);
    walker_vm_destroy(&vm);

}


void test_lru() {
    int start = clock();
    lru l;
    lru_init(&l);
    srand(time(0));
    for (int i = 0; i < 1000; i++) {
        lru_get(&l, rand() % 100);
    }
    int end = clock();
    printf("%d\n", end - start);
}


int main() {

    printf("Hello! Welcome to Walker Studio!\n");
    printf("This is a Relationship Database implementation which is named walker sql.\n");
    printf("Thanks for your using!\n");
    printf("The first version of walker sql just implements the base functions.\n");

    test_lru();

    return 0;
}
