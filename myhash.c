#include "myhash.h"
#include "assert.h"

#include "stdio.h"

static uint16_t genStrHash(struct database_s *db, char *key);
static struct entry_s *
I_myhashFindItemByKeyOrEmpty(struct database_s *db, char *key, struct entry_s *deleted);
static void I_myhashResizeArray(struct database_s *db, HASHTABLE_TYPE newsize);


/* -----------------------------------------------------------------------------------
 * ----------------------------------------------------------------------------------- */

static uint16_t
genStrHash(struct database_s *db, char *key) {

    /* Jenkins's one-at-a-time hash is
     * http://en.wikipedia.org/wiki/Jenkins_hash_function
     */

  uint16_t keyLen = DEFAULT_KEY;
  uint32_t hash;
  uint16_t i;

  hash = 0;
  for(i=0; i<keyLen; i++) {
    
    if (key[i] == '\0') break;
    
    hash += key[i];
    hash += (hash << 10);
    hash ^= (hash >> 6);
  }
  
  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);
  
  return (uint16_t)(hash % db->size);
}

int myhashDBInit(struct database_s *db) {
  assert(db); // Database passed in

  db->size = DEFAULT_SIZE;
  db->used = 0;

  db->data = calloc(db->size, sizeof(struct entry_s));

  if (!db->data) {
    return -1;
  }

  memset(db->data, 0, db->size);

  assert(db->data); // Database has data
  
  return 0;
}

void myhashDBDestroy(struct database_s *db) {
  assert(db);

  db->size = 0;
  db->used = 0;
  free(db->data);
  db->data = NULL;

  assert(!db->data);
}

struct entry_s *myhashAllocateItem(struct database_s *db, char *key) {
  struct entry_s* entry;
  struct entry_s *deletedEntry = NULL;

  assert(db); // Database passed in
  assert(key); // Key passed in

  entry = I_myhashFindItemByKeyOrEmpty(db, key, deletedEntry);

  if (entry) {
    if (!entry->state) {
      // Found an empty one, can we find a deleted one?
      if (deletedEntry)
	entry = deletedEntry;

      entry->state = ENTRY_USED;
      strncpy(entry->key, key, DEFAULT_KEY);
      db->used++;

      // Check if database needs to grow?
      if (db->used > (4*db->size/5)) {
	if (db->size < (UINT16_MAX/2)) {
	  I_myhashResizeArray(db, 2*db->size);
	} else {
	  I_myhashResizeArray(db, UINT16_MAX);
	}
      }
    }

    return entry;
  }

  return NULL;
}

struct entry_s *myhashFindItemByKey(struct database_s *db, char *key) {
  struct entry_s* entry;

  assert(db);
  assert(key);

  entry = I_myhashFindItemByKeyOrEmpty(db, key, NULL);
  if (entry && entry->state) {
    return entry;
  }

  return NULL;
}

static struct entry_s*
I_myhashFindItemByKeyOrEmpty(struct database_s *db, char *key, struct entry_s *deleted) {
  HASHTABLE_TYPE i, index;
  struct entry_s *entry, *entryLast;
 
  assert(db);
  assert(key);

  if (deleted) deleted = NULL;

  index = genStrHash(db, key);
  entry = &db->data[index];
  entryLast = &db->data[db->size];

  for (i = 0; i < db->size; i++) {
    if (entry->state) {
      if (entry->state && strncmp(entry->key, key, DEFAULT_KEY) == 0) {
	return entry;
      } else if (!deleted) {
	deleted = entry;
      }

      entry++;
      if (entry == entryLast) entry = &db->data[0];
    } else {
      return entry;
    }
  }
  
  return NULL; 
}

