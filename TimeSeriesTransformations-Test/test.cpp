#include "gtest/gtest.h"
#include "../TimeSeriesTransformations/TimeSeriesTransformations.h"

// I've included this to check when you read in files for comparison.
bool test(double in, double in2) {
    return abs((in - in2) / in) < 0.00001;
}

TEST(TimeSeriesTransformations, loadFileExaminationFile) {

    // Load file.
    TimeSeriesTransformations v("C:\\Users\\Admin\\Desktop\\AMF-Assignment3-students (1)\\AMF-Assignment3\\Problem3_DATA.csv");

    // Check start value.
    EXPECT_NEAR(v.price_vector[0], 61.43814, 10e-5);
    EXPECT_EQ(v.time_vector[0], 1619120010);

    int vector_length = static_cast<int>(v.price_vector.size());

    // Check end value.
    EXPECT_NEAR(v.price_vector[vector_length-1], 90.50422, 10e-5);
    EXPECT_EQ(v.time_vector[vector_length-1], 1669115010);

    // Alternative constructor.
    std::vector<int> time_vec = { 1,2,3,4 };
    std::vector<double> price_vec = { 100,110,120,110 };
    
    TimeSeriesTransformations v1(time_vec, price_vec);

    EXPECT_EQ(v1.time_vector, time_vec);
    EXPECT_EQ(v1.price_vector, price_vec);
}

TEST(TimeSeriesTransformations, meanOfExaminationFile) {
    TimeSeriesTransformations v("C:\\Users\\Admin\\Desktop\\AMF-Assignment3-students (1)\\AMF-Assignment3\\Problem3_DATA.csv");

    // Create ptr to double and pass through mean function.
    double mean_output;
    v.mean(&mean_output);

    EXPECT_NEAR(mean_output, 51.5734, 10e-5);
}

TEST(TimeSeriesTransformations, standardDeviationOfExaminationFile) {
    TimeSeriesTransformations v("C:\\Users\\Admin\\Desktop\\AMF-Assignment3-students (1)\\AMF-Assignment3\\Problem3_DATA.csv");
    
    // Create ptr to double and pass through standardDeviation function.
    double sd_output;
    v.standardDeviation(&sd_output);

    EXPECT_NEAR(sd_output, 28.703254238387661, 10e-6);
}

TEST(TimeSeriesTransformations, incrementMeanOfExaminationFile) {
    TimeSeriesTransformations v("C:\\Users\\Admin\\Desktop\\AMF-Assignment3-students (1)\\AMF-Assignment3\\Problem3_DATA.csv");

    // Create ptr to double and pass through standardDeviation function.
    double mean_output;
    v.computeIncrementMean(&mean_output);

    EXPECT_NEAR(mean_output, 0.002906899018901951, 10e-6);

    // Second test. Ensures ordered state.
    TimeSeriesTransformations v_1({1,5,4,2,3}, { 1,5,4,2,3 });
    double mean_output_1;
    v_1.computeIncrementMean(&mean_output_1);

    EXPECT_EQ(mean_output_1, 1);
}

TEST(TimeSeriesTransformations, incrementStandardDeviationOfExaminationFile) {
    TimeSeriesTransformations v("C:\\Users\\Admin\\Desktop\\AMF-Assignment3-students (1)\\AMF-Assignment3\\Problem3_DATA.csv");

    // Create ptr to double and pass through standardDeviation function.
    double sd_output;
    v.computeIncrementStandardDeviation(&sd_output);

    EXPECT_NEAR(sd_output, 40.359444049806953, 10e-6);
}

// test files with no data;

TEST(TimeSeriesTransformations, meanOfEmptyFileWithHeader)
{
    TimeSeriesTransformations v("C:\\Users\\Admin\\Desktop\\AMF-Assignment3-students (1)\\AMF-Assignment3\\empty_with_header.csv");
    double mean_output;
    EXPECT_EQ(v.mean(&mean_output), false);
    EXPECT_TRUE(std::isnan(mean_output));
}

TEST(TimeSeriesTransformations, standardDeviationEmptyFileOfEmptyFileWithHeader) {
    TimeSeriesTransformations v("C:\\Users\\Admin\\Desktop\\AMF-Assignment3-students (1)\\AMF-Assignment3\\empty_with_header.csv");
    double standard_deviation;
    EXPECT_EQ(v.standardDeviation(&standard_deviation), false);
    EXPECT_TRUE(std::isnan(standard_deviation));
}

