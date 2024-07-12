#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <exception>
#include <cctype>
#include <algorithm>
#include <regex>
//#include <locale>

//using MultiType = std::variant<int, std::string, double>;

//using filestruct = std::map< std::string, std::map<std::string,MultiType>>;
using filestruct = std::map< std::string, std::map<std::string,std::string>>;


class ini_parser{
private:
    std::string filename_;
    filestruct fstruct_;
    //public:
    std::string eraseWS(const std::string& s){
        std::string res = s;
        res.erase(std::remove_if(res.begin(), res.end(), ::isspace),
                  res.end());
        return res;
    }
    void split_in_two(const std::string& s, std::string& before, std::string& after, const char& del)
    {
        std::stringstream ss(s);
        before = "";
        after = "";
        bool got_before = false;
        while (!ss.eof()) {
            if(!got_before){
                getline(ss, before, del);
                got_before = true;
            }else
                getline(ss, after);
        }
    }
    void build_fstruct(){
        std::ifstream infile(filename_);
        if(!infile){
            throw std::runtime_error("Error opening file\nException thrown");
        }
        std::string line;
        std::string before;
        std::string after;
        std::string section;
        std::string bad_syntax_comment = "";
        int count = 1;
        while(getline(infile,line)){
            split_in_two(line,before,after,';');
            if(!eraseWS(before).empty()){
                line = before;
                split_in_two(line,before,after,'[');
                //before = eraseWS(before);
                if(eraseWS(before).empty()){
                    line = after;
                    split_in_two(line,before,after,']');
                    if(line != before){ //']' HAS BEEN FOUND
                        section = before;
                    }else bad_syntax_comment = "No closing ']' found\n";
                }else if(line == before){ //'[' HASN'T BBEN FOUND
                    //before = "";
                    split_in_two(line,before,after,'=');
                    if(before != line && !section.empty() && !after.empty()){
                        fstruct_[section][before] = after;
                    }else if(before==line){ //NONE OF ; OR [ OR = HAS BEEN FOUND
                        bad_syntax_comment = "None of ';' or '[' or '=' has been found among other symbols";
                    }
                }else bad_syntax_comment = "Symbols before opening '[' found\n";
            }//else std::cout << "Line " << count << " is a comment or empty\n";
            if(bad_syntax_comment != ""){
                std::cout << "Bad syntax in line " << count << ": " << bad_syntax_comment << "\n";
                bad_syntax_comment = "";
            }
            count++;
        }
    }
    std::string get_value_string(const std::string& section, const std::string& key) {
        return fstruct_[section][key];
    }

public:
    template<typename T>
    T get_value(const std::string& str){
        T result{};
        std::string section;
        std::string key;
        split_in_two(str, section, key, '.');
        if(fstruct_.count(section)==0) {
            std::cout << "Section not found\n";
            std::cout << "Perhaps you meant one of the non-empty sections available:\n";
            for(const auto& sec : fstruct_){
                std::cout << sec.first << "\n";
            }
            throw std::runtime_error("Exception thrown");
        }
        else if(fstruct_[section].count(key)==0) {
            std::cout << "Variable name not found\n";
            std::cout << "Perhaps you meant one of the variables available in section \"" << section << "\":\n";
            for(const auto& record : fstruct_[section]){
                std::cout << record.first << "\n";
            }
            throw std::runtime_error("Exception thrown");
        }
        else{
            std::string string_value = get_value_string(section, key);
            //дальше может варьироваться в зависимости от фантазии typeid(int) == typeid(T)
            std::regex re { R"(^-?[0-9]+(,)[0-9]+$)" };
            if(std::regex_match(eraseWS(string_value), re)){
                string_value = std::regex_replace(eraseWS(string_value),std::regex(","),".");
            }
            if constexpr (std::is_same<int, T>::value) {
                result = std::stoi(string_value);
            }
            else if constexpr (std::is_same<double, T>::value) {
                result = std::stod(string_value);
            }
            else if constexpr (std::is_same<std::string, T>::value) {
                result = string_value;
            }
            else
            {
                static_assert(sizeof(T) == -1, "no implementation for this type!");
            }
        }
        //возвращаем результат
        return result;
    }
    ini_parser(std::string filename):filename_(filename){
        build_fstruct();
    }
};


int main(){
    try{
        ini_parser parser("file.ini");
        auto value = parser.get_value<double>("Section1.var1");
        //  WRONG SECTION NAME
        //auto value = parser.get_value<double>("Section10.var1");
        //  WRONG VALUE NAME
        //auto value = parser.get_value<double>("Section1.var10");
        //  WRONG TYPE
        //auto value = parser.get_value<int>("Section1.var2");
        std::cout << "Value requested is " << value << "\n";
    }catch(const std::invalid_argument& e){
        std::cout << "Conversion failure:\nvalue requested cannot be converted to the type you specified\nException thrown\n";
    }catch(const std::exception& e){
        std::cout << e.what();
    }

    return  0;
}
