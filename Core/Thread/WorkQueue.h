/*
 * This file was automatically generated by SCC - do NOT edit!
 * You should edit WorkQueue.scc instead 
 */

#ifndef SCI_THREAD_WORKQUEUE_H
#define SCI_THREAD_WORKQUEUE_H 1

/*
 * Doles out work assignment to various worker threads.  Simple
 * attempts are made at evenly distributing the workload.
 * Initially, assignments are relatively large, and will get smaller
 * towards the end in an effort to equalize the total effort.
 */

#include "Mutex.h"
#include "ConditionVariable.h"
struct WorkQueue_private;

/**************************************
 
CLASS
   WorkQueue
   
KEYWORDS
   WorkQueue
   
DESCRIPTION
   Doles out work assignment to various worker threads.  Simple
   attempts are made at evenly distributing the workload.
   Initially, assignments are relatively large, and will get smaller
   towards the end in an effort to equalize the total effort.
   
PATTERNS


WARNING
   
****************************************/
class SCICORESHARE WorkQueue {
    WorkQueue_private* priv;
    int* assignments;
    int nallocated;
    int nassignments;

    int nthreads;
    int totalAssignments;
    int granularity;
    int nwaiting;
    bool done;
    bool dynamic;
    const char* name;

    //////////
    //<i>No documentation provided</i>
    void init() ;
public:

    //////////
    //Create the work queue with the specified total number of work assignments.  <i>nthreads</i>
    //specifies the approximate number of threads which will be working from this queue.  The 
    //optional <i>granularity</i> specifies the degree to which the tasks are divided.  A large
    //granularity will create more assignments with smaller assignments.  A granularity of zero will
    //recieve a single assignment of approximately uniform size.  <i>name</i> should be a static
    //string which describes the primitive for debugging purposes.
    WorkQueue(const char* name, int totalAssignments, int nthreads,
	      bool dynamic, int granularity=5)
     ;
    void refill(int totalAssignments, int nthreads,
	      bool dynamic, int granularity=5);

    //////////
    //Copy a work queue.  This only compies the total number of assignments, and not the status
    //of completed assignments
    WorkQueue(const WorkQueue& copy) ;

    //////////
    //Make an empty work queue with no assignments.
    WorkQueue() ;

    //////////
    //Copy a work queue.  This only copies the total number of assignments, and not the status of
    //completed assignments.  This is NOT threadsafe, and should be synchronized carefully.
    WorkQueue& operator=(const WorkQueue& copy) ;

    //////////
    //Destroy the work queue.  Any unassigned work will be lost.  
    ~WorkQueue() ;

    //////////
    //Called by each thread to get the next assignment.  If <i>nextAssignment</i> returns true,
    //the thread has a valid assignment, and then would be responsible for the work from the 
    //returned <i>start</i> through <i>end-l</i>.  Assignments can range from 0 to 
    //<i>totalAssignments</i>-1.  When <i>nextAssignment</i> returns false, all work has been
    //completed (dynamic=true), or has been assigned (dynamic=false).
    bool nextAssignment(int& start, int& end) ;

    //////////
    //Increase the work to be done.  <i>dynamic</i> as provided to the constructor MUST be true.
    //Work should only be added by the workers.
    void addWork(int nassignments) ;
    void waitForEmpty();
};

#endif
