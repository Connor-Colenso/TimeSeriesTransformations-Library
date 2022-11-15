#pragma once
#include <string>
#include <vector>
#include <utility>
#include <set>
#include <memory>

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

class TimeSeriesTransformations {
	void sortInternals();

	const int decimalPlaces = 5;
	std::vector<std::pair<int, double>> timePricePairs;

public:
	// Constructors
	TimeSeriesTransformations();
	explicit TimeSeriesTransformations(const std::string& filenameAndPath);
	TimeSeriesTransformations(const std::vector<int>& timeVec, const std::vector<double>& priceVec, const std::string& name = "");
	TimeSeriesTransformations(const TimeSeriesTransformations& TSSObject);

	// Operator overloads.
	TimeSeriesTransformations& operator=(const TimeSeriesTransformations& TTSObject);
	bool operator==(const TimeSeriesTransformations& TTSObject) const;

	bool mean(double* meanValue) const;
	bool standardDeviation(double* standardDeviationValue) const;
	bool computeIncrementMean(double* meanValue) const;
	bool computeIncrementStandardDeviation(double* standardDeviationValue) const;
	void addASharePrice(const std::string& date, double price);
	bool removeEntryAtTime(const std::string& date);
	bool removePricesGreaterThan(double price);
	bool removePricesLowerThan(double price);
	bool removePricesBefore(const std::string& date);
	bool removePricesAfter(const std::string& date);
	std::string printSharePricesOnDate(const std::string& date) const;
	std::string printIncrementsOnDate(const std::string& date) const;
	bool findGreatestIncrements(double* price_increment) const;
	bool getPriceAtDate(const std::string& date, double* value) const;
	void saveData(const std::string& filename) const;
	size_t count() const noexcept;
	std::string getName() const noexcept;
	std::vector<std::pair<int, double>> getTimePricePairs() const noexcept;

	char getSeparator() const noexcept;
	char separator = ',';

	std::string name = "";

	std::vector<int> getTimeVector() const;
	std::vector<double> getPriceVector() const;
};
