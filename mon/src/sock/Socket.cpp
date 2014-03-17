#include <fcntl.h>
#include <netdb.h>
#include <exception>
#include <iostream>
#include <boost/chrono/duration.hpp>
#include <boost/lexical_cast.hpp>

#include "Socket.h"
#include "EPollManager.h"
#include "incl/log/log.h"

namespace msvc { namespace sock {

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::chrono;
using namespace msvc::log;

atomic_bool Socket::_init(false);
mutex Socket::_lock;
list< pair<int, time_t> > Socket::_closing;

PerfCounter *Socket::_perfQsfd = 0;
PerfCounter *Socket::_perfQsnd = 0;
PerfCounter *Socket::_perfAccept = 0;
PerfCounter *Socket::_perfConnect = 0;
PerfCounter *Socket::_perfRecv = 0;
PerfCounter *Socket::_perfSend = 0;

void Socket::Init()
{
	if (!_init.exchange(true, memory_order_seq_cst)) {
		EPollManager::Init();
		_perfQsfd = PerfManager::Fetch("socket^que^closing");
		_perfQsnd = PerfManager::Fetch("socket^que^sending");
		_perfAccept = PerfManager::Fetch("socket^opr^accept");
		_perfConnect = PerfManager::Fetch("socket^opr^connect");
		_perfRecv = PerfManager::Fetch("socket^opr^recv");
		_perfSend = PerfManager::Fetch("socket^opr^send");
		thread(&Socket::ClosePendingFD).detach();
	}
}

void Socket::DelayCloseFD(const int sockfd)
{
	const time_t now = time(NULL);
	mutex::scoped_lock lock(_lock);
	_closing.push_back(make_pair(sockfd, now));
	_perfQsfd->Increment();
}

void Socket::ClosePendingFD()
{
	// close sockfd after TIME_TO_CLOSE seconds
#ifdef LOCAL_DEBUG
	static const seconds interval(5);
	static const int TIME_TO_CLOSE = 15;
#else
	static const seconds interval(15);
	static const int TIME_TO_CLOSE = 45;
#endif
	while (true) try {
		this_thread::sleep_for(interval);
		const time_t now = time(NULL);
		{
			mutex::scoped_lock lock(_lock);
			while (!_closing.empty()) {
				const pair<int, time_t> &sock = _closing.front();
				if (now - sock.second < TIME_TO_CLOSE)
					break;

				if (-1 != sock.first)
					::close(sock.first);
				_closing.pop_front();
				_perfQsfd->Decrement();
			}
		}
	} catch (const std::exception &e) {
		LC_TRACE_ERROR("error closing sockfd", e);
	}
}

shared_ptr<Socket> Socket::Fetch()
{	// maybe we can use boost::enable_shared_from_this
	shared_ptr<Socket> socket = shared_ptr<Socket>(new Socket());
	socket->_ref = socket;
	return socket;
}

int Socket::NewSockFD(const struct sockaddr_storage &addr)
{
	const int sock = ::socket(addr.ss_family, SOCK_STREAM, IPPROTO_TCP);
	if (-1 == sock)
      	throw runtime_error(string("failed to create socket") + ::strerror(errno));
	return InitSockFD(sock);
}

int Socket::InitSockFD(const int sock)
{
	int opts = ::fcntl(sock, F_GETFL);
	if (-1 == opts) {
		const int err = errno; ::close(sock);
		throw runtime_error(string("failed to read fcntl: ") + ::strerror(err));
	}
	if (0 != ::fcntl(sock, F_SETFL, opts | O_NONBLOCK)) {
		const int err = errno; ::close(sock);
		throw runtime_error(string("failed to set O_NONBLOCK: ") + ::strerror(err));
	}

	static const int enable = 1;
	if (0 != ::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1) {
		const int err = errno; ::close(sock);
		throw runtime_error(string("failed to set SO_REUSADDR: ") + ::strerror(err));
	}

	static const int bufferSize = 8192;
	if (0 != ::setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize)) == -1) {
		const int err = errno; ::close(sock);
		throw runtime_error(string("failed to set SO_RCVBUF: ") + ::strerror(err));
	}

	if (0 != ::setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize)) == -1) {
		const int err = errno; ::close(sock);
		throw runtime_error(string("failed to set SO_SNDBUF: ") + ::strerror(err));
	}
	return sock;
}

string Socket::GetSockAddr(const int sock, const bool local)
{
	struct sockaddr_storage addr = {0};
	socklen_t len = sizeof(addr);
	if (-1 == (local ? ::getsockname(sock, (struct sockaddr *)&addr, &len)
		             : ::getpeername(sock, (struct sockaddr *)&addr, &len)))
		return string();
	return ParseSockAddr(addr);
}

