
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

    static uint64_t* text = static_cast<uint64_t*>(std::malloc(200 * sizeof(uint64_t)));
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
    /*
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
    */

    /*
    text[0] = 3;
    text[1] = 5;
    text[2] = 1;
    text[3] = 1;
    text[4] = 5;
    gtaat
    text[5] = 5;
    text[6] = 5;
    text[7] = 5;
    text[8] = 5;
    text[9] = 5;
    ttttt

    text[10] = 3;
    text[11] = 1;
    text[12] = 5;
    text[13] = 3;
    text[14] = 1;
    gatga
    text[15] = 2;
    text[16] = 2;
    text[17] = 5;
    text[18] = 3;
    text[19] = 5;
    cctgt

    text[20] = 5;
    text[21] = 2;
    text[22] = 1;
    text[23] = 1;
    text[24] = 5;
    tcaat
    text[25] = 5;
    text[26] = 5;
    text[27] = 2;
    text[28] = 5;
    text[29] = 5;
    ttctt


    text[30] = 2;
    text[31] = 5;
    text[32] = 5;
    text[33] = 5;
    text[34] = 5;
    ctttt
    text[35] = 3;
    text[36] = 5;
    text[37] = 3;
    text[38] = 1;
    text[39] = 5;
    gtgat

    text[40] = 3;
    text[41] = 3;
    text[42] = 3;
    text[43] = 2;
    text[44] = 2;
    gggcc
    text[45] = 5;
    text[46] = 3;
    text[47] = 5;
    text[48] = 5;
    text[49] = 2;
    tgttc


    text[50] = 1;
    text[51] = 1;
    text[52] = 1;
    text[53] = 5;
    text[54] = 5;
    aaatt
    text[55] = 5;
    text[56] = 5;
    text[57] = 5;
    text[58] = 2;
    text[59] = 1;
    tttca

    text[60] = 2;
    text[61] = 5;
    text[62] = 5;
    text[63] = 2;
    text[64] = 1;
    cttca
    text[65] = 3;
    text[66] = 5;
    text[67] = 2;
    text[68] = 5;
    text[69] = 3;
    gtctg

    text[70] = 5;
    text[71] = 3;
    text[72] = 5;
    text[73] = 5;
    text[74] = 1;
    tgtta
    text[75] = 3;
    text[76] = 5;
    text[77] = 5;
    text[78] = 5;
    text[79] = 1;
    gttta
    text[80] = 4;
    n


    gtaattttttgatgacctgttcaatttcttcttttgtgatgggcctgttcaaatttttcacttcagtctgtgttagtttan*/

    text[0] = 2;
    text[1] = 5;
    text[2] = 3;
    text[3] = 5;
    text[4] = 3;
    text[5] = 5;
    text[6] = 5;
    text[7] = 1;
    text[8] = 3;
    text[9] = 5;
    text[10] = 1;
    text[11] = 4;
    text[12] = 2;
    text[13] = 5;
    text[14] = 3;
    text[15] = 5;
    text[16] = 3;
    text[17] = 5;
    text[18] = 5;
    text[19] = 1;
    text[20] = 3;
    text[21] = 5;
    text[22] = 1;
    text[23] = 4;
    text[24] = 0;

    /*
    text[0] = 4;
    text[1] = 3;
    text[2] = 5;
    text[3] = 1;
    text[4] = 2;
    text[5] = 5;
    text[6] = 4;
    text[7] = 3;
    text[8] = 5;
    text[9] = 1;
    text[10] = 2;
    text[11] = 5;
    text[12] = 0;
    */

    /*text[0] = 5;
    text[1] = 6;
    text[2] = 1;
    text[3] = 3;
    text[4] = 4;
    text[5] = 5;
    text[6] = 6;
    text[7] = 1;
    text[8] = 2;
    text[9] = 0;*/

    std::ofstream output(op_path);

    std::size_t n = 25;
    std::cerr << "Text length: " << n << ".\n";
    Goto_SA::Suffix_Array<uint64_t> suf_arr(text, n, 6, true);
    suf_arr.construct();
    suf_arr.dump(output);

    output.close();


    return 0;
}
