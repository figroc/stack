#ifndef STACK_SOCKET_H
#define STACK_SOCKET_H

#include <time.h>
#include <sys/socket.h>
#include <string>
#include <utility>
#include <list>
#include <boost/smart_ptr.hpp>
#include <boost/atomic.hpp>
#include <boost/thread.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/streambuf.hpp>

#include "EventArgs.h"
#include "incl/util/util.h"
#include "incl/perf/perf.h"

namespace msvc { namespace sock {

using namespace msvc::util;
using namespace msvc::perf;

class Socket {
private:
	static boost::atomic_bool _init;
	static boost::mutex _lock;
	static std::list< std::pair<int, time_t> > _closing;

	static PerfCounter *_perfQsfd;
	static PerfCounter *_perfQsnd;
	static PerfCounter *_perfAccept;
	static PerfCounter *_perfConnect;
	static PerfCounter *_perfRecv;
	static PerfCounter *_perfSend;

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
	typedef boost::shared_ptr<boost::asio::streambuf> DataBuffer;

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

	typedef IoEventArgs<DataBuffer> RecvEventArgs;
	typedef SafeFunctor<void (const boost::shared_ptr<Socket> &, RecvEventArgs, const StatePtr &)> FnRecvCallback;
	bool RecvAsync(DataBuffer &data, const FnRecvCallback &callback, const StatePtr &state = StatePtr());

	typedef IoEventArgs<DataBuffer> SendEventArgs;
	typedef SafeFunctor<void (const boost::shared_ptr<Socket> &, RecvEventArgs, const StatePtr &)> FnSendCallback;
	bool SendAsync(DataBuffer &data, const FnSendCallback &callback, const StatePtr &state = StatePtr());

private:
	boost::shared_ptr<Socket> CheckAlive(const bool callback);

	typedef CallbackContext<FnAcceptCallback> CtxAcceptCallback;
	bool AcceptAttempt(const bool user, const CtxAcceptCallback &ctx);
	void AcceptReady(const bool event, const int sock, const StatePtr &state);
	void AcceptCallback(const int err, const int sock, const CtxAcceptCallback &ctx);

	typedef CallbackContext<FnConnectCallback> CtxConnectCallback;
	bool ConnectAttempt(const std::string &remote, const CtxConnectCallback &ctx);
	void ConnectReady(const bool event, const int sock, const StatePtr &state);
	void ConnectCallback(const int err, const int sock, const std::string &remote, const CtxConnectCallback &ctx);

	typedef CallbackContext<FnRecvCallback> CtxRecvCallback;
	bool RecvAttempt(const bool user, DataBuffer &data, const CtxRecvCallback &ctx);
	void RecvReady(const bool event, const int sock, const StatePtr &state);
	void RecvCallback(DataBuffer &data, const CtxRecvCallback &ctx);

	typedef CallbackContext<FnSendCallback> CtxSendCallback;
	bool SendAttempt(const bool user, DataBuffer &data, const CtxSendCallback &ctx);
	void SendReady(const bool event, const int sock, const StatePtr &state);
	void SendCallback(DataBuffer &data, const CtxSendCallback &ctx);

private:
	boost::circular_buffer< std::pair<DataBuffer, CtxSendCallback> > _queue;

};

} }

#endif // STACK_SOCKET_H
