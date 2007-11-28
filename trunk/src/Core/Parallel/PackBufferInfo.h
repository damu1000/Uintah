
#ifndef UINTAH_HOMEBREW_PackBufferInfo_H
#define UINTAH_HOMEBREW_PackBufferInfo_H

#include <sci_defs/mpi_defs.h> // For MPIPP_H on SGI
#include <mpi.h>
#include <sgi_stl_warnings_off.h>
#include <vector>
#include <sgi_stl_warnings_on.h>
#include <Core/Parallel/BufferInfo.h>
#include <Core/Util/RefCounted.h>
#include <Core/Parallel/ProcessorGroup.h>
#include <Core/Malloc/Allocator.h>

#include <Core/Parallel/uintahshare.h>
namespace Uintah {
  using namespace std;
  
  class UINTAHSHARE PackedBuffer : public RefCounted
  {
  public:
    PackedBuffer(int bytes)
      : buf((void*)(scinew char[bytes])), bufsize(bytes) {}
    ~PackedBuffer()
    { delete[] (char*)buf; }
    void* getBuffer() { return buf; }
    int getBufSize() { return bufsize; }
  private:
    void* buf;
    int bufsize;
  };

  class UINTAHSHARE PackBufferInfo : public BufferInfo {
  public:
    PackBufferInfo();
    ~PackBufferInfo();

    void get_type(void*&, int&, MPI_Datatype&, MPI_Comm comm);
    void get_type(void*&, int&, MPI_Datatype&);
    void pack(MPI_Comm comm, int& out_count);
    void unpack(MPI_Comm comm);

    // PackBufferInfo is to be an AfterCommuncationHandler object for the
    // MPI_CommunicationRecord template in MPIScheduler.cc.  After receive
    // requests have finished, then it needs to unpack what got received.
   void finishedCommunication(const ProcessorGroup * pg)
    { unpack(pg->getComm()); }
    
  private:
    PackBufferInfo(const PackBufferInfo&);
    PackBufferInfo& operator=(const PackBufferInfo&);

    PackedBuffer* packedBuffer;
  };
}

#endif
