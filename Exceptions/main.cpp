#include <iostream>
#include <csetjmp>
#include <exception>
#include <vector>
#include <array>
#include "ObjectStack.h"

class First: public ObjectStack {
public:
    First(int number = 0) : num_(number) {
        std::cout << "First >> " << num_ << std::endl;
    }
    virtual ~First() {
        std::cout << "First << " << num_ << std::endl;
    }

    int num_;
};

class Second: public ObjectStack {
public:
    Second(int number = 0) : num_(number) {
        std::cout << "Second >> " << num_ << std::endl;
    }
    virtual ~Second() {
        std::cout << "Second << " << num_ << std::endl;
    }

private:
    int num_;
};


int main() {
// // first test
//    TRY {
//        TRY {
//            THROW(EXC_2)
//        } CATCH(EXC_1) {
//            std::cout << "ououou" << std::endl;
//        } END_TRY
//
//    } CATCH(EXC_2) {
//        std::cout << "erererreer" << std::endl;
//    } END_TRY

    // second test
    First f0(0);
    TRY {
        First f1(1);
        TRY {
            First f2(2);
            Second s0(0);
            TRY {
                First f3(3);
                THROW(EXC_1)
            } END_TRY
        } CATCH(OTHER_EXC) {
            std::cerr << "OTHER_EXC" << std::endl;
        } CATCH(EXC_1) {
            std::cerr << "EXC_1" << std::endl;
        } END_TRY
        Second s1(1);
    } CATCH(EXC_1) {
        std::cerr << "error in code!" << std::endl;
    } END_TRY

    return 0;
}