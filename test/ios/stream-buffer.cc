/*	Copyright 2016 (c) Michael Thomas (malinka) <malinka@entropy-development.com>
	Distributed under the terms of the GNU Affero General Public License v3
*/

#include "../Mock.hh"
#include <gtest/gtest.h>

#include <cstring>

using namespace Entropy::Tethys;
using namespace testing;
using namespace std;

namespace {
	TEST(StreamBuffer, Create) {
		MockStream<> mock;
	}

	TEST(StreamBuffer, ReadOne) {
		MockStream<> mock;
		const char *data =
			"Hello World!\n"		//12
			"Goodbye World!\n"		//27
			"ThisIsALongString"		//44
			"ThatWillDefinitely"	//62
			"BeOnMultipleBuffers"	//81
		;
		Buffer<> buff(256);
		strcpy(buff.data(), data);
		buff.size() = strlen(data);
		mock.Read(move(buff));

		string hello,goodbye,rest;
		string chunk(17, 0);
		getline(mock, hello);
		EXPECT_EQ(mock.tellg(), 13);
		getline(mock, goodbye);
		EXPECT_EQ(mock.tellg(), 28);
		mock.read(chunk.data(), 17);
		EXPECT_EQ(mock.tellg(), 45);
		mock >> rest;
		EXPECT_EQ(mock.tellg(), -1);

		EXPECT_EQ(hello, "Hello World!"s);
		EXPECT_EQ(goodbye, "Goodbye World!"s);
		EXPECT_EQ(chunk, "ThisIsALongString"s);
		EXPECT_EQ(rest, "ThatWillDefinitelyBeOnMultipleBuffers"s);
	}

	TEST(StreamBuffer, Read) {
		MockStream<> mock;
		const char *data[] = {
			"Hello World!\n",		//12
			"Goodbye World!\n",		//27
			"ThisIsALongString",	//44
			"ThatWillDefinitely",	//62
			"BeOnMultipleBuffers"	//81
		};

		for(auto x = 0; x < 5; x++) {
			Buffer<> buff(256);
			strcpy(buff.data(), data[x]);
			buff.size() = strlen(data[x]);
			mock.Read(move(buff));
		}

		string hello,goodbye,rest;
		string chunk(17, 0);
		getline(mock, hello);
		EXPECT_EQ(mock.tellg(), 13);
		getline(mock, goodbye);
		EXPECT_EQ(mock.tellg(), 28);
		mock.read(chunk.data(), 17);
		EXPECT_EQ(mock.tellg(), 45);
		mock >> rest;
		EXPECT_EQ(mock.tellg(), -1);

		EXPECT_EQ(hello, "Hello World!"s);
		EXPECT_EQ(goodbye, "Goodbye World!"s);
		EXPECT_EQ(chunk, "ThisIsALongString"s);
		EXPECT_EQ(rest, "ThatWillDefinitelyBeOnMultipleBuffers"s);
	}

	TEST(StreamBuffer, Write) {
		MockStream<> mock;

		mock << "Hello Stream!" << endl;
		EXPECT_EQ(mock.Data(), "Hello Stream!\n"s);

		mock << "This is also some data, hopefully will be buffered";
		EXPECT_EQ(mock.Data(), "Hello Stream!\n"s);
		mock << flush;
		EXPECT_EQ(mock.Data(), "Hello Stream!\nThis is also some data, hopefully will be buffered"s);
	}

	TEST(StreamBuffer, Seek) {
		MockStream<> mock;
		const char *data[] = {
			"Hello World!\n",
			"Goodbye World!\n",		// 13
			"ThisIsALongString",	// 28
			"ThatWillDefinitely",	// 45
			"BeOnMultipleBuffers"	// 63
		};

		for(auto x = 0; x < 5; x++) {
			Buffer<> buff(256);
			strcpy(buff.data(), data[x]);
			buff.size() = strlen(data[x]);
			mock.Read(move(buff));
		}

		mock.seekg(0);
		EXPECT_EQ(static_cast<char>(mock.peek()), 'H');
		mock.seekg(6);
		EXPECT_EQ(static_cast<char>(mock.peek()), 'W');
		mock.seekg(0);
		EXPECT_EQ(static_cast<char>(mock.peek()), 'H');
		mock.seekg(6, ios_base::cur);
		EXPECT_EQ(static_cast<char>(mock.peek()), 'W');
		mock.seekg(-6, ios_base::cur);
		EXPECT_EQ(static_cast<char>(mock.peek()), 'H');
		mock.seekg(-1, ios_base::end);
		EXPECT_EQ(static_cast<char>(mock.peek()), 's');
		mock.seekg(0);
		EXPECT_EQ(static_cast<char>(mock.peek()), 'H');
		mock.seekg(13);
		EXPECT_EQ(static_cast<char>(mock.peek()), 'G');
		mock.seekg(45);
		EXPECT_EQ(static_cast<char>(mock.peek()), 'T');
		mock.seekg(0);
		EXPECT_EQ(static_cast<char>(mock.peek()), 'H');

		string hello,goodbye,rest;
		getline(mock, hello);
		getline(mock, goodbye);
		mock >> rest;

		EXPECT_EQ(hello, "Hello World!"s);
		EXPECT_EQ(goodbye, "Goodbye World!"s);
		EXPECT_EQ(rest, "ThisIsALongStringThatWillDefinitelyBeOnMultipleBuffers"s);
	}

	// 2017-06-27 AMR TODO: test utf-32, maybe utf-16
}
