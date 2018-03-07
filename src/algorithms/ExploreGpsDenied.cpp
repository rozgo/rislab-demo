 
#include "ExploreGpsDenied.h"

#include <iostream>

gams::algorithms::BaseAlgorithm *
algorithms::ExploreGpsDeniedFactory::create (
  const madara::knowledge::KnowledgeMap & /*args*/,
  madara::knowledge::KnowledgeBase * knowledge,
  gams::platforms::BasePlatform * platform,
  gams::variables::Sensors * sensors,
  gams::variables::Self * self,
  gams::variables::Agents * agents)
{
  gams::algorithms::BaseAlgorithm * result (0);
  
  if (knowledge && sensors && platform && self)
  {
    result = new ExploreGpsDenied (knowledge, platform, sensors, self);
  }
  else
  {
    madara_logger_ptr_log (gams::loggers::global_logger.get (),
      gams::loggers::LOG_MAJOR,
      "algorithms::ExploreGpsDeniedFactory::create:" 
      " failed to create due to invalid pointers. " 
      " knowledge=%p, sensors=%p, platform=%p, self=%p, agents=%p\n",
      knowledge, sensors, platform, self, agents);
  }

  /**
   * Note the usage of logger macros with the GAMS global logger. This
   * is highly optimized and is just an integer check if the log level is
   * not high enough to print the message
   **/
  if (result == 0)
  {
    madara_logger_ptr_log (gams::loggers::global_logger.get (),
      gams::loggers::LOG_ERROR,
      "algorithms::ExploreGpsDeniedFactory::create:" 
      " unknown error creating ExploreGpsDenied algorithm\n");
  }
  else
  {
    madara_logger_ptr_log (gams::loggers::global_logger.get (),
      gams::loggers::LOG_MAJOR,
      "algorithms::ExploreGpsDeniedFactory::create:" 
      " successfully created ExploreGpsDenied algorithm\n");
  }

  return result;
}

algorithms::ExploreGpsDenied::ExploreGpsDenied (
  madara::knowledge::KnowledgeBase * knowledge,
  gams::platforms::BasePlatform * platform,
  gams::variables::Sensors * sensors,
  gams::variables::Self * self,
  gams::variables::Agents * agents)
  : gams::algorithms::BaseAlgorithm (knowledge, platform, sensors, self, agents)
{
  status_.init_vars (*knowledge, "ExploreGpsDenied", self->agent.prefix);
  status_.init_variable_values ();
}

algorithms::ExploreGpsDenied::~ExploreGpsDenied ()
{
}

int
algorithms::ExploreGpsDenied::analyze (void)
{
  return 0;
}
      

int
algorithms::ExploreGpsDenied::execute (void)
{
  return 0;
}


int
algorithms::ExploreGpsDenied::plan (void)
{
  return 0;
}