TEST(TimeSeriesTransformations, incrementMeanOfEmptyFileWithHeader) {
    TimeSeriesTransformations v("C:\\Users\\Admin\\Desktop\\AMF-Assignment3-students (1)\\AMF-Assignment3\\empty_with_header.csv");

    double mean_output;
    EXPECT_EQ(v.computeIncrementMean(&mean_output), false);
    EXPECT_TRUE(std::isnan(mean_output));
}

TEST(TimeSeriesTransformations, incrementMeanUnorderedFileWithData) {
    TimeSeriesTransformations v({ 4,1,2,3 }, {1,2,3,4} );
    double mean_output;
    EXPECT_EQ(v.mean(&mean_output), true);
    EXPECT_EQ(mean_output, 2.5);
}

TEST(TimeSeriesTransformations, incrementStandardDeviationEmptyFileWithHeader) {
    TimeSeriesTransformations v("C:\\Users\\Admin\\Desktop\\AMF-Assignment3-students (1)\\AMF-Assignment3\\empty_with_header.csv");
    double sd_output;
    EXPECT_EQ(v.computeIncrementStandardDeviation(&sd_output), false);
    EXPECT_TRUE(std::isnan(sd_output));
}
//------------------------------------------------- NAME

TEST(TimeSeriesTransformations, testNameEmptyFileWithHeader) {
    TimeSeriesTransformations v("C:\\Users\\Admin\\Desktop\\AMF-Assignment3-students (1)\\AMF-Assignment3\\empty_with_header.csv");
    EXPECT_EQ(v.getName(), "ShareX");
}

TEST(TimeSeriesTransformations, testNameOfExaminationFile) {
    TimeSeriesTransformations v("C:\\Users\\Admin\\Desktop\\AMF-Assignment3-students (1)\\AMF-Assignment3\\Problem3_DATA.csv");
    EXPECT_EQ(v.getName(), "ShareX");
}

// ----Test Empty file exception
TEST(TimeSeriesTransformations, throwRunTimeFromFileDoesNotExist) {
    EXPECT_THROW(TimeSeriesTransformations v("C:\\Users\\Admin\\Desktop\\AMF-Assignment3-students (1)\\AMF-Assignment3\\file_does_not_exist.csv"), std::runtime_error);
}

//--------------- Test build a TimeSeriesTransformation from vectors
TEST(TimeSeriesTransformations, buildFromTSTFromFullEqualTimePriceVectors) {
    TimeSeriesTransformations v({ 10,20,30 }, { 1,2,3 });

    EXPECT_EQ(v.time_vector[0], 10);
    EXPECT_EQ(v.time_vector[1], 20);
    EXPECT_EQ(v.time_vector[2], 30);

    EXPECT_EQ(v.price_vector[0], 1);
    EXPECT_EQ(v.price_vector[1], 2);
    EXPECT_EQ(v.price_vector[2], 3);
}

TEST(TimeSeriesTransformations, throwRuntimeErrorFromTSTWithUnequalTimePriceVectors) {
    EXPECT_THROW(TimeSeriesTransformations v({ 10,20,30 }, { 1,2 }), std::runtime_error);
}

// Check getPriceAtDate
TEST(TimeSeriesTransformations, getAPriceAtADateThatExists) {
    TimeSeriesTransformations v({ 1, 86400, 86401 }, { 9,5,1 });

    double value = 0;
    EXPECT_TRUE(v.getPriceAtDate("1970-01-01 00:00:01", &value));
    EXPECT_EQ(value, 9);
}

TEST(TimeSeriesTransformations, getAPriceAtADateThatDoesNotExist) {
    TimeSeriesTransformations v({ 1, 86400, 86401 }, { 9,5,1 });

    double value = 0;
    EXPECT_FALSE(v.getPriceAtDate("1970-01-01 00:00:02", &value));
    EXPECT_TRUE(std::isnan(value));
}

// Check empty constructor
TEST(TimeSeriesTransformations, checkEmptyConstructor) {
    TimeSeriesTransformations v();

    // I don't really know what to test for here? If the object is created then the test passes I guess.
    EXPECT_TRUE(true);
}

