/*
 * =====================================================================================
 *
 *       Filename:  RIO.c
 *
 *    Description: rio implementation
 *
 *        Version:  1.0
 *        Created:  06/29/2014 05:10:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  GuoJH
 *   Organization:  Home
 *
 * =====================================================================================
 */
#include    "RIO.h"

/*
 * DES: Our internal fundation read function which has a internal buffer.
 * It will be called in our following different reading functions.
 * @param rio_t *rp :pointer points to rio_t structure
 * @param char *usrbuf :user`s buffer
 * @param size_t n : read bytes
 */
static ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n)
{
	int cnt;
	while( rp->rio_cnt <= 0 )	/* Refill if buf is empty */
	{
		rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf));
		if( rp->rio_cnt < 0 )
		{
			if(errno != EINTR)	/* Interrupted by sig handler return */
			  return -1;
		}
		else if(rp->rio_cnt == 0)	/* EOF */
		  rp->rio_bufptr = rp->rio_buf;	/* Reset buffer ptr */
	}

	/* Copy min(n, rp->rio_cnt) bytes from internal buf to user buf */
	cnt = n;
	if(rp->rio_cnt < n)
	  cnt = rp->rio_cnt;
	memcpy(usrbuf, rp->rio_bufptr, cnt);
	rp->rio_bufptr += cnt;
	rp->rio_cnt -= cnt;
	return cnt;
}


/*
 * DES:Read without buffer.Read files pointed by fd, reading until meet EOF.
 * @param int fd :file descripter
 * @param void *usrbuf :usr`s buffer
 * @param size_t n :length to be read
 * return read bytes
 */
ssize_t rio_readn(int fd, void *usrbuf, size_t n)
{
	size_t nleft = n;
	ssize_t nread;
	char *bufp = (char*)usrbuf;

	while( nleft > 0 )
	{
		if((nread = read(fd, bufp, nleft)) < 0)
		{
			if(errno == EINTR)
			  nread = 0;
			else
			  return -1;
		}
		else if(nread == 0)
		{
			break;
		}
		nleft -= nread;
		bufp += nread;
	}

	return (n - nleft);
}


/*
 * DES:Write without buffer.Write files pointed by fd.
 * @param int fd :file descripter
 * @param void *usrbuf :usr`s buffer
 * @param size_t n :length to be read
 * return write bytes
 */
ssize_t rio_writen(int fd, void *usrbuf, size_t n)
{
	size_t nleft = n;
	ssize_t nwritten;
	char *bufp = (char*)usrbuf;

	while( nleft > 0 )
	{
		if( (nwritten = write(fd, bufp, nleft)) <= 0 )
		{
			if(errno == EINTR)
			  nwritten = 0;
			else
			  return -1;
		}
		nleft -= nwritten;
		bufp += nwritten;
	}

	return n;
}

/*
 * DES: Init internal buffer when use read functions with a b
 * @param rio_t *rp : pointer points to rio_t structure
 * @param int fd :file descriptor
 */
void rio_readinitb(rio_t *rp, int fd)
{
	rp->rio_fd = fd;
	rp->rio_cnt = 0;
	rp->rio_bufptr = rp->rio_buf;
}

/*
 * DES: read n bytes in interanl buffer and copy a line into usrbuf
 * @param rio_t *rp :pointer points to rio_t structure
 * @param void *usrbuf :user's buffer
 * @param size_t n :read bytes
 * return read bytes
 */
ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen)
{
	int n, rc;
	char c, *bufp = (char*)usrbuf;

	for( n = 1 ; n < maxlen ; ++n )
	{
		if((rc = rio_read(rp, &c, 1)) == 1)
		{
			*bufp++ = c;
			if(c == '\n')
			  break;
		}
		else if(rc == 0)
		{
			if(n == 1)
			  return 0;	/* EOF, no data read */
			else 
			  break;	/* EOF, some data was read */
		}
		else
		  return -1;	/* Error */
		
	}
	*bufp = 0;
	return n;
	
}

/*
 * DES: Read with internal buffer.
 * It will be called in our following different reading functions.
 * @param rio_t *rp :pointer points to rio_t structure
 * @param char *usrbuf :user`s buffer
 * @param size_t n : read bytes
 */
ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n)
{
	size_t nleft = n;
	ssize_t nread;
	char *bufp = (char*)usrbuf;

	while( nleft > 0 )
	{
		if((nread = rio_read(rp, bufp, nleft)) < 0)
		{
			if(errno == EINTR)	/* Interrupt by sig hanler return */
			  nread = 0;		/* Call read() again */
			else
			  return -1;		/* errno set by read() */
		}
		else if(nread == 0)
		  break;				/* EOF */
		nleft -= nread;
		bufp += nread;
	}
	return(n - nleft);
}
