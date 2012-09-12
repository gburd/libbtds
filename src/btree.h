/*-
 * btree
 *
 * This work, including the source code, documentation and related data, is
 * placed into the public domain by all authors.
 *
 * THIS SOFTWARE IS PROVIDED AS-IS WITHOUT WARRANTY OF ANY KIND, NOT EVEN THE
 * IMPLIED WARRANTY OF MERCHANTABILITY. THE AUTHOR OF THIS SOFTWARE, ASSUMES
 * _NO_ RESPONSIBILITY FOR ANY CONSEQUENCE RESULTING FROM THE USE,
 * MODIFICATION, OR REDISTRIBUTION OF THIS SOFTWARE.
 */


#ifndef BTREE_H

typedef unsigned int            uint;
typedef unsigned short          ushort;
typedef unsigned long long      off64_t;
typedef unsigned long long      uid;


#define BT_ro 0x6f72    /* ro */
#define BT_rw 0x7772    /* rw */
#define BT_fl 0x6c66    /* fl */

#define BT_maxbits              24                      /* maximum page size in bits */
#define BT_minbits              9                       /* minimum page size in bits */
#define BT_minpage              (1 << BT_minbits)       /* minimum page size */

#define BT_cache_sz             512                     /* size of hash index for page cache */
#define BT_hashprime            8191                    /* prime number for hashing */
#define BT_default_page_sz      4096

#define BT_clockpro_hashfactor   2
#define BT_clockpro_cold_pct     10
#define BT_clockpro_cold_pct_max 90

/*
 *  There are five lock types for each node in two independent sets:
 *   1. (set 1) AccessIntent: Sharable. Going to Read the node. Incompatible with NodeDelete.
 *   2. (set 1) NodeDelete: Exclusive. About to release the node. Incompatible with AccessIntent.
 *   3. (set 2) ReadLock: Sharable. Read the node. Incompatible with WriteLock.
 *   4. (set 2) WriteLock: Exclusive. Modify the node. Incompatible with ReadLock and other WriteLocks.
 *   5. (set 3) ParentModification: Exclusive. Change the parent's keys. Incompatible with another ParentModification.
 */

typedef enum {
  BtLockAccess,
  BtLockDelete,
  BtLockRead,
  BtLockWrite,
  BtLockParent
} BtLock;

/*      Define the length of the page and key pointers. */
#define BtId 6

/*      Page key slot definition.
 *
 *      If BT_maxbits is 15 or less, you can save 4 bytes for each key stored
 *      by making the first two uints into ushorts.  You can also save 4 bytes
 *      by removing the tod field from the key.
 *
 *      Keys are marked dead, but remain on the page until it cleanup is
 *      called. The fence key (highest key) for the page is always present,
 *      even after cleanup.
 */

typedef struct {
  uint off:BT_maxbits;    /* page offset for key start */
  uint dead:1;            /* set for deleted key */
  uint tod;               /* time-stamp for key */
  unsigned char id[BtId]; /* id associated with key */
} BtSlot;

/*
 *      The key structure occupies space at the upper end of each page.  It's a
 *      length byte followed by the value bytes.
 */

typedef struct {
  unsigned char len;
  unsigned char key[1];
} *BtKey;

/*
 *      The first part of an index page.  It is immediately followed by the
 *      BtSlot array of keys.
 */

typedef struct {
  uint cnt;                     // count of keys in page
  uint act;                     // count of active keys
  uint min;                     // next key offset
  unsigned char bits;           // page size in bits
  unsigned char lvl:7;          // level of page
  unsigned char kill:1;         // page is being deleted
  unsigned char right[BtId];    // page number to right
} *BtPage;

/*
 *      The memory mapping hash table entry
 */

typedef struct {
  BtPage page;          // mapped page pointer
  uid  page_no;         // mapped page number
  void *lruprev;        // least recently used previous cache block
  void *lrunext;        // lru next cache block
  void *hashprev;       // previous cache block for the same hash idx
  void *hashnext;       // next cache block for the same hash idx
} BtHash;

/*
 *      The object structure for Btree access
 */

typedef struct _BtDb {
  uint page_size;       // each page size
  uint page_bits;       // each page size in bits
  uid page_no;          // current page number
  uid cursor_page;      // current cursor page number
  int  err;
  uint mode;            // read-write mode
  uint mapped_io;       // use memory mapping
  BtPage temp;          // temporary frame buffer (memory mapped/file IO)
  BtPage alloc;         // frame buffer for alloc page ( page 0 )
  BtPage cursor;        // cached frame for start/next (never mapped)
  BtPage frame;         // spare frame for the page split (never mapped)
  BtPage page;          // current page
  int idx;
  unsigned char *mem;   // frame, cursor, page memory buffer
  int nodecnt;          // highest page cache node in use
  int nodemax;          // highest page cache node allocated
  int hashmask;         // number of hash headers in cache - 1
  BtHash *lrufirst;     // lru list head
  BtHash *lrulast;      // lru list tail
  ushort cache[BT_cache_sz]; // hash index for cache
  BtHash nodes[1];      // page cache follows
} BtDb;

typedef enum {
  BTERR_ok = 0,
  BTERR_struct,
  BTERR_ovflw,
  BTERR_lock,
  BTERR_map,
  BTERR_wrt,
  BTERR_hash
} BTERR;

/* B-Tree API Functions */
extern void bt_close(BtDb *bt);
extern BtDb *bt_open(char *name, uint mode, uint bits, uint cacheblk);
extern BTERR bt_insertkey(BtDb *bt, unsigned char *key, uint len, uint lvl, uid id, uint tod);
extern BTERR bt_deletekey(BtDb *bt, unsigned char *key, uint len, uint lvl);
extern uint bt_findkey(BtDb *bt, unsigned char *key, uint len);
extern uint bt_startkey(BtDb *bt, unsigned char *key, uint len);
extern uint bt_nextkey(BtDb *bt, uint slot);

/* Helper functions to return slot values */
extern BtKey bt_key (BtDb *bt, uint slot);
extern uid bt_uid (BtDb *bt, uint slot);
extern uint bt_tod (BtDb *bt, uint slot);

#endif // BTREE_H
