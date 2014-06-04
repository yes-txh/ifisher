/********************************
 File name: submitter/cmd_parse.h
 Author:   Tangxuehai
 Date:     2013-11-19
 Version:  0.1
 Description: cmd parse
*********************************/

#ifndef SRC_SUBMITTER_CMD_PARSE_H
#define SRC_SUBMITTER_CMD_PARSE_H

#include "common/clynn/singleton.h"

class CmdParse{
public:
     void UserCmdParse();
     void AppCmdParse();
     void ImageCmdParse();
     void JobCmdParse();
     void DefaultCmdParse();
     void UsageCmdParse();
     //bool QueryAppList();
};
typedef Singleton<CmdParse> CmdParsePtrI;
#endif


