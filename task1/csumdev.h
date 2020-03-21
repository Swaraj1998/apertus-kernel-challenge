#ifndef CSUMDEV_H
#define CSUMDEV_H

#define CSUM_RET_SIZE 32

struct csum_arg_t {
        char str[CSUM_RET_SIZE];
};

#define CSUM_IOC_MAGIC '['
#define CSUM_GET_STRING _IOR(CSUM_IOC_MAGIC, 0x10, struct csum_arg_t *)

#endif
