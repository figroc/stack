#ifndef STACK_SOCKET_H
#define STACK_SOCKET_H

#include <time.h>
#include <sys/socket.h>
#include <string>
#include <utility>
#include <list>
#include <boost/smart_ptr.hpp>
#include <boost/atomic.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/thread/mutex.hpp>

#include "EventArgs.h"
#include "BufferNode.h"
#include "util/util_export.h"
#include "perf/perf_export.h"

namespace msvc { namespace stack {

using namespace msvc::util;
using namespace msvc::perf;

class Socket {
private:
	static boost::atomic_bool _init;
	static boost::mutex _lock;
	static std::list< std::pair<int, time_t> > _closing;

	static boost::shared_ptr<PerfCounter> _perfQsfd;
	static boost::shared_ptr<PerfCounter> _perfQsnd;
	static boost::shared_ptr<PerfCounter> _perfAccept;
	static boost::shared_ptr<PerfCounter> _perfConnect;
	static boost::shared_ptr<PerfCounter> _perfRecv;
	static boost::shared_ptr<PerfCounter> _perfSend;

public:
	static void Init();
	static boost::shared_ptr<Socket> Fetch();

private:
	static void DelayCloseFD(const int sockfd);
	static void ClosePendingFD();

	static int NewSockFD(const struct sockaddr_storage &addr);
	static int InitSockFD(const int sockfd);

	static std::string GetSockAddr(const int sock, const bool local);
	static std::string ParseSockAddr(const struct sockaddr_storage &addr);
	static struct sockaddr_storage ParseSockAddr(const std::string &endpoint);
	static std::pair<std::string, std::string> ParseEndpoint(const std::string &endpoint);

	// EINTR would not occur for non-blocking, should we treat it as error or just guard around it?
	inline static bool RetryAccept(const int err) { return EINTR == err || ECONNABORTED == err || EPROTO == err; }
	inline static bool RetryConnect(const int err) { return EINTR == err; }
	inline static bool RetryRecv(const int err) { return EINTR == err; }
	inline static bool RetrySend(const int err) { return EINTR == err; }
	inline static bool WouldBlock(const int err) {
		return EWOULDBLOCK == err || EAGAIN == err || EINPROGRESS == err || ENOBUFS == err;
	}

private:
	boost::weak_ptr<Socket> _ref;
	std::string _localEP;
	std::string _remoteEP;
	int _sock;

	bool _listening;
	boost::atomic_bool _accepting;
	boost::atomic_bool _connecting;
	boost::atomic_bool _recving;
	boost::mutex _sending;

private:
	Socket();
	Socket(const Socket &socket);
	Socket &operator=(const Socket &socket);
public:
	virtual ~Socket();

public:
	inline bool IsBound() const { return -1 != _sock; }
	inline std::string GetLocalEP() const { return _localEP; }
	inline std::string GetRemoteEP() const { return _remoteEP; }

	void Listen(const std::string &localEP);

	typedef IoEventArgs< boost::shared_ptr<Socket> > AcceptEventArgs;
	typedef SafeFunctor<void (const boost::shared_ptr<Socket> &, AcceptEventArgs, const StatePtr &)> FnAcceptCallback;
	bool AcceptAsync(const FnAcceptCallback &callback, const StatePtr &state = StatePtr());

	typedef IoEventArgs<std::string> ConnectEventArgs;
	typedef SafeFunctor<void (const boost::shared_ptr<Socket> &, ConnectEventArgs, const StatePtr &)> FnConnectCallback;
	bool ConnectAsync(const std::string &remote, const FnConnectCallback &callback, const StatePtr &state = StatePtr());

	typedef IoEventArgs<BufferNode> RecvEventArgs;
	typedef SafeFunctor<void (const boost::shared_ptr<Socket> &, RecvEventArgs, const StatePtr &)> FnRecvCallback;
	bool RecvAsync(BufferNode &data, const FnRecvCallback &callback, const StatePtr &state = StatePtr());

	typedef IoEventArgs<BufferNode> SendEventArgs;
	typedef SafeFunctor<void (const boost::shared_ptr<Socket> &, RecvEventArgs, const StatePtr &)> FnSendCallback;
	bool SendAsync(BufferNode &data, const FnSendCallback &callback, const StatePtr &state = StatePtr());

private:
	boost::shared_ptr<Socket> CheckAlive(const bool callback);

	typedef CallbackContext<FnAcceptCallback> CtxAcceptCallback;
	bool AcceptAttempt(const bool user, const CtxAcceptCallback &ctx);
	void AcceptReady(const bool event, const int sock, const StatePtr &state);
	void AcceptCallback(const int err, const int sock, const CtxAcceptCallback &ctx);

	typedef CallbackContext<FnConnectCallback> CtxConnectCallback;
	bool ConnectAttempt(const std::string &remote, const CtxConnectCallback &ctx);
	void ConnectReady(const bool event, const int sock, const StatePtr &state);
	void ConnectCallback(const int err, const int sock, const string &remote, const CtxConnectCallback &ctx);

	typedef CallbackContext<FnRecvCallback> CtxRecvCallback;
	bool RecvAttempt(const bool user, BufferNode &data, const CtxRecvCallback &ctx);
	void RecvReady(const bool event, const int sock, const StatePtr &state);
	void RecvCallback(BufferNode &data, const CtxRecvCallback &ctx);

	typedef CallbackContext<FnSendCallback> CtxSendCallback;
	bool SendAttempt(const bool user, BufferNode &data, const CtxSendCallback &ctx);
	void SendReady(const bool event, const int sock, const StatePtr &state);
	void SendCallback(BufferNode &data, const CtxSendCallback &ctx);

private:
	boost::circular_buffer< std::pair<BufferNode, CtxSendCallback> > _queue;

};

} }

#endif // STACK_SOCKET_H
