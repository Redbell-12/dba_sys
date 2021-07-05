#ifndef _NET_NETMAP_USER_H_
#define _NET_NETMAP_USER_H_

#define NETMAP_IF(b, o)	(struct netmap_if *)((char *)(b) + (o))

#define NETMAP_TXRING(nifp, index)			\
	((struct netmap_ring *)((char *)(nifp) +	\
		(nifp)->ring_ofs[index] ) )

#define NETMAP_RXRING(nifp, index)			\
	((struct netmap_ring *)((char *)(nifp) +	\
	    (nifp)->ring_ofs[index + (nifp)->ni_tx_rings + 1] ) )

#define NETMAP_BUF(ring, index)				\
	((char *)(ring) + (ring)->buf_ofs + ((index)*(ring)->nr_buf_size))

#define NETMAP_BUF_IDX(ring, buf)			\
	( ((char *)(buf) - ((char *)(ring) + (ring)->buf_ofs) ) / \
		(ring)->nr_buf_size ) 

#define	NETMAP_RING_NEXT(r, i)				\
	((i)+1 == (r)->num_slots ? 0 : (i) + 1 )

#define	NETMAP_RING_FIRST_RESERVED(r)			\
	( (r)->cur < (r)->reserved ?			\
	  (r)->cur + (r)->num_slots - (r)->reserved :	\
	  (r)->cur - (r)->reserved )

/*
 * Return 1 if the given tx ring is empty.
 */
#define NETMAP_TX_RING_EMPTY(r)	((r)->avail >= (r)->num_slots - 1)

#endif /* _NET_NETMAP_USER_H_ */
