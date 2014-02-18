#ifndef __MYHASH_H__
#define __MYHASH_H__ 1

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define HASHTABLE_TYPE uint16_t

#define DEFAULT_SIZE 40
#define DEFAULT_KEY  32

enum entry_state {
  ENTRY_EMPTY,
  ENTRY_USED,
  ENTRY_DELETED
};

struct entry_s {
  enum entry_state state : 2;
  void *data;
  char key[DEFAULT_KEY];
};

struct database_s {
  HASHTABLE_TYPE size;
  HASHTABLE_TYPE used;
  struct entry_s *data;
};

struct myhashStats_s {
    uint16_t numDeleted;
    uint16_t maxRun;
    uint16_t minRun;
};

/* Function Prototypes */
/* =================== */

/**
*******************************************************************************
\brief		    Create a new hashtable
\param db	    Hashtable (\e input)
\return		    0 on Succes and !0 on Error

Create a new hashtable.
Using the default array length.
The table will automatically grow and shrink as required to store any number of table
entries.
*******************************************************************************
*/
int myhashDBInit(struct database_s *db);

/* /\** */
/* ******************************************************************************* */
/* \brief		      Create a new hashtable */
/* \param db	      Hashtable (\e input) */
/* \param numBuckets How many hash buckets should be created (\e input) */
/* \param keyLen	  How long is the default key (\e input) */
/* \return		      0 on Succes and !0 on Error  */

/* Create a new hashtable with optional flags. */
/* The value of numBuckets specifies the initial set of buckets to be allocated. */
/* The value of keyLen specifies the maximum key length, if it is set to 0 the */
/* key will be allocated automatically. */
/* ******************************************************************************* */
/* *\/ */
/* int myhashDBInitEX(struct database_s *db, int numBuckets, int keyLen, int flags); */

/**
*******************************************************************************
\brief		    Insert a new or update an old item in the hashtable.
\param db	    Hashtable (\e input)
\param key	    The key for the item we want to insert/update (\e input)
\return		    The item that is to be updated/inserted or NULL on error.

Insert a new or update an old item in the hashtable.
If the item already exists the field 'data' of the returned item will NOT be NULL.
If the item does not exist in the field 'data' of the returned item will be NULL.
NULL will be returned on an error.
After calling this function the users should update the 'data' field in the returned
entry to the value they want associated with the key.
*******************************************************************************
*/
struct entry_s *myhashAllocateItem(struct database_s *db, char *key);

/**
*******************************************************************************
\brief		    Remove an existing item in the hashtable.
\param db	    Hashtable (\e input)
\param key	    The key for the item we want to remove (\e input)
\param data	    Hashtable (\e input, output)

Remove an existing item in the hashtable.
Resources allocated to this item will be freed, it is the users reponsibility
to free their 'data', the address of the users 'data' is returned in 'data'
*******************************************************************************
*/
void myhashDeallocateItemByKey(struct database_s *db, char *key);

/**
*******************************************************************************
\brief		    Find an item in hashtable
\param db	    Hashtable (\e input)
\param key	    The key for the item we want to locate (\e input)
\return		    An item holding the key and data.

Find an item in a hashtable based on its key.
*******************************************************************************
*/
struct entry_s *myhashFindItemByKey(struct database_s *db, char *key);

/**
*******************************************************************************
\brief		    Get the next item in the hashtable
\param db	    Hashtable (\e input)
\param index        Location of current index (\e input)
\return		    The next item or NULL to indicate the end of the list.

Get the next item in a hashtable. Pass in an index of 0 to get the first item.
Pass in the same variable to get each subsequent item. When there are no 
more items left to iterate, NULL is returned.
*******************************************************************************
*/
struct entry_s *myhashGetNextItem(struct database_s *db, uint16_t *index);


/**
*******************************************************************************
\brief		    Print all items in the hashtable
\param db	    Hashtable (\e input)

Print all items present (or deleted) in the hashtable. 
*******************************************************************************
*/
void myhashPrintDatabase(struct database_s *db);

/**
*******************************************************************************
\brief		    Remove all hashtable entries
\param db	    Hashtable (\e input)

Remove all hashtable entries.
*******************************************************************************
*/
void myhashDBClear(struct database_s *db);

/**
*******************************************************************************
\brief		    Destroy a previously created hashtable
\param db	    Hashtable (\e input)

Destroy a previously created hashtable, freeing any resources being used.
*******************************************************************************
*/
void myhashDBDestroy(struct database_s *db);

/**
*******************************************************************************
\brief		    Calculate hashtable statistics
\param db	    Hashtable (\e input, output)
\param stats	statistics (\e input, output)

Generate a set of statistics for the given table.
*******************************************************************************
*/
void myhashGetStats(struct database_s *db, struct myhashStats_s *stats);


/* /\* */
/*   Example usage: */

/*   NOTE - if KEY is less than MAX_KEY then it MUST be '\0'  */
/*   terminated. */
	   
/*     int  */
/*     main(int           const argc,  */
/*          const char ** const argv) { */
		 
/*         struct entry_s *entry; */
/*         struct database_s mydatabase;	  */

/*         myhashDBInit(&mydatabase); */

/*         entry = myhashAllocateItem(&mydatabase, "bob"); */
/*         entry->data = "bob 1234"; */
/*         entry = myhashAllocateItem(&mydatabase, "dave"); */
/*         entry->data = "dave 1234"; */
/*         entry = myhashAllocateItem(&mydatabase, "bill"); */
/*         entry->data = "bill 1234"; */
/*         entry = myhashAllocateItem(&mydatabase, "fred"); */
/*         entry->data = "fred 1234"; */

/*         entry = myhashFindItemByKey(&mydatabase, "dave"); */
/*         entry = myhashFindItemByKey(&mydatabase, "bill"); */
/*         entry = myhashFindItemByKey(&mydatabase, "bob"); */
/*         entry = myhashFindItemByKey(&mydatabase, "fred"); */

/*         myhashDeallocateItemByKey(&mydatabase, "bill"); */

/*         entry = myhashAllocateItem(&mydatabase, "bill"); */
/*         entry->data = "bill 3423"; */
/*         entry = myhashFindItemByKey(&mydatabase, "bill"); */

/*         myhashDeallocateItemByKey(&mydatabase, "bob"); */

/*         entry = myhashFindItemByKey(&mydatabase, "fred"); */
/*         entry = myhashFindItemByKey(&mydatabase, "dave"); */

/*         entry = NULL; */
/*         while (entry = myhashGetNextItem(&mydatabase, entry)) { */
/*             printf("%s:%s\n", entry->key, (char *)entry->data); */
/*         } */

/*         return 0; */
/*     }	   */
/* *\/ */


#ifdef __cplusplus
}
#endif

#endif
