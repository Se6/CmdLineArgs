#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

/**
   @brief The CmdLineArgs class implements the command line parsing.
 */
class CmdLineArgs {
public:

    CmdLineArgs(int argc, char** argv, const std::string &usage_intro, bool allow_set_with_equal=true);

    // Get parameters (with or without a short name):
    template <class T> T getParam(const std::string &long_name, char short_name, T default_value, const std::string &desc);
    template <class T> T getParam(const std::string &long_name,                  T default_value, const std::string &desc);

    // Get multiple values in a vector: (with or without a short name)
    template <class T> std::vector<T> getParams(const std::string &long_name, char short_name,
                                                const std::vector<T> &default_vals, bool enforce_default_size,
                                                const std::string &desc, char separator=',');
    template <class T> std::vector<T> getParams(const std::string &long_name,
                                                const std::vector<T> &default_vals, bool enforce_default_size,
                                                const std::string &desc, char separator=',');

    // Get a flag (with or without a short name):
    int getFlag(const std::string &long_name, char short_name, const std::string &desc);
    int getFlag(const std::string &long_name,                  const std::string &desc);

    /// @cond SPECIALISATIONS
    // String and char* specialisation of the above:
    std::string getParam(const std::string &long_name, char short_name, const std::string &default_value, const std::string &desc);
    std::string getParam(const std::string &long_name,                  const std::string &default_value, const std::string &desc);
    std::string getParam(const std::string &long_name, char short_name, const char* default_value,        const std::string &desc);
    std::string getParam(const std::string &long_name,                  const char* default_value,        const std::string &desc);

    // String specialisation for multiple values:
    std::vector<std::string> getParams(const std::string &long_name, char short_name,
                                       const std::vector<std::string> &default_vals, bool enforce_default_size,
                                       const std::string &desc, char separator=',');
    std::vector<std::string> getParams(const std::string &long_name,
                                       const std::vector<std::string> &default_vals, bool enforce_default_size,
                                       const std::string &desc, char separator=',');
    /// @endcond

    // To format the usage, adding a separation between options:
    void addUsageSeparator(const std::string &desc);

    // Add some description which will come after the descriptions of the different options.
    void addUsageOutro(const std::string &str);

    // To get the nicely formatted usage:
    std::string usage();

    // Get the remaining and unparsed arguments:
    std::vector<std::string> getRemaining();
    std::vector<std::string> getUnparsedOpts();

    // Throw an error if there is any remaining or unparsed arguments:
    void throwIfRemaining();
    void throwIfUnparsed();

    // To check if a parameter or flag is present, without retriving it:
    bool isPresent(const std::string &long_name,  char short_name=' ');

private:

    std::vector<std::string> args_;
    std::string usage_intro_, usage_outro_;
    std::vector<std::pair<std::string, std::string> > usage_;

    static std::vector<std::string> split(const std::string &s, char delim);
    void addUsage(const std::string &long_name, char short_name, const std::string &default_val, const std::string &desc);
    std::vector<std::string>::iterator findLongName(const std::string &name);
    std::vector<std::string>::iterator findShortName(char);
};


// To split a string into parts
std::vector<std::string> CmdLineArgs::split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;

    while(getline(ss, item, delim))
        elems.push_back(item);

    return elems;
}


/**
   @brief Constructor, passing argc and argv.
   @param argc the number of arguments. (is typically main argc parameter)
   @param argv the array of char* containing the arguments. (is typicaly main argv parameter)
   @param usage_intro A sting giving a short summary of what the programs does.
   @param allow_set_with_equal When true, parameters can also be set with an equal sign. Exple: "--param=10"
   That is the default. The only consequence is that '=' is not allowed in a parameter value.
   @note It is expected that the first argument in argv to be the program name. It will be discarded.
 */
CmdLineArgs::CmdLineArgs(int argc, char **argv, const std::string &usage_intro, bool allow_set_with_equal)
    :usage_intro_(usage_intro)
{
    for(int i=1; i<argc; ++i) {
        if (allow_set_with_equal) {
            auto vec = split(argv[i], '=');
            args_.reserve(args_.size() + vec.size());
            args_.insert(args_.end(), vec.begin(), vec.end());
        } else {
            args_.push_back(argv[i]);
        }
    }

    usage_intro_ += "\nOptions are:";
}


