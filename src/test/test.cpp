#include <stdio.h>
#include <zookeeper/zookeeper.h>
#include <errno.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
#include <classad/classad.h>
#include <classad/classad_distribution.h>
#include <iostream>
#include "common/classad/classad_complement.h"
#include "include/type.h"

using std::string;

int32_t main(int argc, char* argv[]){
    string         classad_string = "[ATTR_JOB_ID = 1; b = \"Cardini\"]";
    string         str = "[ ATTR_JOB_ID = 18; ATTR_TASK_ID = 1; ATTR_VMHB_CPU = 0.000000; ATTR_VMHB_MEMORY = 0.456332; ATTR_VMHB_BYTES_IN = 170008; ATTR_VMHB_BYTES_OUT = 27905; ATTR_VMHB_STATE = \"APP_RUNNING\" ]";
    //string	str = "[ ATTR_JOB_ID = 2 ]";
    ClassAd        *classad;
    ClassAdParser  parser;
    classad = parser.ParseClassAd(str, true);
    ClassAdPtr ptr(classad);
    printf("%d\n", ptr.use_count());
    int32_t i;
    string state;
    ClassAdPtr ptr2(new ClassAd(*ptr.get()));
    printf("%d\n", ptr.use_count()); 
    // if(!ptr->EvaluateAttrInt(ATTR_JOB_ID, id.job_id))
    if(!ptr->EvaluateAttrString("ATTR_VMHB_STATE",state)) { 
         printf("error\n");
    } 
    //printf("%s\n",state);
    std::cout<<state<<std::endl;
    string printed_classad;
   
    ClassAdXMLUnParser xml_unparser;
    xml_unparser.SetCompactSpacing(false);
    xml_unparser.Unparse(printed_classad, ptr.get());
    printf("%s\n", printed_classad.c_str());

    printed_classad = "";
    ClassAdUnParser unparser;
    unparser.Unparse(printed_classad, ptr.get());
    printf("%s\n", printed_classad.c_str()); 

    return 0;
}