string Socket::ParseSockAddr(const struct sockaddr_storage &addr)
{
	string name("");
	char host[NI_MAXHOST] = {0};
	char port[NI_MAXSERV] = {0};
	if (0 == ::getnameinfo((struct sockaddr *)&addr, sizeof(addr),
			host, sizeof(host), port, sizeof(port),
			NI_NUMERICHOST | NI_NUMERICSERV)) {
        name = ComposeEndpoint(host, port);
	}
	return name;
}

struct sockaddr_storage Socket::ParseSockAddr(const string &endpoint)
{
	struct sockaddr_storage addr = {0};
	const pair<string, string> serv = ParseEndpoint(endpoint);
	if (serv.first.empty() || serv.second.empty()) {
		LC_TRACE_ERROR("invalid endpoint %%", endpoint);
		return addr;
	}

	struct addrinfo hint = {0};
	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_protocol = IPPROTO_TCP;

	int status(0);
	struct addrinfo *host(0);
	if (0 != (status = ::getaddrinfo(serv.first.c_str(), serv.second.c_str(), &hint, &host))) {
		LC_TRACE_ERROR("invalid endpoint %%: %%", endpoint, ::gai_strerror(status));
		return addr;
	}

	for (struct addrinfo *p = host; 0 != p; p = p->ai_next) {
		if (AF_INET6 == p->ai_family) {
			*(struct sockaddr_in6 *)&addr = *(struct sockaddr_in6 *)(p->ai_addr);
			break;
		}
		if (AF_INET == p->ai_family) {
			*(struct sockaddr_in *)&addr = *(struct sockaddr_in *)(p->ai_addr);
			break;
		}
	}
	::freeaddrinfo(host);
	return addr;
}

Socket::Socket() : _ref(),
		_localEP(), _remoteEP(), _sock(-1),
		_listening(false), _accepting(false), _connecting(false),
		_recving(false), _sending(), _queue(10000)
{
	Init();
}

Socket::~Socket()
{
	if (-1 != _sock) {
		::shutdown(_sock, SHUT_RDWR);
		DelayCloseFD(_sock);
	}
	_sock = -1;
}

void Socket::Listen(const string &localEP)
{
	if (-1 != _sock)
		throw logic_error("socket already bound");

	struct sockaddr_storage addr = ParseSockAddr(localEP);
	const int sock = NewSockFD(addr);

	if (0 != ::bind(sock, (struct sockaddr*)&addr, sizeof(addr))) {
		const int err = errno; ::close(sock);
		throw runtime_error(string("failed to bind on ") + localEP + ": " + strerror(err));
	}

	if (0 != ::listen(sock, 1000)) {
		const int err = errno; ::close(sock);
		throw runtime_error(string("failed to listen on ") + localEP + ": " + strerror(err));
	}

	_listening = true;
	_sock = sock;
	_localEP = localEP;
	LC_TRACE_WARN("Socket::Listen[%%] | listening on %%", _sock, _localEP);
}

shared_ptr<Socket> Socket::CheckAlive(const bool user)
{
	return user ? _ref.lock() : (_ref.use_count() > 1 ? _ref.lock() : shared_ptr<Socket>());
}

bool Socket::AcceptAsync(const FnAcceptCallback &callback, const StatePtr &state)
{
	if (-1 == _sock)
		throw logic_error("socket not bound yet");
	if (_accepting.exchange(true, memory_order_relaxed))
		throw logic_error("accept in progress");
	return AcceptAttempt(true, CtxAcceptCallback(callback, state));
}

bool Socket::AcceptAttempt(const bool user, const CtxAcceptCallback &ctx)
{
	struct sockaddr_storage addr;
	socklen_t size = (socklen_t) sizeof(addr);

	int sock(-1), err(-1);
	do {
		sock = ::accept(_sock, (struct sockaddr*)&addr, &size);
		err = -1 == sock ? errno : 0;
		LC_TRACE_DEBUG("Socket::AcceptAttempt[%%:L%%] | accept result %%: %%",
				_sock, _localEP, sock, ::strerror(err));

		if(0 != err) {
			if (-1 != sock) {
				::close(sock);
				sock = -1;
			}
			if (WouldBlock(err)) {
				shared_ptr<Socket> pin = CheckAlive(user);
				if (pin && EPollManager::Watch(EPM_READ, _sock,
						make_functor(&Socket::AcceptReady, pin, true),
						make_shared<CtxAcceptCallback>(ctx)))
					return true;
			}
			if (RetryAccept(err))
				continue;
		} else {
			_perfAccept->Increment();
		}
		break;
	} while (true);

	AcceptCallback(err, sock, ctx);
	return false;
}

void Socket::AcceptReady(const bool event, const int sock, const StatePtr &state)
{
	CtxAcceptCallback *ctx = reinterpret_cast<CtxAcceptCallback *>(state.get());
	if (event) {
		AcceptAttempt(false, *ctx);
	} else {
		AcceptCallback(-1, -1, *ctx);
	}
}