// Test assignment operator and equality operator.
TEST(TimeSeriesTransformations, checkAssignmentAndEqualityOperator) {
    TimeSeriesTransformations v1("C:\\Users\\Admin\\Desktop\\AMF-Assignment3-students (1)\\AMF-Assignment3\\Problem3_DATA.csv");
    TimeSeriesTransformations v2;

    // Check assignment operator.
    v2 = v1;

    EXPECT_NEAR(v2.price_vector[0], 61.43814, 10e-5);
    EXPECT_EQ(v2.time_vector[0], 1619120010);

    // Check equality operator.
    EXPECT_TRUE(v1 == v2);
}

// Test Save function
TEST(TimeSeriesTransformations, checkSaveFunctionality) {
    TimeSeriesTransformations v_1("C:\\Users\\Admin\\Desktop\\AMF-Assignment3-students (1)\\AMF-Assignment3\\Problem3_DATA.csv");

    v_1.saveData("C:\\Users\\Admin\\Desktop\\AMF-Assignment3-students (1)\\AMF-Assignment3\\TEST_SAVE");

    TimeSeriesTransformations v("C:\\Users\\Admin\\Desktop\\AMF-Assignment3-students (1)\\AMF-Assignment3\\TEST_SAVE.csv");
    // Check start value.
    EXPECT_NEAR(v.price_vector[0], 61.43814, 10e-5);
    EXPECT_EQ(v.time_vector[0], 1619120010);

    int vector_length = static_cast<int>(v.price_vector.size());

    // Check end value.
    EXPECT_NEAR(v.price_vector[vector_length - 1], 90.50422, 10e-5);
    EXPECT_EQ(v.time_vector[vector_length - 1], 1669115010);
}

// Test find greatestincrements
TEST(TimeSeriesTransformations, findGreatestIncrementsWithData) {
    TimeSeriesTransformations v({ 10, 20, 30, 40, 86401 }, { 1,100,3,4,100000 });

    double price;
    std::string date = "1970-01-01 00:00:00";
    EXPECT_TRUE(v.findGreatestIncrements(&date, &price));

    EXPECT_EQ(price, 99);
}

TEST(TimeSeriesTransformations, findGreatestIncrementsWithNoData) {
    TimeSeriesTransformations v({}, {});

    double price;
    std::string date = "1970-01-01 00:00:00";
    EXPECT_FALSE(v.findGreatestIncrements(&date, &price));

    EXPECT_TRUE(std::isnan(price));
}

// ------------------------------------------------- Count
TEST(TimeSeriesTransformations, countDataRowsLoadedInExaminationFile) {
    TimeSeriesTransformations v({ 10, 20, 30, 40 }, { 1, 2, 3, 2 });
    EXPECT_EQ(v.count(), 4);
}

TEST(TimeSeriesTransformations, countEmptyFile) {
    TimeSeriesTransformations v("C:\\Users\\Admin\\Desktop\\AMF-Assignment3-students (1)\\AMF-Assignment3\\empty_with_header.csv");
    EXPECT_EQ(v.count(), 0);
}

// addASharePrice
TEST(TimeSeriesTransformations, testAddASharePrice) {
    TimeSeriesTransformations v({ 10, 20, 30, 40 }, { 1, 2, 3, 2 });

    v.addASharePrice("1970-01-01 00:00:00", 2);

    EXPECT_EQ(v.price_vector[0], 2);
    EXPECT_EQ(v.time_vector[0], 0);
}

// removeEntryAtTime
TEST(TimeSeriesTransformations, removeEntryThatExistsAtTime) {
    TimeSeriesTransformations v({ 10, 20, 30, 40 }, { 1, 2, 3, 2 });

    v.removeEntryAtTime("1970-01-01 00:00:10");

    EXPECT_EQ(v.price_vector[0], 2);
    EXPECT_EQ(v.time_vector[0], 20);
}

TEST(TimeSeriesTransformations, removeEntryThatHasNeverExisted) {
    TimeSeriesTransformations v({ 10, 20, 30, 40 }, { 1, 2, 3, 2 });

    EXPECT_FALSE(v.removeEntryAtTime("1970-01-01 00:00:05"));

    EXPECT_EQ(v.price_vector[0], 1);
    EXPECT_EQ(v.time_vector[0], 10);
}

// removePricesGreaterThan
TEST(TimeSeriesTransformations, removePricesGreaterThan) {
    TimeSeriesTransformations v({ 10,20,30,40,50 }, { 7,7,4,3,9 });
    v.removePricesGreaterThan(5);

    EXPECT_EQ(v.price_vector[0], 4 );
    EXPECT_EQ(v.time_vector[0], 30 );

    EXPECT_EQ(v.price_vector[1], 3);
    EXPECT_EQ(v.time_vector[1], 40);

}

