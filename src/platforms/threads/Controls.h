
#ifndef   _PLATFORM_THREAD_CONTROLS_H_
#define   _PLATFORM_THREAD_CONTROLS_H_

#include <string>

#include "madara/threads/BaseThread.h"

namespace platforms
{
  namespace threads
  {
    /**
    * Provides controls feeedback for RISLab quadcopter platforms
    **/
    class Controls : public madara::threads::BaseThread
    {
    public:
      /**
       * Default constructor
       **/
      Controls ();
      
      /**
       * Destructor
       **/
      virtual ~Controls ();
      
      /**
        * Initializes thread with MADARA context
        * @param   context   context for querying current program state
        **/
      virtual void init (madara::knowledge::KnowledgeBase & knowledge);

      /**
        * Executes the main thread logic
        **/
      virtual void run (void);

    private:
      /// data plane if we want to access the knowledge base
      madara::knowledge::KnowledgeBase data_;
    };
  } // end namespace threads
} // end namespace platforms

#endif // _PLATFORM_THREAD_CONTROLS_H_
