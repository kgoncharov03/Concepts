#include "ObjectStack.h"

int tryNum = 0;
int throwNum = 0;
int catchNum = 0;
std::vector<std::vector<ObjectStack*>> tryStack;
std::vector<JumpBuffer> jumpPointers;

const int ObjectStack::getCode(Exception exc) {
    switch (exc) {
        case OK_EXC:
            return 0;
        case CONTINUE_OK:
            return 1;
        case EXC_1:
            return 2;
        case EXC_2:
            return 3;
        default:
            return 4;
    }
}

const Exception ObjectStack::getException(int code) {
    switch (code) {
        case 0:
            return OK_EXC;
        case 1:
            return CONTINUE_OK;
        case 2:
            return EXC_1;
        case 3:
            return EXC_2;
        default:
            return OTHER_EXC;
    }
}

ObjectStack::~ObjectStack() {
    if (tryCounter_ > 0) {
        tryStack[tryCounter_ - 1].pop_back();
        if (tryStack[tryCounter_ - 1].size() == 0)
            tryStack.pop_back();
    }
}

ObjectStack::ObjectStack() {
    tryCounter_ = tryNum;
    if (tryCounter_ > 0) {
        if (tryStack.size() < tryCounter_)
            tryStack.push_back(std::vector<ObjectStack*>());

        tryStack[tryCounter_ - 1].push_back(this);
    }
}
