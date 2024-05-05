
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

    text[0] = 3;
    text[1] = 5;
    text[2] = 1;
    text[3] = 1;
    text[4] = 5;
    text[5] = 5;
    text[6] = 5;
    text[7] = 5;
    text[8] = 5;
    text[9] = 5;
    text[10] = 3;
    text[11] = 1;
    text[12] = 5;
    text[13] = 3;
    text[14] = 1;
    text[15] = 2;
    text[16] = 2;
    text[17] = 5;
    text[18] = 3;
    text[19] = 5;
    text[20] = 5;
    text[21] = 2;
    text[22] = 1;
    text[23] = 1;
    text[24] = 5;
    text[25] = 5;
    text[26] = 5;
    text[27] = 2;
    text[28] = 5;
    text[29] = 5;
    text[30] = 2;
    text[31] = 5;
    text[32] = 5;
    text[33] = 5;
    text[34] = 5;
    text[35] = 3;
    text[36] = 5;
    text[37] = 3;
    text[38] = 1;
    text[39] = 5;
    text[40] = 3;
    text[41] = 3;
    text[42] = 3;
    text[43] = 2;
    text[44] = 2;
    text[45] = 5;
    text[46] = 3;
    text[47] = 5;
    text[48] = 5;
    text[49] = 2;
    text[50] = 1;
    text[51] = 1;
    text[52] = 1;
    text[53] = 5;
    text[54] = 5;
    text[55] = 5;
    text[56] = 5;
    text[57] = 5;
    text[58] = 2;
    text[59] = 1;
    text[60] = 2;
    text[61] = 5;
    text[62] = 5;
    text[63] = 2;
    text[64] = 1;
    text[65] = 3;
    text[66] = 5;
    text[67] = 2;
    text[68] = 5;
    text[69] = 3;
    text[70] = 5;
    text[71] = 3;
    text[72] = 5;
    text[73] = 5;
    text[74] = 1;
    text[75] = 3;
    text[76] = 5;
    text[77] = 5;
    text[78] = 5;
    text[79] = 1;
    text[80] = 4;
    text[81] = 3;
    text[82] = 5;
    text[83] = 1;
    text[84] = 1;
    text[85] = 5;
    text[86] = 5;
    text[87] = 5;
    text[88] = 5;
    text[89] = 5;
    text[90] = 5;
    text[91] = 3;
    text[92] = 1;
    text[93] = 5;
    text[94] = 3;
    text[95] = 1;
    text[96] = 2;
    text[97] = 2;
    text[98] = 5;
    text[99] = 3;
    text[100] = 5;
    text[101] = 5;
    text[102] = 2;
    text[103] = 1;
    text[104] = 1;
    text[105] = 5;
    text[106] = 5;
    text[107] = 5;
    text[108] = 2;
    text[109] = 5;
    text[110] = 5;
    text[111] = 2;
    text[112] = 5;
    text[113] = 5;
    text[114] = 5;
    text[115] = 5;
    text[116] = 3;
    text[117] = 5;
    text[118] = 3;
    text[119] = 1;
    text[120] = 5;
    text[121] = 3;
    text[122] = 3;
    text[123] = 3;
    text[124] = 2;
    text[125] = 2;
    text[126] = 5;
    text[127] = 3;
    text[128] = 5;
    text[129] = 5;
    text[130] = 2;
    text[131] = 1;
    text[132] = 1;
    text[133] = 1;
    text[134] = 5;
    text[135] = 5;
    text[136] = 5;
    text[137] = 5;
    text[138] = 5;
    text[139] = 2;
    text[140] = 1;
    text[141] = 2;
    text[142] = 5;
    text[143] = 5;
    text[144] = 2;
    text[145] = 1;
    text[146] = 3;
    text[147] = 5;
    text[148] = 2;
    text[149] = 5;
    text[150] = 3;
    text[151] = 5;
    text[152] = 3;
    text[153] = 5;
    text[154] = 5;
    text[155] = 1;
    text[156] = 3;
    text[157] = 5;
    text[158] = 5;
    text[159] = 5;
    text[160] = 1;
    text[161] = 4;
    text[162] = 0;

    /*
    text[0] = 3;
    text[1] = 10;
    text[2] = 4;
    text[3] = 20;
    text[4] = 2;
    text[5] = 14;
    text[6] = 15;
    text[7] = 17;
    text[8] = 11;
    text[9] = 12;
    text[10] = 20;
    text[11] = 1;
    text[12] = 5;
    text[13] = 6;
    text[14] = 13;
    text[15] = 18;
    text[16] = 19;
    text[17] = 7;
    text[18] = 9;
    text[19] = 16;
    text[20] = 3;
    text[21] = 10;
    text[22] = 4;
    text[23] = 20;
    text[24] = 2;
    text[25] = 14;
    text[26] = 15;
    text[27] = 17;
    text[28] = 11;
    text[29] = 12;
    text[30] = 20;
    text[31] = 1;
    text[32] = 5;
    text[33] = 6;
    text[34] = 13;
    text[35] = 18;
    text[36] = 19;
    text[37] = 7;
    text[38] = 8;
    text[39] = 0;
    */

    /*text[0] = 2;
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
    text[24] = 0;*/

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

    /*text[0] = 5;
    text[1] = 1;
    text[2] = 1;
    text[3] = 5;
    text[4] = 5;
    text[5] = 5;
    text[6] = 5;
    text[7] = 4;
    text[8] = 5;
    text[9] = 3;
    text[10] = 1;
    text[11] = 5;
    text[12] = 3;
    text[13] = 1;
    text[14] = 2;
    text[15] = 2;
    text[16] = 5;
    text[17] = 3;
    text[18] = 5;
    text[19] = 5;
    text[20] = 2;
    text[21] = 1;
    text[22] = 1;
    text[23] = 5;
    text[24] = 5;
    text[25] = 5;
    text[26] = 1;
    text[27] = 1;
    text[28] = 5;
    text[29] = 5;
    text[30] = 5;
    text[31] = 5;
    text[32] = 4;
    text[33] = 5;
    text[34] = 3;
    text[35] = 1;
    text[36] = 5;
    text[37] = 3;
    text[38] = 1;
    text[39] = 2;
    text[40] = 2;
    text[41] = 5;
    text[42] = 3;
    text[43] = 5;
    text[44] = 5;
    text[45] = 2;
    text[46] = 1;
    text[47] = 1;
    text[48] = 5;
    text[49] = 5;
    text[50] = 0;*/

    /*text[0] = 3;
    text[1] = 7;
    text[2] = 5;
    text[3] = 4;
    text[4] = 6;
    text[5] = 2;
    text[6] = 3;
    text[7] = 7;
    text[8] = 5;
    text[9] = 4;
    text[10] = 6;
    text[11] = 1;
    text[12] = 0;*/


    /*
    text[0] = 5;
    text[1] = 5;
    text[2] = 5;
    text[3] = 5;
    text[4] = 5;
    text[5] = 3;
    text[6] = 1;
    text[7] = 5;
    text[8] = 3;
    text[9] = 1;
    text[10] = 2;
    text[11] = 3;
    text[12] = 1;
    text[13] = 5;
    text[14] = 5;
    text[15] = 5;
    text[16] = 2;
    text[17] = 1;
    text[18] = 1;
    text[19] = 5;
    text[20] = 5;
    text[21] = 4;
    text[22] = 5;
    text[23] = 5;
    text[24] = 5;
    text[25] = 5;
    text[26] = 5;
    text[27] = 3;
    text[28] = 1;
    text[29] = 5;
    text[30] = 3;
    text[31] = 1;
    text[32] = 2;
    text[33] = 3;
    text[34] = 1;
    text[35] = 5;
    text[36] = 5;
    text[37] = 5;
    text[38] = 2;
    text[39] = 1;
    text[40] = 1;
    text[41] = 5;
    text[42] = 5;
    text[43] = 4;
    text[44] = 0;*/

    /*text[0] = 5;
    text[1] = 5;
    text[2] = 5;
    text[3] = 5;
    text[4] = 2;
    text[5] = 3;
    text[6] = 1;
    text[7] = 4;
    text[8] = 5;
    text[9] = 5;
    text[10] = 5;
    text[11] = 5;
    text[12] = 2;
    text[13] = 3;
    text[14] = 1;
    text[15] = 4;
    text[16] = 0;*/


    std::ofstream output(op_path);

    std::size_t n = 163;
    std::cerr << "Text length: " << n << ".\n";
    Goto_SA::Suffix_Array<uint64_t> suf_arr(text, n, 6);
    suf_arr.construct();
    suf_arr.dump(output);

    output.close();


    return 0;
}
