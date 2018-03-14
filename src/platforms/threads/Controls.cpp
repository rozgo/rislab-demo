
#include "gams/loggers/GlobalLogger.h"
#include "Controls.h"

namespace knowledge = madara::knowledge;

// constructor
platforms::threads::Controls::Controls ()
{
}

// destructor
platforms::threads::Controls::~Controls ()
{
}

/**
 * Initialization to a knowledge base. If you don't actually need access
 * to the knowledge base, just scheduling things in madara::threads::Threader,
 * then you can decide to delete this function or simply do nothing inside of
 * the function.
 **/
void
platforms::threads::Controls::init (knowledge::KnowledgeBase & knowledge)
{
  // point our data plane to the knowledge base initializing the thread
  data_ = knowledge;

  // initialize control_vars_;
  control_vars_.init (knowledge);

  std::vector <double> orientation = {72.0010, -82.1346, 100.2346};
  knowledge.set (".orientation", orientation);
}

/**
 * Executes the actual thread logic. Best practice is to simply do one loop
 * iteration. If you want a long running thread that executes something
 * frequently, see the madara::threads::Threader::runHz method in your
 * controller.
 **/
void
platforms::threads::Controls::run (void)
{
  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_MINOR,
    "platforms::threads::Controls::run:" 
    " reading from knowledge base\n");
  control_vars_.read ();

  /**
   * After this point, we can use our publicly accessible control_vars vars.
   * This access pattern is called read-compute-write, and it allows for
   * consistent access to the knowledge base. It is very difficult to have
   * race conditions while using this pattern. The state you read, is the
   * state you compute in. The state you write is guarded with the mutex
   * to the knowledge base and applied as a complete transaction of changes.
   **/

  ++control_vars_.controls_clock;

  /**
   * In the Hivemind: Edge to Analytics deck, the Controls thread state
   * machine is diagramed like this:
   * 1) Access IMU and PID values
   * 2) Access Position (control_vars_.position)
   * 3) Access Orientation (control_vars_.orientation)
   * 4) Project/predict Thrust (may require new variables in KB)
   * 5) Modify Thrust (usually involves actuating a subsystem)
   * 6) Update PID
   * 7) Update IMU
   * Exercise is to the think about how to implement this by accessing
   * platform subsystems or knowledge base variables to communicate
   * necessary information to other threads that depend on 
   **/

  if (control_vars_.orientation.size () == 3)
  {
    madara_logger_ptr_log (gams::loggers::global_logger.get (),
      gams::loggers::LOG_ALWAYS,
      "platforms::threads::Controls::run: %d: " 
      " orientation = [%.4f, %.4f, %.4f]\n",
        control_vars_.controls_clock,
        control_vars_.orientation[0],
        control_vars_.orientation[1],
        control_vars_.orientation[2]);
  }

  /**
   * After this point, we write what's in our local vars to the knowedge base,
   * even if there were no changes. This marks the value as modified to send
   * to others
   **/

  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_MINOR,
    "platforms::threads::Controls::run:" 
    " writing to knowledge base\n");
  control_vars_.write ();
}
