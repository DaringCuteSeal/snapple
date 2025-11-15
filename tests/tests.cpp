#include <gtest/gtest.h>
#include "../src/backend/components.hpp"

using namespace GameComponents;

double fake_time() {
	return 10.0;
}

TEST(Components, TimerUpdate) {
	Timer timer(fake_time);
	timer.attach(1.0, [](){});
	timer.attach(2.0, [](){});
	timer.attach(-1.0, [](){});
	timer.attach(0.0, [](){});
	timer.attach(3.0, [](){});
	timer.attach(4.0, [](){});
	timer.attach(0.0, [](){});
	timer.attach(5.0, [](){});
	timer.update();

	EXPECT_EQ(timer.timers.size(), 5);
	EXPECT_EQ(timer.timers[0].expire, 11.0);
	EXPECT_EQ(timer.timers[1].expire, 12.0);
	EXPECT_EQ(timer.timers[2].expire, 13.0);
	EXPECT_EQ(timer.timers[3].expire, 14.0);
	EXPECT_EQ(timer.timers[4].expire, 15.0);
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
