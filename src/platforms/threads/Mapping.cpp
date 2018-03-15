
#include "gams/loggers/GlobalLogger.h"
#include "Mapping.h"
#include "ace/High_Res_Timer.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

namespace knowledge = madara::knowledge;

// constructor
platforms::threads::Mapping::Mapping ()
{
}

// destructor
platforms::threads::Mapping::~Mapping ()
{
}

/**
 * Copies one buffer to another using the magic of Duff's Device
 * @param dest     destination buffer
 * @param source   source buffer
 * @param size     size in bytes
 **/
void duff_copy (void * dest, void * source, size_t size_bytes)
{
  int * dest_int ((int *)dest);
  int * source_int ((int *)source);

  int num_ints = (int) size_bytes / sizeof (int);
  int remainder_chars = (int) size_bytes % sizeof (int);
  int count = num_ints % 10;
  int num_iterations = (num_ints + 9) / 10;

  switch (count)
  {
    do {
    default:
    case 0:
      *dest_int++ = *source_int++;
    case 9:
      *dest_int++ = *source_int++;
    case 8:
      *dest_int++ = *source_int++;
    case 7:
      *dest_int++ = *source_int++;
    case 6:
      *dest_int++ = *source_int++;
    case 5:
      *dest_int++ = *source_int++;
    case 4:
      *dest_int++ = *source_int++;
    case 3:
      *dest_int++ = *source_int++;
    case 2:
      *dest_int++ = *source_int++;
    case 1:
      *dest_int++ = *source_int++;
    } while (--num_iterations > 0);
  }

  memcpy (dest_int, source_int, remainder_chars);
}

/**
 * Initialization to a knowledge base. If you don't actually need access
 * to the knowledge base, just scheduling things in madara::threads::Threader,
 * then you can decide to delete this function or simply do nothing inside of
 * the function.
 **/
void
platforms::threads::Mapping::init (knowledge::KnowledgeBase & knowledge)
{
  // point our data plane to the knowledge base initializing the thread
  data_ = knowledge;

  // reasonably random seed
  srand (time (NULL));
}

/**
 * Executes the actual thread logic. Best practice is to simply do one loop
 * iteration. If you want a long running thread that executes something
 * frequently, see the madara::threads::Threader::runHz method in your
 * controller.
 **/
