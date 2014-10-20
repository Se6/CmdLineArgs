// Testing CmdLineArgs. 

#include <iostream>
#include <string>
#include <vector>

#include "CmdLineArgs.h"

#define nelem(x) (sizeof(x)/sizeof(x[0]))

using namespace std;

// Pass here argc and argv so that it contains:
// help flag,
// nb=12, there are 2 -v flags, name is "hello me"
// ratio=0.25f, numbers= 3,4
// there are remaining argumemts: "remain"
//
void fixTest(int test_no, int argc, const char **argv, int& failures)
{
	int n;
	string s;
	int v;
	int h;
	float f;
	string usage;
	vector<int> numbers;
	vector<string> remaining;
	
	try{
        CmdLineArgs cl(argc, const_cast<char**>(argv), "Test of command line arguments");

        h = cl.getFlag("help", 'h', "Getting usage");
        v = cl.getFlag("verbose", 'v', "To increase the verbosity");
        n = cl.getParam("nb", 'n', 0, "The number of frames\nanother line");
        s = cl.getParam("name", "", "The name of frame");

        cl.addUsageSeparator("  == Advanced options:");
        f = cl.getParam("ratio", 0.2f, "The frame ratio");
        vector<int> default_numbers = {1, 2};
        numbers = cl.getParams("numbers", default_numbers, "A comma separated list of values");

		usage = cl.usage();
        remaining = cl.getRemaining();
        cl.throwIfUnparsed();
    } catch (const exception& error) {
        cout << "Test " << test_no << ": Unparsed failure: " << error.what() << endl;
        ++failures;
		return;
	}
    
    if( n != 12 ) {
        cout << "Test " << test_no << ": Integer parameter failure. (Got " << n << " instead of 12)\n";
        ++failures;
		return;
    }
    
    if( v != 2 ) {
        cout << "Test " << test_no << ": Multiple verbose flags failure. (Got " << v << " v flags instead of 2)\n";
        ++failures;
		return;
    }
    
    if( s != "hello me" ) {
        cout << "Test " << test_no << ": String parameter failure. (Got \"" << s << "\" instead of \"hello me\")\n";
        ++failures;
		return;
    }
    
    if( f != 0.25f ) {
        cout << "Test " << test_no << ": Float parameter failure. (Got " << f << " instead of 0.25)\n";
        ++failures;
		return;
    }
    
    vector<int> zeNumbers = {3,4};
    if (numbers != zeNumbers) {
        cout << "Test " << test_no << ": Vector of int parameter failure.\n";
        ++failures;
		return;
    }
	
    vector<string> zeRemaining = {"remain"};
	if(remaining != zeRemaining){
		cout << "Test " << test_no << ": Remaining args failure.\n";
        ++failures;
		return;
	}
}

int main(int argc, char**argv) {

    int nbFails = 0;

    // test 1
    const char* argv1[] = {"test", "-hv", "--nb", "12", "--name",  "hello me", "--ratio", "0.25", "--num", "3,4", "remain", "-v" };
    int argc1 = nelem(argv1);
    fixTest(1, argc1, argv1, nbFails);
        
    // test 2
    const char* argv2[] = {"test", "remain", "-vhv", "--nb", "0xC", "--name=hello me", "--ratio=0.250", "--num", "3,4"};
    int argc2 = nelem(argv2);
    fixTest(2, argc2, argv2, nbFails);
        
    if( nbFails ) {
        cout << nbFails << " test failed!\n";
        return 1;
    } else {
        cout << "All tests passed.\n";
        return 0;
    }
}
