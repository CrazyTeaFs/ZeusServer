#ifndef RIO__H
#define RIO__H

#include  <stdio.h>
#include  <sys/types.h>
#include  <sys/stat.h>
#include  <fcntl.h>
#include  <errno.h>
#include  <string.h>
#include  <unistd.h>

/*
 * DES:Read without buffer.Read files pointed by fd, reading until meet EOF.
 * @param int fd :file descripter
 * @param void *usrbuf :usr`s buffer
 * @param size_t n :length to be read
 * return read bytes
 */
ssize_t rio_readn(int fd, void *usrbuf, size_t n);

/*
 * DES:Write without buffer.Write files pointed by fd.
 * @param int fd :file descripter
 * @param void *usrbuf :usr`s buffer
 * @param size_t n :length to be read
 * return write bytes
 */
ssize_t rio_writen(int fd, void *usrbuf, size_t n);

/*
 * structure when read or write with inner buffer.
 */
#define RIO_BUFSIZE 8192
typedef struct
{
	int rio_fd;	                  /* Descriptor for this internal buf*/
	int rio_cnt;                  /* Unread bytes in internal buf */
	char *rio_bufptr;             /* Next unread byte in internal buf */
	char rio_buf[RIO_BUFSIZE];	  /* Interanl buffer */
} rio_t;

/*
 * DES: Init internal buffer when use read functions with a b
 * @param rio_t *rp : pointer points to rio_t structure
 * @param int fd :file descriptor
 */
void rio_readinitb(rio_t *rp, int fd);

/*
 * DES: read n bytes in interanl buffer and copy a line into usrbuf
 * @param rio_t *rp :pointer points to rio_t structure
 * @param void *usrbuf :user's buffer
 * @param size_t n :read bytes
 * return read bytes
 */
ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);

/*
 * DES: Read with internal buffer.
 * It will be called in our following different reading functions.
 * @param rio_t *rp :pointer points to rio_t structure
 * @param char *usrbuf :user`s buffer
 * @param size_t n : read bytes
 */
ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n);
#endif
