#ifndef MSVC_CACHE_REDIS_HELPER_H
#define MSVC_CACHE_REDIS_HELPER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <boost/smart_ptr.hpp>
#include <hiredis/hiredis.h>

namespace msvc { namespace cache {

typedef boost::shared_ptr<redisReply> redis_reply_ptr;
typedef boost::shared_ptr<redisContext> redis_conn_ptr;

class RedisHelper {
public:
	inline static void Call(const redis_conn_ptr &conn, const std::string &cmd) {
		redisAppendCommand(conn.get(), cmd.c_str());
	}

	inline static void Call(const redis_conn_ptr &conn, const std::string &cmd, const int arg) {
		redisAppendCommand(conn.get(), (cmd + " %d").c_str(), arg);
	}

	inline static void Call(const redis_conn_ptr &conn,	const std::string &cmd, const std::string &arg) {
		redisAppendCommand(conn.get(), (cmd + " %b").c_str(), arg.c_str(), arg.size());
	};

	inline static void Call(const redis_conn_ptr &conn,	const std::string &cmd,
			const std::string &key, const int arg) {
		redisAppendCommand(conn.get(), (cmd + " %b %d").c_str(), key.c_str(), key.size(), arg);
	};

	inline static void Call(const redis_conn_ptr &conn,	const std::string &cmd,
			const std::string &key, const std::string &arg) {
		redisAppendCommand(conn.get(), (cmd + " %b %b").c_str(), key.c_str(), key.size(), arg.c_str(), arg.size());
	};

	inline static void Call(const redis_conn_ptr &conn,	const std::string &cmd,
			const std::string &key, const int arg1, const std::string &arg2) {
		redisAppendCommand(conn.get(), (cmd + " %b %d %b").c_str(), key.c_str(), key.size(), arg1, arg2.c_str(), arg2.size());
	};

	inline static void Call(const redis_conn_ptr &conn,	const std::string &cmd,
			const std::string &key, const std::string &arg1, const int arg2) {
		redisAppendCommand(conn.get(), (cmd + " %b %d %b").c_str(), key.c_str(), key.size(), arg1.c_str(), arg1.size(), arg2);
	};

	inline static void Call(const redis_conn_ptr &conn,	const std::string &cmd,
			const std::string &key, const std::string &arg1, const std::string &arg2) {
		redisAppendCommand(conn.get(), (cmd + " %b %b %b").c_str(), key.c_str(), key.size(), arg1.c_str(), arg1.size(), arg2.c_str(), arg2.size());
	};

	inline static void Call(const redis_conn_ptr &conn, const std::string &cmd,	const std::vector<std::string> &args) {
		int argc = 1 + args.size();
		boost::scoped_array<const char *> argv(new const char *[argc]); 
		boost::scoped_array<size_t> argl(new size_t[argc]);
		argv[0] = cmd.c_str(); argl[0] = cmd.size();
		for (int i = 0; i < args.size(); ++i) {
			argv[i + 1] = args[i].c_str(); argl[i + 1] = args[i].size();
		}
		redisAppendCommandArgv(conn.get(), argc, argv.get(), argl.get());
	}

	inline static void Call(const redis_conn_ptr &conn, const std::string &cmd,	const std::string &key, const std::vector<std::string> &args) {
		int argc = 2 + args.size();
		boost::scoped_array<const char *> argv(new const char *[argc]); 
		boost::scoped_array<size_t> argl(new size_t[argc]);
		argv[0] = cmd.c_str(); argl[0] = cmd.size();
		argv[1] = key.c_str(); argl[1] = key.size();
		for (int i = 0; i < args.size(); ++i) {
			argv[i + 2] = args[i].c_str(); argl[i + 2] = args[i].size();
		}
		redisAppendCommandArgv(conn.get(), argc, argv.get(), argl.get());
	}

	inline static void Call(const redis_conn_ptr &conn, const std::string &cmd, const std::map<std::string, std::string> &args) {
		int argc = 1 + args.size() * 2;
		boost::scoped_array<const char *> argv(new const char *[argc]); 
		boost::scoped_array<size_t> argl(new size_t[argc]);
		int i = 0; argv[i] = cmd.c_str(); argl[i++] = cmd.size();
		for (std::map<std::string, std::string>::const_iterator
				it = args.begin(); it != args.end(); ++it) {
			argv[i] = it->first.c_str(); argl[i++] = it->first.size();
			argv[i] = it->second.c_str(); argl[i++] = it->second.size();
		}
		redisAppendCommandArgv(conn.get(), argc, argv.get(), argl.get());
	}

	inline static redis_reply_ptr Reply(const redis_conn_ptr &conn) {
		redisReply *rpl = 0;
		redisGetReply(conn.get(), reinterpret_cast<void **>(&rpl));
		return Reply(rpl);
	}

private:
	inline static redis_reply_ptr Reply(redisReply *reply) {
		return redis_reply_ptr(reply, &_FreeReplyObject);
	}
	inline static void _FreeReplyObject(redisReply *reply) {
		if (reply) freeReplyObject(reply);
	}

private:
	RedisHelper();
};

}}

#endif