// To find an argument which is a long name (starting with "--")
//
std::vector<std::string>::iterator CmdLineArgs::findLongName(const std::string &name)
{
    std::vector<std::string>::iterator pos;
    bool found = false;

    for( pos=args_.begin(); pos!=args_.end(); ++pos ) {

        if( pos->size()<3 || pos->substr(0,2) != "--")
            continue;

        if( pos->substr(2) == name.substr(0,pos->size()-2) ){
            found = true;
            break;
        }
    }

    return found ? pos : args_.end();
}


// To find an argument which is a short name (starting with a single "-")
//
std::vector<std::string>::iterator CmdLineArgs::findShortName(char name)
{
    if(name==' ')
        return args_.end();

    std::vector<std::string>::iterator pos;
    bool found = false;

    for(pos=args_.begin(); pos!=args_.end(); ++pos){

        if( pos->size()<2 || (*pos)[0] != '-')
            continue;

        if((*pos)[1] == '-')
            continue;

        std::string::size_type idx = pos->find(name,1);
        if( idx != std::string::npos ) {
            found = true;
            break;
        }
    }

    return found ? pos : args_.end();
}


/**
   @brief To get a parameter (so an argument starting with "--" or "-", followed by a value)
   @param long_name long name of the parameter (so starting with "--").
   @param short_name short name of the parameter (so a single letter starting with "-").
   @param default_value default value to give if the parameter is not present.
   @param desc A description of the parameter. (will go into the usage)
   @return The value of the parameter.
 */
template <class T>
T CmdLineArgs::getParam(const std::string &long_name, char short_name, T default_value, const std::string &desc)
{
    T val;
    std::ostringstream oss;
    oss << default_value;
    addUsage(long_name, short_name, oss.str(), desc);

    // First search the long names:
    //
    auto pos = findLongName(long_name);
    if( pos != args_.end() ) {

        if( pos+2 > args_.end() )
            throw std::runtime_error("\nError: parameter --" + long_name + " is not followed by a value");

        pos = args_.erase(pos);

    } else {

        pos = findShortName(short_name);
        if( pos != args_.end() ) {

            if( pos+2>args_.end() )
                throw std::runtime_error("\nError: parameter -" + std::string(1,short_name) + " is not followed by a value");

            if( pos->size()==2 )
                pos = args_.erase(pos);
            else {
                pos->erase(pos->find(short_name, 1), 1);
                ++pos;
            }
        }
    }

    if ( pos != args_.end() ) {

        std::istringstream ss(*pos);
        ss >> val;

        if( ss.fail() )
            throw std::runtime_error("\nError: parameter --" + long_name + " is not followed by a correct value");

        if( !ss.eof() ) {
            std::stringstream sshex;
            sshex << std::hex << *pos;
            sshex >> val;

            if(sshex.fail())
                throw std::runtime_error("\nError: parameter --" + long_name + " is not followed by a correct value");

            if(!sshex.eof())
                throw std::runtime_error("\nError: parameter --" + long_name + " has an incorrect value");
        }

        args_.erase(pos);
        return val;

    } else
        return default_value;
}


/**
   @brief To get a parameter, no short name allowed.
   @param long_name long name of the parameter (so starting with "--").
   @param default_value default value to give if the parameter is not present.
   @param desc A description of the parameter. (will go into the usage)
   @return the value of the parameter.
 */
template <class T>
T CmdLineArgs::getParam(const std::string &long_name, T default_value, const std::string &desc)
{
    return getParam<T>(long_name, ' ', default_value, desc);
}


/**
   @brief To get a parameter with multiple values.
   @param long_name long name of the parameter (so starting with "--").
   @param short_name short name of the parameter (so a single letter starting with "-").
   @param default_vals default values to give if the parameter is not present.
   @param enforce_default_size When true, expect to have the same number of elements than default_vals. If only one value is given,
   will replicate the value. If a different number of values is given will throw.
   @param desc A description of the parameter. (will go into the usage)
   @param separator the character used to separate values. Multiple of them will be ignored and compressed to one.
   Also there is not way of escaping a separator (no way the values can contain this separartor)
   @return the values of the parameter, returned as a vector.
 */
