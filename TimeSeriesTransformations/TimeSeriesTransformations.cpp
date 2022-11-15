// TimeSeriesTransformations.cpp : Defines the functions for the static library.
#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <numeric>
#include <ctime>
#include <iomanip>
#include <stdexcept>
#include <algorithm>
#include <limits>
#include <memory>
#include "TimeSeriesTransformations.h"

// Helper functions.
std::vector<double> vectorDiff(const std::vector<double>& v) {
    std::vector<double> diff(v.size() - 1);

    for (int i = 0; i < v.size() - 1; i++) {
        diff[i] = v[i+1] - v[i];
    }

    return diff;
}

// Convert human readable date to unix epoch timestamp.
bool stringDateToUnix(const std::string& date, int* unix_epoch) {
    std::tm t{};
    std::istringstream string_stream(date);

	if (date.size() == 10) {
		string_stream >> std::get_time(&t, "%Y-%m-%d");
	} else {
		string_stream >> std::get_time(&t, "%Y-%m-%d %H:%M:%S");
	}

    // Check if the date has been parsed correctly.
    if (string_stream.fail()) {
        return false;
    }

    // I am going to cast this to an int for simplicity sake given this program
    // being used beyond 2038 seems rather unlikely.
    *unix_epoch = static_cast<int>(mktime(&t));

    return true;
}

// Convert unix epoch timestamp to a readable date format.
std::string unixEpochToString(const int input_time) {
    struct tm* timeinfo;
    time_t epoch_time = input_time;
    timeinfo = gmtime(&epoch_time);

    std::stringstream s;
    s << std::put_time(timeinfo, "%Y-%m-%d %H:%M:%S");
    return s.str();
}

// Check if a date is valid.
bool isDateValid(const std::string& date) {
    int unix_epoch;
    stringDateToUnix(date, &unix_epoch);

    std::string comparison_date = unixEpochToString(unix_epoch);

    if (comparison_date == date) {
        return true;
    } else {
        if (std::string(comparison_date.begin() + 11, comparison_date.end()) == "00:00:00") {
            return (std::string(comparison_date.begin(), comparison_date.end() - 9) == date);
        }
        return false;
    }
}

// Empty constructor.
TimeSeriesTransformations::TimeSeriesTransformations() { }

// Constructor using the filepath.
TimeSeriesTransformations::TimeSeriesTransformations(const std::string& filenameandpath) {
    std::string line;
    std::ifstream csv(filenameandpath);

    if (!csv.is_open()) {
        throw std::runtime_error("Unable to open file " + filenameandpath);
    }

    std::string string_time;
    std::string string_price;

    double power_of_10 = std::pow(10, decimalPlaces);

    // Get data from file header.
    std::getline(csv, line);
    std::istringstream xyz{ line };

    std::string column_name;

    // Just somewhere to ditch the time column header.
    std::getline(xyz, line, this->getSeparator());
    // Actual name of the share price column.
    std::getline(xyz, column_name, this->getSeparator());
    name = column_name;

    // Iterate down the file line by line until end of file.
    while (std::getline(csv, line)) {
        std::istringstream iss{ line };

        std::getline(iss, string_time, this->getSeparator());
        std::getline(iss, string_price, this->getSeparator());

        int time = std::stoi(string_time);
        double price = std::round(std::stod(string_price) * power_of_10) / power_of_10;

        timePricePairs.emplace_back( time, price );
    }

    sortInternals();
}

// Constructor from std::vector inputs directly.
TimeSeriesTransformations::TimeSeriesTransformations(const std::vector<int>& time, const std::vector<double>& price, const std::string& name) : name(name) {
    if (time.size() != price.size()) {
        throw std::runtime_error("Price and time vectors are not equally sized.");
    }

    for (int i = 0; i < price.size(); i++) {
        timePricePairs.emplace_back( time[i], price[i] );
    }

    sortInternals();
}

// Copy constructor.
TimeSeriesTransformations::TimeSeriesTransformations(const TimeSeriesTransformations& TSSObject) {
    name = TSSObject.getName();
    separator = TSSObject.getSeparator();
    timePricePairs = TSSObject.getTimePricePairs();
}

// Assignment Operator.
TimeSeriesTransformations& TimeSeriesTransformations::operator=(const TimeSeriesTransformations& TSSObject) {
    this->name = TSSObject.getName();
    this->separator = TSSObject.getSeparator();
    this->timePricePairs = TSSObject.getTimePricePairs();

    return (*this);
}

