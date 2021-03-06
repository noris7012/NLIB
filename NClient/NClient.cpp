﻿// NClient.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <thread>
#include <chrono>

#include "GameConfig.h"
#include "NClientHandler.h"
#include "GameClientFactory.h"

int main()
{
	auto handler = new NClientHandler();
	auto client = GameClientFactory::Create(handler);

	GameConfig config;
	config.host = "127.0.0.1";
	config.port = 7171;

	client->Connect(config);

	while (!client->IsConnected())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(3000));

	for (int i = 0; i < 5; ++i)
	{
		client->WritePacket(const_cast<char*>("data01"), 7);
		client->WritePacket(const_cast<char*>("data02"), 7);
		client->WritePacket(const_cast<char*>("data03"), 7);
		std::this_thread::sleep_for(std::chrono::milliseconds(500));		
	}

	// for (int i = 0; i < 1; ++i)
	// {
	// 	client->WritePacket((const byte*)"data01", 7);
	// 	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	// 	client->WritePacket((const byte*)"data02", 7);
	// 	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	// 	client->WritePacket((const byte*)"data03", 7);
	// 	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	// 	client->WritePacket((const byte*)"data04", 7);
	// 	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	// 	client->WritePacket((const byte*)"data05", 7);
	// 	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	// 	client->WritePacket((const byte*)"data06", 7);
	// 	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	// 	client->WritePacket((const byte*)"data07", 7);
	// 	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	// 	client->WritePacket((const byte*)"data08", 7);
	// 	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	// 	client->WritePacket((const byte*)"data09", 7);
	// 	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	// 	client->WritePacket((const byte*)"data10", 7);
	// 	std::this_thread::sleep_for(std::chrono::milliseconds(100));		
	// }

	// std::stringstream ss;
	// for (int i = 1; i <= 800; ++i)
	// {
	// 	if (i < 10)
	// 	{
	// 		ss << "00" << i;
	// 	}
	// 	else if (i < 100)
	// 	{
	// 		ss << "0" << i;
	// 	}
	// 	else
	// 	{
	// 		ss << i;
	// 	}
	// }
	//
	// auto str = ss.str();
	// auto cstr = str.c_str();
	// auto length = str.length() + 1;
	// client->WritePacket(reinterpret_cast<const byte*>(cstr), length);

	std::this_thread::sleep_for(std::chrono::milliseconds(10000));

	client->Disconnect();

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10000));
	}
}

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
