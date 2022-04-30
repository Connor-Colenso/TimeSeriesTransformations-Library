#include "gtest/gtest.h"
#include "../TimeSeriesTransformations/TimeSeriesTransformations.h"

// I've included this to check when you read in files for comparison.
bool test(double in, double in2) {
    return abs((in - in2) / in) < 0.00001;
}

TEST(TimeSeriesTransformations, loadFileExaminationFile) {
    // Load file.
    TimeSeriesTransformations v("C:\\Users\\Admin\\Desktop\\AMF-Assignment3\\Problem3_DATA.csv");

    // Check start value.
    EXPECT_NEAR(v.getPriceVector()[0], 61.43814, 10e-5);
    EXPECT_EQ(v.getTimeVector()[0], 1619120010);

    int vector_length = static_cast<int>(v.getPriceVector().size());

    // Check end value.
    EXPECT_NEAR(v.getPriceVector()[vector_length - 1], 90.50422, 10e-5);
    EXPECT_EQ(v.getTimeVector()[vector_length - 1], 1669115010);

    // Alternative constructor.
    std::vector<int> time_vec = { 1, 2, 3, 4 };
    std::vector<double> price_vec = { 100, 110, 120, 110 };

    TimeSeriesTransformations v1(time_vec, price_vec);

    EXPECT_EQ(v1.getTimeVector(), time_vec);
    EXPECT_EQ(v1.getPriceVector(), price_vec);
}

TEST(TimeSeriesTransformations, meanOfExaminationFile) {
    TimeSeriesTransformations v("C:\\Users\\Admin\\Desktop\\AMF-Assignment3\\Problem3_DATA.csv");

    // Create ptr to double and pass through mean function.
    double mean_output;
    v.mean(&mean_output);

    EXPECT_NEAR(mean_output, 51.5734, 10e-5);
}

TEST(TimeSeriesTransformations, standardDeviationOfExaminationFile) {
    TimeSeriesTransformations v("C:\\Users\\Admin\\Desktop\\AMF-Assignment3\\Problem3_DATA.csv");

    // Create ptr to double and pass through standardDeviation function.
    double sd_output;
    v.standardDeviation(&sd_output);

    EXPECT_NEAR(sd_output, 28.703254238387661, 10e-6);
}

TEST(TimeSeriesTransformations, incrementMeanOfExaminationFile) {
    TimeSeriesTransformations v("C:\\Users\\Admin\\Desktop\\AMF-Assignment3\\Problem3_DATA.csv");

    // Create ptr to double and pass through standardDeviation function.
    double mean_output;
    v.computeIncrementMean(&mean_output);

    EXPECT_NEAR(mean_output, 0.0029068986898690904, 10e-6);

    // Second test. Ensures ordered state.
    TimeSeriesTransformations v_1({ 1, 5, 4, 2, 3 }, { 1, 5, 4, 2, 3 });
    double mean_output_1;
    v_1.computeIncrementMean(&mean_output_1);

    EXPECT_EQ(mean_output_1, 1);
}

TEST(TimeSeriesTransformations, incrementStandardDeviationOfExaminationFile) {
    TimeSeriesTransformations v("C:\\Users\\Admin\\Desktop\\AMF-Assignment3\\Problem3_DATA.csv");

    double sd_output;
    v.computeIncrementStandardDeviation(&sd_output);

    EXPECT_NEAR(sd_output, 40.35944404799585, 10e-6);

    // Second test. Ensures ordered state.
    TimeSeriesTransformations v_1({ 1, 5, 4, 2, 3 }, { 1, 5, 4, 2, 3 });
    double sd_output_1;
    v_1.computeIncrementStandardDeviation(&sd_output_1);

    EXPECT_EQ(sd_output_1, 0);
}

// test files with no data;

TEST(TimeSeriesTransformations, meanOfEmptyFileWithHeader) {
    TimeSeriesTransformations v("C:\\Users\\Admin\\Desktop\\AMF-Assignment3\\empty_with_header.csv");
    double mean_output;
    EXPECT_EQ(v.mean(&mean_output), false);
    EXPECT_TRUE(std::isnan(mean_output));
}