// removePricesLessThan
TEST(TimeSeriesTransformations, removePricesLessThan) {
    TimeSeriesTransformations v({ 10,20,30,40,50 }, { 1,2,6,7,3 });
    v.removePricesLowerThan(5);

    EXPECT_EQ(v.price_vector[0], 6 );
    EXPECT_EQ(v.time_vector[0], 30 );

    EXPECT_EQ(v.price_vector[1], 7);
    EXPECT_EQ(v.time_vector[1], 40);
}

// removePricesBefore
TEST(TimeSeriesTransformations, removePricesBefore) 
{
    TimeSeriesTransformations v({ 10,20,30,40,50 }, { 1,2,3,4,5 });
    v.removePricesBefore("1970-01-01 00:00:11");

    EXPECT_EQ(v.price_vector[0], 2);
    EXPECT_EQ(v.time_vector[0], 20);
}

// removePricesAfter
TEST(TimeSeriesTransformations, removePricesAfter) 
{
    TimeSeriesTransformations v({ 10,20,30,40,50 }, { 1,2,3,4,5 });
    v.removePricesAfter("1970-01-01 00:00:11");

    EXPECT_EQ(v.price_vector[0], 1);
    EXPECT_EQ(v.time_vector[0], 10);
}

TEST(TimeSeriesTransformations, removePricesAfterAndBefore) 
{
    TimeSeriesTransformations v({ 10,20,30,40,50 }, { 1,2,3,4,5 });
    v.removePricesBefore("1970-01-01 00:00:11");
    v.removePricesAfter("1970-01-01 00:00:41");

    EXPECT_EQ(v.price_vector[0], 2);
    EXPECT_EQ(v.time_vector[0], 20);
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
    TimeSeriesTransformations v({ 10, 20, 30, 86399, 86400, 86400+10, 86400+20, 86400*2, 86400*2+10, 86400*2+20 }, { 1,2,3,4,5,6,7,8,9,10 });

    int number_of_lines = 0;
    std::string output_string = v.printSharePricesOnDate("1970-01-02");

    number_of_lines = countNewLines(output_string);

    EXPECT_EQ(number_of_lines, 3);
}

TEST(TimeSeriesTransformations, printPricesOnDayExactDayBoundary) {
    TimeSeriesTransformations v({1, 86400, 86401}, { 1,2,3 });

    double value = 0;
    v.getPriceAtDate("1970-01-02", &value);

    EXPECT_EQ(value, 2);

}

// print days prices
TEST(TimeSeriesTransformations, printPricesOnDayThatDoesntExist) {
    TimeSeriesTransformations v({ 86401, 86402, 86403 }, { 1,2,3 });
    std::string output_prices = v.printSharePricesOnDate("1970-01-01");

    EXPECT_EQ(countNewLines(output_prices), 0);
}

// print days incrementPrices
TEST(TimeSeriesTransformations, printIncrementPricesOnDay) {
    TimeSeriesTransformations v({ 1, 2, 3, 86401 }, { 1,2,3,4 });
    std::string output_prices = v.printIncrementsOnDate("1970-01-01");

    EXPECT_EQ(countNewLines(output_prices), 2);
}


TEST(TimeSeriesTransformations, printIncrementPricesOnDayExactDayBoundary) { //??? Shouldn't this be a special function?
    TimeSeriesTransformations v({ 1, 2, 3, 86400 }, { 1,2,1,100 });
    std::string output_prices = v.printIncrementsOnDate("1970-01-02");

    EXPECT_EQ(countNewLines(output_prices), 1);
}

// print days
TEST(TimeSeriesTransformations, printIncrementPricesOnDayThatDoesntExist) {
    TimeSeriesTransformations v({ 1, 2, 3, 86401 }, { 1,2,3,4 });
    std::string output_prices = v.printIncrementsOnDate("1970-02-27");

    EXPECT_EQ(countNewLines(output_prices), 0);
}

TEST(TimeSeriesTransformations, printPricesOnTrickyDay) {
    TimeSeriesTransformations v({ 86401, 86402, 86403 }, { 1,2,3 });

    EXPECT_THROW(v.printSharePricesOnDate("1970-02-31"), std::invalid_argument);
    EXPECT_THROW(v.printSharePricesOnDate("1970-02-31 00:00:00"), std::invalid_argument);
}


