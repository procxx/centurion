#include "platform.hpp"

#include <fff.h>
#include <gtest/gtest.h>

extern "C" {
FAKE_VALUE_FUNC(const char*, SDL_GetPlatform)
}

class PlatformTest : public testing::Test
{
 public:
 protected:
  void SetUp() override
  {
    RESET_FAKE(SDL_GetPlatform);
  }
};

TEST_F(PlatformTest, ID)
{
  SDL_GetPlatform_fake.return_val = "Windows";
  EXPECT_EQ(cen::platform::platform_id::windows, cen::platform::id());

  SDL_GetPlatform_fake.return_val = "Mac OS X";
  EXPECT_EQ(cen::platform::platform_id::mac_osx, cen::platform::id());

  SDL_GetPlatform_fake.return_val = "Linux";
  EXPECT_EQ(cen::platform::platform_id::linuxx, cen::platform::id());

  SDL_GetPlatform_fake.return_val = "iOS";
  EXPECT_EQ(cen::platform::platform_id::ios, cen::platform::id());

  SDL_GetPlatform_fake.return_val = "Android";
  EXPECT_EQ(cen::platform::platform_id::android, cen::platform::id());

  SDL_GetPlatform_fake.return_val = "foobar";
  EXPECT_EQ(cen::platform::platform_id::unknown, cen::platform::id());
}

TEST_F(PlatformTest, IsWindows)
{
  SDL_GetPlatform_fake.return_val = "Windows";
  EXPECT_TRUE(cen::platform::is_windows());
}

TEST_F(PlatformTest, IsMacOSX)
{
  SDL_GetPlatform_fake.return_val = "Mac OS X";
  EXPECT_TRUE(cen::platform::is_mac_osx());
}

TEST_F(PlatformTest, IsLinux)
{
  SDL_GetPlatform_fake.return_val = "Linux";
  EXPECT_TRUE(cen::platform::is_linux());
}

TEST_F(PlatformTest, IsIOS)
{
  SDL_GetPlatform_fake.return_val = "iOS";
  EXPECT_TRUE(cen::platform::is_ios());
}

TEST_F(PlatformTest, IsAndroid)
{
  SDL_GetPlatform_fake.return_val = "Android";
  EXPECT_TRUE(cen::platform::is_android());
}

TEST_F(PlatformTest, Name)
{
  SDL_GetPlatform_fake.return_val = "Windows";
  EXPECT_EQ("Windows", cen::platform::name().value());

  SDL_GetPlatform_fake.return_val = "Mac OS X";
  EXPECT_EQ("Mac OS X", cen::platform::name().value());

  SDL_GetPlatform_fake.return_val = "Linux";
  EXPECT_EQ("Linux", cen::platform::name().value());

  SDL_GetPlatform_fake.return_val = "iOS";
  EXPECT_EQ("iOS", cen::platform::name().value());

  SDL_GetPlatform_fake.return_val = "Android";
  EXPECT_EQ("Android", cen::platform::name().value());

  SDL_GetPlatform_fake.return_val = "Unknown";
  EXPECT_FALSE(cen::platform::name().has_value());
}