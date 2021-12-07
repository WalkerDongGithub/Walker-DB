/**
 * 2021/12/04 Walker
 */

#include "walker_vm.h"


/**
 * vm 层 file 接口：
 *
 *      vm 层为上层提供了一个虚拟的文件系统空间
 *      该代码提供的是页式管理的文件系统，他为上层的系统提供了一个页式存储空间
 *      从而从逻辑上创造出一个虚拟的互相隔离的文件空间。
 *
 */







/**
 * vm write 接口：
 *
 *                      vm write接口负责调用 vfs 的 xWrite 虚函数。
 *                      其中最关键的部分是严格控制其一次调用只能最多写入一页数据。
 *                      如果想要写入的量超过一页数据，将最多写入到页尾，然后返回实际写入的数量。
 *
 *
 *
 *
 * @param file      需要写入的文件。
 * @param page_id   写入页位置。
 * @param start     写入位置的页偏移量。
 * @param buf       写缓冲区的指针。
 * @param len       写入的最大数目。
 * @return          返回成功写入的数据字节数。
 */

walker_uint64 walker_vm_write (
        walker_file*        file,
        walker_page_id      page_id,

        walker_uint64         start,
        walker_const_ptr    buf,
        walker_uint64         len
) {

    assert (start < walker_page_size);
    walker_file_function* f = file->pFunction;

    f->xSet(file, page_id * walker_page_size + start);
    walker_uint64 num;
    walker_status status =
            f->xWrite(file, buf,
                      min(len, walker_page_size - start), &num);
    if (status == walker_error) {
        debug("error write.", walker_error);
    }
    return num;
}


/**
 * vm read 接口：
 *
 *                      vm read接口负责调用 vfs 的 xRead 虚函数。
 *                      其中最关键的部分是严格控制其一次调用只能最多读取一页数据。
 *                      如果想要读取的量超过一页数据，将最多读取到页尾，然后返回实际读取的数量。
 *
 *
 *
 *
 * @param file      需要读取的文件。
 * @param page_id   读取页位置。
 * @param start     读取位置的页偏移量。
 * @param buf       读缓冲区的指针。
 * @param len       读取的最大数目。
 * @return          返回成功读取的数据字节数。
 */


walker_uint64 walker_vm_read (
        walker_file*         file,
        walker_page_id       page_id,

        walker_uint64          start,
        walker_ptr           buf,
        walker_uint64          len
) {
    assert(start < walker_page_size);
    walker_file_function* f = file->pFunction;

    f->xSet(file, page_id * walker_page_size + start);
    walker_uint64 num;
    walker_status status =
            f->xRead(file, buf,
                     min(len, walker_page_size - start), &num);
    if (status == walker_error) {
        debug("error read.", walker_error);
    }
    return num;
}




/**
 * set next
 *                  为某一页设置next，
 *                  每一页的next指针被安排在了每一页的页首八个字节上。
 *                  实际就是将next_num参数按小端法写入到了该页的头八个字节中。
 *
 * @param file      需要写入的文件
 * @param page_id   需要写入的页号
 * @param next_num  需要被设置的next。
 * @return          是否设置成功。
 */
walker_status walker_vm_set_next(
        walker_file*    file,
        walker_page_id  page_id,
        walker_page_id  next_num
) {
    walker_uint64 count =
            walker_vm_write(file, page_id, 0, &next_num, walker_page_id_size);
    assert(count == walker_page_id_size);
    return walker_ok;
}


/**
 * get next
 *                  获取某一页的next页号
 *                  实际是获取该页的前八个字节对应的小段存储的long数据类型数据。
 *
 * @param file      需要读取的文件
 * @param page_id   需要读取的页号
 * @return          是否设置成功。
 */
walker_page_id walker_vm_get_next(
        walker_file*    file,
        walker_page_id  page_id
) {
    walker_page_id next_num;
    walker_uint64 count =
            walker_vm_read(file, page_id, 0, &next_num, walker_page_id_size);
    assert(count == walker_page_id_size);
    return next_num;
}



/**
 * create page
 *                  create page 即向文件末尾填充零字符，主要是为了预留空间。
 * @param file      需要写入的file
 * @return          返回写入状态。
 */
walker_status walker_vm_create_page(
        walker_file* file
) {
    return file->pFunction->xAppend(file, walker_page_size);
}



/**
 * get page num
 *                  获取文件页数，以文件大小作为参考标准，获取实际页面数量。
 *
 * @param file      需要获取页数的文件。
 * @return          获取到的页数。
 */
walker_uint64 walker_vm_get_page_num(
        walker_file* file
) {
    walker_uint64 size = file->pFunction->xSize(file);
    return size / walker_page_size + ((size % walker_page_size == 0) ? 0 : 1);
}




/**
 * new page
 *                  新建页，新建页是指新建一个空页，而不是获取一个空页，具体区别见后文。
 *                  新建页操作即获取到当前页数作为新页的id，然后返回给调用者。
 *                  同时初始化页面，将其后继页号置为 end
 * @param file
 * @return
 */
