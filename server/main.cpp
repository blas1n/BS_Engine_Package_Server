#include "TCP.h"
#include <fstream>

struct Protocol
{
	std::byte id;
	char[123]
};

void OnAccept(Socket socket, const std::byte* buffer)
{
	char name[123];

	if (!std::to_integer<bool>(buffer[0])) {
		strncpy(name, reinterpret_cast<const char*>(buffer) + 1, 122);
		std::ifstream in{ name };
		
		if (!in.is_open())
			socket.Send("1");
	}
}

int main()
{
	TCP tcp{ "config.ini" };
	tcp.SetOnAccept(OnAccept);
	tcp.Run();
	return 0;
}