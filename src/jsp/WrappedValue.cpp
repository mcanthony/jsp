/*
 * JSP: https://github.com/arielm/jsp
 * COPYRIGHT (C) 2014-2015, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/jsp/blob/master/LICENSE
 */

#include "jsp/WrappedValue.h"

#include "chronotext/Log.h"

using namespace std;
using namespace chr;

namespace jsp
{
    bool WrappedValue::LOG_VERBOSE = false;
    set<void*> WrappedValue::heapTraced;

    // ---
    
    WrappedValue::WrappedValue()
    :
    value(UndefinedValue())
    {
        dump(__PRETTY_FUNCTION__);
    }
    
    WrappedValue::~WrappedValue()
    {
        heapTraced.erase(this);
        dump(__PRETTY_FUNCTION__);
    }
    
    WrappedValue::WrappedValue(const Value &v)
    :
    value(v)
    {
        dump(__PRETTY_FUNCTION__);
    }
    
    WrappedValue& WrappedValue::operator=(const Value &v)
    {
        set(v);
        dump(__PRETTY_FUNCTION__);

        return *this;
    }
    
    WrappedValue::WrappedValue(const WrappedValue &other)
    :
    value(other.value)
    {
        dump(__PRETTY_FUNCTION__);
    }
    
    void WrappedValue::operator=(const WrappedValue &other)
    {
        set(other.value);
        dump(__PRETTY_FUNCTION__);
    }
    
    WrappedValue::operator const bool () const
    {
        RootedValue rooted(cx, value);
        return ToBoolean(rooted);
    }
    
    // ---
    
    bool WrappedValue::operator==(const WrappedValue &other) const
    {
        return compare(value, other.value);
    }
    
    bool WrappedValue::operator!=(const WrappedValue &other) const
    {
        return !compare(value, other.value);
    }
    
    bool WrappedValue::operator==(const Value &other) const
    {
        return compare(value, other);
    }
    
    bool WrappedValue::operator!=(const Value &other) const
    {
        return !compare(value, other);
    }
    
    bool WrappedValue::operator==(const nullptr_t) const
    {
        return compare(value, nullptr);
    }
    
    bool WrappedValue::operator!=(const nullptr_t) const
    {
        return !compare(value, nullptr);
    }
    
    bool WrappedValue::operator==(const JSObject *other) const
    {
        return compare(value, other);
    }
    
    bool WrappedValue::operator!=(const JSObject *other) const
    {
        return !compare(value, other);
    }
    
    bool WrappedValue::operator==(float other) const
    {
        return compare(value, other);
    }
    
    bool WrappedValue::operator!=(float other) const
    {
        return !compare(value, other);
    }
    
    bool WrappedValue::operator==(double other) const
    {
        return compare(value, other);
    }
    
    bool WrappedValue::operator!=(double other) const
    {
        return !compare(value, other);
    }
    
    bool WrappedValue::operator==(int32_t other) const
    {
        return compare(value, other);
    }
    
    bool WrappedValue::operator!=(int32_t other) const
    {
        return !compare(value, other);
    }
    
    bool WrappedValue::operator==(uint32_t other) const
    {
        return compare(value, other);
    }
    
    bool WrappedValue::operator!=(uint32_t other) const
    {
        return !compare(value, other);
    }
    
    bool WrappedValue::operator==(bool other) const
    {
        RootedValue rooted(cx, value);
        return ToBoolean(rooted) == other;
    }
    
    bool WrappedValue::operator!=(bool other) const
    {
        RootedValue rooted(cx, value);
        return ToBoolean(rooted) != other;
    }
    
    bool WrappedValue::operator==(const char *other) const
    {
        RootedValue rooted(cx, value);
        return compare(rooted, other);
    }
    
    bool WrappedValue::operator!=(const char *other) const
    {
        RootedValue rooted(cx, value);
        return !compare(rooted, other);
    }
    
    // ---
    
    void WrappedValue::set(const Value &newValue)
    {
        if (newValue.isMarkable())
        {
            value = newValue;
            
            if (heapTraced.count(this))
            {
                beginTracing();
            }
        }
        else if (value.isMarkable())
        {
            if (heapTraced.count(this))
            {
                endTracing();
            }
            
            value = newValue;
        }
        else
        {
            value = newValue;
        }
    }
    
    void WrappedValue::clear()
    {
        set(UndefinedValue());
        dump(__PRETTY_FUNCTION__);
    }
    
    void WrappedValue::dump(const char *prefix)
    {
        LOGD_IF(LOG_VERBOSE) << prefix << " " << this << " | value: " << JSP::writeDetailed(value) << endl;
    }
    
    // ---
    
    void WrappedValue::postBarrier()
    {
        heapTraced.insert(this);
        beginTracing();
        
        dump(__PRETTY_FUNCTION__);
    }
    
    void WrappedValue::relocate()
    {
        heapTraced.erase(this);
        endTracing();
        
        dump(__PRETTY_FUNCTION__);
    }
    
    void WrappedValue::beginTracing()
    {
        addTracerCallback(this, BIND_INSTANCE1(&WrappedValue::trace, this));
        HeapValuePostBarrier(&value);
    }
    
    void WrappedValue::endTracing()
    {
        HeapValueRelocate(&value);
        removeTracerCallback(this);
    }
    
    void WrappedValue::trace(JSTracer *trc)
    {
        JS_CallValueTracer(trc, &value, "WrappedValue");
        
        /*
         * MUST TAKE PLACE AFTER JS_CallValueTracer
         */
        dump(__PRETTY_FUNCTION__);
    }
}