TEST(TimeSeriesTransformations, standardDeviationEmptyFileOfEmptyFileWithHeader) {
    TimeSeriesTransformations v("C:\\Users\\Admin\\Desktop\\AMF-Assignment3\\empty_with_header.csv");
    double standard_deviation;
    EXPECT_EQ(v.standardDeviation(&standard_deviation), false);
    EXPECT_TRUE(std::isnan(standard_deviation));
}

TEST(TimeSeriesTransformations, incrementMeanOfEmptyFileWithHeader) {
    TimeSeriesTransformations v("C:\\Users\\Admin\\Desktop\\AMF-Assignment3\\empty_with_header.csv");

    double mean_output;
    EXPECT_EQ(v.computeIncrementMean(&mean_output), false);
    EXPECT_TRUE(std::isnan(mean_output));
}

TEST(TimeSeriesTransformations, incrementMeanUnorderedFileWithData) {
    TimeSeriesTransformations v({ 4, 1, 2, 3 }, { 1, 2, 3, 4 });

    double mean_output;
    EXPECT_EQ(v.computeIncrementMean(&mean_output), true);
    EXPECT_NEAR(mean_output, -1.0 / 3.0, 10e-6);
}

TEST(TimeSeriesTransformations, incrementStandardDeviationEmptyFileWithHeader) {
    TimeSeriesTransformations v("C:\\Users\\Admin\\Desktop\\AMF-Assignment3\\empty_with_header.csv");
    double sd_output;
    EXPECT_EQ(v.computeIncrementStandardDeviation(&sd_output), false);
    EXPECT_TRUE(std::isnan(sd_output));
}
//------------------------------------------------- NAME

TEST(TimeSeriesTransformations, testNameEmptyFileWithHeader) {
    TimeSeriesTransformations v("C:\\Users\\Admin\\Desktop\\AMF-Assignment3\\empty_with_header.csv");
    EXPECT_EQ(v.getName(), "ShareX");
}

TEST(TimeSeriesTransformations, testNameOfExaminationFile) {
    TimeSeriesTransformations v("C:\\Users\\Admin\\Desktop\\AMF-Assignment3\\Problem3_DATA.csv");
    EXPECT_EQ(v.getName(), "ShareX");
}

// ----Test Empty file exception
TEST(TimeSeriesTransformations, throwRunTimeFromFileDoesNotExist) {
    EXPECT_THROW(TimeSeriesTransformations v("C:\\Users\\Admin\\Desktop\\AMF-Assignment3\\file_does_not_exist.csv"), std::runtime_error);
}

//--------------- Test build a TimeSeriesTransformation from vectors
TEST(TimeSeriesTransformations, buildFromTSTFromFullEqualTimePriceVectors) {
    TimeSeriesTransformations v({ 10, 20, 30 }, { 1, 2, 3 });

    EXPECT_EQ(v.getTimeVector()[0], 10);
    EXPECT_EQ(v.getTimeVector()[1], 20);
    EXPECT_EQ(v.getTimeVector()[2], 30);

    EXPECT_EQ(v.getPriceVector()[0], 1);
    EXPECT_EQ(v.getPriceVector()[1], 2);
    EXPECT_EQ(v.getPriceVector()[2], 3);
}

TEST(TimeSeriesTransformations, throwRuntimeErrorFromTSTWithUnequalTimePriceVectors) {
    EXPECT_THROW(TimeSeriesTransformations v({ 10, 20, 30 }, { 1, 2 }), std::runtime_error);
}

// Check getPriceAtDate
TEST(TimeSeriesTransformations, getAPriceAtADateThatExists) {
    TimeSeriesTransformations v({ 1, 86400, 86401 }, { 9, 5, 1 });

    double value = 0;
    EXPECT_TRUE(v.getPriceAtDate("1970-01-01 00:00:01", &value));
    EXPECT_EQ(value, 9);
}

