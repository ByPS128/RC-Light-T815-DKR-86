#ifndef LedController_h
#define LedController_h

#include "IButtonPressListener.h"

class LedController : public IButtonPressListener {
public:
    void init();
    void onClick() override;
};

#endif