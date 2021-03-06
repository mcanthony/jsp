/*
 * JSP: https://github.com/arielm/jsp
 * COPYRIGHT (C) 2014-2015, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/jsp/blob/master/LICENSE
 */

#include "TestingWrappedValue.h"

#include "chronotext/Context.h"

using namespace std;
using namespace ci;
using namespace chr;

using namespace jsp;

void TestingWrappedValue::performSetup()
{
    WrappedValue::LOG_VERBOSE = false;
    WrappedObject::LOG_VERBOSE = false;
}

void TestingWrappedValue::performShutdown()
{
    WrappedValue::LOG_VERBOSE = false;
    WrappedObject::LOG_VERBOSE = false;
}

void TestingWrappedValue::performRun(bool force)
{
    if (force || true)
    {
        JSP_TEST(force || true, testStackCreationAndAssignment);
        JSP_TEST(force || true, testAutomaticConversion);
    }
    
    if (force || true)
    {
        JSP_TEST(force || true, testObjectStackRooting1);
        JSP_TEST(force || true, testObjectStackRooting2);
        JSP_TEST(force || true, testStringStackRooting1);
        JSP_TEST(force || true, testStringStackRooting2);
    }
    
    if (force || true)
    {
        JSP_TEST(force || true, testValueComparison);
        JSP_TEST(force || true, testObjectComparison);
        JSP_TEST(force || true, testAutomaticComparison);
    }

    if (force || true)
    {
        JSP_TEST(force || true, testBooleanComparison);
        JSP_TEST(force || true, testBooleanCasting);
        JSP_TEST(force || true, testHeapBooleanCasting);
    }
    
    if (force || true)
    {
        JSP_TEST(force || true, testStringComparison1);
        JSP_TEST(force || true, testStringComparison2);
        JSP_TEST(force || true, testStringCasting);
    }
    
    if (force || true)
    {
        JSP_TEST(force || true, testRootedComparison);
        JSP_TEST(force || true, testHeapComparison);
    }
    
    if (force || true)
    {
        JSP_TEST(force || true, testAutoWrappedValueVector);
        JSP_TEST(force || true, testHeapWrappedToHandle1);
    }
}

// ---

void TestingWrappedValue::testStackCreationAndAssignment()
{
    WrappedValue wrapped;
    JSP_CHECK(wrapped.isUndefined());
    
    wrapped = NumberValue(55.55);
    JSP_CHECK(wrapped.toDouble() == 55.55);
    
    wrapped = Int32Value(-255);
    JSP_CHECK(wrapped.toInt32() == -255);
    
    wrapped = NumberValue(0xff123456);
    JSP_CHECK(wrapped.toNumber() == 0xff123456);

    wrapped = FalseValue();
    JSP_CHECK(wrapped.isFalse());
    
    wrapped = NullValue();
    JSP_CHECK(wrapped.isNull());

    // --
    
    wrapped = ObjectOrNullValue(Barker::create("ASSIGNED-TO-VALUE 1"));
    JSP_CHECK(writeGCDescriptor(wrapped) == 'n'); // I.E. IN NURSERY
    
    forceGC();
    JSP_CHECK(!isHealthy(wrapped)); // REASON: GC-THING NOT ROOTED
    
    // ---
    
    wrapped = StringValue(toJSString("assigned-to-value 1"));
    JSP_CHECK(writeGCDescriptor(wrapped) == 'W'); // I.E. TENURED
    
    forceGC();
    JSP_CHECK(!isHealthy(wrapped)); // REASON: GC-THING NOT ROOTED
}