// Equality Operator.
bool TimeSeriesTransformations::operator==(const TimeSeriesTransformations& TSSObject) const {
    bool names_equal = (name == TSSObject.getName());
    bool separator_equal = (separator == TSSObject.getSeparator());
    bool time_and_price_equal = (timePricePairs == TSSObject.getTimePricePairs());
    return (names_equal && separator_equal && time_and_price_equal);
}

// Calculate mean of price.
bool TimeSeriesTransformations::mean(double* meanValue) const {
    if (timePricePairs.empty()) {
        *meanValue = std::numeric_limits<double>::quiet_NaN();
        return false;
    }

    double sum = 0.0;
    for(const auto& pair : timePricePairs) {
        sum += pair.second;
    }

    *meanValue = sum / timePricePairs.size();

    return true;
}

// Calculate SD of price.
bool TimeSeriesTransformations::standardDeviation(double* standardDeviationValue) const {
    if (timePricePairs.empty()) {
        *standardDeviationValue = std::numeric_limits<double>::quiet_NaN();
        return false;
    }

    double mean_val;
    this->mean(&mean_val);

    std::vector<double> transformed_price = getPriceVector();
    std::for_each(transformed_price.begin(), transformed_price.end(), [&](double& i) -> void { i = pow(i - mean_val, 2); });

    if (timePricePairs.empty()) { return 0; }

    double sum = std::reduce(transformed_price.begin(), transformed_price.end(), 0.0);

    *standardDeviationValue = std::sqrt((1.0 / static_cast<double>(timePricePairs.size() - 1)) * sum);

    return true;
}

// Calculate mean of diff of price.
bool TimeSeriesTransformations::computeIncrementMean(double* meanValue) const {
    if (timePricePairs.size() <= 1) {
        *meanValue = std::numeric_limits<double>::quiet_NaN();
        return false;
    }

    std::vector<double> priceVector = getPriceVector();
    std::vector<double> diff = vectorDiff(priceVector);

    std::vector<int> time_index(priceVector.size() - 1);
    std::iota(time_index.begin(), time_index.end(), 0);

    TimeSeriesTransformations ts(time_index, diff);
    return ts.mean(meanValue);
}

// Calculate SD of diff of price.
bool TimeSeriesTransformations::computeIncrementStandardDeviation(double* standardDeviationValue) const {
    if (timePricePairs.size() <= 1) {
        *standardDeviationValue = std::numeric_limits<double>::quiet_NaN();
        return false;
    }

    std::vector<double> priceVector = getPriceVector();
    std::vector<double> diff = vectorDiff(priceVector);

    std::vector<int> timeIndex(priceVector.size() - 1);
    std::iota(timeIndex.begin(), timeIndex.end(), 0);

    TimeSeriesTransformations ts(timeIndex, diff);
    return ts.standardDeviation(standardDeviationValue);
}

void TimeSeriesTransformations::addASharePrice(const std::string& datetime, double price) {
    int unix_epoch_time;
    if ((!stringDateToUnix(datetime, &unix_epoch_time)) || !isDateValid(datetime)) {
        throw std::invalid_argument("Date " + datetime + " cannot be parsed.");
    }
    
    timePricePairs.emplace_back(unix_epoch_time, price);
    sortInternals();
}

bool TimeSeriesTransformations::removeEntryAtTime(const std::string& time) {
    int unixEpochTime;
    if ((!stringDateToUnix(time, &unixEpochTime)) || !isDateValid(time)) {
        return false;
    }

    return std::erase_if(timePricePairs, [unixEpochTime](const auto& pair) { return (pair.first == unixEpochTime); });
}

bool TimeSeriesTransformations::removePricesBefore(const std::string& date) {
    int unixEpochTime;
    if ((!stringDateToUnix(date, &unixEpochTime)) || !isDateValid(date)) {
        return false;
    }
    return std::erase_if(timePricePairs, [unixEpochTime](const auto& pair) { return (pair.first < unixEpochTime); });
}

bool TimeSeriesTransformations::removePricesGreaterThan(double priceCondition) {
    return std::erase_if(timePricePairs, [priceCondition](const auto& pair) { return (pair.second > priceCondition); });
}

bool TimeSeriesTransformations::removePricesLowerThan(double priceCondition) {
    return std::erase_if(timePricePairs, [priceCondition](const auto& pair) { return (pair.second < priceCondition); });
}

