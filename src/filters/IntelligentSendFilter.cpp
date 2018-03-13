 
#include "IntelligentSendFilter.h"

filters::IntelligentSendFilter::IntelligentSendFilter ()
{
}

filters::IntelligentSendFilter::~IntelligentSendFilter ()
{
}

void
filters::IntelligentSendFilter::filter (
  madara::knowledge::KnowledgeMap & records,
  const madara::transport::TransportContext & transport_context,
  madara::knowledge::Variables & vars)
{
  // if we're using greater than 1MB/s, let's do something smarter
  if (transport_context.get_receive_bandwidth () > 1000000)
  {
    // iterate through and erase any variables that are binary blobs
    for (auto i = records.begin (); i != records.end (); )
    {
      // if binary, erase
      if (i->second.is_binary_file_type ())
      {
        records.erase (i++);
      }
      // else, keep it by continuing to next record
      else
      {
        ++i;
      }
    }
  }
}

