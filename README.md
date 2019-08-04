CmdLineArgs
===========


A simple and easy to use C++ command line parsing library contained in a single header file.


Goal
----
The motivation behind this library is to be very simple to use, yet quite complete and getting a nicely formatted usage.
It should take 2 minutes to be able to use it after looking at an example.


Features
--------
- Supports "parameters" which we define by a name preceded by "--" and followed by a value. Example: `--nb 10` (`--nb=10` is also valid). Here *nb* is the parameter name, *10* its value.
- Supports "flags" which we define by a name preceded by "--" and not followed by a value. Example: `--enable` Here *enable* is the flag name.
- Supports long and short names: 
    - Long names starts with "--".
    - Short names are one letter starting with a single "-".
- Gives a nicely formatted usage.
- Parameters and flags are defined and retrieved in a single call: no need to first install the parameter or flag and then retrieve its value.
- Throws runtime_error exception when a parsing error occurs.
- Abbreviation of long names: `--my_long_parameter_name` can be used as `--my` as long as it does not conflict with another parameter name. Conflicts are not checked though.
- Single header file.
- Short flags can be combined. (`-f -l` is equivalent to `-fl`)
- Integer parameters can be entered in decimal or hexadecimal notation. (Exple: `--number 0xff`)
- Supports multiple values: Example `--values 2,3,4` Here *values* is the parameter name and is given 3 values. 


Requirements
------------
A C++11 compiler.
To generate the documentation you will need Doxygen.


Example
--------
```C
#include "CmdLineArgs.h"
    
using namespace std;
    
int main(int argc, char **argv) {

    bool help;
    string name;
    float ratio;
    string usage;
    vector<int> numbers;
    vector<int> default_numbers = {1, 2};
    vector<float> filter;
    vector<float> default_filter = {0.25, 0.5, 0.25};
    vector<string> files;
    
    try{
        CmdLineArgs cl(argc, argv, "To compute the size of the universe.\n 
                                    Usage: prog [options] <filenames>\n");
                                    
        help = cl.getFlag("help", "Getting usage");
        name = cl.getParam("name", "", "The name of frame");
        
        cl.addUsageSeparator("\n  == Advanced options:");
        ratio       = cl.getParam("ratio", 0.2f, "The frame ratio");
        numbers     = cl.getParams("numbers", default_numbers, true, 
                                "A comma or space separated list of 2 values");
        
        cl.addUsageSeparator("\n  == Filter controls:");
        filter = cl.getParams("filter", 'f', default_filter, false, 
                            "Filter coefficients. Can be of any size.");
        
        cl.addUsageOutro("\nExample: prog --filter 0.1, 0.9, 0.1");
        
        usage = cl.usage();
        files = cl.getRemaining();
        cl.throwIfUnparsed();
    } catch (const exception& error) {
        cerr << usage << error.what() << endl;
        return 1;
    }
    
    if (help || !files.size()) {
        cout << usage;
        return 1;
    }   
    
    // using name, ratio, numbers ...
}
```

The above will generate on the command line the following usage message:

    To compute the size of the universe.
     Usage: prog [options] <filenames>

    Options are:
        --help                                      Getting usage
        --name (default: "")                        The name of frame

      == Advanced options:
        --ratio (default: 0.2)                      The frame ratio
        --numbers (default: 1,2)                    A comma or space separated list of 2 values

      == Filter controls:
        --filter (-f)  (default: 0.25,0.5,0.25)     Filter coefficients. Can be of any size.
        
    Example: prog --filter 0.1, 0.9, 0.1


List of files
-------------
- README.md This file.
- CmdLineArgs.h It contains the command line parsing object CmdLineArgs.
- LICENSE The license file. (MIT license)
- example.cpp A simple example. You can compile it and try it.
- test.cpp Some tests to check CmdLineArgs.
- Makefile A gnu make file to build the example, test and gnerate the doxygen documentation.
- Doxyfile.in Doxygen configuration file for the documentation.


Building & testing
------------------
- `make all` to generate the example and the test.
- `./test` to run the tests.
- `make doc` to generate the html documentation.


Limitations & Known issues
--------------------------
- Conflict of shorten parameters names are not checked.
  if we have `getParam("aparam", 0, ...)`
  then `--aparam 0` will be fine, and also `--ap 0`. That is a feature. However if we also have `getParam("apart", 0)`, then `--ap 0` will set parameter aparam since it was the first defined....
- If let's say `--flag` is a flag and `--param` an integer parameter, then it is possible that the command line `--para -fl 5` be accepted if getFlag is called before getParam...
  Nothing much can be done about it, expect putting the getParam before the getFlag... but that is not always convenient, we might want this particular order in our usage display...

