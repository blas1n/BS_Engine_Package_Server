#include "TCP.h"
#include <fstream>

struct Protocol
{
	std::byte id;
	char[123]
};

void OnAccept(Socket socket, std::byte* buffer)
{
	if (!std::to_integer<bool>(buffer[0])) {
		buffer <<= 1;
	}
}

int main()
{
	TCP tcp{ "config.ini" };
	tcp.GetBufferSize();

	tcp.SetOnAccept(OnAccept);
	tcp.Run();
	return 0;
}