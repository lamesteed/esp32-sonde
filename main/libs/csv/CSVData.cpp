#include "CSVData.h"

void CSVData::addData(const std::vector<std::string>& row) {
    data.push_back(row);
}

const std::vector<std::vector<std::string>>& CSVData::getData() const {
    return data;
}

void CSVData::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (const auto& row : data) {
            for (size_t i = 0; i < row.size(); ++i) {
                file << row[i];
                if (i < row.size() - 1) {
                    file << ",";
                }
            }
            file << "\n";
        }
        file.close();
    } else {
        std::cerr << "Unable to open file " << filename << std::endl;
    }
}