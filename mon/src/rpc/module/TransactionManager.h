#ifndef MSVC_RPC_TRANSACTION_MANAGER_H
#define MSVC_RPC_TRANSACTION_MANAGER_H

#include "RpcTransaction.h"
#include <utility>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/unordered_map.hpp>

namespace msvc { namespace rpc { namespace _di {

using namespace msvc::rpc;

class TransactionManager {
private:
	typedef boost::chrono::steady_clock::time_point time_point;
	typedef std::pair<RpcTransaction, time_point> tx_item;

	boost::mutex _lock;
	boost::unordered_map<std::string, tx_item> _txs;

public:
	void Register(const RpcTransaction &tx);
	void Unregister(const RpcTransaction &tx);

};

}}}

#endif