void Socket::AcceptCallback(const int err, const int sock, const CtxAcceptCallback &ctx)
{
	_accepting.store(false, memory_order_relaxed);

	boost::shared_ptr<Socket> socket = Socket::Fetch();
	if (-1 != sock) try {
		socket->_localEP = GetSockAddr(sock, true);
		socket->_remoteEP = GetSockAddr(sock, false);
		socket->_sock = Socket::InitSockFD(sock);

		LC_TRACE_DEBUG("Socket::AcceptCallback[%%:L%%-R%%] | created from %%",
				sock, socket->_localEP, socket->_remoteEP, _sock);
	} catch (const std::exception &e) {
		LC_TRACE_WARN("Socket::AcceptCallback[%%:L%%-R%%] | creating from %%",
				sock, socket->_localEP, socket->_remoteEP, _sock, e);
		socket.reset();
	} else {
		socket.reset();
	}

	if (ctx.callback().callable()) try {
		ctx.callback()(_ref.lock(), socket ? AcceptEventArgs(socket) : AcceptEventArgs(err), ctx.state());
	} catch (const std::exception &e) {
		LC_TRACE_WARN("Socket::AcceptCallback[%%:L%%-R%%] | error invoking callback",
			sock, _localEP, socket->_remoteEP, e);
	}
}

bool Socket::ConnectAsync(const string &remote, const FnConnectCallback &callback, const StatePtr &state)
{
	if (-1 != _sock)
		throw logic_error("socket already bound");
	if (_connecting.exchange(true, memory_order_relaxed))
		throw logic_error("connect in progress");
	return ConnectAttempt(remote, CtxConnectCallback(callback, state));
}

bool Socket::ConnectAttempt(const string &remote, const CtxConnectCallback &ctx)
{
	struct sockaddr_storage addr = ParseSockAddr(remote);
	int sock = NewSockFD(addr);
	int err(0);
	do {
		err = 0 == ::connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr)) ? 0 : errno;
		if (0 != err) {
			if (WouldBlock(err)) {
				if (EPollManager::Watch(EPM_WRITE, sock,
						make_functor(&Socket::ConnectReady, _ref.lock(), true),
						make_shared< pair<string, CtxConnectCallback> >(remote, ctx)))
					return true;
			}
			if (RetryConnect(err))
				continue;
			if (-1 != sock)
				::close(sock);
			sock = -1;
		} else {
			_perfConnect->Increment();
		}
		break;
	} while (true);
	ConnectCallback(err, sock, remote, ctx);
	return false;
}

void Socket::ConnectReady(const bool event, const int sock, const StatePtr &state)
{
	pair<string, CtxConnectCallback>
		*ctx = reinterpret_cast<pair<string, CtxConnectCallback> *>(state.get());
	if (event) {
		ConnectCallback(0, sock, ctx->first, ctx->second);
	} else {
		::close(sock);
		ConnectCallback(-1, -1, ctx->first, ctx->second);
	}
}

void Socket::ConnectCallback(const int err, const int sock, const string &remote, const CtxConnectCallback &ctx)
{
	if (-1 != sock) {
		_localEP = GetSockAddr(_sock, true);
		_remoteEP = remote;
		_sock = sock;
	}
	LC_TRACE_DEBUG("Socket::ConnectCallback[%%:L%%-R%%] | connect result: %%",
		sock, _localEP, remote, ::strerror(err));

	_connecting.store(false, memory_order_relaxed);

	if (ctx.callback().callable()) try {
		ctx.callback()(_ref.lock(), -1 == sock ? ConnectEventArgs(remote) : ConnectEventArgs(err), ctx.state());
	} catch (const std::exception &e) {
		LC_TRACE_WARN("Socket::ConnectCallback[%%:L%%-R%%] | error invoking callback",
			sock, _localEP, remote, e);
	}
}

bool Socket::RecvAsync(const DataBuffer &data, const FnRecvCallback &callback, const StatePtr &state)
{
	if (-1 == _sock)
		throw logic_error("socket not bound yet");
	if (_listening)
		throw logic_error("socket accept only");
	if (_recving.exchange(true, memory_order_relaxed))
		throw logic_error("recv in progress");
	return RecvAttempt(true, data, CtxRecvCallback(callback, state));
}

