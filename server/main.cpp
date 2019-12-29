#include "TCP.h"
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
	switch (state)
	{
	case State::Normal:
		buf.Set(0, "00");
		break;

	case State::Exit:
		buf.Set(0, "01");
		break;

	case State::Exception:
		buf.Set(0, "10");
		break;
	}
}

void OnAccept(ServWork::Socket socket)
{
	char tmp[BUFFER_SIZE - 2];
	ServWork::Buffer buf{ BUFFER_SIZE };
	socket.Recv(buf);

	auto name = std::string{ buf.Get(1) };
	buf.Init();

	if (buf[0])
	{
		std::ifstream in{ name.c_str(), std::ios::binary };

		if (!in.is_open())
		{
			SetState(buf, State::Exception);
			buf.Set(2, "File not found.");
			socket.Send(buf);
			return;
		}

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
	else
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
}

int main()
{
	ServWork::TCP tcp{ "config.ini" };
	tcp.SetOnAccept(OnAccept);
	tcp.Run();
	return 0;
}