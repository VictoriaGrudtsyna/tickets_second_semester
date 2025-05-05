#include <iostream>
#include <vector>



int main(){
    #ifdef __cplusplus
    std::cout << "We are C++\n";
    #else
    std::cout << "We are not C++???\n";
    // Preprocessor warning/error commands
    // #error Compiling with C is not supported  // Supported
    // #warning Compiling with C is badly supported  // Supported, official standard since C++23
    #endif
}
