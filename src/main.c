#include <kos.h>
#include <dc/vmufs.h>
#include <dc/maple.h>
#include <dc/maple/vmu.h>
#include <stdio.h>

KOS_INIT_FLAGS(INIT_DEFAULT);

int main() {
    maple_device_t *vmu;
    file_t src, dst;
    vmu_dirent_t *dir;
    int file_count;

    dbglog(DBG_INFO, "Searching for VMU in port A1...\n");

    vmu = maple_enum_type(0, MAPLE_FUNC_MEMCARD);
    if (!vmu) {
        dbglog(DBG_ERROR, "No VMU found in port A1.\n");
        return -1;
    }

    mkdir("/sd/vmu_backup", 0777);

    dir = vmu_dir_read(vmu, &file_count);
    for (int i = 0; i < file_count; ++i) {
        char src_path[32], dst_path[64];
        sprintf(src_path, "/vmu/a1/%s", dir[i].filename);
        sprintf(dst_path, "/sd/vmu_backup/%s", dir[i].filename);

        src = fs_open(src_path, O_RDONLY);
        dst = fs_open(dst_path, O_WRONLY | O_CREAT);

        if (src >= 0 && dst >= 0) {
            char buf[512];
            int r;
            while ((r = fs_read(src, buf, sizeof(buf))) > 0) {
                fs_write(dst, buf, r);
            }
            dbglog(DBG_INFO, "Backed up: %s\n", dir[i].filename);
        }

        fs_close(src);
        fs_close(dst);
    }

    free(dir);
    dbglog(DBG_INFO, "Backup complete.\n");

    while(1);
    return 0;
}
