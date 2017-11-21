/*
 * This file contains various convenient functions for writing
 * tested code listings in C++.
 */

#ifndef _BRESTPROG_LISTING_UTILS_HH
#define _BRESTPROG_LISTING_UTILS_HH

#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include <chrono>

using std::cerr;
using std::endl;
using std::string;
using std::ostream;
using std::istringstream;
using std::ostringstream;
using std::stringstream;

typedef std::function<void(istringstream&, stringstream&)> solution;

// Tested solution inputs/outputs with length exceeding this value will be truncated.
const int MAX_REPORTED_STRING_LEN = 50;

// Maximum allowed execution time, in seconds.
// WARN: if a test case exceeds this time, testing WILL NOT immediately halt.
// Testing routine will wait for the solution to exit normally, and only then check
// its running time.
const double MAX_EXECUTION_TIME = 10.0;

void truncate_string(string& str) {
    if (str.length() > MAX_REPORTED_STRING_LEN) {
        int length = str.length();

        str.resize(MAX_REPORTED_STRING_LEN);
        str += "... [length = ";
        str += std::to_string(length);
        str += "]";
    }
}

// A helper function for converting various printable objects to strings.
template <typename T>
string to_string(const T& object) {
    ostringstream ss;
    ss << object;
    return object.str();
}

// A shorthand for creating vectors.
template <typename... T>
auto vec(T&&... args) {
    using first_type = typename std::tuple_element<0, std::tuple<T...>>::type;
    return std::vector<first_type>{std::forward<T>(args)...};
}

template <typename T>
bool operator==(const std::vector<T>& a, const std::vector<T>& b) {
    if (a.size() != b.size()) {
        return false;
    }

    for (int i = 0; i < a.size(); i++) {
        if (a[i] != b[i]) {
            return false;
        }
    }

    return true;
}

template <typename T>
ostream& operator<<(ostream& out, const std::vector<T>& v) {
    out << "[";

    for (int i = 0; i < v.size() - 1; i++) {
        out << v[i] << ",";
    }

    if (v.size()) {
        out << v.back();
    }

    out << "]";

    return out;
}

/*
 * Tests a solution against a given output. Solution output must
 * match the expected output: if it doesn't, the input is logged and
 * exit(1) is called.
 *
 * Each non-whitespace token must exactly match the expected output.
 * Trailing whitespace in line and newlines in file are ignored.
 */
void test_token_sequence(const solution& solution_main,
                         const string& test_input,
                         const string& expected_output) {

    using namespace std::chrono;

    istringstream in(test_input);
    stringstream out;
    istringstream ans(expected_output);

    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    solution_main(in, out);

    high_resolution_clock::time_point t2 = high_resolution_clock::now();

    double execution_time = duration_cast<duration<double>>(t2 - t1).count();

    if (execution_time > MAX_EXECUTION_TIME) {
        string input = test_input;
        truncate_string(input);
        cerr << "Failed on test:" << endl << input << endl;
        cerr << "Execution time was " << execution_time << "s" << endl;
        std::exit(1);
    }

    while (out || ans) {
        string out_line, ans_line;
        std::getline(out, out_line);
        std::getline(ans, ans_line);

        istringstream out_line_s(out_line);
        istringstream ans_line_s(ans_line);

        while (out_line_s || ans_line_s) {
            string out_token;
            string ans_token;

            out_line_s >> out_token;
            ans_line_s >> ans_token;

            if (out_token != ans_token) {
                string input = test_input, answer = expected_output;
                string output = out.str();

                truncate_string(input);
                truncate_string(output);
                truncate_string(answer);

                cerr << "Failed on test:" << endl << input << endl;
                cerr << "Expected output:" << endl << answer << endl;
                cerr << "Actual output: " << endl << output << endl;
                std::exit(1);
            }
        }
    }
}

/*
 * Tests an unary function. Requires input and output types to be printable
 * (overloaded operator<< with ostream) and more importantly, output type
 * instances to be comparable using operator==.
 *
 * Test is considered successful if and only if actual_output == expected_output.
 */
template <typename In, typename Out>
void test_function(const std::function<Out(In)>& function,
                   const In& input,
                   const Out& expected_output) {
    using namespace std::chrono;

    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    const auto actual_output = function(input);

    high_resolution_clock::time_point t2 = high_resolution_clock::now();

    double execution_time = duration_cast<duration<double>>(t2 - t1).count();

    if (execution_time > MAX_EXECUTION_TIME) {
        string input_str = to_string(input);
        truncate_string(input_str);
        cerr << "Failed on test:" << endl << input_str << endl;
        cerr << "Execution time was " << execution_time << "s" << endl;
        std::exit(1);
    }

    if (!(actual_output == expected_output)) {
        string input_str = to_string(input);
        string answer_str = to_string(expected_output);
        string output_str = to_string(actual_output);

        truncate_string(input_str);
        truncate_string(output_str);
        truncate_string(answer_str);

        cerr << "Failed on test:" << endl << input_str << endl;
        cerr << "Expected output:" << endl << answer_str << endl;
        cerr << "Actual output: " << endl << output_str << endl;
        std::exit(1);
    }
}

#endif