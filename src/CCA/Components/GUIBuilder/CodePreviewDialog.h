/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2004 Scientific Computing and Imaging Institute,
   University of Utah.

   License for the specific language governing rights and limitations under
   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
*/

#ifndef CCA_Components_GUIBuilder_CodePreviewDialog_h
#define CCA_Components_GUIBuilder_CodePreviewDialog_h

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#include<wx/grid.h>
#endif

#include <CCA/Components/GUIBuilder/ComponentSkeletonWriter.h>
#include <vector>

class wxWindow;
class wxButton;
class wxStaticText;
class wxTextCtrl;
class wxGrid;

namespace GUIBuilder {
class CodePreviewDialog : public wxDialog
{
  
 public:
  enum
    {
      ID_ViewSourceFileCode = wxID_HIGHEST,
      ID_ViewHeaderFileCode,
      ID_ViewMakeFileCode
    };
  CodePreviewDialog(wxWindow *parent,wxWindowID id,const wxString &title,const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE, const wxString& name = "Code Preview Dialog Box");
  
 private:
  wxTextCtrl *codePreview;
  wxButton *viewSourceFileCode;
  wxButton *viewHeaderFileCode;
  wxButton *viewMakeFileCode;
  
  void OnViewSourceFileCode( wxCommandEvent &event );
  void OnViewHeaderFileCode( wxCommandEvent &event );
  void OnViewMakeFileCode( wxCommandEvent &event );
  DECLARE_EVENT_TABLE()
};

}
#endif
