/*	Copyright 2016 (c) Michael Thomas (malinka) <malinka@entropy-development.com>
	Distributed under the terms of the GNU Affero General Public License v3
*/

#include "Buffer.hh"
#include <gtest/gtest.h>

#include <cstring>
#include <cwchar>

using namespace Entropy::Asio;
using namespace testing;
using namespace std;

namespace {
	TEST(Buffer, Create) {
		const char *data = "Hello World!";
		size_t len = 12;

		char *temp = new char[1024];
		memcpy(temp, data, len);

		// 2016-10-05 AMR NOTE: not a \0
		temp[13] = 'A';

		Buffer<> buffer(len, 1024, temp);

		EXPECT_EQ(buffer.size(), len);
		EXPECT_EQ(buffer.capacity(), 1024);
		EXPECT_EQ(buffer.data(), temp);
		EXPECT_EQ(memcmp(temp, data, len), 0);
		EXPECT_NE(strcmp(data, temp), 0);
	}

	TEST(Buffer, WCreate) {
		const wchar_t *data = L"Hello World!";
		size_t len = 12;

		wchar_t *temp = new wchar_t[1024];
		memcpy(temp, data, len);

		// 2016-10-05 AMR NOTE: not a \0
		temp[13] = 'A';

		Buffer<wchar_t> buffer(len, 1024, temp);

		EXPECT_EQ(buffer.size(), len);
		EXPECT_EQ(buffer.capacity(), 1024);
		EXPECT_EQ(buffer.data(), temp);
		EXPECT_EQ(memcmp(temp, data, len), 0);
		EXPECT_NE(wcscmp(data, temp), 0);
	}

	TEST(Buffer, Iterate) {
		const char *data = "Hello World!";
		size_t len = strlen(data);

		char *temp = new char[1024];
		strcpy(temp, data);

		Buffer<> buffer(1024, len, temp);

		size_t x = 0;
		for(auto &&c : buffer) {
			EXPECT_EQ(c, temp[x]);
			EXPECT_EQ(c, data[x]);
			++x;

			if(c == '!')
				c = '?';
		}

		EXPECT_EQ(strcmp(buffer.data(), temp), 0);
		EXPECT_NE(strcmp(buffer.data(), data), 0);
	}

	TEST(Buffer, Construct) {
		Buffer<> buff(1024);

		EXPECT_EQ(buff.size(), 0);
		EXPECT_EQ(buff.capacity(), 1024);
		EXPECT_NE(buff.data(), nullptr);

		Buffer<> nbuff(move(buff));

		EXPECT_EQ(buff.data(), nullptr);
		EXPECT_NE(nbuff.data(), nullptr);
	}

	TEST(Buffer, Write) {
		Buffer<> buff(1024);

		strcpy(buff.data(), "Hello World!");
		buff.size() = 12;

		EXPECT_EQ(buff.size(), 12);
		EXPECT_EQ(memcmp(buff.data(), "Hello World!", buff.size()), 0);
		EXPECT_EQ(buff.capacity(), 1024);
	}
}
