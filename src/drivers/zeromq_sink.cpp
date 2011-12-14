//
// Copyright (C) 2011 Andrey Sibiryov <me@kobology.ru>
//
// Licensed under the BSD 2-Clause License (the "License");
// you may not use this file except in compliance with the License.
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <boost/algorithm/string/join.hpp>
#include <boost/assign.hpp>

#include "cocaine/drivers/zeromq_sink.hpp"
#include "cocaine/engine.hpp"

using namespace cocaine::engine::driver;

zeromq_sink_t::zeromq_sink_t(engine_t* engine, const std::string& method, const Json::Value& args):
    zeromq_server_t(engine, method, args, ZMQ_PULL)
{ }

Json::Value zeromq_sink_t::info() const {
    Json::Value result(zeromq_server_t::info());

    result["type"] = "zeromq-sink";

    return result;
}

void zeromq_sink_t::process(ev::idle&, int) {
    if(m_socket.pending()) {
        do {
            boost::shared_ptr<publication_t> job(new publication_t(this, client::policy_t()));
            BOOST_VERIFY(m_socket.recv(job->request()));
            m_engine->enqueue(job);
        } while(m_socket.more()); 
    } else {
        m_processor.stop();
    }
}
