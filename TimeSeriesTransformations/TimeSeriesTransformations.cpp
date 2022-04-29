// TimeSeriesTransformations.cpp : Defines the functions for the static library.
#define _CRT_SECURE_NO_WARNINGS
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
#include <stdlib.h>
#include <time.h>
#include "TimeSeriesTransformations.h"

// Helper functions.
std::vector<double> vectorDiff(std::vector<double> v) {
	std::vector<double> diff(v.size() - 1);

	for (int i = 0; i < v.size() - 1; i++) {
		diff[i] = v[(int64_t)(i)+1] - v[i];
	}

	return diff;
}

// Convert human readable date to unix epoch timestamp.
bool StringDateToUnix(std::string date, int* unix_epoch) {
	std::tm t{};
	std::istringstream string_stream(date);

	string_stream >> std::get_time(&t, "%Y-%m-%d %H:%M:%S");

	// Check if the date has been parsed correctly.
	if (string_stream.fail()) {
		return false;
	}

	// I am going to cast this to an int for simplicity sake given this program
	// being used beyond 2038 seems rather unlikely.
	*unix_epoch = (int) mktime(&t);

	return true;
}

// Convert unix epoch timestamp to a readable date format.
std::string UnixEpochToString(int input_time) {
	struct tm* timeinfo;
	time_t epoch_time = input_time;
	timeinfo = gmtime(&epoch_time);

	std::stringstream s;
	s << std::put_time(timeinfo, "%Y-%m-%d %H:%M:%S");
	return s.str();
}

// Check if a date is valid.
bool IsDateValid(std::string date) {
	int unix_epoch;
	StringDateToUnix(date, &unix_epoch);

	std::string comparison_date = UnixEpochToString(unix_epoch);

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
	std::getline(xyz, line, this->getSeparator()); // Just somewhere to ditch the time column
	std::getline(xyz, column_name, this->getSeparator()); // Actual name of the share price column.
	name = column_name;
	
	// Iterate down the file line by line until end of file.
	while (std::getline(csv, line)) {
		std::istringstream iss{ line };

		std::getline(iss, string_time, this->getSeparator());
		std::getline(iss, string_price, this->getSeparator());

		int time = std::stoi(string_time);
		double price = std::round(std::stod(string_price) * power_of_10) / power_of_10;

		internal_set.insert({ time, price });
	}

	for (auto const &pair : internal_set) {
		time_vector.push_back(pair.first);
		price_vector.push_back(pair.second);
	}

	// Sanity check.
	if (price_vector.size() != time_vector.size()) {
		throw std::runtime_error("Unable to read csv at " + filenameandpath + ". Columns are not equal in length."); 
	}
}

// Constructor from std::vector inputs directly.
TimeSeriesTransformations::TimeSeriesTransformations(const std::vector<int>& time, const std::vector<double>& price, std::string name) {
	if (time.size() != price.size()) { 
		throw std::runtime_error("Price and time vectors are not equally sized."); 
	}

	for (int i = 0; i < price.size(); i++) {
		internal_set.insert({ time[i], price[i] });
	}

	for (auto const& pair : internal_set) {
		time_vector.push_back(pair.first);
		price_vector.push_back(pair.second);
	}

	std::string series_name = name;
}

// Copy constructor.
TimeSeriesTransformations::TimeSeriesTransformations(const TimeSeriesTransformations& t) {
	price_vector = t.price_vector;
	time_vector = t.time_vector;
}

// Assignment Operator.
TimeSeriesTransformations& TimeSeriesTransformations::operator=(const TimeSeriesTransformations& t) {
	// Copy the data.
	price_vector = t.price_vector;
	time_vector = t.time_vector;

	// Return the dereferenced modified object.
	return *this;
}

// Equality Operator.
bool TimeSeriesTransformations::operator==(const TimeSeriesTransformations& t) const {
	// Because of && if (t.time == time) is false then (t.price == price) will not be evaluated. Saves time.
	// It is also better that (t.time == time) is evaluated first since integer comparison is faster than float comparison (though very minor).
	return ((t.time_vector == time_vector) && (t.price_vector == price_vector));
}

// Calculate mean of price.
bool TimeSeriesTransformations::mean(double* meanValue) const {
	if ((price_vector.size() == 0) || (time_vector.size() == 0)) { 
		*meanValue = std::numeric_limits<double>::quiet_NaN(); 
		return false;
	}

	double sum = std::accumulate(price_vector.begin(), price_vector.end(), 0.0);

	*meanValue = sum / price_vector.size();

	return true;
}

