
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

    static uint64_t* text = static_cast<uint64_t*>(std::malloc(1000 * sizeof(uint64_t)));
    // read_input(ip_path, text);
    // hard-coding test string

    std::string text_string = "cagctgctgtctgagcacacagcaacacacacacacacacacacacacacacacacacacacacacacacatctcaagGCAGGAGCCTCTCAGCTAAAGCCATGCTAACTCAGCACGGAGTCACATGACCGTGGCACCATTGTGGTTTCATAATGACGGCTAAAGCCGACGGTAAGTTGTCTTGTTCTCTTTCGGTGGTCAGGCTGTGGCACATCAGGGGGAACAGACGGTTGCTATAGCAACACTATAAATAAAAGAGGGAAGTTCAGGCTGCTGCGTCAGAAGTGACAGGAAGCATCTTTGTTCAGGTCAGGGAAGTGACCTGAGAGGGGTCATCTTCTGACCGGCTCATCGCTCCTGTCACCAGGCTGGGAACTGGTCAATGTAAACCAGCCCTGGAGGCCTTCAGGTCCAGGACCTGTAGAGGTTTAGGACAGGTGCTGAGAGGGTTCCAGACCAGCCCTGGAGGCCTTCAGGTCCAGGACCTGTAGAGGTTTAGGACAGGTGCTGAGAGGGTTCCAGACCAGCCCTGGAGGCCTTCAGGTCCAGGACCTGTAGAGGTTTAGGACAGGTGCTGAGAGGGTTCCAGACCAGCCCTGGAGGCCTTCAGGTCCAGGACCTGTAGAGGTTTAGGACAGGTGCTGAGAGGGTTCCAGACCAGCCCTGGAGGCCTTCAGGTCCAGGACCTGTAGAGGTTCAGGACAGGTGCTGAGTGGGTTCCAGACCAGCCCTGGAGGCCTCCAGGGCTGCTGGTGCAGCCCGGTCGTTGGGCTGTTGCCTCCATCGGAAGCAACCAAATGNNNNN";
    //std::string text_string = "ctgagcacaccacacAGGAGCAGCTAAAGCCAGCACGGAGTCACATGACCTTTCATAATGGGTCATCTTCTGACCGGCTCATCGCTAAACCAGCCCTGAGAGGACCTGTAGAGGTACAGGTGCTGAGAGGGTTCCAGACCAGCCCTGGAGGCCTTCAGGTCCAGGACCTGTAGAGGTTTAGGACAGGTGCTGAGAGGGTTCCAGACCAGCCCTGGAGGCCTTCAGGTCCAGGACCTGTAGAGGTTTAGGACAGGTGCTGAGAGGGTTCCAGACCAGCCCTGGAGGCCTTCAGGTCCAGACCAGCCCTGGAGGCCTGCTGCCAGCCGCTGTCCATCNNNNN";
    //std::string text_string = "ttttcganttttcgan";

    uint32_t i = 0;
    for(; i < text_string.length(); ++i) {
        if(text_string[i] == 'a' || text_string[i] == 'A') {
            text[i] = 1;
        } else if(text_string[i] == 'c' || text_string[i] == 'C') {
            text[i] = 2;
        } else if(text_string[i] == 'g' || text_string[i] == 'G') {
            text[i] = 3;
        } else if(text_string[i] == 'n' || text_string[i] == 'N') {
            text[i] = 4;
        } else if(text_string[i] == 't' || text_string[i] == 'T') {
            text[i] = 5;
        }
    }
    text[i] = 0;
    std::size_t n = text_string.length() + 1;

    std::ofstream output(op_path);

    std::cerr << "Text length: " << n << ".\n";
    Goto_SA::Suffix_Array<uint64_t> suf_arr(text, n, 6);
    suf_arr.construct();
    suf_arr.dump(output);

    output.close();


    return 0;
}