TEST(TimeSeriesTransformations, getAPriceAtADateThatDoesNotExist) {
    TimeSeriesTransformations v({ 1, 86400, 86401 }, { 9, 5, 1 });

    double value = 0;
    EXPECT_FALSE(v.getPriceAtDate("1970-01-01 00:00:02", &value));
    EXPECT_TRUE(std::isnan(value));
}

// Check empty constructor
TEST(TimeSeriesTransformations, checkEmptyConstructor) {
    TimeSeriesTransformations v;

    EXPECT_EQ(v.getName(), "");
    EXPECT_EQ(v.count(), 0);
    EXPECT_EQ(v.getSeparator(), ',');
}

// Check copy constructor
TEST(TimeSeriesTransformations, checkCopyConstructor) {
    TimeSeriesTransformations v("C:\\Users\\Admin\\Desktop\\AMF-Assignment3\\Problem3_DATA.csv");
    v.separator = '|';
    TimeSeriesTransformations v_1(v);

    EXPECT_EQ(v.getName(), v_1.getName());
    EXPECT_EQ(v.count(), v_1.count());
    EXPECT_EQ(v.getSeparator(), v_1.getSeparator());
}

// Test assignment operator and equality operator.
TEST(TimeSeriesTransformations, checkAssignmentAndEqualityOperator) {
    TimeSeriesTransformations v1("C:\\Users\\Admin\\Desktop\\AMF-Assignment3\\Problem3_DATA.csv");
    TimeSeriesTransformations v2;

    // Check assignment operator.
    v2 = v1;

    EXPECT_EQ(v1.getName(), "ShareX");
    EXPECT_EQ(v1.getSeparator(), ',');

    EXPECT_EQ(v2.getName(), "ShareX");
    EXPECT_EQ(v2.getSeparator(), ',');

    EXPECT_EQ(v1.getTimeVector(), v2.getTimeVector());
    EXPECT_EQ(v1.getPriceVector(), v2.getPriceVector());

    // Check equality operator.
    EXPECT_TRUE(v1 == v2);
}

// Test Save function
TEST(TimeSeriesTransformations, checkSaveFunctionality) {
    TimeSeriesTransformations v_1("C:\\Users\\Admin\\Desktop\\AMF-Assignment3\\Problem3_DATA.csv");

    v_1.saveData("C:\\Users\\Admin\\Desktop\\AMF-Assignment3\\TEST_SAVE.csv");

    TimeSeriesTransformations v_2("C:\\Users\\Admin\\Desktop\\AMF-Assignment3\\TEST_SAVE.csv");
    // Check start value.
    EXPECT_NEAR(v_2.getPriceVector()[0], 61.43814, 10e-5);
    EXPECT_EQ(v_2.getTimeVector()[0], 1619120010);

    int vector_length = static_cast<int>(v_2.getPriceVector().size());

    // Check end value.
    EXPECT_NEAR(v_2.getPriceVector()[vector_length - 1], 90.50422, 10e-5);
    EXPECT_EQ(v_2.getTimeVector()[vector_length - 1], 1669115010);

    // Check custom names.
    TimeSeriesTransformations v_3({ 1, 2, 3 }, { 10, 20, 30 }, "APPL");
    v_3.saveData("C:\\Users\\Admin\\Desktop\\AMF-Assignment3\\APPL_SAVE.csv");

    TimeSeriesTransformations v_4("C:\\Users\\Admin\\Desktop\\AMF-Assignment3\\APPL_SAVE.csv");
    EXPECT_EQ(v_4.getPriceVector()[0], 10);
    EXPECT_EQ(v_4.getPriceVector()[1], 20);
    EXPECT_EQ(v_4.getPriceVector()[2], 30);

    EXPECT_EQ(v_4.getTimeVector()[0], 1);
    EXPECT_EQ(v_4.getTimeVector()[1], 2);
    EXPECT_EQ(v_4.getTimeVector()[2], 3);

    EXPECT_EQ(v_4.count(), 3);
    EXPECT_EQ(v_4.getName(), "APPL");
    EXPECT_EQ(v_4.getSeparator(), ',');

    v_4.separator = '|';
    v_4.saveData("C:\\Users\\Admin\\Desktop\\AMF-Assignment3\\APPL_SAVE_DIFF_SEP.csv");
    // However this file cannot be loaded back in because we cannot provide a separator to the filename constructor.
}

