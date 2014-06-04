#ifndef JOBS_MANAGER_JOB_ATTR_CHECKE_H
#define JOBS_MANAGER_JOB_ATTR_CHECKE_H

#include "common/classad/classad_complement.h"

class JobAttrChecker {
public:
    JobAttrChecker() {
        m_errnum = 0;
    }
    int CheckJobAttr(ClassAdPtr ad_ptr);
    int GetErrorNum() {
        return m_errnum;
    }
private:
    int m_errnum;
};
#endif
