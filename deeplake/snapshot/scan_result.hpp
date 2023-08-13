//
// Created by Nathan Voxland on 8/22/23.
//

#ifndef DEEPLAKE_SCAN_RESULT_HPP
#define DEEPLAKE_SCAN_RESULT_HPP

#include <string>
#include "../actions/add_file_action.hpp"

namespace deeplake {
    class scan_result {
    public:

        explicit scan_result(std::vector<std::string> data);
        explicit scan_result(std::string base_path, std::vector<add_file_action> files);

        std::string& operator[](int idx);

        std::vector<std::string>::iterator begin();

        std::vector<std::string>::iterator end();

    private:
        std::vector<std::string> data_;
    };
}


#endif //DEEPLAKE_SCAN_RESULT_HPP
