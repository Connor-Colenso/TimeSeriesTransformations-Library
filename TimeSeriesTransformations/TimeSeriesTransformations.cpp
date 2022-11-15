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

	if (v.size() < 2) {
		throw std::invalid_argument("Vector provided to vectorDiff must be at least two elements.");
	}

	std::vector<double> diff;
	std::adjacent_difference(v.begin(), v.end(), std::back_inserter(diff));
	diff.erase(diff.begin());

	return diff;
}

// Convert human readable date to unix epoch timestamp.
bool stringDateToUnix(const std::string& date, int* unix_epoch) {
	std::tm t{};
	std::istringstream string_stream(date);

	if (date.size() == 10) {
		string_stream >> std::get_time(&t, "%Y-%m-%d");
	}
	else {
		string_stream >> std::get_time(&t, "%Y-%m-%d %H:%M:%S");
	}

	// Check if the date has been parsed correctly.
	if (string_stream.fail()) {
		return false;
	}

	*unix_epoch = mktime(&t);

	return true;
}

// Convert unix epoch timestamp to a readable date format.
std::string unixEpochToString(const int input_time) {
	struct tm* timeinfo;
	time_t epochTime = input_time;
	timeinfo = gmtime(&epochTime);

	std::stringstream s;
	s << std::put_time(timeinfo, "%Y-%m-%d %H:%M:%S");
	return s.str();
}

// Check if a date is valid.
bool isDateValid(const std::string& date) {
	int unixEpochTimestamp;
	stringDateToUnix(date, &unixEpochTimestamp);

	std::string comparisonDate = unixEpochToString(unixEpochTimestamp);

	if (comparisonDate == date) {
		return true;
	}
	else {
		if (std::string(comparisonDate.begin() + 11, comparisonDate.end()) == "00:00:00") {
			return (std::string(comparisonDate.begin(), comparisonDate.end() - 9) == date);
		}
		return false;
	}
}

// Empty constructor.
TimeSeriesTransformations::TimeSeriesTransformations() { }

// Constructor using the filepath.
TimeSeriesTransformations::TimeSeriesTransformations(const std::string& filenameAndPath) {
	std::string line;
	std::ifstream csv(filenameAndPath);

	if (!csv.is_open()) {
		throw std::runtime_error("Unable to open file " + filenameAndPath);
	}

	std::string stringTime;
	std::string stringPrice;

	double powerOf10 = std::pow(10, decimalPlaces);

	// Get data from file header.
	std::getline(csv, line);
	std::istringstream xyz{ line };

	std::string colName;

	// Just somewhere to ditch the time column header.
	std::getline(xyz, line, this->getSeparator());
	// Actual name of the share price column.
	std::getline(xyz, colName, this->getSeparator());
	name = colName;

	// Iterate down the file line by line until end of file.
	while (std::getline(csv, line)) {
		std::istringstream iss{ line };

		std::getline(iss, stringTime, this->getSeparator());
		std::getline(iss, stringPrice, this->getSeparator());

		int time = std::stoi(stringTime);
		double price = std::round(std::stod(stringPrice) * powerOf10) / powerOf10;

		timePricePairs.emplace_back(time, price);
	}

	sortInternals();
}

