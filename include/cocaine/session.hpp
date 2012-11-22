/*
    Copyright (c) 2011-2012 Andrey Sibiryov <me@kobology.ru>
    Copyright (c) 2011-2012 Other contributors as noted in the AUTHORS file.

    This file is part of Cocaine.

    Cocaine is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    Cocaine is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>. 
*/

#ifndef COCAINE_SESSION_HPP
#define COCAINE_SESSION_HPP

#include "cocaine/common.hpp"
#include "cocaine/io.hpp"
#include "cocaine/slave.hpp"
#include "cocaine/unique_id.hpp"

#include "cocaine/api/event.hpp"

#include "cocaine/helpers/birth_control.hpp"

namespace cocaine { namespace engine {

typedef boost::shared_ptr<
    api::stream_t
> stream_ptr_t;

struct session_t:
    public birth_control<session_t>
{
    session_t(const api::event_t& event,
              const boost::shared_ptr<api::stream_t>& upstream);

    template<class Event, typename... Args>
    bool
    send(Args&&... args);

    void
    attach(slave_t * const slave);

    void
    abandon(error_code code,
            const std::string& message);

public:
    // Session ID.
    const unique_id_t id;

    // Session event type.
    const api::event_t event;

    // Session upstream.
    const boost::shared_ptr<api::stream_t> upstream;

private:
    typedef std::vector<
        std::pair<int, std::string>
    > chunk_list_t;

    // Request chunk cache.
    chunk_list_t m_cache;
    boost::mutex m_mutex;

    // Responsible slave.
    slave_t * m_slave;
};

template<class Event, typename... Args>
bool
session_t::send(Args&&... args) {
    if(!m_slave) {
        io::detail::outgoing<Event> event(id, std::forward<Args>(args)...);

        boost::unique_lock<boost::mutex> lock(m_mutex);

        m_cache.emplace_back(
            io::detail::outgoing<Event>::id,
            std::string(event.data(), event.size())
        );

        return true;
    }

    return m_slave->send<Event>(id, std::forward<Args>(args)...);    
}

}}

#endif
