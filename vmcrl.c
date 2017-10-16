#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "vmcrl.h"

void get_proc_page_data(int pid, unsigned long vm_address, uint64_t *data, unsigned int *distance) {
    int fd, pagesize, ret;
    char pm_path[32];
    unsigned long offset;
    off_t lsret;
    ssize_t nr;

    sprintf(pm_path, "/proc/%d/pagemap", pid);
    fd = open(pm_path, O_RDONLY);
    if(fd == -1) {
        perror("open");
        exit(1);
    }

    pagesize = getpagesize();
    /* dist. from page boundary */
    *distance = vm_address % pagesize;
    /* page index * data entry size */
    offset = vm_address / pagesize * 8;

    lsret = lseek(fd, offset, SEEK_SET);
    if(lsret == (off_t)-1) {
        perror("lseek");
        exit(1);
    }

again:
    nr = read(fd, data, 8);
    if(nr == -1) {
        if(errno == EINTR)
            goto again;
        perror("read");
        exit(1);
    }

    ret = close(fd);
    if(ret == -1) {
        perror("close");
        exit(1);
    }
}

void write_dev_mem(unsigned long offset, char *data, int count) {
    int fd, ret;
    off_t lsret;
    ssize_t nr;

    fd = open("/dev/mem", O_WRONLY);
    if(fd == -1) {
        perror("open");
        exit(1);
    }

    lsret = lseek(fd, offset, SEEK_SET);
    if(lsret == (off_t)-1) {
        perror("lseek");
        exit(1);
    }

    nr = write(fd, data, count);
    if(nr == -1) {
        perror("write");
        exit(1);
    }

    ret = close(fd);
    if(ret == -1) {
        perror("close");
        exit(1);
    }
}

void write_proc_mem(int pid, unsigned long vm_address, char *data, int count) {
    int fd, ret;
    off_t lsret;
    ssize_t nr;
    char pm_path[32];

    sprintf(pm_path, "/proc/%d/mem", pid);
    fd = open(pm_path, O_WRONLY);
    if(fd == -1) {
        perror("open");
        exit(1);
    }

    lsret = lseek(fd, vm_address, SEEK_SET);
    if(lsret == (off_t)-1) {
        perror("lseek");
        exit(1);
    }

    nr = write(fd, data, count);
    if(nr == -1) {
        perror("write");
        exit(1);
    }

    ret = close(fd);
    if(ret == -1) {
        perror("close");
        exit(1);
    }
}

int main(int argc, char **argv) {
    int pid;
    unsigned long vm_addr, ph_addr;
    unsigned int dist;
    uint64_t data, pfn;

    printf("** vmcrl %s **\n\n", VMCRL_VERSION);

    if(argc < 3) {
        fprintf(stderr, "wrong number of args\n");
        exit(1);
    }
    pid = atoi(argv[1]);
    vm_addr = strtoul(argv[2], NULL, 0);

    get_proc_page_data(pid, vm_addr, &data, &dist);
    /* Bits 0-54  page frame number (PFN) if present */
    pfn = data & 0x7FFFFFFFFFFFFF;
    ph_addr = (pfn << PAGE_SHIFT) + dist;

    printf("PID=%d, virt. address='%p'\nphys. address='%p'\n",
            pid, (void *)vm_addr, (void *)ph_addr);

    if(argc == 4) {
        if(argv[3][0] == 'd') {
            write_dev_mem(ph_addr, DM_DATA, strnlen(DM_DATA, 12));
            printf("wrote '"DM_DATA"' via /dev/mem\n");
        } else if (argv[3][0] == 'p') {
            write_proc_mem(pid, vm_addr, PM_DATA, strnlen(PM_DATA, 12));
            printf("wrote '"PM_DATA"' via /proc/%d/mem\n", pid);
        }
        else {
            fprintf(stderr, "wrong command parameter\n");
            exit(1);
        }
    } else
        printf("no command, just listed\n");

    return 0;
}