void TestingWrappedValue::testAutomaticConversion()
{
    WrappedValue wrapped;
    
    wrapped = 2.5f;
    JSP_CHECK(float(wrapped.toDouble()) == 2.5f);

    wrapped = 55.55;
    JSP_CHECK(wrapped.toDouble() == 55.55);

    wrapped = -255;
    JSP_CHECK(wrapped.toInt32() == -255);
    
    wrapped = 0xff123456;
    JSP_CHECK(wrapped.toNumber() == 0xff123456);
    
    wrapped = false;
    JSP_CHECK(wrapped.isFalse());
    
    wrapped = nullptr;
    JSP_CHECK(wrapped.isNull());
    
    wrapped = newPlainObject();
    JSP_CHECK(wrapped.isObject());
    
    // ---
    
    wrapped = "from ASCII const chars";
    JSP_CHECK(stringEqualsASCII(wrapped.toString(), "from ASCII const chars"));
    
    wrapped = string("from UTF8 (אבג) string");
    JSP_CHECK(stringEquals(wrapped.toString(), "from UTF8 (אבג) string"));
    
    wrapped = "";
    JSP_CHECK(stringEqualsASCII(wrapped.toString(), ""));
    JSP_CHECK(stringEquals(wrapped.toString(), ""));
}

// ---

/*
 * DEMONSTRATING THE WRONG-WAY TO WORK
 */
void TestingWrappedValue::testObjectStackRooting1()
{
    RootedObject rootedObject(cx, Barker::create("STACK-ROOTED STANDALONE"));
    JSP_CHECK(isInsideNursery(rootedObject.get())); // JS-OBJECTS ARE USUALLY CREATED IN THE NURSERY
    
    /*
     * MISTAKE: ASSERTING THAT THE ASSIGNED GC-POINTER WILL NOT MOVE
     * IN PRACTICE: IT WILL, BECAUSE THE GC-THING IS IN THE NURSERY
     */
    WrappedValue wrapped(rootedObject);
    
    forceGC();
    JSP_CHECK(!isHealthy(wrapped)); // REASON: GC-POINTER HAS MOVED FROM THE NURSERY
    JSP_CHECK(Barker::isHealthy("STACK-ROOTED STANDALONE")); // REASON: GC-THING IS ROOTED
    
    // ---
    
    rootedObject = nullptr;
    
    forceGC();
    JSP_CHECK(Barker::isFinalized("STACK-ROOTED STANDALONE"));
}

/*
 * DEMONSTRATING THE RIGHT-WAY TO WORK
 */
void TestingWrappedValue::testObjectStackRooting2()
{
    Rooted<WrappedValue> rootedWrapped(cx, Barker::create("STACK-ROOTED VIA-WRAPPED-VALUE"));
    JSP_CHECK(isInsideNursery(rootedWrapped.get())); // JS-OBJECTS ARE USUALLY CREATED IN THE NURSERY
    
    forceGC();
    JSP_CHECK(isHealthy(rootedWrapped.get()));
    JSP_CHECK(Barker::isHealthy("STACK-ROOTED VIA-WRAPPED-VALUE"));
    
    // ---
    
    /*
     * WILL CAUSE GC-THING TO BE FINALIZED DURING THE NEXT GC
     */
    rootedWrapped = 123;
    
    forceGC();
    JSP_CHECK(Barker::isFinalized("STACK-ROOTED VIA-WRAPPED-VALUE"));
}

/*
 * DEMONSTRATING THE WRONG-WAY TO WORK
 */
void TestingWrappedValue::testStringStackRooting1()
{
    RootedString rootedString(cx, toJSString("stack-rooted standalone"));
    JSP_CHECK(!isInsideNursery(rootedString.get())); // JS-STRINGS ARE ALWAYS TENURED
    
    /*
     * MISTAKE: ASSERTING THAT THE ASSIGNED GC-POINTER WILL NOT MOVE
     * IN PRACTICE: IT WON'T, BUT ONLY BECAUSE THE GC-THING IS TENURED
     */
    WrappedValue wrapped(rootedString);
    
    forceGC();
    JSP_CHECK(isHealthy(wrapped)); // REASON: GC-POINTER WAS TENURED AND GC-THING IS ROOTED
    
    // ---
    
    rootedString = nullptr;
    
    forceGC();
    JSP_CHECK(!isHealthy(wrapped)); // REASON: GC-THING IS NOT ROOTED ANYMORE
}

/*
 * DEMONSTRATING THE RIGHT-WAY TO WORK
 */