void myhashDeallocateItemByKey(struct database_s *db, char* key) {
  struct entry_s *entry;

  assert(db);
  assert(key);

  entry = I_myhashFindItemByKeyOrEmpty(db, key, NULL);

  if (entry && entry->state) {
    assert(!strncmp(entry->key, key, DEFAULT_KEY));

    entry->state = ENTRY_DELETED;
    memset(entry->key, 0, DEFAULT_KEY);
    entry->data = NULL;
    //db->used--;

    if (db->used < db->size/10) I_myhashResizeArray(db, db->size/2);
  }
}

void myhashDBClear(struct database_s *db) {
  HASHTABLE_TYPE index = 0;
  struct entry_s *entry;

  while ((entry = myhashGetNextItem(db, &index))) {
    myhashDeallocateItemByKey(db, entry->key);
  }
}

void I_myhashResizeArray(struct database_s *db, HASHTABLE_TYPE newsize) {

  HASHTABLE_TYPE index = 0;
  struct database_s newdb;
  struct entry_s *entry;
  struct entry_s *new_entry;

  assert(db);

  newdb.size = newsize;
  
  if (newdb.size > UINT16_MAX) newdb.size = UINT16_MAX;
  if (newdb.size < DEFAULT_SIZE) newdb.size = DEFAULT_SIZE;

  if (db->size == newdb.size) return;

  printf("Resizing from %hu to %hu\n", db->size, newdb.size);

  newdb.used = 0;
  newdb.data = calloc(newdb.size, sizeof(struct entry_s));

  if (!newdb.data) {
    goto exit;
  }

  memset(newdb.data, 0, newdb.size);

  while ((entry = myhashGetNextItem(db, &index))) {
    new_entry = myhashAllocateItem(&newdb, entry->key);
    if(!new_entry) goto exit;
    new_entry->data = entry->data;
  }

  db->size = newdb.size;
  db->used = newdb.used;

  free(db->data);
  db->data = newdb.data;
  newdb.data = NULL;
  
 exit:
  if (newdb.data) {
    free(newdb.data);
  } 
}

struct entry_s *myhashGetNextItem(struct database_s *db, uint16_t *index) {
  struct entry_s *entry, *entryLast;

  assert(db);
  assert(index);

  if (*index > db->size) return NULL;

  entry = &db->data[*index];
  entryLast = &db->data[db->size];

  while (entry != entryLast) {
    (*index)++;
    entry++;
    if (entry->state == ENTRY_USED) return entry;
  }

  return NULL;
}

  
void myhashPrintDatabase(struct database_s *db) {
  struct entry_s *entry, *entryLast;
  HASHTABLE_TYPE i = 0;

  assert(db);

  entry = &db->data[0];
  entryLast = &db->data[db->size];

  printf("Size: %hu. Filled: %hu (%.2g%%)\n", db->size, db->used, (double) 100.*db->used/db->size);

  do {
    if (entry->state) {
      switch (entry->state) {
      case ENTRY_USED:
	printf("%3hu: %s : %s\n", i, entry->key, (char *)entry->data);
      break;
      case ENTRY_DELETED:
	printf("%3hu: Deleted\n", i);
	break;
      default:
	assert("Undefined hashtable entry state");
      }
      }
    i++;
  } while (entry++ != entryLast);

}

void myhashGetStats(struct database_s *db, struct myhashStats_s *stats) {
  struct entry_s *entry, *entryLast;
  HASHTABLE_TYPE run = 0;

  assert(db);
  assert(stats);

  stats->numDeleted = 0;
  stats->maxRun = 0;
  stats->minRun = db->size;

  entry = &db->data[0];
  entryLast = &db->data[db->size];

  do {
    if (entry->state) {
      run++;

      if (entry->state == ENTRY_DELETED) stats->numDeleted++;

    } else if (run) {
      if (run > stats->maxRun) stats->maxRun = run;
      if (run < stats->minRun) stats->minRun = run;
      run = 0;
    }
  } while (entry++ != entryLast);

  if (run > stats->maxRun) stats->maxRun = run;
  if (run < stats->minRun) stats->minRun = run;
}
