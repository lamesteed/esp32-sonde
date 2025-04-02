#ifndef CSVDATA_H
#define CSVDATA_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

class CSVData {
public:
    // Setter to add data
    void addData(const std::vector<std::string>& row);

    // Getter to retrieve data
    const std::vector<std::vector<std::string>>& getData() const;

    // Method to save data to a CSV file
    void saveToFile(const std::string& filename) const;

private:
    std::vector<std::vector<std::string>> data;
};

#endif // CSVDATA_H