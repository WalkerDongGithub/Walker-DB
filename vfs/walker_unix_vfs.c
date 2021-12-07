//
// Created by Walker on 2021/11/30.
//

#include "walker_vfs_implement.h"

/**
 * Unix系统管理文件的方式的 walker_vfs 实现。
 */

#include <fcntl.h>
#include <unistd.h>

const walker_byte buf[1024] = {0};

static walker_status unix_vfs_close(
        walker_file* vfs_file
        ) {
    close(vfs_file->fd);
    return walker_ok;
}

static walker_status unix_vfs_set(
        walker_file* vfs_file,
        walker_uint64   pos
        ) {
    walker_status status = lseek(vfs_file->fd, pos, SEEK_SET);
    if (status == -1) {
        debug("error set pos", walker_error);
        return walker_error;
    } return walker_ok;
}

static walker_status unix_vfs_append(
        walker_file* vfs_file,
        walker_uint64  num
        ) {
    walker_status status = lseek(vfs_file->fd, 0, SEEK_END);

    while (num != 0) {
        int cnt = write(vfs_file->fd, buf, min(1024, num));
        if (cnt > 0) {
            num -= cnt;
        } else {
            return walker_error;
        }
    }
}



static walker_status unix_vfs_read(
        walker_file*   vfs_file,
        walker_ptr     buf,
        walker_uint64    num,
        walker_uint64*   count
        ) {

    int real_count = read(vfs_file->fd, buf, num);
    if (real_count == -1) {
        debug("error read file.", walker_error);
        return walker_error;
    } else {
        *count = real_count;
        return walker_ok;
    }
}

static walker_status unix_vfs_write(
        walker_file*   vfs_file,
        walker_ptr     buf,
        walker_uint64    num,
        walker_uint64*   count
        ) {
    int real_count = write(vfs_file->fd, buf, num);
    if (real_count == -1) {
        debug("error write file.", walker_error);
        return walker_error;
    } else {
        *count = real_count;
        return walker_ok;
    }
}

static walker_status unix_vfs_lock(
        walker_file* vfs_file
        ) {
    return walker_empty_operation;
}

static walker_status unix_vfs_unlock(
        walker_file* vfs_file
        ) {
    return walker_empty_operation;
}

static walker_status unix_vfs_check_lock(
        walker_file* vfs_file
        ) {
    return walker_empty_operation;
}

static walker_status unix_vfs_size(
        walker_file* vfs_file
        ) {
    walker_int64 size = lseek(vfs_file->fd, 0, SEEK_END);
    assert(size != -1);
    return size;
}

static walker_file_function unix_vfs_file_function = {
        unix_vfs_close,
        unix_vfs_set,
        unix_vfs_append,
        unix_vfs_read,
        unix_vfs_write,
        unix_vfs_lock,
};




static walker_status unix_vfs_open(
        walker_vfs*    vfs,
        walker_status  status,
        walker_name    file_name,
        walker_file*   pFile
) {

    printf("open file %s\n", file_name);
    int fd = open(file_name, O_RDWR | O_CREAT | O_EXCL, 0666);

    if (fd == -1) {
        debug("error create file.", walker_error);
        fd = open(file_name, O_RDWR, 0666);
        if (fd == -1) {
            pFile = NULL;
            debug("error open file.", walker_error);
            return walker_error;
        }
    }

    pFile->fd = fd;
    pFile->pFunction   = &unix_vfs_file_function;
    pFile->file_size   = pFile->pFunction->xSize(pFile);

    return walker_ok;
}


walker_vfs walker_load_vfs(walker_status chance) {
    walker_vfs vfs;
    vfs.xOpen    = unix_vfs_open;
    return vfs;
}