walker_page_id walker_vm_new_page(
        walker_file*        file
) {
    walker_page_id new_page_id = walker_vm_get_page_num(file);
    walker_page_id id = walker_end;
    walker_vm_write(file, new_page_id, 0, &id, walker_page_id_size);
    return new_page_id;
}

/**
 * get empty page
 *                  获取一个空页，是更加常用的一个接口
 *                  页式管理文件系统安排一个特定的空页仓库，
 *                  将所有的空页组织为一个链表结构，
 *                  每次获取空页即获取该空页链表的头节点，此时该空页将不再记作空页，被移除出空页链表。
 *                  若此时链表中没有空页，则新建一个空页插入到链表当中去，以保证仓库中永远都有一个空页存在。
 *
 * @param file      需要处理的文件。
 * @return          返回的空页号。
 */
walker_page_id walker_vm_get_empty_page(
        walker_file* file
) {
    // write(file, id, start, buf, size)
    walker_page_id page_id;
    walker_vm_read(file, 0, walker_page_id_size, &page_id, walker_page_id_size);
    walker_page_id next_id = walker_vm_get_next(file, page_id);
    if (next_id == walker_end) {
        walker_page_id new_id = walker_vm_new_page(file);
        walker_vm_write(file, 0, walker_page_id_size, &new_id, walker_page_id_size);
    } else {
        walker_vm_write(file, 0, walker_page_id_size, &next_id, walker_page_id_size);
    } return page_id;
}


/**
 * new table
 *                   new table 是更上层的调用
 *                   new table 是为上层新建一个文件系统，
 *                   返回一个页面号，该页面号即本文件系统的起始地址。
 *
 * @param file      需要建表的文件
 * @return          文件系统的初始页面号。
 * todo:            注意这个位置的write 的 start 实际一直指向首页的第三个8位上，这个地方是为了方便调试，并非bug
 */
walker_page_id walker_vm_new_table(
        walker_file* file
) {
    walker_page_id id = walker_vm_get_empty_page(file);
    walker_vm_write(file, 0, walker_page_id_size * 2, &id, walker_page_id_size);
    return id;
}


/**
 * write all
 *                   上层获取到一个文件系统的初始页面号之后，就可以进行页面读写了
 *                   关于页面读入的细节，由于缓冲区在读方，事先并未知晓具体需要读多大，所以实现应在上层调用。
 *                   而关于写入细节则在vm层控制。
 *                   写入时通过write接口实现最多一页的写入
 *                   当发现写入不够时，说明需要开辟新的页面
 *                   此时将获取一个空白页继续写入，并设置为旧页的后继页号，直至写完。
 * @param file
 * @param page_id
 * @param buffer
 * @param size
 * @return
 */
walker_status walker_vm_write_all(
        walker_file*    file,
        walker_page_id  page_id,
        walker_ptr      buffer,
        walker_uint64     size
) {
    walker_uint64 i = 0;
    while (i < size) {
        walker_uint64 count =
                walker_vm_write(file, page_id, walker_page_id_size, &buffer[i], size-i);
        printf("count : %d\n", count);
        if (i + count < size) {
            walker_page_id next_id = walker_vm_get_next(file, page_id);
            if (next_id == walker_end) {
                next_id = walker_vm_get_empty_page(file);
                walker_vm_set_next(file, page_id, next_id);
            }
            page_id = next_id;
        }
        i += count;
    } return walker_ok;
}



walker_status walker_list_page(
        walker_vm* vm,
        walker_file* file,
        walker_page_id id
) {
    while (id != walker_end) {
        printf("id : %d", id);
        id = walker_vm_get_next(file, id);
    } return walker_ok;
}


/**
 * vm 的全局 init 函数。
 * todo：还有动态页面和磁盘页面的初始化等。
 */
walker_status walker_vm_init(walker_vm * vm) {

    /*
     * 配置vfs信息。
     */
    vm->vfs = walker_load_vfs(walker_default);
    walker_vfs* vfs = &vm->vfs;

    vm->vfs.xOpen(vfs, walker_default, "w_sql.w", &vm->file);

    walker_file* file = &vm->file;
    if (file->file_size == 0) {
        debug("new database, initialize", walker_info);
        /*
         * 为首页和第一页配置end指针，并在首页上写下空页链表表头的地址。
         */
        walker_vm_create_page(file);
        walker_vm_create_page(file);
        walker_vm_set_next(file, 0, walker_end);
        walker_vm_set_next(file, 1, walker_end);
        walker_page_id id = 1;
        walker_vm_write(
                file, 0,
                walker_page_id_size, &id, walker_page_size);
    }

    return walker_ok;

}



walker_status walker_vm_destroy(walker_vm* vm) {
    vm->file.pFunction->xClose(&vm->file);
    return walker_ok;
}




