/*
  The contents of this file are subject to the University of Utah Public
  License (the "License"); you may not use this file except in compliance
  with the License.
  
  Software distributed under the License is distributed on an "AS IS"
  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
  License for the specific language governing rights and limitations under
  the License.
  
  The Original Source Code is SCIRun, released March 12, 2001.
  
  The Original Source Code was developed by the University of Utah.
  Portions created by UNIVERSITY are Copyright (C) 2001, 1994 
  University of Utah. All Rights Reserved.
*/

/*
 *  DTMessage.h defines the message structure used in the data transmitter
 *
 *  Written by:
 *   Keming Zhang
 *   Department of Computer Science
 *   University of Utah
 *   Jun 2003
 *
 *  Copyright (C) 1999 SCI Group
 */


#ifndef CORE_CCA_COMPONENT_COMM_DT_DTMESSAGE_H
#define CORE_CCA_COMPONENT_COMM_DT_DTMESSAGE_H

#include <Core/CCA/Component/Comm/DT/DTAddress.h>
#include <iostream>
#include <string.h>
namespace SCIRun {

  class DTMessage{
  public:
    //The message being sent has the following structure:
    //DTMessage | buf

    char *buf;
    int length;
    bool autofree;
    DTPoint *recver;  //recver sp/ep  
    DTPoint *sender;  //sender sp/ep   
    DTAddress fr_addr;  //filled by sender
    DTAddress to_addr;  //filled by recver
    
    void display(){
#define DISPLAY_MSG      
#ifdef DISPLAY_MSG      
      char *str=new char[length];
      strncpy(str, buf+sizeof(int), length-sizeof(int));
      str[length-sizeof(int)]='\0';
      std::cerr<<"DTMessage:\n"
	       <<"\t recver="<<(long)recver<<"\n"
	       <<"\t sender="<<(long)sender<<"\n"
	       <<"\t fr_addr="<<fr_addr.ip<<"/"<<fr_addr.port<<"\n"
	       <<"\t to_addr="<<to_addr.ip<<"/"<<to_addr.port<<"\n"
	       <<"\t length="<<length<<"\n"
	       <<"\t buf(id)="<<*((int*)(this->buf))<<"\n";
      /*
      int n=length-sizeof(int);
      std::cerr<<"\t buf(msg)=";
      for(int i=0; i<n; ){
	if(i>=4 && i<36+4){
	  std::cerr<<" "<<*(buf+sizeof(int)+i);
	  i++;
	}
	else{
	  std::cerr<<" "<<*(int*)(buf+sizeof(int)+i);
	  i+=sizeof(int);
	}
      }
      std::cerr<<"\n";      
      */
#endif      
    }
  };

}// namespace SCIRun
#endif