void
platforms::threads::Mapping::run (void)
{
  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_ALWAYS,
    "platforms::threads::Mapping::run:" 
    " testing map creation and copying.\n");

  /**
   * 100x100m @ 5cm cells. 20 cells per meter. [100 x 20][100 x 20] = 2000x2000
   * 4MB if 1B per cell, but why? We need 256 values per cell? For what?
   **/
  const int BIG_ARRAY (2000*2000);
  const int NUM_INTS (BIG_ARRAY / sizeof (int));
  const int NUM_BITS (BIG_ARRAY / 8);
  const int NUM_INTS_IN_BIT_STRUCTURE (NUM_BITS / sizeof (int));
  const int ITERATIONS (10000);
  const int BITCELL_IN_KB (NUM_BITS / 1000);
  const int BYTECELL_IN_KB (BIG_ARRAY / 1000);

  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_ALWAYS,
    "platforms::threads::Mapping::run:" 
    " creating arrays of %d bytes and %d bytes\n",
    (int)BIG_ARRAY, (int)NUM_BITS);

  unsigned char * byte_map_source = (unsigned char *)malloc (BIG_ARRAY);

  /// fill the map with random data
  int * next = (int *)byte_map_source;

  for (int i = 0; i < NUM_INTS; ++i, ++next)
  {
    *next = rand ();
  }

  /**
   * 500KB. We can make this smaller, and I'm not sure why 5cm accuracy makes
   * sense, but this isn't my map. I just work here.
   **/
  unsigned char * bit_map_source = (unsigned char *)malloc (NUM_BITS);
  next = (int *)bit_map_source;

  for (int i = 0; i < NUM_INTS_IN_BIT_STRUCTURE; ++i, ++next)
  {
    *next = rand ();
  }

  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_ALWAYS,
    "platforms::threads::Mapping::run:" 
    " arrays of %d size and %d size have been created and initialized.\n",
    BIG_ARRAY, NUM_BITS);

  /**
   * Since timers on any operating system have a LOT of jitter, let's do these
   * copies thousands of times and time the operation, then divide by the
   * number of times we did the copy operation.
   **/


  // keep track of time
  ACE_hrtime_t malloc_memcpy_8bitcells_time, memcpy_8bitcells_time,
               malloc_memcpy_1bitcells_time, memcpy_1bitcells_time,
               duffcopy_8bitcells_time, duffcopy_1bitcells_time,
               dumb_copy_8bitcells_time;

  unsigned char * byte_map_dest (0);
  unsigned char * bit_map_dest (0);

  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_ALWAYS,
    "platforms::threads::Mapping::run:" 
    " running Test 1: Full malloc, memcpy %d KB, free. 10k iterations\n",
    BYTECELL_IN_KB);

  /**
   * Test 1: Full malloc, then memcpy of 4M bytes, and free 10000 times
   **/

  {
    ACE_High_Res_Timer timer;
    timer.start ();
    for (int i = 0; i < ITERATIONS; ++i)
    {
      byte_map_dest = (unsigned char *)malloc (BIG_ARRAY);
      memcpy (byte_map_dest, byte_map_source, BIG_ARRAY);
      free (byte_map_dest);
    }
    timer.stop ();
    timer.elapsed_time (malloc_memcpy_8bitcells_time);
  }

  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_ALWAYS,
    "platforms::threads::Mapping::run:" 
    " running Test 2: memcpy %d KB. 10k iterations\n",
    BYTECELL_IN_KB);

  /**
   * Test 2: memcpy of 4M bytes, 10000 times
   **/

  {
    ACE_High_Res_Timer timer;
    timer.start ();
    for (int i = 0; i < ITERATIONS; ++i)
    {
      memcpy (byte_map_dest, byte_map_source, BIG_ARRAY);
    }
    timer.stop ();
    timer.elapsed_time (memcpy_8bitcells_time);
  }

  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_ALWAYS,
    "platforms::threads::Mapping::run:" 
    " running Test 3: %d KB malloc, memcpy %d KB, free. 10k iterations\n",
    BITCELL_IN_KB, BITCELL_IN_KB);

  /**
   * Test 3: Full malloc, then memcpy of 500K bytes, and free 10000 times
   **/

  {
    ACE_High_Res_Timer timer;
    timer.start ();
    for (int i = 0; i < ITERATIONS; ++i)
    {
      bit_map_dest = (unsigned char *) malloc (NUM_BITS);
      memcpy (bit_map_dest, bit_map_source, NUM_BITS);
      free (bit_map_dest);
    }
    timer.stop ();
    timer.elapsed_time (malloc_memcpy_1bitcells_time);
  }

  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_ALWAYS,
    "platforms::threads::Mapping::run:" 
    " running Test 4: memcpy %d KB. 10k iterations\n",
    BITCELL_IN_KB);

  /**
   * Test 4: memcpy of 500K bytes, 10000 times
   **/

  {
    ACE_High_Res_Timer timer;
    timer.start ();
    for (int i = 0; i < ITERATIONS; ++i)
    {
      memcpy (bit_map_dest, bit_map_source, NUM_BITS);
    }
    timer.stop ();
    timer.elapsed_time (memcpy_1bitcells_time);
  }

  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_ALWAYS,
    "platforms::threads::Mapping::run:" 
    " running Test 5: duffcopy %d KB. 10k iterations\n",
    BYTECELL_IN_KB);

  /**
   * Test 5: Duff's Device of 4M bytes, 10000 times
   **/

  {
    ACE_High_Res_Timer timer;
    timer.start ();
    for (int i = 0; i < ITERATIONS; ++i)
    {
      duff_copy (byte_map_dest, byte_map_source, BIG_ARRAY);
    }
    timer.stop ();
    timer.elapsed_time (duffcopy_8bitcells_time);
  }

  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_ALWAYS,
    "platforms::threads::Mapping::run:" 
    " running Test 6: duffcopy %d KB. 10k iterations\n",
    BITCELL_IN_KB);

  /**
   * Test 6: Duff's Device of 500K bytes, 10000 times
   **/

  {
    ACE_High_Res_Timer timer;
    timer.start ();
    for (int i = 0; i < ITERATIONS; ++i)
    {
      duff_copy (bit_map_dest, bit_map_source, NUM_BITS);
    }
    timer.stop ();
    timer.elapsed_time (duffcopy_1bitcells_time);
  }

  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_ALWAYS,
    "platforms::threads::Mapping::run:" 
    " running Test 7: Dumb copy %d KB. 10k iterations\n",
    BYTECELL_IN_KB);

  /**
   * Test 7: Byte-by-byte copy of 4MB bytes, 10000 times
   **/

  {
    ACE_High_Res_Timer timer;
    timer.start ();
    for (int i = 0; i < ITERATIONS; ++i)
    {
      duff_copy (bit_map_dest, bit_map_source, NUM_BITS);
      for (int j = 0; j < BIG_ARRAY; ++j)
      {
        byte_map_dest[j] = byte_map_source[j];
      }
    }
    timer.stop ();
    timer.elapsed_time (dumb_copy_8bitcells_time);
  }


  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_ALWAYS,
    "platforms::threads::Mapping::run:" 
    " Results (4MB 8bit map, 500KB 1bit map, 10k iterations):\n"
    "  Test 1: malloc_memcpy_8bitcells: total (%u time, %u avg).\n"
    "  Test 2: memcpy_8bitcells: total (%u time, %u avg).\n"
    "  Test 3: malloc_memcpy_1bitcells: total (%u time, %u avg).\n"
    "  Test 4: memcpy_1bitcells: total (%u time, %u avg).\n"
    "  Test 5: duffcopy_8bitcells: total (%u time, %u avg).\n"
    "  Test 6: duffcopy_1bitcells: total (%u time, %u avg).\n"
    "  Test 7: dumbcopy_8bitcells: total (%u time, %u avg).\n"
    ,
    (unsigned int)malloc_memcpy_8bitcells_time,
    (unsigned int)malloc_memcpy_8bitcells_time / ITERATIONS,
    (unsigned int)memcpy_8bitcells_time,
    (unsigned int)memcpy_8bitcells_time / ITERATIONS,
    (unsigned int)malloc_memcpy_1bitcells_time,
    (unsigned int)malloc_memcpy_1bitcells_time / ITERATIONS,
    (unsigned int)memcpy_1bitcells_time,
    (unsigned int)memcpy_1bitcells_time / ITERATIONS,
    (unsigned int)duffcopy_8bitcells_time,
    (unsigned int)duffcopy_8bitcells_time / ITERATIONS,
    (unsigned int)duffcopy_1bitcells_time,
    (unsigned int)duffcopy_1bitcells_time / ITERATIONS,
    (unsigned int)dumb_copy_8bitcells_time,
    (unsigned int)dumb_copy_8bitcells_time / ITERATIONS
  );

  /**
   * But wait... No way. Does this crazier version of Duff's device work?
   **/

  char * source_buffer =
"James. Seriously. There's no way this works."
" You should be fired. I should take your job."
" Your skills in C++ are subpar. My three-year-old"
" cousin could write in circles around you. Duff's"
" Device? More like Your-Face-Is-Dumb Device. This"
" isn't even the code he submitted to an obfuscated "
" contest. Why are you even wasting all of our time?"
" Could someone please get him away from a keyboard"
" or computer for the rest of time? I want to talk"
" about the impact of map copying and how there needs"
" to be zero-copy. Come on! Some people. Nuts.";

  char * dest_buffer = (char *) malloc (535);

  duff_copy (dest_buffer, source_buffer, 535);

  dest_buffer[534] = 0;

  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_ALWAYS,
    "platforms::threads::Mapping::run:" 
    " duff copy test for the doubters:\n%s\n",
    dest_buffer);
  

}
