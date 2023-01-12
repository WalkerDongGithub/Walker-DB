cmake_minimum_required(VERSION 3.15)
project(WalkerDB)

set(CMAKE_CXX_STANDARD 14)
include_directories(ds fs error page table DB)

add_executable(WalkerDB

        definition.h

        page/page.h

        fs/file_system/AbstractFileSystem.h
        fs/file_system/BufferFileSystem.h
        fs/file_system/MemoryFileSystem.h

        fs/Buffer/Buffer.h
        fs/Buffer/SimpleBuffer.h
        fs/Buffer/LRUBuffer.h

        fs/Disk/Disk.h
        fs/Disk/MemoryDisk.h
        fs/Disk/FileDisk.h

        fs/File.h

        ds/double_linked_list.h
        ds/stack.h
        ds/hash_map.h
        ds/MemoryManager.h
        ds/priority_queue.h

        table/page_type/b_plus_page.h
        table/page_type/list_page.h

        table/Iterator/Iterable.h

        table/table_type/index_table/BPlusTreeIndexTable.h
        table/table_type/KVTable.h
        table/table_type/WithIndexTable.h
        table/table_type/record_table/QueueRecordTable.h
        table/table_type/SortedTable.h
        table/table_type/record_table/RecordTable.cpp
        table/table_type/index_table/IndexTable.cpp
        table/table_type/Table.h

        #        sql/tree/SqlTree.h
        #        sql/tree/SqlExpressionNode.h
        #        sql/tree/SqlSelectNode.h
        #        sql/tree/SqlTableNode.h
        #        sql/tree/SqlNameNode.h
        #        sql/tree/SqlProjectionNode.h

        definition.cpp

        error/DebugLog.h
        error/DebugLog.cpp

        Query/Group.h
        Query/Sub.h
        Query/Select.h
        Query/Sort.h
        Query/Join.h
        Query/Projection.h
        Query/Union.h
        Query/Intersection.h
        Query/Distinct.h
        Query/IterableBehaviour.cpp

        #        test/test_with_index_table.cpp

        #        test/test_iterable.cpp
        #        test/all_test.h
        #        test/all_test.cpp
        DB/Client.h
        DB/Launcher.h
        DB/Transaction.h
        DB/Database.h
        DB/UserManager.h
        DB/Engine.h

        fs/file_system/TransactionFileSystem.h
        fs/File.h

        table/table_type/record_table/RecordTable.cpp
        DB/DDLEngine.h
        DB/launcher/RootFileLauncher.h
        DB/Console.h

        test/test_console.cpp
        DB/DMLEngine.h
        DB/DQLEngine.h)
