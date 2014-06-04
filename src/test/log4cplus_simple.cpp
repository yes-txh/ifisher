#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <iomanip>
using namespace std;
using namespace log4cplus;
int
main()
{
BasicConfigurator config;
config.configure();
Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("logger"));
LOG4CPLUS_WARN(logger, "This is"
<< " a reall"
<< "y long message." << endl
<< "Just testing it out" << endl
<< "What do you think?");
LOG4CPLUS_WARN(logger, "This is a bool: " << true);
LOG4CPLUS_WARN(logger, "This is a char: " << 'x');
LOG4CPLUS_WARN(logger, "This is a short: " << (short)-100);
LOG4CPLUS_WARN(logger, "This is a unsigned short: " << (unsigned short)100);
LOG4CPLUS_WARN(logger, "This is a int: " << (int)1000);
LOG4CPLUS_WARN(logger, "This is a unsigned int: " << (unsigned int)1000);
LOG4CPLUS_WARN(logger, "This is a long(hex): " << hex << (long)100000000);
LOG4CPLUS_WARN(logger, "This is a unsigned long: "
<< (unsigned long)100000000);
LOG4CPLUS_WARN(logger, "This is a float: " << 1.2345f);
LOG4CPLUS_WARN(logger, "This is a double: "
<< setprecision(15)
<< 1.2345234234);
LOG4CPLUS_WARN(logger, "This is a long double: "
<< setprecision(15)
<< (long double)123452342342.342);
return 0;