void TestingWrappedValue::testStringStackRooting2()
{
    Rooted<WrappedValue> rootedWrapped(cx, "stack-rooted via-wrapped-value");
    JSP_CHECK(!isInsideNursery(rootedWrapped.get())); // JS-STRINGS ARE ALWAYS TENURED

    JSString *gcPointer = rootedWrapped->toString();

    forceGC();
    JSP_CHECK(isHealthy(rootedWrapped.get()));
    JSP_CHECK(isHealthy(gcPointer)); // REASON: GC-POINTER WAS TENURED
    
    // ---
    
    /*
     * WILL CAUSE GC-THING TO BE FINALIZED DURING THE NEXT GC
     */
    rootedWrapped = 123;
    
    forceGC();
    JSP_CHECK(!isHealthy(gcPointer));
}

// ---

void TestingWrappedValue::testValueComparison()
{
    WrappedValue wrapped1 = NumberValue(33.33);
    JSP_CHECK(NumberValue(33.33) == wrapped1);
    JSP_CHECK(wrapped1 == NumberValue(33.33)); // NO TEMPORARIES, THANKS TO WrappedValue::operator==(const Value&)
    
    WrappedValue wrapped2;
    JSP_CHECK(wrapped2 != wrapped1); // NO TEMPORARIES, THANKS TO WrappedValue::operator==(const WrappedValue&)
    
    wrapped2 = NumberValue(33.33);
    JSP_CHECK(wrapped2 == wrapped1); // NO TEMPORARIES, THANKS TO WrappedValue::operator!=(const WrappedValue&)
}

void TestingWrappedValue::testObjectComparison()
{
    /*
     * WARNING: NOT GC-SAFE
     *
     * REASON: THE CREATION OF object2 COULD TRIGGER A GC (WHICH IN TURN WOULD CAUSE THE FINALIZATION OF object1)
     * PROBABILITY: NULL (FORCED-GC IS TAKING PLACE BEFORE EACH TEST)
     */
    
    JSObject *object1 = Barker::create("BARKER 1");
    JSObject *object2 = Barker::create("BARKER 2");
    
    WrappedValue wrapped1(object1);
    WrappedValue wrapped2(object2);
    
    JSP_CHECK(wrapped1 == object1); // NO TEMPORARIES, THANKS TO WrappedValue::operator==(const JSObject*)
    
    JSP_CHECK(wrapped1 != object2); // NO TEMPORARIES, THANKS TO WrappedValue::operator!=(const JSObject*)
    JSP_CHECK(wrapped1 != wrapped2);
    
    wrapped2 = object1;
    JSP_CHECK(wrapped1 == wrapped2);
}

void TestingWrappedValue::testAutomaticComparison()
{
    /*
     * WARNING: NOT GC-SAFE
     *
     * REASON: THE COMPARISON OF wrapped WITH "foo" COULD TRIGGER A GC
     * PROBABILITY: NULL (FORCED-GC IS TAKING PLACE BEFORE EACH TEST)
     */
    
    WrappedValue wrapped = 33.33;
    JSP_CHECK(wrapped == 33.33);
    JSP_CHECK(wrapped != 55.55);
    
    wrapped = -255;
    JSP_CHECK(wrapped == -255);
    JSP_CHECK(wrapped != 127);
    
    JSP_CHECK(wrapped != "foo"); // SLOW, BECAUSE wrapped WILL BE CONVERTED TO STRING (ACCORDING TO JAVASCRIPT RULES)
    JSP_CHECK(wrapped != nullptr);
    
    wrapped = 0xff123456; // VALUES > 0x7fffffff ARE PROPERLY CONSIDERED AS UNSIGNED
    JSP_CHECK(wrapped == 0xff123456);
    JSP_CHECK(wrapped != 0x12345678);
    
    JSP_CHECK(wrapped);
    
    wrapped = 0;
    JSP_CHECK(!wrapped);
}

// ---

void TestingWrappedValue::testBooleanComparison()
{
    WrappedValue wrapped = true;
    JSP_CHECK(wrapped == true); // NO TEMPORARIES, THANKS TO WrappedValue::operator==(bool)
    JSP_CHECK(wrapped != false); // NO TEMPORARIES, THANKS TO WrappedValue::operator!=(bool)
}

