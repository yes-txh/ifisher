#ifndef RESOURCE_SCHEDULER_MATCH_LIST_H
#define RESOURCE_SCHEDULER_MATCH_LIST_H

#include "common/classad/classad_complement.h"
#include "ResourceScheduler/machine_constances.h"

struct ListEntry {
    double rank_value;
    ClassAdPtr ad;
};

class MatchList {
public:
    explicit MatchList();   
private:
    
};
#endif
