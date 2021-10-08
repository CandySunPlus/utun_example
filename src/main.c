#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#include <net/if_utun.h>
#include <sys/ioctl.h>
#include <sys/kern_control.h>
#include <sys/socket.h>
#include <sys/sys_domain.h>
#include <sys/types.h>

int tun(void) {
    struct sockaddr_ctl sc = {0};
    struct ctl_info ctlInfo = {0};
    int fd;

    if (strlcpy(ctlInfo.ctl_name, UTUN_CONTROL_NAME, sizeof(ctlInfo.ctl_name)) >= sizeof(ctlInfo.ctl_name)) {
        perror("UTUN_CONTROL_NAME too long");
        return -1;
    }

    if ((fd = socket(PF_SYSTEM, SOCK_DGRAM, SYSPROTO_CONTROL)) == -1) {
        perror("socket(SYSPROTO_CONTROL)");
        return -1;
    }

    if (ioctl(fd, CTLIOCGINFO, &ctlInfo) == -1) {
        perror("ioctl(CTLIOCGINFO");
        close(fd);
        return -1;
    }

    sc.sc_id = ctlInfo.ctl_id;
    sc.sc_len = sizeof(sc);
    sc.sc_family = AF_SYSTEM;
    sc.ss_sysaddr = AF_SYS_CONTROL;
    sc.sc_unit = 10; // just a example

    if (connect(fd, (struct sockaddr*)&sc, sizeof(sc)) == -1) {
        perror("connect(AF_SYS_CONTROL");
        close(fd);
        return -1;
    }

    return fd;
}

int main(int argc, char* argv[]) {
    int utunfd = tun();
    if (utunfd == -1) {
        fprintf(stderr, "Unable to establish UTUN descriptor - aborting\n");
    }

    printf("Utun interface is up.. Configure UPv4 using \"ifconfig utun9 <ip_a> "
           "<ip_b>\"\n");
    printf("                       Configure UPv6 using \"ifconfig utun9 inet6 "
           "<ip_6>\"\n");
    printf("Then (e.g.) ping <ip_b> (IPv6 will automatically generate ND "
           "messages)\n");

    for (;;) {
        unsigned char c[1500];
        int len, i;
        len = read(utunfd, c, 1500);

        for (i = 4; i < len; i++) {
            printf("%02x ", c[i]);
            if ((i - 4) % 16 == 15) {
                printf("\n");
            }
        }
        printf("\n");
    }

    return 0;
}
