#ifndef _NET_NETMAP_H_
#define _NET_NETMAP_H_


struct netmap_slot {
	uint32_t buf_idx; /* buffer index */
	uint16_t len;	/* packet length, to be copied to/from the hw ring */
	uint16_t flags;	/* buf changed, etc. */
#define	NS_BUF_CHANGED	0x0001	/* must resync the map, buffer changed */
#define	NS_REPORT	0x0002	/* ask the hardware to report results
				 * e.g. by generating an interrupt
				 */
};


struct netmap_ring {
	/*
	 * nr_buf_base_ofs is meant to be used through macros.
	 * It contains the offset of the buffer region from this
	 * descriptor.
	 */
	const ssize_t	buf_ofs;
	const uint32_t	num_slots;	/* number of slots in the ring. */
	uint32_t	avail;		/* number of usable slots */
	uint32_t        cur;		/* 'current' r/w position */
	uint32_t	reserved;	/* not refilled before current */

	const uint16_t	nr_buf_size;
	uint16_t	flags;
#define	NR_TIMESTAMP	0x0002		/* set timestamp on *sync() */

	struct timeval	ts;		/* time of last *sync() */

	/* the slots follow. This struct has variable size */
	struct netmap_slot slot[0];	/* array of slots. */
};



struct netmap_if {
	char		ni_name[IFNAMSIZ]; /* name of the interface. */
	const u_int	ni_version;	/* API version, currently unused */
	const u_int	ni_rx_rings;	/* number of rx rings */
	const u_int	ni_tx_rings;	/* if zero, same as ni_rx_rings */
	/*
	 * The following array contains the offset of each netmap ring
	 * from this structure. The first ni_tx_queues+1 entries refer
	 * to the tx rings, the next ni_rx_queues+1 refer to the rx rings
	 * (the last entry in each block refers to the host stack rings).
	 * The area is filled up by the kernel on NIOCREG,
	 * and then only read by userspace code.
	 */
	const ssize_t	ring_ofs[0];
};

#ifndef NIOCREGIF	

struct nmreq {
	char		nr_name[IFNAMSIZ];
	uint32_t	nr_version;	/* API version */
#define	NETMAP_API	3		/* current version */
	uint32_t	nr_offset;	/* nifp offset in the shared region */
	uint32_t	nr_memsize;	/* size of the shared region */
	uint32_t	nr_tx_slots;	/* slots in tx rings */
	uint32_t	nr_rx_slots;	/* slots in rx rings */
	uint16_t	nr_tx_rings;	/* number of tx rings */
	uint16_t	nr_rx_rings;	/* number of rx rings */
	uint16_t	nr_ringid;	/* ring(s) we care about */
#define NETMAP_HW_RING	0x4000		/* low bits indicate one hw ring */
#define NETMAP_SW_RING	0x2000		/* process the sw ring */
#define NETMAP_NO_TX_POLL	0x1000	/* no automatic txsync on poll */
#define NETMAP_RING_MASK 0xfff		/* the ring number */
	uint16_t	spare1;
	uint32_t	spare2[4];
};


#define NIOCGINFO	_IOWR('i', 145, struct nmreq) /* return IF info */
#define NIOCREGIF	_IOWR('i', 146, struct nmreq) /* interface register */
#define NIOCUNREGIF	_IO('i', 147) /* interface unregister */
#define NIOCTXSYNC	_IO('i', 148) /* sync tx queues */
#define NIOCRXSYNC	_IO('i', 149) /* sync rx queues */
#endif /* !NIOCREGIF */

#endif /* _NET_NETMAP_H_ */
