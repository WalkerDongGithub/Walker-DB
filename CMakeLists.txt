cmake_minimum_required(VERSION 3.20)
project(wsql C)

set(CMAKE_C_STANDARD 99)

add_executable(
        wsql
        walker_main.c
        walker_context.h

        vfs/walker_vfs.h
        vfs/walker_unix_vfs.c
        vfs/walker_vfs_implement.h

        vm/walker_vm.h walker_context.c
        vm/walker_vm_file.c

        lru/lru.h 
        lru/lru.c)
