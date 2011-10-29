#ifndef COCAINE_DRIVERS_ABSTRACT_HPP
#define COCAINE_DRIVERS_ABSTRACT_HPP

#include "cocaine/common.hpp"
#include "cocaine/engine.hpp"

namespace cocaine { namespace engine { namespace drivers {

class driver_t:
    public boost::noncopyable
{
    public:
        driver_t(const std::string& method, boost::shared_ptr<engine_t> parent):
            m_method(method),
            m_parent(parent)
        {
            syslog(LOG_DEBUG, "driver [%s:%s]: constructing", 
                m_parent->name().c_str(), m_method.c_str());
        }
        
        virtual ~driver_t() {
            syslog(LOG_DEBUG, "driver [%s:%s]: destructing",
                m_parent->name().c_str(), m_method.c_str());
        }

        std::string method() const {
            return m_method;
        }

        virtual Json::Value info() const = 0;

    protected:
        const std::string m_method;
        const boost::shared_ptr<engine_t> m_parent;
};

}}}

#endif