// Test find greatestincrements
TEST(TimeSeriesTransformations, findGreatestIncrementsWithData) {
    TimeSeriesTransformations v({ 1, 2, 3, 4, 5, 6, 7, 8, 9 }, { 1, 2000, 3, 4, 5, 6, 7, 8, 9 });

    double price;
    std::string date;
    EXPECT_TRUE(v.findGreatestIncrements(&date, &price));

    EXPECT_EQ(price, 1999);
    EXPECT_EQ(date, "1970-01-01 00:00:02");
}

TEST(TimeSeriesTransformations, findGreatestIncrementsWithNoData) {
    TimeSeriesTransformations v;

    double price;
    std::string date;

    EXPECT_FALSE(v.findGreatestIncrements(&date, &price));
    EXPECT_TRUE(std::isnan(price));
    EXPECT_EQ(date, "");

    // Test for not enough data.
    TimeSeriesTransformations v_1({ 1 }, { 1 });

    double price_1;
    std::string date_1;

    EXPECT_FALSE(v_1.findGreatestIncrements(&date_1, &price_1));
    EXPECT_TRUE(std::isnan(price_1));
    EXPECT_EQ(date_1, "");
}

TEST(TimeSeriesTransformations, countDataRowsLoadedInExaminationFile) {
    TimeSeriesTransformations v({ 10, 20, 30, 40 }, { 1, 2, 3, 2 });
    EXPECT_EQ(v.count(), 4);
}

TEST(TimeSeriesTransformations, countEmptyFile) {
    TimeSeriesTransformations v("C:\\Users\\Admin\\Desktop\\AMF-Assignment3\\empty_with_header.csv");
    EXPECT_EQ(v.count(), 0);
    EXPECT_EQ(v.getName(), "ShareX");
}

TEST(TimeSeriesTransformations, testAddASharePrice) {
    TimeSeriesTransformations v({ 10, 20, 30, 40 }, { 1, 2, 3, 2 });

    // Strictly speaking I would personally say this shouldn't be void because adding a price at the same time twice should logically fail.
    v.addASharePrice("1970-01-01 00:00:00", 2);

    EXPECT_EQ(v.getPriceVector()[0], 2);
    EXPECT_EQ(v.getTimeVector()[0], 0);
}

TEST(TimeSeriesTransformations, removeEntryThatExistsAtTime) {
    TimeSeriesTransformations v({ 10, 20, 30, 40 }, { 1, 2, 3, 2 });

    EXPECT_TRUE(v.removeEntryAtTime("1970-01-01 00:00:10"));

    EXPECT_EQ(v.getPriceVector()[0], 2);
    EXPECT_EQ(v.getTimeVector()[0], 20);

    EXPECT_EQ(v.getPriceVector().size(), 3);
    EXPECT_EQ(v.getTimeVector().size(), 3);
}

TEST(TimeSeriesTransformations, removeEntryThatHasNeverExisted) {
    TimeSeriesTransformations v({ 10, 20, 30, 40 }, { 1, 2, 3, 2 });

    EXPECT_FALSE(v.removeEntryAtTime("1970-01-01 00:00:05"));

    EXPECT_EQ(v.getPriceVector()[0], 1);
    EXPECT_EQ(v.getTimeVector()[0], 10);
}

// removePricesGreaterThan
TEST(TimeSeriesTransformations, removePricesGreaterThan) {
    TimeSeriesTransformations v({ 10, 20, 30, 40, 50, 60 }, { 7, 7, 4, 3, 9, 5 });
    EXPECT_TRUE(v.removePricesGreaterThan(5));

    EXPECT_EQ(v.getPriceVector()[0], 4);
    EXPECT_EQ(v.getTimeVector()[0], 30);

    EXPECT_EQ(v.getPriceVector()[1], 3);
    EXPECT_EQ(v.getTimeVector()[1], 40);

    EXPECT_EQ(v.getPriceVector().size(), 3);
    EXPECT_EQ(v.getTimeVector().size(), 3);

    TimeSeriesTransformations v_1({ 10, 11, 20, 30, 40, 50 }, { 1, 2, 3, 4, 5, 6 });
    EXPECT_FALSE(v_1.removePricesGreaterThan(6));
    EXPECT_FALSE(v_1.removePricesGreaterThan(7));
    EXPECT_EQ(v_1.getPriceVector().size(), 6);
    EXPECT_EQ(v_1.getTimeVector().size(), 6);
}

