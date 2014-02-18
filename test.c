#include <stdio.h>
#include "myhash.h"
#include "assert.h"

/***********************/
/* TODO:	       */
/* Tests	       */
/* Variable length key */
/*       	       */
/***********************/
    int 
    main(int           const argc, 
         const char ** const argv) {
		 
        struct entry_s *entry;
        struct database_s mydatabase;
	struct myhashStats_s stats;
	uint16_t currentIndex;

	int i;
	char key[255];


	// Create a database
        myhashDBInit(&mydatabase);

	// Put in some basic elements
        entry = myhashAllocateItem(&mydatabase, "bob");
	entry->data = "bob0";
	entry = myhashAllocateItem(&mydatabase, "dave");
        entry->data = "dave0";
        entry = myhashAllocateItem(&mydatabase, "bill");
        entry->data = "bill0";
        entry = myhashAllocateItem(&mydatabase, "fred");
        entry->data = "fred0";

	myhashPrintDatabase(&mydatabase);

        entry = myhashFindItemByKey(&mydatabase, "dave");

	if (entry) {
	  printf("Found Dave: %s:%s\n", entry->key,  (char *)entry->data);
	} else {
	  printf("No Dave!\n");
	}

        myhashDeallocateItemByKey(&mydatabase, "dave");

        entry = myhashFindItemByKey(&mydatabase, "dave");

	if (entry) {
	  printf("Found Dave: %s:%s\n", entry->key,  (char *)entry->data);
	} else {
	  printf("No Dave!\n");
	}


        entry = myhashFindItemByKey(&mydatabase, "bill");
        entry = myhashFindItemByKey(&mydatabase, "bob");
        entry = myhashFindItemByKey(&mydatabase, "fred");

	entry = myhashAllocateItem(&mydatabase, "bill");
        entry->data = "bill1";
        entry = myhashFindItemByKey(&mydatabase, "bill");

        myhashDeallocateItemByKey(&mydatabase, "bob");

        entry = myhashFindItemByKey(&mydatabase, "fred");
        entry = myhashFindItemByKey(&mydatabase, "dave");


	// Print all values
	while ((entry = myhashGetNextItem(&mydatabase, &currentIndex))) {
	  printf("%s:%s\n", entry->key,  (char *)entry->data);

	}
	myhashPrintDatabase(&mydatabase);

	myhashDBClear(&mydatabase);
	
	printf("CLeared: Size: %hu, Used: %hu\n", mydatabase.size, mydatabase.used);

	myhashDBDestroy(&mydatabase);
        myhashDBInit(&mydatabase);

	printf("Doing Bigfill and delete:\n");

	for (i = 0; i<60; i++) {
	  sprintf(key, "Paul%d", i);
	  entry = myhashAllocateItem(&mydatabase, key);

	  //assert(entry);	  

	  if(!entry) 	myhashPrintDatabase(&mydatabase);



	  if (!(i % 4)) {
	    myhashDeallocateItemByKey(&mydatabase, key);
	    printf("Deleted %s\n", key);
	  }

	  if (i % 512 == 0) {
	    printf(".", i);
	    fflush(stdout);
	  }
	}

	myhashGetStats(&mydatabase, &stats);
	myhashPrintDatabase(&mydatabase);

	printf("\n\nStatistics:\n");
	printf("Size: %hu. Filled: %hu (%.2g%%)\n",
	       mydatabase.size,
	       mydatabase.used,
	       (double) 100.*mydatabase.used/mydatabase.size);
	printf("Deleted: %hu\n", stats.numDeleted);
        printf("Max Run:    %hu\n", stats.maxRun);
	printf("Min Run:    %hu\n", stats.minRun);

	for (i = 0; i<65536; i++) {
	  sprintf(key, "Paul%d", i);
	  myhashDeallocateItemByKey(&mydatabase, key);
	  
	  if (i % 512 == 0) {
	    printf(".", i);
	    fflush(stdout);
	  }
	}

	myhashPrintDatabase(&mydatabase);
	
	printf("Done\n");
	fflush(stdout);

       return 0;
    }	  
