/*
 *  FieldSetWriter.cc: Save persistent representation of a field to a file
 *
 *  Written by:
 *   Michael Callahan
 *   Department of Computer Science
 *   University of Utah
 *   March 2001
 *
 *  Copyright (C) 2001 SCI Group
 */

#include <Core/GuiInterface/GuiVar.h>
#include <Core/Malloc/Allocator.h>
#include <Dataflow/Network/Module.h>
#include <Dataflow/Ports/FieldSetPort.h>

namespace SCIRun {

class FieldSetWriter : public Module {
  FieldSetIPort* inport_;
  GuiString filename_;
  GuiString filetype_;
public:
  FieldSetWriter(const clString& id);
  virtual ~FieldSetWriter();
  virtual void execute();
};

extern "C" Module* make_FieldSetWriter(const clString& id) {
  return new FieldSetWriter(id);
}

FieldSetWriter::FieldSetWriter(const clString& id)
  : Module("FieldSetWriter", id, Source), filename_("filename", id, this),
    filetype_("filetype", id, this)
{
  // Create the output port
  inport_ =
    scinew FieldSetIPort(this, "Persistent Data", FieldSetIPort::Atomic);
  add_iport(inport_);
}

FieldSetWriter::~FieldSetWriter()
{
}

void FieldSetWriter::execute()
{
  // Read data from the input port
  FieldSetHandle handle;
  if(!inport_->get(handle))
    return;

  // If no name is provided, return
  clString fn(filename_.get());
  if(fn == "") {
    error("Warning: no filename in FieldSetWriter");
    return;
  }
   
  // Open up the output stream
  Piostream* stream;
  clString ft(filetype_.get());
  if(ft=="Binary"){
    stream=scinew BinaryPiostream(fn, Piostream::Write);
  } else { // "ASCII"
    stream=scinew TextPiostream(fn, Piostream::Write);
  }

  // Write the file
  Pio(*stream, handle);
  delete stream;
}

} // End namespace SCIRun
