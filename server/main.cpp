#include "TCP.h"
#include <fstream>

static char name[123];
static char buf[1024];

enum class State : unsigned char
{
	Normal,
	Exit,
	Exception
};

void SetState(char* buf, State state)
{
	switch (state)
	{
	case State::Normal:
		buf[0] = '0';
		buf[1] = '0';
		break;

	case State::Exit:
		buf[0] = '0';
		buf[1] = '1';
		break;

	case State::Exception:
		buf[0] = '1';
		buf[1] = '0';
		break;
	}
}

void OnAccept(ServWork::Socket socket, const std::byte* buffer)
{
	if (!std::to_integer<bool>(buffer[0])) {
		strncpy(name, reinterpret_cast<const char*>(buffer) + 1, 122);
		std::ifstream in{ name };

		if (!in.is_open())
		{
			SetState(buf, State::Exception);
			strncpy(buf + 2, "File not found.", 16);
			socket.Send(buf);
			return;
		}

		while (!in.eof())
		{
			SetState(buf, State::Normal);
			in.read(buf + 2, 1022);
			socket.Send(buf);
		}

		SetState(buf, State::Exit);
		socket.Send(buf);
	}
}

int main()
{
	ServWork::TCP tcp{ "config.ini" };
	tcp.SetOnAccept(OnAccept);
	tcp.Run();
	return 0;
}