/*
 * WrappedValue::operator const bool() IN ACTION
 *
 *
 * WARNING: NOT GC-SAFE
 *
 * REASON: JS::Value::ToBoolean (USED BEHIND THE SCENE) COULD TRIGGER A GC
 * PROBABILITY: NULL (FORCED-GC IS TAKING PLACE BEFORE EACH TEST)
 */
void TestingWrappedValue::testBooleanCasting()
{
    WrappedValue wrapped;
    JSP_CHECK(!wrapped);
    
    JSP_CHECK(!evaluateBoolean("var tmp = {}; return (tmp.foo ? true : false)"));
    
    //
    
    wrapped = nullptr;
    JSP_CHECK(!wrapped);
    
    JSP_CHECK(!evaluateBoolean("var tmp = {foo: null}; return (tmp.foo ? true : false)"));
    
    //

    wrapped = ""; // ACCORDING TO JAVASCRIPT RULES: EMPTY STRINGS CAST TO FALSE
    JSP_CHECK(!wrapped);
    
    JSP_CHECK(!evaluateBoolean("var tmp = {foo: ''}; return (tmp.foo ? true : false)"));
    
    //
    
    wrapped = "false"; // ACCORDING TO JAVASCRIPT RULES: NON-EMPTY STRINGS CAST TO TRUE
    JSP_CHECK(wrapped);
    
    JSP_CHECK(evaluateBoolean("var tmp = {foo: 'false'}; return (tmp.foo ? true : false)"));
    
    /*
     * THE FOLLOWING IS OBVIOUS-ENOUGH NOT TO PROVIDE PROOFS...
     */
    
    wrapped = 0;
    JSP_CHECK(!wrapped);
    
    wrapped = false;
    JSP_CHECK(!wrapped);
}

/*
 * WrappedValue::operator const bool() IN ACTION
 */
void TestingWrappedValue::testHeapBooleanCasting()
{
    JSP_CHECK(!heapWrapped);

    heapWrapped = nullptr;
    JSP_CHECK(!heapWrapped);

    heapWrapped = "";
    JSP_CHECK(!heapWrapped);

    heapWrapped = "false";
    JSP_CHECK(heapWrapped);
    
    heapWrapped = 0;
    JSP_CHECK(!heapWrapped);
    
    heapWrapped = false;
    JSP_CHECK(!heapWrapped);
}

// ---

/*
 * WARNING: THE FOLLOWING 3 ARE NOT GC-SAFE
 *
 * REASON: SEVERAL STRING-RELATED OPERATIONS USED BEHIND THE SCENES COULD TRIGGER A GC
 * PROBABILITY: NULL (FORCED-GC IS TAKING PLACE BEFORE EACH TEST)
 */

void TestingWrappedValue::testStringComparison1()
{
    WrappedValue wrapped = "foo";
    JSP_CHECK(wrapped == "foo"); // NO TEMPORARIES, THANKS TO WrappedValue::operator==(const char*)
    JSP_CHECK(wrapped != "FOO"); // NO TEMPORARIES, THANKS TO WrappedValue::operator!=(const char*)

    //
    
    string fo = "fo";
    string FO = "FO";

    wrapped = FO + "O";
    JSP_CHECK(wrapped == FO + "O"); // NO TEMPORARIES, THANKS TO WrappedValue::operator==(const string&)
    JSP_CHECK(wrapped != fo + "o"); // NO TEMPORARIES, THANKS TO WrappedValue::operator==(const string&)
    
    //
    
    JSP_CHECK(WrappedValue("bar") == toValue("bar")); // NO TEMPORARIES, THANKS TO WrappedValue::operator==(const Value&)
    JSP_CHECK(WrappedValue("BAR") != toValue("bar")); // NO TEMPORARIES, THANKS TO WrappedValue::operator!=(const Value&)
    
    compareConstChars("hello", "HELLO");
    compareConstStrings("hello", "HELLO");
}

void TestingWrappedValue::compareConstChars(const char *s1, const char *s2)
{
    WrappedValue wrapped = s1;
    JSP_CHECK(wrapped == s1);
    JSP_CHECK(wrapped != s2);
    
    JSP_CHECK(compare(wrapped, s1));
    JSP_CHECK(!compare(wrapped, s2));
}

