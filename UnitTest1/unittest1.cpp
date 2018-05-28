#include "stdafx.h"
#include "CppUnitTest.h"

#include <optional>
#include <iostream>
#include <map>
#include <mutex>
#include <algorithm>
#include <any>
#include <experimental/filesystem>
#include <fstream>
#include <variant>
#include <shared_mutex>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// 
// https://blogs.msdn.microsoft.com/vcblog/2017/05/10/c17-features-in-vs-2017-3/
// http://www.modernescpp.com/

namespace cpp_17
{		
	struct ItemData
	{
		int Number{ 0 };
		int Index{ 0 };
		bool Value0{ false };
		bool Value1{ false };

		friend std::ostream& operator<<(std::ostream& out, const cpp_17::ItemData& t)
		{
			out << t.Number << "," << t.Index;
			return out;
		}
	};

	
	// c# �� ? ���� �ɼų�, ���� �������� �ִٸ� ǥ���Ҽ� �ְ� �Ǿ���.
	TEST_CLASS(Optionals)
	{
		TEST_METHOD(Test1)
		{
		struct ItemFactory {
			int Index{ 0 };

			bool IsValid() { return Index != 0; }

			std::optional<ItemData> Create()
			{
				if (!this->IsValid())
					return {};

				static int Uid = 0;
				ItemData out
				{
					++Uid, this->Index
				};

				return { out };
			}

		};
		ItemFactory Factory{ 1 };

		auto Item = Factory.Create();
		if (Item)
		{
			std::cout << L"has item - true\n";
		}

		if (Item.has_value())
		{
			std::cout << L"has item - true\n";
		}

		std::cout << Item.value() << std::endl;
		}
	};


	namespace AA::BB::CC
	{
		struct TEST {
			int a;
		};
	}
	// ��ø�� ���ӽ����̽��� �̷��Ե� ���� ǥ���� �����ϴ�(���ž�...) N4026
	TEST_CLASS(N4026)
	{
		TEST_METHOD(A)
		{
			AA::BB::CC::TEST T{ 1 };

			Assert::AreEqual(T.a, 1);
		}
	};

	// u8 ���ͷ� �߰�(utf8�̴�)
	TEST_CLASS(U8)
	{
		TEST_METHOD(A)
		{
			const char* A = u8"�ѱ�";
			Assert::AreEqual(A, u8"�ѱ�");
		}
	};

	// for each (�ν��Ϳ� for_each �� ���� range �� ����)
	TEST_CLASS(FOR_EACH)
	{
		TEST_METHOD(A)
		{
			std::string hello("Hello, world!");

			for each (char ch in hello)
			{
				std::cout << ch;
			}
		}
	};

	// ���� ������ ���ٿ� �����־�����.(��ȣ���� �ʴ� ������)
	TEST_CLASS(INIT_CONDITION)
	{
		std::map<int, std::string> m;
		std::mutex mx;
		bool shared_flag{ true };

		int demo()
		{
			if (auto it = m.find(10); it != m.end()) { return it->second.size(); }

			if (char buf[10]; std::fgets(buf, 10, stdin)) { m[0] += buf; }

			if (std::lock_guard<std::mutex> lock(mx); shared_flag) { shared_flag = false; }

			std::string s = "no";
			if (auto keywords = { "if", "for", "while" };
			std::any_of(keywords.begin(), keywords.end(), [&s](const char* kw) { return s == kw; }))
			{
				std::cout << "Token must not be a keyword";
			}			
		}
	};

	// emplace �� ����Ÿ���� void ���� reference�� �����
	TEST_CLASS(EMPLACE_RETURN_TYPE)
	{
		TEST_METHOD(A)
		{
			std::vector<int> V;
			V.emplace_back(1) = 2;

			Assert::AreEqual(V[0], 2);
		}
	};

	// const string& �� �Ἥ �޸� �Ҵ��� �ߴ� �κе��� string_view �� �� �ٲܼ� �ְ� ��
	// ���� ����
	TEST_CLASS(STRING_VIEW)
	{
		TEST_METHOD(A)
		{
			std::string_view View("�ѱ��̾�ȣ");

			Assert::AreEqual(View.data(), "�ѱ��̾�ȣ");
		}
	};
	
	// � ���̶� �޾���(��� �̰� �� variant ������)
	TEST_CLASS(ANY)
	{
		TEST_METHOD(A)
		{
			std::any any(2017);
			
			int myInt = std::any_cast<int>(any);
			
			Assert::IsTrue(any.type() == typeid(int));
		}
	};

	// any�� ����ѵ� �̰� � ���ؼ� �־������)
	TEST_CLASS(VARIANT)
	{
		TEST_METHOD(A)
		{
			std::variant<int, float> v, w;
			v = 12;                              // v contains int
			int i = std::get<int>(v);
			w = std::get<int>(v);
			w = std::get<0>(v);
			w = v;
			try {
				std::get<float>(w);
			}
			catch (std::bad_variant_access&) {}

			std::variant<std::string> s("abc");
			//s = "def";                         
			// s = 1;
		}
	};
	namespace fs = std::experimental::filesystem;
	// boost �� filesystem
	// path, directory, 
	TEST_CLASS(FILESYSTEM)
	{
		TEST_METHOD(A)
		{
			std::cout << "Current path: " << fs::current_path() << std::endl;

			std::string dir = "sandbox/a/b";
			fs::create_directories(dir);

			std::ofstream("sandbox/file1.txt");
			fs::path symPath = fs::current_path() /= "sandbox";
			symPath /= "syma";
			fs::create_symlink("a", "symPath");

			std::cout << "fs::is_directory(dir): " << fs::is_directory(dir) << std::endl;
			std::cout << "fs::exists(symPath): " << fs::exists(symPath) << std::endl;
			std::cout << "fs::symlink(symPath): " << fs::is_symlink(symPath) << std::endl;

			for (auto& p : fs::recursive_directory_iterator("sandbox"))
				std::cout << p << std::endl;

			fs::remove_all("sandbox");

		}
	};

}