// Constructor from std::vector inputs directly.
TimeSeriesTransformations::TimeSeriesTransformations(const std::vector<int>& time, const std::vector<double>& price, const std::string& name) : name(name) {
	if (time.size() != price.size()) {
		throw std::runtime_error("Price and time vectors are not equally sized.");
	}

	for (int i = 0; i < price.size(); i++) {
		timePricePairs.emplace_back(time[i], price[i]);
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
	bool namesEqual = (name == TSSObject.getName());
	bool separatorEqual = (separator == TSSObject.getSeparator());
	bool timeAndPriceEqual = (timePricePairs == TSSObject.getTimePricePairs());
	return (namesEqual && separatorEqual && timeAndPriceEqual);
}

// Calculate mean of price.
bool TimeSeriesTransformations::mean(double* meanValue) const {
	if (timePricePairs.empty()) {
		*meanValue = std::numeric_limits<double>::quiet_NaN();
		return false;
	}

	double sum = 0.0;
	for (const auto& pair : timePricePairs) {
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

	double meanVal;
	this->mean(&meanVal);

	std::vector<double> transformedPrice = getPriceVector();
	std::for_each(transformedPrice.begin(), transformedPrice.end(), [&](double& i) -> void { i = pow(i - meanVal, 2); });

	if (timePricePairs.empty()) { return 0; }

	double sum = std::reduce(transformedPrice.begin(), transformedPrice.end(), 0.0);

	*standardDeviationValue = std::sqrt((1.0 / double(timePricePairs.size() - 1)) * sum);

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

	std::vector<int> timeIndex(priceVector.size() - 1);
	std::iota(timeIndex.begin(), timeIndex.end(), 0);

	TimeSeriesTransformations ts(timeIndex, diff);
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
	int unixEpochTime;
	if ((!stringDateToUnix(datetime, &unixEpochTime)) || !isDateValid(datetime)) {
		throw std::invalid_argument("Date " + datetime + " cannot be parsed.");
	}

	timePricePairs.emplace_back(unixEpochTime, price);
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
	std::string truncatedDate = std::string(date.begin(), date.begin() + 10);
	int unixEpochTime;
	// I'm deliberately leaving IsDateValid with date so it will still error if you put in an invalid date.
	if ((!stringDateToUnix(truncatedDate, &unixEpochTime)) || !isDateValid(date)) {
		throw std::invalid_argument("Date " + date + " cannot be parsed.");
	}

	TimeSeriesTransformations v(getTimeVector(), getPriceVector());

	v.removePricesBefore(date);

	// Recall that 86400 seconds in a day and removePricesAfter retains the passed in date (hence 86400-1).
	v.removePricesAfter(unixEpochToString(unixEpochTime + 86400 - 1));

	std::string stringOfPrices = "";

	for (auto const i : v.getPriceVector()) {
		stringOfPrices += std::to_string(i) + "\n";
	}

	return stringOfPrices;
}

bool TimeSeriesTransformations::getPriceAtDate(const std::string& date, double* value) const {
	int unixEpochTime;
	if ((!stringDateToUnix(date, &unixEpochTime)) || !isDateValid(date)) {
		*value = std::numeric_limits<double>::quiet_NaN();
		return false;
	}

	for (auto const& pair : timePricePairs) {
		if (pair.first == unixEpochTime) { *value = pair.second; return true; }
	}

	*value = std::numeric_limits<double>::quiet_NaN();
	return false;
}

std::string TimeSeriesTransformations::printIncrementsOnDate(const std::string& date) const {
	if (!isDateValid(date)) {
		throw std::invalid_argument("Date " + date + " cannot be parsed.");
	}

	if (timePricePairs.size() <= 1) { return ""; }

	std::vector<double> priceVecDiff = vectorDiff(getPriceVector());
	std::vector<int> timeVecDiff = getTimeVector();
	timeVecDiff.erase(timeVecDiff.begin());

	TimeSeriesTransformations TSSObject(timeVecDiff, priceVecDiff);

	return TSSObject.printSharePricesOnDate(date);
}

bool TimeSeriesTransformations::findGreatestIncrements(double* priceIncrement) const {
	if (timePricePairs.size() <= 1) {
		*priceIncrement = std::numeric_limits<double>::quiet_NaN();
		return false;
	}

	std::vector<double> increments = vectorDiff(getPriceVector());

	*priceIncrement = *std::max_element(increments.begin(), increments.end());
	return true;
}

std::string TimeSeriesTransformations::getName() const noexcept {
	return name;
}

void TimeSeriesTransformations::sortInternals() {
	std::sort(timePricePairs.begin(), timePricePairs.end(), [](const auto& left, const auto& right) {
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
	std::ofstream newCSV;

	newCSV.open(filename);

	if (newCSV.is_open()) {
		// Adds header to csv.
		newCSV << "TIMESTAMP" << this->getSeparator() << name << std::endl;

		for (const auto& pair : timePricePairs) {
			newCSV << pair.first << separator << pair.second << std::endl;
		}
		newCSV.close();
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
