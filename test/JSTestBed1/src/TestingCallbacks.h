/*
 * JSP: https://github.com/arielm/jsp
 * COPYRIGHT (C) 2014-2015, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/jsp/blob/master/LICENSE
 */

#pragma once

#include "TestingJSBase.h"

class TestingCallbacks : public TestingJSBase
{
public:
    void setup() final;
    void shutdown() final;
    void run(bool force = false) final;
    
    // ---
    
    void testMethodDispatch1();
    void testMethodDispatch2();
    
    void testMethodDispatchExtended();

    // ---
    
    static std::map<int32_t, std::function<bool(JS::CallArgs args)>> callbacks1;
    static int32_t callbackCount1;
    
    template<class F>
    void registerCallback1(JS::HandleObject object, const std::string &name, F&& f);
    
    double instanceValue1 = 3;
    bool instanceMethod1(JS::CallArgs args);
    void testInstanceMethod1();
    
    // ---
    
    double instanceValue2 = 2;
    bool instanceMethod2(JS::CallArgs args);
    void testInstanceMethod2();
    
    // ---
    
    void testDefinedFunctionRooting1();
    void testDefinedFunctionRooting2();
    
    // ---
    
    void testRegistrationMacro();
};