//static char *id="@(#) $Id$";

/*
 *  ColumnMatrixWriter.cc: ColumnMatrix Writer class
 *
 *  Written by:
 *   Steven G. Parker
 *   Department of Computer Science
 *   University of Utah
 *   July 1994
 *
 *  Copyright (C) 1994 SCI Group
 */

#include <Util/NotFinished.h>
#include <Persistent/Pstreams.h>
#include <Dataflow/Module.h>
#include <CommonDatatypes/ColumnMatrixPort.h>
#include <CoreDatatypes/ColumnMatrix.h>
#include <Malloc/Allocator.h>
#include <TclInterface/TCLvar.h>

namespace PSECommon {
namespace Modules {

using namespace PSECommon::Dataflow;
using namespace PSECommon::CommonDatatypes;
using namespace SCICore::TclInterface;
using namespace SCICore::PersistentSpace;

class ColumnMatrixWriter : public Module {
    ColumnMatrixIPort* inport;
    TCLstring filename;
    TCLstring filetype;
public:
    ColumnMatrixWriter(const clString& id);
    ColumnMatrixWriter(const ColumnMatrixWriter&, int deep=0);
    virtual ~ColumnMatrixWriter();
    virtual Module* clone(int deep);
    virtual void execute();
};

Module* make_ColumnMatrixWriter(const clString& id) {
  return new ColumnMatrixWriter(id);
}

ColumnMatrixWriter::ColumnMatrixWriter(const clString& id)
: Module("ColumnMatrixWriter", id, Source), filename("filename", id, this),
  filetype("filetype", id, this)
{
    // Create the output data handle and port
    inport=scinew ColumnMatrixIPort(this, "Input Data", ColumnMatrixIPort::Atomic);
    add_iport(inport);
}

ColumnMatrixWriter::ColumnMatrixWriter(const ColumnMatrixWriter& copy, int deep)
: Module(copy, deep), filename("filename", id, this),
  filetype("filetype", id, this)
{
    NOT_FINISHED("ColumnMatrixWriter::ColumnMatrixWriter");
}

ColumnMatrixWriter::~ColumnMatrixWriter()
{
}

Module* ColumnMatrixWriter::clone(int deep)
{
    return scinew ColumnMatrixWriter(*this, deep);
}

#if 0
static void watcher(double pd, void* cbdata)
{
    ColumnMatrixWriter* writer=(ColumnMatrixWriter*)cbdata;
    writer->update_progress(pd);
}
#endif

void ColumnMatrixWriter::execute()
{
    using SCICore::Containers::Pio;

    ColumnMatrixHandle handle;
    if(!inport->get(handle))
	return;
    clString fn(filename.get());
    if(fn == "")
	return;
    Piostream* stream;
    clString ft(filetype.get());
    if(ft=="Binary"){
	stream=scinew BinaryPiostream(fn, Piostream::Write);
    } else {
	stream=scinew TextPiostream(fn, Piostream::Write);
    }
    // Write the file
    //stream->watch_progress(watcher, (void*)this);
    Pio(*stream, handle);
    delete stream;
}

} // End namespace Modules
} // End namespace PSECommon

//
// $Log$
// Revision 1.1  1999/07/27 16:58:19  mcq
// Initial commit
//
// Revision 1.3  1999/07/07 21:10:31  dav
// added beginnings of support for g++ compilation
//
// Revision 1.2  1999/04/27 22:58:04  dav
// updates in Modules for CoreDatatypes
//
// Revision 1.1  1999/04/25 03:25:34  dav
// adding these files in too... should have been there already... oh well, sigh
//
//
