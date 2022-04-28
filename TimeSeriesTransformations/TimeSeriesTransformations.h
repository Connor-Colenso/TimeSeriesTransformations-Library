#pragma once
#include <string>
#include <vector>
#include <utility>
#include <set>

// This is a utility function for the std::set comparisons.
struct sorting_struct {
    template<typename T>
    bool operator()(const T& l, const T& r) const {
        if (l.first == r.first) {
            return l.second < r.second;
        }
        return l.first < r.first;
    }
};

/*
    Convention; PascalCase for classes, constructors and destructors, camelCase for members and other functions
    Throw exceptions from constructors if things go wrong. std::runtime_error ; google RAII
    I've implemented things on a fail as hard as possible as early as possible basis, prevents misunderstandings

    Wait till you're absolutely happy with the code, before you run the google stylesheet code cpplint. You don't want to keep doing it.
    Alternatively write a bit of code, check it and try to write to those guidelines to save yourself a mammoth task at the end.

    cpplint is a python program, I installed using pip in a virtual environment; the commands to install should look something like this
    python -m venv env
    <go into your environment, depends on your operating system; for windows powershell it's .\env\Scripts\Activate.PS1>
    python -m pip install --upgrade pip
    python -m pip install cpplint
    
    I'll be using this when grading.
    cpplint --filter=-legal,-runtime/threadsafe_fn,-whitespace/line_length  <name of file>
    
    I've run these fragments through this code.

    Google stylesheet recommends passing parameters that can be changed in a function via pointer instead of non-const reference.
    This is so you can see if a call changes a parameter from the call site not the function definition. If it's not possible to calculate a value;
    you should return std::numeric_limits<double>::quiet_NaN() for the numerical value

    The idea of the code, is you can set things up internally how you want. 
    That's why dates are supplied to functions as text rather than assuming any internal format.

    Any times in this code have the timezone UTC. Dates are really, really hard to get right (that's get right, rather than look right),
    so have a think, build helper functions and test. There's also the chrono library. I just
    used standard c++/c functions. I expect times, where given or returned as strings to be of the form %Y-%m-%d %H:%M:%S, or a shortened version

    You can write as many functions as you need, and I suggest you do, to complete this assignment.
    You can call those functions from any function I've defined. Just don't change anything here.

    To reiterate what I said in the previous assignment; don't worry about creating your own namespaces, it's overkill and your code may not mark.

    When you save files, make sure to save prices to an appropriate number of decimal places. Otherwise when it's reloaded, you've lost accuracy. 

    I've not hard-coded the Separator; a word of warning when dealing with files from outside the UK, in France for instance, the comma is a decimal point, not a delimiter. Yes I have lost time to that. IRL I always use | for the separator in my work.
 
    Reading in CSV files can give you odd results if the last newline character in file is on the last line of data.

    You may get warnings like "Warning	C26812	The enum type 'testing::TestPartResult::Type' is unscoped. Prefer 'enum class' over 'enum' (Enum.3)." when compiling with googletest
    Don't worry, I shall ignore these.

    For removePricesBefore and removePrices after, these are exclusive in the tests; i.e. they don't include the value passed. So if you pass in "2022-03-18 00:13:25", a data point at that time would be retained.
    
    In removePricesAfter, remember that 2021-04-21 is a point in time, not a day. It's treated as 2021-04-21 00:00:00.

    For the printDates function; if you pass a datetime , it should show you all the values on the same day, i.e. should convert datetime to [startDay, endDay)
    Increments can be a bit ill defined. I've defined an increment at time i to have value   value[i+1] - value[i]; mid-point in time might be better. I've not corrected for any variation in time step, and you shouldn't worry about it.
    So to print increments, find those with time i above, that fall in that day. Don't try and correct the end points.

    Assume each test in test.cpp is worth 2%

    TimeSeriesTransformationsApplication is there for you to make a UI if it makes things easier for you.

    I'll give 10% for compiling without warnings already stated and 5% for style. Each warning will cost 1%.

    I will look at the code and give 10% extra, up to a maximum mark of 100% for things the markers consider to be excellent. 
 */

class TimeSeriesTransformations {
 public:
    TimeSeriesTransformations();

     std::vector<int> time_vector;
     std::vector<double> price_vector;

    // Probably have an option to state whether there a header in csv file,
    // or you could detect and ignore.
    explicit TimeSeriesTransformations(const std::string & filenameandpath);

    TimeSeriesTransformations(const std::vector<int>& time, const std::vector<double>& price, std::string name = "");

    //// Copy Constructor
    TimeSeriesTransformations(const TimeSeriesTransformations& t);

    //// Assignment Operator
    TimeSeriesTransformations& operator=(const TimeSeriesTransformations& t);

    //// equality Operator
    bool operator== (const TimeSeriesTransformations& t) const;
    bool mean(double* meanValue) const;
    bool standardDeviation(double* standardDeviationValue) const;
    bool computeIncrementMean(double* meanValue) const;
    bool computeIncrementStandardDeviation(double* standardDeviationValue) const;
    void addASharePrice(std::string datetime, double price);
    bool removeEntryAtTime(std::string time);
    bool removePricesGreaterThan(double price);
    bool removePricesLowerThan(double price);
    bool removePricesBefore(std::string date);
    bool removePricesAfter(std::string date);
    std::string printSharePricesOnDate(std::string date) const;
    std::string printIncrementsOnDate(std::string date) const;
    bool findGreatestIncrements(std::string* date, double* price_increment) const;
    bool getPriceAtDate(const std::string date, double* value) const;
    //void saveData(std::string filename) const;
    int count() const;
    std::string getName() const;
    char getSeparator() const;

 private:
    const int decimalPlaces = 5;
    std::string name = "";

    // This is really handy as it automatically sorts itself.
    std::set<std::pair<int, double>, sorting_struct> internal_set;
};