template <class T>
std::vector<T> CmdLineArgs::getParams(const std::string &long_name, char short_name,
                                      const std::vector<T> &default_vals, bool enforce_default_size,
                                      const std::string &desc, char separator)
{
    // Join the default values wit the separator for the usage
    //
    std::ostringstream oss;
    for(unsigned int i=0; i<default_vals.size(); i++){
        if(i>0)
            oss << ",";
        oss << default_vals[i];
    }
    addUsage(long_name, short_name, oss.str(), desc);

    T val;
    std::vector<T> vec;
    std::istringstream ss;

    auto pos = findLongName(long_name);
    if( pos != args_.end() ) {

        if(pos+2>args_.end())
            throw std::runtime_error("\nError: parameter --" + long_name + " is not followed by a value");

        pos = args_.erase(pos);

    } else {

        pos = findShortName(short_name);
        if( pos != args_.end() ) {

            if( pos+2>args_.end() )
                throw std::runtime_error("\nError: parameter -" + std::string(1,short_name) + " is not followed by a value");

            if( pos->size()==2 )
                pos = args_.erase(pos);
            else{
                pos->erase( pos->find(short_name, 1), 1 );
                ++pos;
            }
        }

    }

    if( pos != args_.end() ) {
        do {

            ss.clear();
            ss.str(*pos);
            pos = args_.erase(pos);

            while( ss >> val ) {
                vec.push_back(val);
                if( ss.peek() == separator )
                    ss.ignore();
            }

            if( !ss.eof() )
                throw std::runtime_error("\nError: parameter --" + long_name +
                                         " (-" + std::string(1,short_name) +
                                         ") is not followed by a correct value");

        } while ( enforce_default_size && vec.size() < default_vals.size() && pos < args_.end() && (*pos)[0] != '-' );

        if ( enforce_default_size && vec.size() == 1 )
            for( int i=1; i<default_vals.size(); ++i)
                vec.push_back(vec[0]);

        if ( enforce_default_size && vec.size() != default_vals.size() ) {
            std::ostringstream oss;
            oss << "\nError: parameter --" << long_name;
            if ( short_name != ' ' )
                oss << " (-" << std::string(1,short_name) << ")";
            oss << " is not followed by " << default_vals.size() << " values as expected.";
            throw std::runtime_error(oss.str());
        }

        return vec;
    }

    // did not find anything:
    return default_vals;
}


/**
   @brief To get a parameter with multiple values, no short name.
   @param long_name long name of the parameter (so starting with "--").
   @param default_vals default values to give if the parameter is not present.
   @param enforce_default_size When true, expect to have the same number of elements than default_vals. If only one value is given,
   will replicate the value. If a different number of values is given will throw.
   @param desc A description of the parameter. (will go into the usage)
   @param separator the character used to separate values.
   @return the values of the parameter, returned as a vector.
 */
template <class T>
std::vector<T> CmdLineArgs::getParams(const std::string &long_name,
                                      const std::vector<T> &default_vals, bool enforce_default_size,
                                      const std::string &desc, char separator)
{
    return getParams<T>(long_name, ' ', default_vals, enforce_default_size, desc, separator);
}


/**
   @brief To get a flag (so an argument starting with "--" or "-", but not followed by a value)
   @param long_name long name of the flag (so starting with "--").
   @param short_name short name of the flag (so a single letter starting with "-", and can be aggregated).
   @param desc A description of the flag. (will go into the usage)
   @return the number of times the flag is present.
 */
int CmdLineArgs::getFlag(const std::string &long_name, char short_name, const std::string &desc)
{
    addUsage(long_name,short_name,"",desc);

    int nb=0;

    // First search the long names:
    auto pos = findLongName(long_name);
    while(pos != args_.end()){
        ++nb;
        pos = args_.erase(pos);
        pos = findLongName(long_name);
    }

    // Then search the short names:
    pos = findShortName(short_name);
    while(pos != args_.end()){
        ++nb;
        if(pos->size()==2)
            pos = args_.erase(pos);
        else
            pos->erase(pos->find(short_name,1),1);
        pos = findShortName(short_name);
    }

    return nb;
}


