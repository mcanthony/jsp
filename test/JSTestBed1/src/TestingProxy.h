/*
 * JSP: https://github.com/arielm/jsp
 * COPYRIGHT (C) 2014-2015, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/jsp/blob/master/LICENSE
 */

#pragma once

#include "TestingJSBase.h"

class TestingProxy : public TestingJSBase
{
public:
    void performRun(bool force = false) final;
    
    // ---
    
    double instanceValue1 = 5;
    bool instanceMethod1(JS::CallArgs args);
    void testCallbacks1();
};