// Calculate SD of price.
bool TimeSeriesTransformations::standardDeviation(double* standardDeviationValue) const {
	if ((price_vector.size() == 0) || (time_vector.size() == 0)) {
		*standardDeviationValue = std::numeric_limits<double>::quiet_NaN();
		return false;
	}

	double mean_val;
	this->mean(&mean_val);

	std::vector<double> transformed_price = price_vector;
	std::for_each(transformed_price.begin(), transformed_price.end(), [mean_val](double& i) -> void { i = pow(i - mean_val, 2); });

	if (price_vector.size() == 0) { return 0; }

	double sum = std::accumulate(transformed_price.begin(), transformed_price.end(), 0.0);

	*standardDeviationValue = std::sqrt((1.0 / static_cast<double>(price_vector.size() - 1))*sum);

	return true;
}

// Calculate mean of diff of price.
bool TimeSeriesTransformations::computeIncrementMean(double* meanValue) const {
	if ((price_vector.size() == 0) || (time_vector.size() == 0)) {
		*meanValue = std::numeric_limits<double>::quiet_NaN();
		return false;
	}

	std::vector<double> diff = vectorDiff(price_vector);

	std::vector<int> time_index(price_vector.size() - 1);
	std::iota(time_index.begin(), time_index.end(), 0);

	TimeSeriesTransformations ts(time_index, diff);
	return ts.mean(meanValue);
}

// Calculate SD of diff of price.
bool TimeSeriesTransformations::computeIncrementStandardDeviation(double* standardDeviationValue) const {
	if ((price_vector.size() == 0) || (time_vector.size() == 0)) {
		*standardDeviationValue = std::numeric_limits<double>::quiet_NaN();
		return false;
	}

	std::vector<double> diff = vectorDiff(price_vector);

	std::vector<int> time_index(price_vector.size() - 1);
	std::iota(time_index.begin(), time_index.end(), 0);

	TimeSeriesTransformations ts(time_index, diff);
	return ts.standardDeviation(standardDeviationValue);
}

void TimeSeriesTransformations::addASharePrice(std::string datetime, double price) {
	int unix_epoch_time; // ??? Should this throw an error or "".
	if ((!StringDateToUnix(datetime, &unix_epoch_time)) || !IsDateValid(datetime)) {
		throw std::invalid_argument("Date " + datetime + " cannot be parsed.");
	}

	double power_of_10 = std::pow(10, decimalPlaces);
	internal_set.insert({ unix_epoch_time, std::round(price * power_of_10) / power_of_10 });

	// Vector objects must be re-allocated because of new elements in the set.
	price_vector.clear();
	time_vector.clear();

	for (auto const& pair : internal_set) {
		time_vector.push_back(pair.first);
		price_vector.push_back(pair.second);
	}
}

bool TimeSeriesTransformations::removeEntryAtTime(std::string time) {
	int unix_epoch_time;
	if ((!StringDateToUnix(time, &unix_epoch_time)) || !IsDateValid(time)) {
		return false;
	}

	std::set<std::pair<int, double>, sorting_struct> tmp_internal_set;
	bool element_removed = false;

	// Vector objects must be re-allocated because of new elements in the set.
	price_vector.clear();
	time_vector.clear();

	// Iterate through the set and find the element to erase.
	for (auto const& pair : internal_set) {
		if (pair.first != unix_epoch_time) {
			time_vector.push_back(pair.first);
			price_vector.push_back(pair.second);
			tmp_internal_set.insert({pair.first, pair.second});
		}
		else { 
			element_removed = true;
		}
	}

	internal_set = tmp_internal_set;
	tmp_internal_set.clear();

	return element_removed;
}

bool TimeSeriesTransformations::removePricesGreaterThan(double price) {
	std::set<std::pair<int, double>, sorting_struct> tmp_internal_set;

	price_vector.clear();
	time_vector.clear();

	for (auto const& pair : internal_set) {
		if (pair.second <= price) {
			tmp_internal_set.insert({ pair.first, pair.second });

			time_vector.push_back(pair.first);
			price_vector.push_back(pair.second);
		}
	}

	internal_set = tmp_internal_set;
	tmp_internal_set.clear();

	// I see no real reason this should fail, even if no elements actually match the condition.
	return true;
}

bool TimeSeriesTransformations::removePricesLowerThan(double price) {
	std::set<std::pair<int, double>, sorting_struct> tmp_internal_set;

	price_vector.clear();
	time_vector.clear();

	for (auto const& pair : internal_set) {
		if (price <= pair.second) {
			tmp_internal_set.insert({ pair.first, pair.second });

			time_vector.push_back(pair.first);
			price_vector.push_back(pair.second);
		}
	}

	internal_set = tmp_internal_set;
	tmp_internal_set.clear();

	// I see no real reason this should fail, even if no elements actually match the condition.
	return true;
}