bool TimeSeriesTransformations::removePricesAfter(const std::string& date) {
    int unixEpochTime;
    if ((!stringDateToUnix(date, &unixEpochTime)) || !isDateValid(date)) {
        return false;
    }
    return std::erase_if(timePricePairs, [unixEpochTime](const auto& pair) { return (pair.first > unixEpochTime); });
}

std::string TimeSeriesTransformations::printSharePricesOnDate(const std::string& date) const {
    std::string truncated_date = std::string(date.begin(), date.begin() + 10);
    int unix_epoch_time;
    // I'm deliberately leaving IsDateValid with date so it will still error if you put in an invalid date.
    if ((!stringDateToUnix(truncated_date, &unix_epoch_time)) || !isDateValid(date)) {
        throw std::invalid_argument("Date " + date + " cannot be parsed.");
    }

    TimeSeriesTransformations v(getTimeVector(), getPriceVector());

    v.removePricesBefore(date);

    // Recall that 86400 seconds in a day and removePricesAfter retains the passed in date (hence 86400-1).
    v.removePricesAfter(unixEpochToString(unix_epoch_time+86400-1));

    std::string string_of_prices = "";

    for (auto const i : v.getPriceVector()) {
        string_of_prices += std::to_string(i) + "\n";
    }

    return string_of_prices;
}

bool TimeSeriesTransformations::getPriceAtDate(const std::string& date, double* value) const {
    int unix_epoch_time;
    if ((!stringDateToUnix(date, &unix_epoch_time)) || !isDateValid(date)) {
        *value = std::numeric_limits<double>::quiet_NaN();
        return false;
    }

    for (auto const& pair : timePricePairs) {
        if (pair.first == unix_epoch_time) {*value = pair.second; return true; }
    }

    *value = std::numeric_limits<double>::quiet_NaN();
    return false;
}

std::string TimeSeriesTransformations::printIncrementsOnDate(const std::string& date) const {
    if (!isDateValid(date)) {
        throw std::invalid_argument("Date " + date + " cannot be parsed.");
    }

    if (timePricePairs.size() <= 1) { return ""; }

    std::vector<double> price_vector_diff = vectorDiff(getPriceVector());
    std::vector<int> time_vector_diff = getTimeVector();
    time_vector_diff.erase(time_vector_diff.begin());

    TimeSeriesTransformations v(time_vector_diff, price_vector_diff);

    return v.printSharePricesOnDate(date);
}

bool TimeSeriesTransformations::findGreatestIncrements(double* price_increment) const {
    if (timePricePairs.size() <= 1) {
        *price_increment = std::numeric_limits<double>::quiet_NaN();
        return false;
    }

    std::vector<double> increments = vectorDiff(getPriceVector());

    *price_increment = *std::max_element(increments.begin(), increments.end());
    return true;
}

std::string TimeSeriesTransformations::getName() const {
    return name;
}

void TimeSeriesTransformations::sortInternals() {
    std::sort(timePricePairs.begin(), timePricePairs.end(), [](auto& left, auto& right) {
        return left.first < right.first;
    });
}

std::vector<std::pair<int, double>> TimeSeriesTransformations::getTimePricePairs() const noexcept {
    return timePricePairs;
}

size_t TimeSeriesTransformations::count() const noexcept {
    return timePricePairs.size();
}

char TimeSeriesTransformations::getSeparator() const noexcept {
    return separator;
}

void TimeSeriesTransformations::saveData(const std::string& filename) const 
{
    std::ofstream new_csv;

    new_csv.open(filename);

    if (new_csv.is_open()) {
        // Adds header to csv.
        new_csv << "TIMESTAMP" << this->getSeparator() << name << std::endl;

        for (const auto& pair : timePricePairs) {
            new_csv << pair.first << separator << pair.second << std::endl;
        }
        new_csv.close();
    }
}

std::vector<double> TimeSeriesTransformations::getPriceVector() const {
    
    std::vector<double> priceVec{};

    for (const auto& element : timePricePairs) {
        priceVec.push_back(element.second);
    }
    
    return priceVec;
}

std::vector<int> TimeSeriesTransformations::getTimeVector() const {
    std::vector<int> timeVec{};

    for (const auto& element : timePricePairs) {
        timeVec.push_back(element.first);
    }

    return timeVec;
}
