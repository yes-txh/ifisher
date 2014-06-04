#include <stdio.h>
#include <boost/shared_ptr.hpp>
#include <list>
#include <classad/classad.h>
#include <classad/classad_distribution.h>
#include "include/attributes.h"

int main(int argc, char ** argv) {

    ClassAd ad;
    ad.InsertAttr(ATTR_NEED_VCPU, 1);
    ad.InsertAttr(ATTR_NEED_MEMORY, 1024);
    ClassAdParser parser;
    printf("%s\n", EXP_TASK_RANK.c_str());
    ExprTree* expr = parser.ParseExpression(EXP_TASK_RANK);
    ad.Insert(ATTR_TASK_RANK, expr);
    ExprTree* re_expr = parser.ParseExpression(EXP_TASK_REQUIREMENT);
    ad.Insert(ATTR_TASK_REQUIREMENT, re_expr);
   
    ClassAd c1;
    c1.InsertAttr(ATTR_ALL_VCPU, 4);
    c1.InsertAttr(ATTR_AVAIL_VCPU, 4);
    c1.InsertAttr(ATTR_ALL_MEMORY, 6 * 1024); 
    c1.InsertAttr(ATTR_AVAIL_MEMORY, 6 * 1024); 

    ad.alternateScope = &c1;
    bool is_match = false;
    ad.EvaluateAttrBool(ATTR_TASK_REQUIREMENT, is_match);
    if(is_match == true) {
        double d_value;
        ad.EvaluateAttrNumber(ATTR_TASK_RANK, d_value);
        printf("task rank:%f\n", d_value);
    } else {
        printf("nima\n");
    }
 
    return 0;
}

