#pragma once
#include <memory>
#include <asio.hpp>
using asio::ip::tcp;


class Session : public std::enable_shared_from_this<Session> {
private:
	std::shared_ptr<tcp::socket> socket;
	std::string read_msg;
	std::queue<std::shared_ptr<std::string>> writeQueue;
	std::mutex writeMutex;
	bool sending;
public:
	Session(std::shared_ptr<tcp::socket> _socket);
	void start();
	void push_WriteQueue(std::shared_ptr<std::string> msg);
	std::shared_ptr<tcp::socket> get_socket() const { return socket; }

private:
	friend class SessionManager;

	void do_read();
	void do_write();

	bool isValid(const std::string& packet);
	void Close();

};