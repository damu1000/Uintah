
/*
 *  ReplyEP.h:  A set of prebuilt endpoints for replies
 *
 *  Written by:
 *   Steven G. Parker
 *   Department of Computer Science
 *   University of Utah
 *   July 1999
 *
 *  Copyright (C) 1999 SCI Group
 */

#include <Core/CCA/Component/PIDL/ReplyEP.h>
#include <Core/CCA/Component/PIDL/GlobusError.h>
#include <Core/Thread/Mutex.h>
#include <Core/Util/NotFinished.h>
#include <globus_nexus.h>
#include <vector>

using PIDL::ReplyEP;
using namespace SCIRun;

static Mutex mutex("ReplyEP pool lock");
static std::vector<ReplyEP*> pool;

void ReplyEP::reply_handler(globus_nexus_endpoint_t* ep,
			    globus_nexus_buffer_t* buffer,
			    globus_bool_t)
{
    globus_nexus_buffer_save(buffer);
    void* p=globus_nexus_endpoint_get_user_pointer(ep);
    ReplyEP* r=(ReplyEP*)p;
    r->d_bufferHandoff=*buffer;
    r->d_sema.up();
}

static globus_nexus_handler_t reply_table[] =
{
    {GLOBUS_NEXUS_HANDLER_TYPE_NON_THREADED, ReplyEP::reply_handler}
};

static void unknown_handler(globus_nexus_endpoint_t* endpoint,
			    globus_nexus_buffer_t* buffer,
			    int handler_id)
{
    NOT_FINISHED("unknown handler");
}

ReplyEP::ReplyEP()
    : d_sema("Reply wait semaphore", 0)
{
    globus_nexus_endpointattr_t attr;
    if(int gerr=globus_nexus_endpointattr_init(&attr))
	throw GlobusError("endpointattr_init", gerr);
    if(int gerr=globus_nexus_endpointattr_set_handler_table(&attr,
							    reply_table,
							    1))
	throw GlobusError("endpointattr_set_handler_table", gerr);
    if(int gerr=globus_nexus_endpointattr_set_unknown_handler(&attr,
							      unknown_handler,
							      GLOBUS_NEXUS_HANDLER_TYPE_THREADED))
	throw GlobusError("endpointattr_set_unknown_handler", gerr);
    if(int gerr=globus_nexus_endpoint_init(&d_ep, &attr))
	throw GlobusError("endpoint_init", gerr);
    globus_nexus_endpoint_set_user_pointer(&d_ep, this);
    if(int gerr=globus_nexus_endpointattr_destroy(&attr))
	throw GlobusError("endpointattr_destroy", gerr);

    if(int gerr=globus_nexus_startpoint_bind(&d_sp, &d_ep))
	throw GlobusError("bind_startpoint", gerr);
}

ReplyEP::~ReplyEP()
{
    // TODO: free endpoint...
}

ReplyEP* ReplyEP::acquire()
{
    mutex.lock();
    if(pool.size() == 0){
	mutex.unlock();
	ReplyEP* r=new ReplyEP;
	return r;
    } else {
	ReplyEP* r=pool.back();
	pool.pop_back();
	mutex.unlock();
	return r;
    }
}

void ReplyEP::release(ReplyEP* r)
{
    // TODO - limit the number of free ReplyEP's
    mutex.lock();
    pool.push_back(r);
    mutex.unlock();
}

void ReplyEP::get_startpoint_copy(globus_nexus_startpoint_t* spp)
{
    if(int gerr=globus_nexus_startpoint_copy(spp, &d_sp))
	throw GlobusError("startpoint_copy", gerr);
}

globus_nexus_buffer_t ReplyEP::wait()
{
    d_sema.down();
    return d_bufferHandoff;
}

