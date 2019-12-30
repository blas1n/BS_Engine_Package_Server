#include "TCP.h"
#include <bitset>
#include <fstream>

constexpr auto BUFFER_SIZE = 1024;

enum class State : ServWork::byte
{
	Normal,
	Exit,
	Exception
};

void SetState(ServWork::Buffer& buf, State state)
{
	std::bitset<128> bitSet = static_cast<ServWork::byte>(state);
	std::string binaryStr = bitSet.to_string();
	buf.Set(0, binaryStr.c_str());
}

void GetFile(ServWork::Socket& socket, const std::string& name, ServWork::Buffer& buf)
{
	std::ifstream in{ name.c_str(), std::ios::binary };

	if (!in.is_open())
	{
		SetState(buf, State::Exception);
		buf.Set(2, "File not found.");
		socket.Send(buf);
		return;
	}

	char tmp[BUFFER_SIZE - 2];

	while (!in.eof())
	{
		SetState(buf, State::Normal);
		in.read(tmp, BUFFER_SIZE - 3);
		buf.Set(2, tmp);
		socket.Send(buf);
		buf.Init();
	}

	SetState(buf, State::Exit);
	socket.Send(buf);
}

void SetFile(ServWork::Socket& socket, const std::string& name, ServWork::Buffer& buf)
{
	std::ofstream out{ name.c_str(), std::ios::binary };

	while (true)
	{
		int len = socket.Recv(buf);
		out << buf;
		if (len < 1) break;
		buf.Init();
	}
}

void OnAccept(ServWork::Socket socket)
{
	ServWork::Buffer buf{ BUFFER_SIZE };
	socket.Recv(buf);

	auto id = buf[0];
	auto name = std::string{ buf.Get(1) };
	buf.Init();

	using Func = void(*)(ServWork::Socket&, const std::string&, ServWork::Buffer&);
	Func func[] = { GetFile, SetFile };

	func[id](socket, name, buf);
}

int main()
{
	ServWork::TCP tcp{ "config.ini" };
	tcp.SetOnAccept(OnAccept);
	tcp.Run();
	return 0;
}