bool Socket::RecvAttempt(const bool user, const DataBuffer &data, const CtxRecvCallback &ctx)
{
	int size(0), err(-1);
	do {
		DataBuffer::element_type::mutable_buffers_type buf = data->prepare(1500);
		size = ::recv(_sock, buffer_cast<void *>(buf), buffer_size(buf), 0);
		err = size < 0 ? errno : 0;
		LC_TRACE_DEBUG("Socket::RecvAsync[%%:L%%-R%%] | read %% bytes: %%",
				_sock, _localEP, _remoteEP, size, ::strerror(err));

		if (0 != err) {
			if (WouldBlock(err)) {
				shared_ptr<Socket> pin = CheckAlive(user);
				if (pin && EPollManager::Watch(EPM_READ, _sock,
						make_functor(&Socket::RecvReady, pin, true),
						make_shared< pair<DataBuffer, CtxRecvCallback> >(data, ctx)))
					return true;
			}
			if (RetryRecv(err))
				continue;
			size = 0;
		} else {
			_perfRecv->Increment();
		}
		data->commit(size);
		break;
	} while(true);

	RecvCallback(data, ctx);
	return false;
}

void Socket::RecvReady(const bool event, const int sock, const StatePtr &state)
{
	pair<DataBuffer, CtxRecvCallback>
		*ctx = reinterpret_cast<pair<DataBuffer, CtxRecvCallback> *>(state.get());
	if (event) {
		RecvAttempt(false, ctx->first, ctx->second);
	} else {
		RecvCallback(ctx->first, ctx->second);
	}
}

void Socket::RecvCallback(const DataBuffer &data, const CtxRecvCallback &ctx)
{
	_recving.store(false, memory_order_relaxed);

	if (ctx.callback().callable()) try {
		ctx.callback()(_ref.lock(), RecvEventArgs(data), ctx.state());
	} catch (const std::exception &e) {
		LC_TRACE_WARN("Socket::RecvCallback[%%:L%%-R%%] | error invoking callback",
			_sock, _localEP, _remoteEP, e);
	}
}

bool Socket::SendAsync(const DataBuffer &data, const FnSendCallback &callback, const StatePtr &state)
{
	if (-1 == _sock)
		throw logic_error("socket not bound yet");
	if (_listening)
		throw logic_error("socket accept only");

	bool attempt = false;
	{
		mutex::scoped_lock lock(_sending);
		if (_queue.empty()) {
			_queue.push_back();
			attempt = true;
		} else {
			_queue.push_back(make_pair(data, CtxSendCallback(callback, state)));
		}
		_perfQsnd->Increment();
	}
	return attempt ? SendAttempt(true, data, CtxSendCallback(callback, state)) : false;
}

bool Socket::SendAttempt(const bool user, const DataBuffer &data, const CtxSendCallback &ctx)
{
	int size(0), err(-1);
	DataBuffer::element_type::const_buffers_type buf = data->data();
	if (buffer_size(buf) > 0) do {
		size = ::send(_sock, buffer_cast<const void *>(buf), buffer_size(buf), 0);
		err = size < 0 ? errno : 0;
		LC_TRACE_DEBUG("Socket::SendAttempt[%%:L%%-R%%] | send %% bytes: %%",
				_sock, _localEP, _remoteEP, size, ::strerror(err));

		if (0 != err) {
			if (WouldBlock(err)) {
				shared_ptr<Socket> pin = CheckAlive(user);
				if (pin && EPollManager::Watch(EPM_WRITE, _sock,
						make_functor(&Socket::SendReady, pin, true),
						make_shared< pair<DataBuffer, CtxSendCallback> >(data, ctx)))
					return true;
			}
			if (RetrySend(err))
				continue;
			size = 0;
		} else {
			_perfSend->Increment();
		}
		data->consume(size);
		buf = data->data();
	} while (size > 0 && buffer_size(buf) > 0);

	SendCallback(data, ctx);
	return false;
}

void Socket::SendReady(const bool event, const int sock, const StatePtr &state)
{
	pair<DataBuffer, CtxSendCallback>
		*ctx = reinterpret_cast<pair<DataBuffer, CtxSendCallback> *>(state.get());
	if (event) {
		SendAttempt(false, ctx->first, ctx->second);
	} else {
		SendCallback(ctx->first, ctx->second);
	}
}

void Socket::SendCallback(const DataBuffer &data, const CtxSendCallback &ctx)
{
	if (ctx.callback().callable()) try {
		ctx.callback()(_ref.lock(), SendEventArgs(data), ctx.state());
	} catch (const std::exception &e) {
		LC_TRACE_WARN("Socket::SendCallback[%%:L%%-R%%] | error invoking callback",
			_sock, _localEP, _remoteEP, e);
	}

	bool attempt = false;
	pair<DataBuffer, CtxSendCallback> next;
	{
		mutex::scoped_lock lock(_sending);
		_queue.pop_front();
		_perfQsnd->Decrement();
		if (!_queue.empty()) {
			next = _queue.front();
			attempt = true;
		}
	}
	if (attempt) {
		SendAttempt(false, next.first, next.second);
	}
}

} }
