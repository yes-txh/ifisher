/********************************
 FileName: test/scheduler/submit-job.cpp
 Author:   WangMin
 Date:     2014-3-19
 Version:  0.1
 Description: submit a job to scheduler (with xml file)
*********************************/

#include <iostream>
#include <string>
#include <fstream>  
#include <streambuf>  
      
    std::ifstream t("file.txt");  
    std::string str((std::istreambuf_iterator<char>(t)),  
                     std::istreambuf_iterator<char>());  

#include "common/clynn/rpc.h"
#include "include/proxy.h"

using namespace std;

static string usage = "./submit-job xml_file";

int main(int argc, char ** argv) {
    if(argc != 2) {
        cout << "Usage is wrong." << endl;
        cout << "Usage is: " << usage << endl;
        return -1;
    }
    
    // xml_file
    string xml_file = argv[1];

    ifstream in(xml_file);
    string xml_content((istreambuf_iterator<char>(in)),istreambuf_iterator<char>());
    in.close();

    cout << xml_content << endl;

    string endpoint = "127.0.0.1:9999";   // general
    // string endpoint = "192.168.10.80:9999"; // hy
    // string endpoint = "192.168.120.88:9999"; // iie

    struct SubmitJobResult result;
 
    try {
        Proxy<JobsManagerClient> proxy = RpcClient<JobsManagerClient>::GetProxy(endpoint);
        proxy().SubmitJob(result, xml_content);
    } catch (TException &tx) {
        cout<<"error"<<endl;
        return -1;
    }

    return 0;
}
