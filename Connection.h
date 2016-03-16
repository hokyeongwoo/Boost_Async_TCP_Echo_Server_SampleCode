#pragma once

#include <iostream>
#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

// 날짜시간 메세지를 만든다. 
std::string make_daytime_string()
{
	using namespace std; // For time_t, time and ctime; 
	time_t now = time(0);
	return ctime(&now); // ctime_s 권장. 예제니까 그냥 쓰고 넘어가지만 
}

class CConnection : public boost::enable_shared_from_this<CConnection>
{
private:
	enum EEnum
	{
		eBuffSize = 128,
	};

	tcp::socket m_Socket;
	int m_nID;

	boost::array<BYTE, eBuffSize> m_RecvBuf;
	std::string m_sMessage;

	CConnection(boost::asio::io_service& io) : m_Socket(io), m_nID(-1)
	{
	}

	void handle_Accept(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/)
	{
	}

	void handle_Wait_Recv(const boost::system::error_code& error, size_t /*bytes_transferred*/)
	{
		if(!error)	// 0 이 성공 나머지는 오류 플러그
		{
			std::cout << "Waiting..." << std::endl;
			m_Socket.async_read_some(boost::asio::buffer(m_RecvBuf),
				boost::BOOST_BIND(&CConnection::handle_Send, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}
		else
		{
			std::cout << m_nID << " Disconnect(Read) : " << error.message() << std::endl;
		}
	}

	void handle_Send(const boost::system::error_code& error, size_t /*bytes_transferred*/)
	{
		if(!error)	// 0이 성공 나머지는 오류 플러그
		{
			// 선 처리 후
			if(m_RecvBuf.size())
			{
				std::cout << "Recv : " << (*(char*)(m_RecvBuf.data())) << std::endl;
				m_RecvBuf.assign(NULL); // 버퍼 초기화
			}

			if(true)	// 보낼게 있으면 보내고
			{
				boost::asio::async_write(m_Socket, boost::asio::buffer("handle_read", 32),
					boost::BOOST_BIND(&CConnection::handle_Wait_Recv, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
				std::cout << "Send " << std::endl;
			}
			else	// 없으면 말고
			{
				boost::array<BYTE, eBuffSize> buf;
				m_Socket.async_read_some(boost::asio::buffer(buf),
					boost::BOOST_BIND(&CConnection::handle_Send, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
			}
		}
		else
		{
			std::cout << m_nID << " Disconnect(Write) : " << error.message() << std::endl;
		}
	}

public:
	typedef boost::shared_ptr<CConnection> pointer;

	static pointer create(boost::asio::io_service& io)
	{
		return pointer(new CConnection(io));
	}

	tcp::socket& socket()
	{
		return m_Socket;
	}

	void start(int nID)
	{
		std::cout << "new connect id : " << nID << " ::: Welcom!" << std::endl;
		m_nID = nID;
		m_sMessage = make_daytime_string();

		boost::asio::async_write(m_Socket, boost::asio::buffer(m_sMessage),
			boost::BOOST_BIND(&CConnection::handle_Accept, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));

		boost::array<BYTE, eBuffSize> buf;
		m_Socket.async_receive(boost::asio::buffer(buf),
			boost::BOOST_BIND(&CConnection::handle_Send, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}
};