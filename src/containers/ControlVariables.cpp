
#include "ControlVariables.h"

#include "madara/knowledge/ContextGuard.h"

containers::ControlVariables::ControlVariables ()
: context_ (0)
{
}

containers::ControlVariables::ControlVariables (
  madara::knowledge::KnowledgeBase & knowledge)
{
  init (knowledge);
}

void
containers::ControlVariables::init (madara::knowledge::KnowledgeBase & knowledge)
{
  // hold context for use in guards later
  context_ = &knowledge.get_context ();

  imu_sigma_accel_.set_name (".imu.sigma.accel", knowledge);
  orientation_.set_name (".orientation", knowledge);
  position_.set_name (".position", knowledge);
}

void
containers::ControlVariables::read (void)
{
  if (context_)
  {
    // lock the context for consistency
    madara::knowledge::ContextGuard guard (*context_);

    // update user-facing variables
    imu_sigma_accel = imu_sigma_accel_.to_record ().to_doubles ();
    orientation = orientation_.to_record ().to_doubles ();
    position = position_.to_record ().to_doubles ();
  }
}

void
containers::ControlVariables::write (void)
{
  if (context_)
  {
    // lock the context for consistency
    madara::knowledge::ContextGuard guard (*context_);

    // update knowledge base
    imu_sigma_accel_.set (imu_sigma_accel);
    orientation_.set (orientation);
    position_.set (position);
  }
}

void
containers::ControlVariables::modify (void)
{
  // mark containers as modified so the values are resent
  imu_sigma_accel_.modify ();
  orientation_.modify ();
  position_.modify ();
}

