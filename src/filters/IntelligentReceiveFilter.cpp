 
#include "IntelligentReceiveFilter.h"

filters::IntelligentReceiveFilter::IntelligentReceiveFilter ()
{
  // note that we could have provided parameters for domain and bandwidth
}

filters::IntelligentReceiveFilter::~IntelligentReceiveFilter ()
{
}

void
filters::IntelligentReceiveFilter::filter (
  madara::knowledge::KnowledgeMap & records,
  const madara::transport::TransportContext & transport_context,
  madara::knowledge::Variables & vars)
{
  // assume we need to drop everything
  bool drop_it_all = true;

  // domain is similar to pub/sub topics but doesn't indicate type of data
  if (transport_context.get_domain () == "Mosul Mission")
  {
    // check that the total perceived bandwidth usage is less than 1MB/s
    if (transport_context.get_receive_bandwidth () < 1000000)
    {
      drop_it_all = false;
    }
  }
  
  // are we supposed to drop it all?
  if (drop_it_all)
  {
    records.clear ();
  }
}

