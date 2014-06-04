#ifndef COMMON_CLASSAD_COMPLEMENTION_H
#define COMMON_CLASSAD_COMPLEMENTION_H

#include <string>
#include <classad/classad.h>
#include <classad/classad_distribution.h>
#include <boost/shared_ptr.hpp>

using std::string;
using boost::shared_ptr;

typedef shared_ptr<ClassAd> ClassAdPtr;

class ClassAdComplement {
public:
    static string AdTostring(const ClassAdPtr ad) {
        ClassAdUnParser unparser;
        string classad_text = "";
        unparser.Unparse(classad_text, ad.get());
        return classad_text; 
    }

    static ClassAdPtr StringToAd(const string& ad) {
        ClassAdParser parser;
        ClassAdPtr classad(parser.ParseClassAd(ad, true));
        return classad;
    }
};

#endif