// removePricesLessThan
TEST(TimeSeriesTransformations, removePricesLessThan) {
    TimeSeriesTransformations v({ 10, 20, 30, 40, 50, 60 }, { 1, 2, 6, 7, 3, 5 });
    EXPECT_TRUE(v.removePricesLowerThan(5));

    EXPECT_EQ(v.getPriceVector()[0], 6);
    EXPECT_EQ(v.getTimeVector()[0], 30);

    EXPECT_EQ(v.getPriceVector()[1], 7);
    EXPECT_EQ(v.getTimeVector()[1], 40);

    EXPECT_EQ(v.getPriceVector().size(), 3);
    EXPECT_EQ(v.getTimeVector().size(), 3);

    TimeSeriesTransformations v_1({ 10, 11, 20, 30, 40, 50 }, { 1, 2, 3, 4, 5, 6 });
    EXPECT_FALSE(v_1.removePricesLowerThan(1));
    EXPECT_EQ(v_1.getPriceVector().size(), 6);
    EXPECT_EQ(v_1.getTimeVector().size(), 6);
}

// removePricesBefore
TEST(TimeSeriesTransformations, removePricesBefore) {
    TimeSeriesTransformations v({ 10, 11, 20, 30, 40, 50 }, { 1, 2, 3, 4, 5, 6 });
    EXPECT_TRUE(v.removePricesBefore("1970-01-01 00:00:11"));

    EXPECT_EQ(v.getPriceVector()[0], 2);
    EXPECT_EQ(v.getTimeVector()[0], 11);

    EXPECT_EQ(v.getPriceVector()[4], 6);
    EXPECT_EQ(v.getTimeVector()[4], 50);

    EXPECT_EQ(v.getPriceVector().size(), 5);
    EXPECT_EQ(v.getTimeVector().size(), 5);

    TimeSeriesTransformations v_1({ 10, 11, 20, 30, 40, 50 }, { 1, 2, 3, 4, 5, 6 });
    EXPECT_FALSE(v_1.removePricesBefore("1970-01-01 00:00:01"));
    EXPECT_EQ(v_1.getPriceVector().size(), 6);
    EXPECT_EQ(v_1.getTimeVector().size(), 6);
}

// removePricesAfter
TEST(TimeSeriesTransformations, removePricesAfter) {
    TimeSeriesTransformations v({ 10, 11, 20, 30, 40, 50 }, { 1, 2, 3, 4, 5, 6 });
    EXPECT_TRUE(v.removePricesAfter("1970-01-01 00:00:11"));

    EXPECT_EQ(v.getPriceVector()[0], 1);
    EXPECT_EQ(v.getTimeVector()[0], 10);

    EXPECT_EQ(v.getPriceVector()[1], 2);
    EXPECT_EQ(v.getTimeVector()[1], 11);

    EXPECT_EQ(v.getPriceVector().size(), 2);
    EXPECT_EQ(v.getTimeVector().size(), 2);

    TimeSeriesTransformations v_1({ 10, 11, 20, 30, 40, 50 }, { 1, 2, 3, 4, 5, 6 });
    EXPECT_FALSE(v_1.removePricesBefore("1970-01-50 00:00:01"));
    EXPECT_FALSE(v_1.removePricesBefore("1970-01-55 00:00:01"));
    EXPECT_EQ(v_1.getPriceVector().size(), 6);
    EXPECT_EQ(v_1.getTimeVector().size(), 6);
}