bool TimeSeriesTransformations::removePricesBefore(std::string date) {
	int unix_epoch_time;
	if ((!StringDateToUnix(date, &unix_epoch_time)) || !IsDateValid(date)) {
		return false;
	}

	std::set<std::pair<int, double>, sorting_struct> tmp_internal_set;

	price_vector.clear();
	time_vector.clear();

	for (auto const& pair : internal_set) {
		if (unix_epoch_time <= pair.first) {
			tmp_internal_set.insert({ pair.first, pair.second });

			time_vector.push_back(pair.first);
			price_vector.push_back(pair.second);
		}
	}

	internal_set = tmp_internal_set;
	tmp_internal_set.clear();

	return true;
}

bool TimeSeriesTransformations::removePricesAfter(std::string date) {
	int unix_epoch_time;
	if ((!StringDateToUnix(date, &unix_epoch_time)) || !IsDateValid(date)) {
		return false;
	}

	std::set<std::pair<int, double>, sorting_struct> tmp_internal_set;

	price_vector.clear();
	time_vector.clear();

	for (auto const& pair : internal_set) {
		if (unix_epoch_time >= pair.first) {
			tmp_internal_set.insert({ pair.first, pair.second });

			time_vector.push_back(pair.first);
			price_vector.push_back(pair.second);
		}
	}

	internal_set = tmp_internal_set;
	tmp_internal_set.clear();

	return true;
}

std::string TimeSeriesTransformations::printSharePricesOnDate(std::string date) const {
	int unix_epoch_time; // ??? Should this throw an error or "".
	if ((!StringDateToUnix(date, &unix_epoch_time)) || !IsDateValid(date)) {
		throw std::invalid_argument("Date " + date + " cannot be parsed.");
	}

	TimeSeriesTransformations v(this->time_vector, this->price_vector);

	v.removePricesBefore(date);

	// Recall that 86400 seconds in a day.
	v.removePricesAfter(UnixEpochToString(unix_epoch_time+86400-1));

	std::string string_of_prices = "";

	for (auto const i : v.price_vector) {
		string_of_prices += std::to_string(i) + "\n";
	}
	
	return string_of_prices;
}

bool TimeSeriesTransformations::getPriceAtDate(const std::string date, double* value) const {
	int unix_epoch_time; // ??? Should this throw an error or "".
	if ((!StringDateToUnix(date, &unix_epoch_time)) || !IsDateValid(date)) {
		*value = std::numeric_limits<double>::quiet_NaN();
		return false;
	}

	for (auto const pair : internal_set) {
		if (pair.first == unix_epoch_time) {*value = pair.second; return true; }
	}

	*value = std::numeric_limits<double>::quiet_NaN();
	return false;
}

std::string TimeSeriesTransformations::printIncrementsOnDate(std::string date) const {
	// ??? Should this throw an error or "".
	if (!IsDateValid(date)) {
		throw std::invalid_argument("Date " + date + " cannot be parsed.");
	}

	if (price_vector.size() <= 1) { return ""; }

	std::vector<double> price_vector_diff = vectorDiff(price_vector);
	std::vector<int> time_vector_diff = time_vector;
	time_vector_diff.erase(time_vector_diff.begin());

	TimeSeriesTransformations v(time_vector_diff, price_vector_diff);

	return v.printSharePricesOnDate(date);
}

bool TimeSeriesTransformations::findGreatestIncrements(std::string* date, double* price_increment) const {
	std::string output = this->printIncrementsOnDate(*date);
	std::string tmp;

	if (output == "") {
		*price_increment = std::numeric_limits<double>::quiet_NaN();
		return false;
	}

	std::vector<double> increments;
	std::stringstream ss(output);

	while (std::getline(ss, tmp)) {
		increments.push_back(std::stod(tmp));
	}

	*price_increment = *std::max_element(increments.begin(), increments.end());
	return true;
}

std::string TimeSeriesTransformations::getName() const { 
	return this->name; 
}

int TimeSeriesTransformations::count() const {
	return (int) this->price_vector.size();
}

char TimeSeriesTransformations::getSeparator() const {
	return ',';
}

void TimeSeriesTransformations::saveData(std::string filename) const {
	std::ofstream new_csv;

	new_csv.open(filename + ".csv");

	if (new_csv.is_open()) {
		new_csv << "TIMESTAMP," << name << std::endl;

		for (const auto& pair : internal_set) {
			new_csv << pair.first << ',' << pair.second << std::endl;
		}
		new_csv.close();
	}
}
