#include "classad/classad_distribution.h"
#include <iostream>

using namespace std;
#ifdef WANT_CLASSAD_NAMESPACE
using namespace classad;
#endif

string job_classad_text = 
"["
"Requirements = (other.memory >= 4000);"
"]";

string machine_classad_text = 
"["
"]";

int main(int, char **)
{
	// First we turn the textual description of the ClassAd into 
	// our internal representation of the ClassAd. 
	ClassAdParser  parser;
	ClassAd        *job_classad, *machine_classad;

	job_classad     = parser.ParseClassAd(job_classad_text, true);
	machine_classad = parser.ParseClassAd(machine_classad_text, true);
       
        machine_classad->InsertAttr("memory", 5000);
        if(!machine_classad->InsertAttr("memory", 4500)){ 
            printf("error\n");
        }

        int val;
        if(!machine_classad->EvaluateAttrInt("memory", val)) {
            printf("error\n"); 
        }
        printf("%d\n", val);

	// Next we print out the ClassAds. This requires unparsing them first.
	PrettyPrint  printer;
	string       printed_job_classad, printed_machine_classad;

	printer.Unparse(printed_job_classad, job_classad);
	printer.Unparse(printed_machine_classad, machine_classad);
	cout << "Job ClassAd:\n" << printed_job_classad << endl;
	cout << "Machine ClassAd:\n" << printed_machine_classad << endl;

	// Finally we test that the two ClassAds match. They should. 
	// Note that a MatchClassAd is a ClassAd. You could pretty print it
	// like the other ads above, if you wanted to.
	MatchClassAd  match_ad(job_classad, machine_classad);
  
	bool          can_evaluate, ads_match;

	can_evaluate = match_ad.EvaluateAttrBool("rightMatchesLeft", ads_match);
	if (!can_evaluate) {
		cout << "Something is seriously wrong.\n";
	} else if (ads_match) {
		cout << "The ads match, as expected.\n";
	} else {
		cout << "The ads don't match! Something is wrong.\n";
	}
	return 0;
}