void TestingWrappedValue::compareConstStrings(const string &s1, const string &s2)
{
    WrappedValue wrapped = s1;
    JSP_CHECK(wrapped == s1);
    JSP_CHECK(wrapped != s2);
    
    JSP_CHECK(compare(wrapped, s1));
    JSP_CHECK(!compare(wrapped, s2));
}

//

/*
 * WARNING: NOT GC-SAFE
 *
 * REASON: SEVERAL STRING-RELATED OPERATIONS USED BEHIND THE SCENES COULD TRIGGER A GC
 * PROBABILITY: NULL (FORCED-GC IS TAKING PLACE BEFORE EACH TEST)
 */
void TestingWrappedValue::testStringComparison2()
{
    WrappedValue wrapped;
    JSP_CHECK(wrapped != "undefined");

    JSP_CHECK(!evaluateBoolean("var tmp = {}; return (tmp.foo == 'undefined')"));

    //
    
    wrapped = nullptr;
    JSP_CHECK(wrapped != "null");
    
    JSP_CHECK(!evaluateBoolean("var tmp = {foo: null}; return(tmp.foo == 'null')"));
    
    //
    
    wrapped = 123;
    JSP_CHECK(wrapped == "123");
    JSP_CHECK(wrapped != "xxx");
    
    JSP_CHECK(evaluateBoolean("var tmp = {foo: 123}; return (tmp.foo == '123')"));
    JSP_CHECK(!evaluateBoolean("var tmp = {foo: 123}; return (tmp.foo == 'xxx')"));
}

void TestingWrappedValue::testStringCasting()
{
    Rooted<WrappedValue> rootedWrapped(cx);
    JSP_CHECK(toString(rootedWrapped) == "undefined");
    
    rootedWrapped = 2.5f;
    JSP_CHECK(toString(rootedWrapped) == "2.5");
    
    rootedWrapped = 55.55;
    JSP_CHECK(toString(rootedWrapped) == "55.55");

    rootedWrapped = -255;
    JSP_CHECK(toString(rootedWrapped) == "-255");

    rootedWrapped = 0xff123456;
    JSP_CHECK(toString(rootedWrapped) == "4279383126");

    rootedWrapped = true;
    JSP_CHECK(toString(rootedWrapped) == "true");
    
    rootedWrapped.set(nullptr); // XXX: IMPOSSIBLE TO ASSIGN nullptr DIRECTLY TO A Rooted<WrappedValue> DUE TO AMBIGUITIES AT THE Rooted<T> LEVEL
    JSP_CHECK(toString(rootedWrapped) == "null");
    
    rootedWrapped = newPlainObject();
    JSP_CHECK(toString(rootedWrapped) == "[object Object]");
}

// ---

void TestingWrappedValue::testRootedComparison()
{
    Rooted<WrappedValue> rootedWrapped1A(cx, 123);
    Rooted<WrappedValue> rootedWrapped1B(cx, 123);
    
    JSP_CHECK(rootedWrapped1A == rootedWrapped1B); // NO TEMPORARIES, THANKS TO WrappedValue::operator==(const Value&)
    JSP_CHECK(rootedWrapped1A == 123); // CREATES A TEMPORARY WrappedValue
    JSP_CHECK(rootedWrapped1A != 0); // CREATES A TEMPORARY WrappedValue

    // ---
    
    Rooted<WrappedValue> rootedWrapped2A(cx, "hello");
    Rooted<WrappedValue> rootedWrapped2B(cx, "hello");
    
    JSP_CHECK(rootedWrapped2A == rootedWrapped2B); // NO TEMPORARIES, THANKS TO WrappedValue::operator==(const Value&)
    JSP_CHECK(rootedWrapped2A == "hello"); // CREATES A TEMPORARY WrappedValue
    JSP_CHECK(rootedWrapped2A != ""); // CREATES A TEMPORARY WrappedValue
    
    // ---
    
    JSObject *barker = Barker::create("BARKER 3");
    Rooted<WrappedValue> rootedWrapped3A(cx, barker);
    Rooted<WrappedValue> rootedWrapped3B(cx, barker);
    
    JSP_CHECK(rootedWrapped3A == rootedWrapped3B); // NO TEMPORARIES, THANKS TO WrappedValue::operator==(const Value&)
    JSP_CHECK(rootedWrapped3A == barker); // CREATES A TEMPORARY WrappedValue
    JSP_CHECK(rootedWrapped3A != nullptr); // CREATES A TEMPORARY WrappedValue
}

