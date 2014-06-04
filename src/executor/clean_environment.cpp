/********************************
 FileName: executor/clean_environment.h
 Author:   WangMin
 Date:     2013-11-19
 Version:  0.1
 Description: clean environment, include libvirt and lxc
*********************************/

#include <stdio.h>
#include <stdlib.h>
#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>
#include <string.h>
#include <fstream>

using std::ifstream;
using std::string;

int32_t CleanLibvirt() {
    // build connection
    virConnectPtr conn = virConnectOpen("qemu:///system");
    if (NULL == conn) {
        printf("Failed to open connection to qemu:///system\n");
        return -1;
    }

    // get domain list
    int num_domains = virConnectNumOfDomains(conn);
    if (-1 == num_domains) {
        printf("Failed to get num of domains\n");
        return -1;
    }
    int *active_domains = (int*)malloc(sizeof(int) * num_domains);
    num_domains = virConnectListDomains(conn, active_domains, num_domains);
    if (-1 == num_domains) {
        printf("Failed to get domain list\n");
        return -1;
    }

    // shutdown domain list
    for (int i = 0; i < num_domains; i++) {
        virDomainPtr dom = virDomainLookupByID(conn, active_domains[i]);
        if (NULL == dom) {
            printf("Failed to find domain %d\n", active_domains[i]);
            continue;
        }
        // shutdown
        /* if (-1 == virDomainShutdown(dom)) {
            printf("Failed to shutdown domain %d\n", active_domains[i]);
            continue;
        }*/
        // destroy
        if (-1 == virDomainDestroy(dom)) {
            printf("Failed to Destroy domain %d\n", active_domains[i]);
            continue;
        }
    }
    return 0;
}

int32_t CleanIPRules() {
    ifstream fin("/var/lib/lynn/iprules");
    if (!fin) {
        return 0;
    }
    string port, vm_endpoint;
    while (fin >> port >> vm_endpoint) {
        string cmd = "iptableds -t nat -D PREROUTING -i eth0 -p tcp --dport "
                   + port + " -j DNAT --to-destination " + vm_endpoint;
        system(cmd.c_str());
        // TODO
    }
    // clean iptables rules
    string cmd = "iptables -D FORWARD -i eth0 -o virbr0 -p tcp -m state --state NEW -j ACCEPT";
    system(cmd.c_str());

    // clean files
    system("rm /var/lib/lynn/iprules");
    return 0;
}
int32_t main() {
    if (0 != CleanLibvirt()) {
        printf("Failed to clean libvirt\n");
        return -1;
    }
    return 0;
}