/**
   @brief To get a flag (only long name allowed)
   @param long_name long name of the flag (so starting with "--").
   @param desc A description of the flag. (will go into the usage)
   @return the number of times the flag is present.
 */
int CmdLineArgs::getFlag(const std::string &long_name, const std::string &desc)
{
    return getFlag(long_name,' ',desc);
}


/// @cond SPECIALISATIONS

// The following specialisations for string is necessary to cope with spaces inside strings!
//
std::string CmdLineArgs::getParam(const std::string &long_name, char short_name, const std::string &default_value, const std::string &desc)
{
    std::string val;
    addUsage(long_name, short_name, "\"" + default_value + "\"", desc);

    // First search the long names:
    auto pos = findLongName(long_name);
    if( pos != args_.end() ) {

        if(pos+2>args_.end())
            throw std::runtime_error("\nError: parameter --" + long_name + " is not followed by a value");

        pos = args_.erase(pos);
        val = *pos;
        args_.erase(pos);

        return val;
    }

    // Then search the short names:
    pos = findShortName(short_name);
    if( pos != args_.end() ) {

        if(pos+2>args_.end())
            throw std::runtime_error("\nError: parameter -" + std::string(1,short_name) + " is not followed by a value");

        if(pos->size()==2)
            pos = args_.erase(pos);
        else{
            pos->erase(pos->find(short_name,1),1);
            ++pos;
        }

        val = *pos;
        args_.erase(pos);

        return val;
    }

    // did not find anything:
    return default_value;
}

std::string CmdLineArgs::getParam(const std::string &long_name, const std::string &default_value, const std::string &desc)
{
    return getParam(long_name,' ',default_value,desc);
}

std::string CmdLineArgs::getParam(const std::string &long_name, char short_name, const char* default_value, const std::string &desc)
{
    return getParam(long_name,short_name,std::string(default_value),desc);
}

std::string CmdLineArgs::getParam(const std::string &long_name, const char* default_value, const std::string &desc)
{
    return getParam(long_name,' ',std::string(default_value),desc);
}


// The following specialisation for string is necessary to cope with spaces inside strings and to catch the delimiter
//
std::vector<std::string> CmdLineArgs::getParams(const std::string &long_name, char short_name,
                                                const std::vector<std::string> &default_vals, bool enforce_default_size,
                                                const std::string &desc, char separator)
{
    // Join the default values wit the separator for the usage
    //
    std::ostringstream oss;
    for(unsigned int i=0; i!=default_vals.size(); i++){
        if(i>0)
            oss << ",";
        oss << default_vals[i];
    }
    addUsage(long_name,short_name,oss.str(),desc);

    std::string val;
    std::vector<std::string> vec;

    // First search the long names:
    //
    auto pos = findLongName(long_name);
    if( pos != args_.end() ) {

        if( pos+2  > args_.end() )
            throw std::runtime_error("\nError: parameter --" + long_name + " is not followed by a value");

        pos = args_.erase(pos);
        val = *pos;
        args_.erase(pos);

        vec = split(val, separator);
    }

    // Then search the short names:
    //
    pos = findShortName(short_name);
    if( pos != args_.end() ) {

        if( pos+2 > args_.end() )
            throw std::runtime_error("\nError: parameter -" + std::string(1,short_name) + " is not followed by a value");

        if( pos->size() == 2 )
            pos = args_.erase(pos);
        else {
            pos->erase(pos->find(short_name, 1), 1);
            ++pos;
        }
        val = *pos;
        vec = split(val, separator);
    }

    if (vec.empty())
        return default_vals;

    if ( enforce_default_size && vec.size() == 1 )
        for( unsigned i=1; i<default_vals.size(); ++i)
            vec.push_back(vec[0]);

    if ( enforce_default_size && vec.size() != default_vals.size() ) {
        std::ostringstream oss;
        oss << "\nError: parameter --" << long_name;
        if ( short_name != ' ' )
            oss << " (-" << std::string(1,short_name) << ")";
        oss << " is not followed by " << default_vals.size() << " values as expected.";
        throw std::runtime_error(oss.str());
    }

    return vec;
}

