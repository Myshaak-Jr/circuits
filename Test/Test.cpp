#include "pch.h"
#include "CppUnitTest.h"

#include "../circuits/src/lingebra/lingebra.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

template <size_t N>
using ModInt = lingebra::ModInt<N>;

namespace Test {
	TEST_CLASS(TestModInt) {
	public:
		TEST_METHOD(TestInverse) {
			ModInt<7> a = 3;
			auto b = a.inverse();
			Assert::AreEqual((int8_t)5, b.get());
		}
	};
}