void TestingWrappedValue::testHeapComparison()
{
    Heap<WrappedValue> heapWrapped1A(123);
    Heap<WrappedValue> heapWrapped1B(123);
    
    JSP_CHECK(heapWrapped1A == heapWrapped1B); // NO TEMPORARIES, THANKS TO WrappedValue::operator==(const Value&)
    JSP_CHECK(heapWrapped1A == 123); // CREATES A TEMPORARY WrappedValue
    JSP_CHECK(heapWrapped1A != 0); // CREATES A TEMPORARY WrappedValue
    
    // ---
    
    Heap<WrappedValue> heapWrapped2A("hello");
    Heap<WrappedValue> heapWrapped2B("hello");
    
    JSP_CHECK(heapWrapped2A == heapWrapped2B); // NO TEMPORARIES, THANKS TO WrappedValue::operator==(const Value&)
    JSP_CHECK(heapWrapped2A == "hello"); // CREATES A TEMPORARY WrappedValue
    JSP_CHECK(heapWrapped2A != ""); // CREATES A TEMPORARY WrappedValue

    // ---
    
    JSObject *barker = Barker::create("BARKER 4");
    Heap<WrappedValue> heapWrapped3A(barker);
    Heap<WrappedValue> heapWrapped3B(barker);
    
    JSP_CHECK(heapWrapped3A == heapWrapped3B); // NO TEMPORARIES, THANKS TO WrappedValue::operator==(const Value&)
    JSP_CHECK(heapWrapped3A == barker); // CREATES A TEMPORARY WrappedValue
    JSP_CHECK(heapWrapped3A != nullptr); // CREATES A TEMPORARY WrappedValue
}

// ---

void TestingWrappedValue::testAutoWrappedValueVector()
{
    AutoWrappedValueVector args;
    
    args.append(123);
    args.append("foo");
    args.append(Barker::create("AUTO-WRAPPED"));
    args.append(33.33);
    
    /*
     * JS-STRING AND BARKER ARE ROOTED VIA THE AutoWrappedValueVector
     */
    forceGC();
    
    JSP_CHECK(write(args) == "123 foo [object Barker] 33.33");
}

// ---

void TestingWrappedValue::testHeapWrappedToHandle1()
{
    heapWrapped1 = Barker::create("HEAP-WRAPPED 1");
    testHandleValue1(heapWrapped1);
    
    forceGC();
    JSP_CHECK(Barker::isHealthy("HEAP-WRAPPED 1"));
    
    // ---
    
    heapWrapped1 = nullptr;
    
    forceGC();
    JSP_CHECK(Barker::isFinalized("HEAP-WRAPPED 1"));
}

void TestingWrappedValue::testHandleValue1(HandleValue value)
{
    JSP_CHECK(Barker::bark(value));
}

/*
WrappedValue wrapped;

if (true)
{
    RootedString rooted(cx, toJSString("not atomized"));
    wrapped = StringValue(rooted);
    
    forceGC();
    
    if (JSP_CHECK(isHealthy(wrapped.get()), "HEALTHY VALUE"))
    {
        JSP_CHECK(writeGCDescriptor(wrapped) == 'B');
    }
}
else
{
//  TODO: TEST StringIsPermanentAtom(JSString *str) IN barrier.h

    string atomized = "atomized";
    wrapped = StringValue(Atomize(cx, atomized.data(), atomized.size()));
    
    forceGC();
    
    if (JSP_CHECK(!isHealthy(wrapped.get()), "UNHEALTHY VALUE")) // REASON: ATOMS ARE NOT ROOTED BY DEFAULT
    {
        JSP_CHECK(writeGCDescriptor(wrapped) == 'P');
    }
}
*/