std::vector<std::string> CmdLineArgs::getParams(const std::string &long_name,
                                                const std::vector<std::string> &default_vals, bool enforce_default_size,
                                                const std::string &desc, char separator)
{
    return getParams(long_name, ' ', default_vals, enforce_default_size, desc, separator);
}

/// @endcond


// Add some usage for a flag or parameter
//
void CmdLineArgs::addUsage(const std::string &long_name, char short_name, const std::string &default_val, const std::string &desc)
{
    std::string usage(4,' ');
    usage += "--" + long_name;

    if( short_name != ' ' ) {
        usage += " (-";
        usage += short_name;
        usage += ") ";
    }

    if( default_val.size() )
        usage += " (default: " + default_val + ")";

    usage_.push_back( std::pair<std::string, std::string>(usage,desc) );
}


/**
   @brief To add an usage separator in order to group together options.
   @param desc a title for the next group of options (parameters or flags).
 */
void CmdLineArgs::addUsageSeparator(const std::string &desc)
{
    usage_.push_back(std::pair<std::string, std::string>("SEP", desc));
}


/**
   @brief Add some description which will come after the descriptions of the different options.
   @param str The string to appear.
 */
void CmdLineArgs::addUsageOutro(const std::string &str)
{
    usage_outro_ += str;
}


/**
   @brief To get a nicely formatted usage.
   @return the usage string
 */
std::string CmdLineArgs::usage()
{
    // First find the maxim size of the arguments names:
    std::string::size_type left_size=0;
    for( auto pos=usage_.begin(); pos!=usage_.end(); ++pos ) {
        left_size = std::max(left_size,pos->first.size());
    }
    left_size += 5;

    std::string usage(usage_intro_ + "\n");
    for( auto pos=usage_.begin(); pos!=usage_.end(); ++pos ) {

        if(pos->first == "SEP"){
            usage += pos->second;
            usage += "\n";
        }else{
            usage += pos->first;
            if(pos->first.size()<left_size)
                usage += std::string(left_size-pos->first.size(),' ');
            std::string right(pos->second);
            std::string::size_type idx=0;
            while( (idx = right.find('\n',idx)) != std::string::npos){
                right.insert(idx+1,std::string(left_size,' '));
                idx += left_size;
            }
            usage += right;
            usage += '\n';
        }
    }

    usage += usage_outro_;

    return usage;
}


/**
   @brief To get the remaining options on the command line.
   @return the options as a vector of string.
 */
std::vector<std::string> CmdLineArgs::getRemaining()
{
    return args_;
}


/**
   @brief To get the remaining uparsed options on the command line.
   @return a vector of the unparsed options.
 */
std::vector<std::string> CmdLineArgs::getUnparsedOpts()
{
    std::vector<std::string> unparsed;
    for( auto &arg: args_ )
        if( arg[0] == '-' )
            unparsed.push_back(arg);
    return unparsed;
}


/**
   @brief To throw an exception if there are some remaining arguments on the command line.
 */
void CmdLineArgs::throwIfRemaining()
{
    if(!args_.empty()) {
        std::string msg("\nError: remaining args: ");
        for( auto &arg: args_ )
            msg += arg + " ";
        throw std::runtime_error(msg);
    }
}


/**
   @brief To throw an exception if there are some unparsed options on the command line.
 */
void CmdLineArgs::throwIfUnparsed()
{
    std::vector<std::string> unparsed = getUnparsedOpts();
    if( !unparsed.empty() ) {
        std::string msg("\nError: unparsed options: ");
        for( auto &arg: unparsed )
            msg += arg + " ";
        throw std::runtime_error(msg);
    }
}


/**
   @brief Tells if a given parameter or flag is present.
   @param long_name the long name of the parameter or flag. (so it will have "--" in front, as in "--blabla")
   @return true if the parameter or flag was found.
   @param short_name short name of the parameter (so a single letter starting with "-").
   @note You should call this function before you parse with getFlag, getParam  as they remove them.
 */
bool CmdLineArgs::isPresent(const std::string &long_name, char short_name)
{
    auto pos_long = findLongName(long_name);
    auto pos_short = findShortName(short_name);

    return pos_long != args_.end() || pos_short != args_.end() ;
}