TEST(TimeSeriesTransformations, removePricesAfterAndBefore) {
    TimeSeriesTransformations v({ 10, 20, 30, 40, 50 }, { 1, 2, 3, 4, 5 });
    v.removePricesBefore("1970-01-01 00:00:11");
    v.removePricesAfter("1970-01-01 00:00:41");

    EXPECT_EQ(v.getPriceVector()[0], 2);
    EXPECT_EQ(v.getTimeVector()[0], 20);

    TimeSeriesTransformations v_1({ 10, 20, 30, 40, 50 }, { 1, 2, 3, 4, 5 });
    v_1.removePricesBefore("1970-01-01 00:00:30");
    v_1.removePricesAfter("1970-01-01 00:00:30");

    EXPECT_EQ(v_1.getPriceVector()[0], 3);
    EXPECT_EQ(v_1.getTimeVector()[0], 30);

    EXPECT_EQ(v_1.getPriceVector().size(), 1);
    EXPECT_EQ(v_1.getTimeVector().size(), 1);
}

// This is used to check your print methods output the correct number of
int countNewLines(std::string s) {
    int count = 0;
    for (const auto& i : s) {
        if (i == '\n') {
            count++;
        }
    }
    return count;
}

// print days
TEST(TimeSeriesTransformations, printPricesOnDay) {
    // Recall that there are 86400 seconds in a day.
    TimeSeriesTransformations v({ 10, 20, 30, 86399, 86400, 86400 + 10, 86400 + 20, 86400 * 2, 86400 * 2 + 10, 86400 * 2 + 20 }, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 });

    int number_of_lines = 0;
    std::string output_string = v.printSharePricesOnDate("1970-01-02");

    EXPECT_EQ(countNewLines(output_string), 3);
}

TEST(TimeSeriesTransformations, printPricesOnDayExactDayBoundary) {
    TimeSeriesTransformations v({ 1, 86400, 86401 }, { 1, 2, 3 });

    double value = 0;
    v.getPriceAtDate("1970-01-02", &value);

    EXPECT_EQ(value, 2);
}

TEST(TimeSeriesTransformations, printPricesOnDayThatDoesntExist) {
    TimeSeriesTransformations v({ 86401, 86402, 86403 }, { 1, 2, 3 });
    std::string output_prices = v.printSharePricesOnDate("1970-01-01");

    EXPECT_EQ(countNewLines(output_prices), 0);
}

TEST(TimeSeriesTransformations, printIncrementPricesOnDay) {
    TimeSeriesTransformations v({ 1, 2, 3, 86401 }, { 1, 2, 3, 4 });
    std::string output_prices = v.printIncrementsOnDate("1970-01-01");

    EXPECT_EQ(countNewLines(output_prices), 2);

    TimeSeriesTransformations v_1({ 86398, 86399, 86400, 86401 }, { 1, 2, 3, 4 });
    std::string output_prices_1 = v_1.printIncrementsOnDate("1970-01-01");

    EXPECT_EQ(countNewLines(output_prices_1), 1);
}

TEST(TimeSeriesTransformations, printIncrementPricesOnDayExactDayBoundary) {
    TimeSeriesTransformations v({ 1, 2, 3, 86400 }, { 1, 2, 1, 100 });
    std::string output_prices = v.printIncrementsOnDate("1970-01-02");

    EXPECT_EQ(countNewLines(output_prices), 1);
}

TEST(TimeSeriesTransformations, printIncrementPricesOnDayThatDoesntExist) {
    TimeSeriesTransformations v({ 1, 2, 3, 86401 }, { 1, 2, 3, 4 });
    std::string output_prices = v.printIncrementsOnDate("1970-02-27");

    EXPECT_EQ(countNewLines(output_prices), 0);
}

TEST(TimeSeriesTransformations, printPricesOnTrickyDay) {
    TimeSeriesTransformations v({ 86401, 86402, 86403 }, { 1, 2, 3 });

    EXPECT_THROW(v.printSharePricesOnDate("1970-02-31"), std::invalid_argument);
    EXPECT_THROW(v.printSharePricesOnDate("1970-02-31 00:00:00"), std::invalid_argument);
}
