#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdexcept>

// Function to split a string by a delimiter and return a vector of tokens
std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Function to clean the string from leading/trailing quotes and spaces
std::string cleanString(const std::string& str) {
    std::string cleaned = str;
    if (!cleaned.empty() && cleaned.front() == '\"' && cleaned.back() == '\"') {
        cleaned = cleaned.substr(1, cleaned.size() - 2); // Remove surrounding quotes
    }
    // Remove any leading or trailing spaces
    size_t start = cleaned.find_first_not_of(" \t");
    size_t end = cleaned.find_last_not_of(" \t");
    if (start == std::string::npos || end == std::string::npos) {
        return ""; // Return empty string if all characters are spaces
    }
    return cleaned.substr(start, end - start + 1);
}

// Function to convert cost string to float
float convertCost(const std::string& costStr) {
    std::string cleanCostStr = cleanString(costStr);
    if (cleanCostStr.empty() || cleanCostStr[0] != '$') {
        throw std::invalid_argument("Invalid cost format: " + costStr);
    }
    cleanCostStr = cleanCostStr.substr(1); // Remove the '$'
    return std::stof(cleanCostStr); // Convert to float
}

// Function to calculate the total gross
float calculateTotalGross(int qty, float cost) {
    return qty * cost;
}

int main() {
    std::ifstream inputFile("input.csv");
    std::ofstream outputFile("output.csv");
    std::string line;
    std::vector<std::vector<std::string>> data;

    // Read the input CSV file
    if (inputFile.is_open()) {
        while (std::getline(inputFile, line)) {
            // Replace commas inside double quotes to prevent splitting of reference values
            std::string modifiedLine;
            bool insideQuotes = false;
            for (char c : line) {
                if (c == '\"') {
                    insideQuotes = !insideQuotes;
                }
                if (c == ',' && insideQuotes) {
                    modifiedLine += ';'; // Use semicolon as temporary delimiter
                } else {
                    modifiedLine += c;
                }
            }

            std::vector<std::string> row = split(modifiedLine, ',');
            for (auto& field : row) {
                field = cleanString(field);
            }
            data.push_back(row);
        }
        inputFile.close();
    } else {
        std::cerr << "Unable to open input file" << std::endl;
        return 1;
    }

    // Process the data and calculate the total gross
    if (!data.empty()) {
        data[0].push_back("Total Gross"); // Add header for the new column

        for (size_t i = 1; i < data.size(); ++i) {
            try {
                int qty = std::stoi(data[i][4]); // Quantity
                float cost = convertCost(data[i][12]); // Cost @ 10000
                float totalGross = calculateTotalGross(qty, cost);
                data[i].push_back("$" + std::to_string(totalGross)); // Add total gross to the row
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid argument: " << e.what() << " in row " << i << std::endl;
                data[i].push_back("$0"); // Add zero if there's an error
            } catch (const std::out_of_range& e) {
                std::cerr << "Out of range: " << e.what() << " in row " << i << std::endl;
                data[i].push_back("$0"); // Add zero if there's an error
            }
        }
    }

    // Write the updated data to the output CSV file
    if (outputFile.is_open()) {
        for (const auto& row : data) {
            for (size_t j = 0; j < row.size(); ++j) {
                outputFile << "\"" << row[j] << "\""; // Write fields with quotes to preserve formatting
                if (j < row.size() - 1) {
                    outputFile << ",";
                }
            }
            outputFile << std::endl;
        }
        outputFile.close();
    } else {
        std::cerr << "Unable to open output file" << std::endl;
        return 1;
    }

    std::cout << "CSV file processed and saved as output.csv" << std::endl;

    return 0;
}
