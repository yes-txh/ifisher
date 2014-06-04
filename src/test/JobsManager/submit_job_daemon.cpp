/********************************
 FileName: test/submit-job-daemon.cpp
 Author:   Tang Xuehai
 Date:     2014-5-21
 Version:  0.1
 Description: submit job to JM with daemon
*********************************/

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>  
#include <streambuf>  

#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

std::ifstream t("file.txt");  
std::string str((std::istreambuf_iterator<char>(t)),  
                 std::istreambuf_iterator<char>());  
#include "common/clynn/rpc.h"
#include "include/proxy.h"

using namespace std;

static string usage = "./submit-job-daemon  -job_num -max_waiting_job_num -user -xml_file";
static int32_t interval = 2;
static string endpoint = "10.128.85.10:9999";   

vector<int32_t> job_id_list;
vector<int32_t> del_job_id_list;
string result_file = "./result";
string total_result_file = "./total_result";
int64_t finished_task_num = 0;
int64_t failed_task_num = 0;
int64_t task_num = 0;


void* TaskResultProcessor(void* unused) {
    ofstream file_in;
    file_in.open(result_file.c_str());
    file_in << "作业号  任务号  状态" <<"\n";
    file_in.close();
    while(true) {
        cout << "job list size:"<<job_id_list.size() << endl;
	if(job_id_list.size() == 0) {
	    sleep(1);
	    continue;
	}
	for(vector<int32_t>::iterator it = job_id_list.begin(); it != job_id_list.end(); ++it) {
	    int32_t job_id = *it;
	    vector<TaskStateInfo> task_state_info_list;
	    bool flag = true;
	    try {
        	Proxy<JobsManagerClient> proxy = RpcClient<JobsManagerClient>::GetProxy(endpoint);
        	proxy().GetTaskStateInfo(task_state_info_list, job_id);
    	    } catch (TException &tx) {
        	return NULL;
    	    }

	    if(task_state_info_list.size() == 0) {
		continue;
	    } 
	    for( vector<TaskStateInfo>::iterator it_task = task_state_info_list.begin(); it_task != task_state_info_list.end(); ++it_task) {
    	        if((*it_task).state == 0 || (*it_task).state == 1 || (*it_task).state == 2) {
		    // task is running
		    flag = false;
		    break;
	        }
	    }
	    
	    if(flag == false) {
		continue;
	    }
	    //open result file 
	    ofstream file_in;
            file_in.open(result_file.c_str(),ios::out|ios::app);

	    string result;
	    stringstream job_id_ss;
	    stringstream task_id_ss;
	    for( vector<TaskStateInfo>::iterator it_1 = task_state_info_list.begin(); it_1 != task_state_info_list.end(); ++it_1) {
		job_id_ss.str("");
		task_id_ss.str("");
		job_id_ss << (*it_1).job_id;
		task_id_ss << (*it_1).task_id;
		if((*it_1).state == 3) {
		    //task is finished
		   result = job_id_ss.str() + "       " + task_id_ss.str() + "       完成";
		   finished_task_num += 1;
		} else {
		    //task is failed
		   result = job_id_ss.str() + "       " + task_id_ss.str() + "       失败";			
		   failed_task_num += 1;
		}
		cout << "result:" << result << endl;
                file_in << result <<"\n";
		result = "";
            } 
	    file_in.close();
	    del_job_id_list.push_back(job_id);
	}

	for(vector<int32_t>::iterator it = del_job_id_list.begin(); it != del_job_id_list.end(); ++it) {
	    vector<int32_t>::iterator iter = find(job_id_list.begin(), job_id_list.end(), *it);
	    if(iter != job_id_list.end()) {
		job_id_list.erase(iter);
	    }
	}

	//clear delete job id list
	del_job_id_list.clear();	

	//write result to file
	task_num = finished_task_num + failed_task_num;
	stringstream task_num_ss;
        task_num_ss << task_num;

	stringstream finished_task_num_ss;
        finished_task_num_ss << finished_task_num;

	stringstream failed_task_num_ss;
        failed_task_num_ss << failed_task_num;

	string total_result = "Task_num: " + task_num_ss.str() + "\nFinished task num: " + finished_task_num_ss.str() 
	       + "\nFaile task num: " + failed_task_num_ss.str();
	ofstream file_in;
        file_in.open(total_result_file.c_str(),ios::out);
	file_in << total_result;
	file_in.close();
	sleep(2);
    }
    return NULL;
} 

int main(int argc, char ** argv) {
    if(argc != 5) {
        cout << "Usage is wrong." << endl;
        cout << "Usage is: " << usage << endl;
        return -1;
    }
    
    // cmd parse
    char* ch = argv[1];
    int32_t max_job_num = atoi(ch);
    char* ch_ = argv[2];
    int32_t max_waiting_job_num = atoi(ch_);
    string user = argv[3];
    string xml_file = argv[4];
  
    int32_t job_num = 0;
    //create thread to get task result
    //pthread_t task_result_t;
    //pthread_create(&task_result_t, NULL, TaskResultProcessor, NULL);

    ifstream in(xml_file);
    //open file error ?
    if(!in){
        cout<< "Failed to open xml file." << endl;
        return -1;
    }

    cout<< "max job num:" << max_job_num << endl;
    cout<< "max waiting job num:" << max_waiting_job_num <<endl;

    string xml_content((istreambuf_iterator<char>(in)),istreambuf_iterator<char>());
    in.close();
    while(true) {
	Proxy<JobsManagerClient> proxy = RpcClient<JobsManagerClient>::GetProxy(endpoint);
        int32_t waiting_job_num = proxy().GetWaitJobNumByGroupName(user);
	cout<<"waiting job num:"<<job_num<<endl;
	if(waiting_job_num < max_waiting_job_num) {
            struct SubmitJobResult result;
            try {
                Proxy<JobsManagerClient> proxy = RpcClient<JobsManagerClient>::GetProxy(endpoint);
                proxy().SubmitJob(result, xml_content);
            } catch (TException &tx) {
                cout<<"error"<<endl;
                return -1;
            }
	    job_id_list.push_back(result.job_id);
            job_num ++;
        }
	if(job_num == max_job_num) {
	    cout<< "job num:" << job_num << endl;
	    return 0;
	}
	sleep(interval);
    }
    return 0;
}
