/********************************
 FileName: ResourceScheduler/match_maker
 Author:   ZhangZhang & Tangxuehai
 Date:     2014-05-11
 Version:  0.1
 Description: match machine with job
*********************************/

#ifndef SRC_RESOURCE_SCHEDULER_MATCH_MAKER_H
#define SRC_RESOURCE_SCHEDULER_MATCH_MAKER_H

#include <vector>
#include <string>
#include "common/clynn/singleton.h"

using std::vector;
using std::string;

class MatchMaker {
public:
    int Negotiation(const string& job, vector<string>& machines);
    int NegotiationTask(const string& task, string& machine);
    int NegotiationTaskConstraint(const string& task_ad, const vector<string>& soft_list, string& machine);
private:
};

typedef Singleton<MatchMaker> MATCH_MAKER;
#endif
