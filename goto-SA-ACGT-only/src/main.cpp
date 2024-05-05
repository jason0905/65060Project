
#include "Suffix_Array.hpp"

#include <string>
#include <cstdlib>
#include <limits>
#include <fstream>
#include <iostream>
#include <filesystem>

void read_input(const std::string& ip_path, std::string& text)
{
    std::error_code ec;
    const auto file_size = std::filesystem::file_size(ip_path, ec);

    if(ec)
    {
        std::cerr << ip_path << " : " << ec.message() << "\n";
        std::exit(EXIT_FAILURE);
    }

    text.resize(file_size);
    std::ifstream input(ip_path);
    input.read(text.data(), file_size);
    input.close();
}


template <typename T_idx_>
void pretty_print(const Goto_SA::Suffix_Array<T_idx_>& suf_arr, std::ofstream& output)
{
    const std::size_t n = suf_arr.n();
    for(std::size_t i = 0; i < n; ++i)
        output << suf_arr.SA()[i] << " \n"[i == n - 1];
}


int main(int argc, char* argv[])
{
    constexpr auto arg_count = 3;
    if(argc < arg_count)
    {
        std::cerr << "Usage: goto <input_path> <output_path> <(optional)--pretty-print>\n";
        std::exit(EXIT_FAILURE);
    }


    const std::string ip_path(argv[1]);
    const std::string op_path(argv[2]);

    static uint64_t* text = static_cast<uint64_t*>(std::malloc(30 * sizeof(uint64_t)));
    // read_input(ip_path, text);
    // hard-coding test string

    /*
    text[0] = 4;
    text[1] = 5;
    text[2] = 3;
    text[3] = 1;
    text[4] = 2;
    text[5] = 2;
    text[6] = 5;
    text[7] = 3;
    text[8] = 5;
    text[9] = 5;
    text[10] = 2;
    text[11] = 1;
    text[12] = 1;
    text[13] = 5;
    text[14] = 5;
    text[15] = 5;
    text[16] = 3;
    text[17] = 1;
    text[18] = 2;
    text[19] = 2;
    text[20] = 5;
    text[21] = 3;
    text[22] = 5;
    text[23] = 5;
    text[24] = 2;
    text[25] = 1;
    text[26] = 1;
    text[27] = 5;
    text[28] = 5;
    text[29] = 0;*/

    /*
    text[0] = 5;
    text[1] = 4;
    text[2] = 3;
    text[3] = 5;
    text[4] = 4;
    text[5] = 5;
    text[6] = 1;
    text[7] = 5;
    text[8] = 1;
    text[9] = 5;
    text[10] = 2;
    text[11] = 5;
    text[12] = 2;
    text[13] = 5;
    text[14] = 2;
    text[15] = 5;
    text[16] = 0;
    */

    /*
    text[0] = 5;
    text[1] = 4;
    text[2] = 3;
    text[3] = 2;
    text[4] = 1;
    text[5] = 0;
    */

    /*
    text[0] = 4;
    text[1] = 5;
    text[2] = 3;
    text[3] = 1;
    text[4] = 2;
    text[5] = 2;
    text[6] = 5;
    text[7] = 3;
    text[8] = 5;
    text[9] = 0;
    */

    /*
    text[0] = 5;
    text[1] = 4;
    text[2] = 3;
    text[3] = 5;
    text[4] = 4;
    text[5] = 5;
    text[6] = 1;
    text[7] = 4;
    text[8] = 1;
    text[9] = 4;
    text[10] = 2;
    text[11] = 4;
    text[12] = 2;
    text[13] = 4;
    text[14] = 2;
    text[15] = 4;
    text[16] = 0;
    */

    /*
    text[0] = 3;
    text[1] = 5;
    text[2] = 4;
    text[3] = 5;
    text[4] = 1;
    text[5] = 4;
    text[6] = 1;
    text[7] = 4;
    text[8] = 2;
    text[9] = 4;
    text[10] = 2;
    text[11] = 4;
    text[12] = 2;
    text[13] = 4;
    text[14] = 0;
    */
    /*text[0] = 5;
    text[1] = 6;
    text[2] = 1;
    text[3] = 2;
    text[4] = 4;
    text[5] = 4;
    text[6] = 3;
    text[7] = 0;*/
    text[0] = 1;
    text[1] = 2;
    text[2] = 3;
    text[3] = 4;
    text[4] = 5;
    text[5] = 1;
    text[6] = 2;
    text[7] = 3;
    text[8] = 4;
    text[9] = 5;
    text[10] = 0;

    std::ofstream output(op_path);

    std::size_t n = 11;
    std::cerr << "Text length: " << n << ".\n";
    Goto_SA::Suffix_Array<uint64_t> suf_arr(text, n, 6, true);
    suf_arr.construct();
    suf_arr.dump(output);

    output.close();


    return 0;
}
