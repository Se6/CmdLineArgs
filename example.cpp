#include <iostream>
#include <string>
#include <vector>
#include "CmdLineArgs.h"

using namespace std;

int main(int argc, char **argv){
	
	string name;
	bool help;
    int number;
	float ratio;
	string usage;
	vector<int> values;
    vector<string>  some_strings;
	vector<string> remaining;
	
	try{
        CmdLineArgs cl(argc, argv, "Example of command line arguments");

        help = cl.getFlag("help", "Getting usage");
        name = cl.getParam("name", "stone", "The name of something");
        number = cl.getParam("number", 'n', 5, "Number of whatever");

        cl.addUsageSeparator("  == Advanced options:");
        vector<string> default_strings = {"one", "two", "three"};
        some_strings = cl.getParams("some_stings", default_strings, "3 strings separated by comma");
        ratio = cl.getParam("ratio", 0.2f, "The ratio");
        vector<int> default_numbers = {1, 2};
        values = cl.getParams("values", default_numbers, "A comma separated list of values", false);

		usage = cl.usage();
        remaining = cl.getRemaining();
        cl.throwIfUnparsed();
        
    } catch (const exception& error) {
		cerr << usage << error.what() << endl;
		return 1;
	}
	
	if(argc==1 || help)
		cout << usage;

	cout << endl;
	cout << "name=" << name << endl;
    cout << "some_stings=";
    for(auto &st: some_strings)
        cout << st << ",";
	cout << endl;
    cout << "number=" << number << endl;
	cout << "ratio=" << ratio << endl;
	cout << "numbers=";
    for(auto num: values)
        cout << num << ",";
	cout << endl;
	
	if(!remaining.empty()){
		cout << "remaining: ";
		for(auto rem: remaining)
			cout << rem << " ";
	}else
		cout << "no arg remaining";
	
	cout << endl;
}
