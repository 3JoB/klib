#include <unistd.h>

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include <catch2/catch.hpp>

#include "klib/util.h"

TEST_CASE("ChangeWorkingDir", "[util]") {
  REQUIRE(!std::filesystem::exists("work-dir"));

  {
    klib::ChangeWorkingDir dir("work-dir");
    REQUIRE(std::filesystem::exists("../work-dir"));
  }

  REQUIRE(std::filesystem::remove("work-dir"));
}

TEST_CASE("read_file & write_file", "[util]") {
  REQUIRE(std::filesystem::exists("zlib-v1.2.11.tar.gz"));

  auto data = klib::read_file("zlib-v1.2.11.tar.gz", true);
  REQUIRE(std::size(data) == 644596);

  REQUIRE_NOTHROW(klib::write_file("write-file.zip", true, data));

  REQUIRE(std::filesystem::exists("write-file.zip"));
  REQUIRE(std::filesystem::file_size("write-file.zip") == 644596);

  std::filesystem::remove("write-file.zip");
}

TEST_CASE("utf8_to_utf16", "[util]") {
  auto utf16 = klib::utf8_to_utf16("zß水🍌");

  REQUIRE(std::size(utf16) == 5);
  REQUIRE(utf16[0] == 0x007A);
  REQUIRE(utf16[1] == 0x00DF);
  REQUIRE(utf16[2] == 0x6C34);
  REQUIRE(utf16[3] == 0xD83C);
  REQUIRE(utf16[4] == 0xDF4C);
}

TEST_CASE("utf8_to_utf32", "[util]") {
  auto utf32 = klib::utf8_to_utf32("zß水🍌");

  REQUIRE(std::size(utf32) == 4);
  REQUIRE(utf32[0] == 0x0000007A);
  REQUIRE(utf32[1] == 0x000000DF);
  REQUIRE(utf32[2] == 0x00006C34);
  REQUIRE(utf32[3] == 0x0001F34C);
}

TEST_CASE("utf32_to_utf8", "[util]") {
  auto utf32 = klib::utf8_to_utf32("书客");
  auto utf8 = klib::utf32_to_utf8(utf32);

  REQUIRE(std::size(utf8) == 6);
  REQUIRE(static_cast<std::uint8_t>(utf8[0]) == 0xE4);
  REQUIRE(static_cast<std::uint8_t>(utf8[1]) == 0xB9);
  REQUIRE(static_cast<std::uint8_t>(utf8[2]) == 0xA6);
  REQUIRE(static_cast<std::uint8_t>(utf8[3]) == 0xE5);
  REQUIRE(static_cast<std::uint8_t>(utf8[4]) == 0xAE);
  REQUIRE(static_cast<std::uint8_t>(utf8[5]) == 0xA2);
}

TEST_CASE("is_ascii", "[util]") {
  REQUIRE(klib::is_ascii('A'));
  REQUIRE_FALSE(klib::is_ascii(static_cast<char>(190)));

  REQUIRE(klib::is_ascii("AAA"));
  REQUIRE_FALSE(klib::is_ascii("你"));

  REQUIRE(klib::is_ascii(klib::utf8_to_utf32("AAA")));
  REQUIRE_FALSE(klib::is_ascii(klib::utf8_to_utf32("你")));
}

TEST_CASE("is_chinese", "[util]") {
  REQUIRE(klib::is_chinese("你"));
  REQUIRE_FALSE(klib::is_chinese("a"));
  REQUIRE_FALSE(klib::is_chinese("🍌"));
}

TEST_CASE("base64_encode", "[util]") {
  CHECK(klib::base64_encode("hello") == "aGVsbG8=");
  CHECK(klib::base64_encode("Online Tools") == "T25saW5lIFRvb2xz");
  CHECK(klib::base64_encode(
            "How to resolve the \"EVP_DecryptFInal_ex: bad decrypt\"") ==
        "SG93IHRvIHJlc29sdmUgdGhlICJFVlBfRGVjcnlwdEZJbmFsX2V4OiBiYWQgZGVjcnlwdC"
        "I=");
}

TEST_CASE("base64_decode", "[util]") {
  CHECK(klib::base64_decode("aGVsbG8=") == "hello");
  CHECK(klib::base64_decode("T25saW5lIFRvb2xz") == "Online Tools");
  CHECK(klib::base64_decode("SG93IHRvIHJlc29sdmUgdGhlICJFVlBfRGVjcnlwdEZJbmFsX2"
                            "V4OiBiYWQgZGVjcnlwdCI=") ==
        "How to resolve the \"EVP_DecryptFInal_ex: bad decrypt\"");
}

TEST_CASE("sha_256_file", "[util]") {
  REQUIRE(std::filesystem::exists("zlib-v1.2.11.tar.gz"));

  REQUIRE(klib::sha_256_file("zlib-v1.2.11.tar.gz") ==
          "143df9ab483578ce7a1019b96aaa10f6e1ebc64b1a3d97fa14f4b4e4e7ec95e7");
}

TEST_CASE("sha3_512_file", "[util]") {
  REQUIRE(std::filesystem::exists("zlib-v1.2.11.tar.gz"));

  REQUIRE(klib::sha3_512_file("zlib-v1.2.11.tar.gz") ==
          "38af19362e48ec80f6565cf18245f520c8ee5348374cb0c11286f3b23cc93fd05a6a"
          "2a2b8784f20bb2307211a2a776241797857b133056f4b33de1d363db7bb2");
}

TEST_CASE("aes_256_cbc_encrypt", "[util]") {
  std::vector<std::uint8_t> iv;
  iv.resize(16, 0);

  REQUIRE(
      klib::base64_encode(klib::aes_256_cbc_encrypt(
          "{\"code\":\"100000\",\"data\":{\"login_token\":"
          "\"06d3b540ecde7843d79fa0c790b4c968\",\"user_code\":"
          "\"9827638bc3c6ae0a43174f2a2d25d35b\",\"reader_info\":{\"reader_id\":"
          "\"9986391\",\"account\":\"\\u4e66\\u5ba287999639162\",\"is_bind\":"
          "\"1\","
          "\"is_bind_qq\":\"0\",\"is_bind_weixin\":\"0\",\"is_bind_huawei\":"
          "\"0\","
          "\"is_bind_apple\":\"0\",\"phone_num\":\"15041557811\",\"phone_"
          "crypto\":"
          "\"HOcCgi\\/"
          "crmKmnAKvlSoZbQ==\",\"mobileVal\":\"1\",\"email\":\"\",\"license\":"
          "\"\","
          "\"reader_name\":\"\\u4e66\\u5ba287999639162\",\"avatar_url\":\"\","
          "\"avatar_thumb_url\":\"\",\"base_status\":\"1\",\"exp_lv\":\"4\","
          "\"exp_"
          "value\":\"697\",\"gender\":\"1\",\"vip_lv\":\"0\",\"vip_value\":"
          "\"0\","
          "\"is_author\":\"0\",\"is_uploader\":\"0\",\"book_age\":\"1\","
          "\"category_"
          "prefer\":[],\"used_decoration\":[{\"decoration_type\":\"1\","
          "\"decoration_url\":\"https:\\/\\/app.hbooker.com\\/resources\\/"
          "image\\/"
          "decoration\\/"
          "pendant_82.png\",\"decoration_id\":\"82\",\"reader_decoration_id\":"
          "\"2631577\"}],\"rank\":\"0\",\"ctime\":\"2021-06-04 "
          "16:43:55\"},\"prop_info\":{\"rest_gift_hlb\":\"10\",\"rest_hlb\":"
          "\"10\","
          "\"rest_yp\":\"0\",\"rest_recommend\":\"2\",\"rest_total_blade\":"
          "\"0\","
          "\"rest_month_blade\":\"0\",\"rest_total_100\":\"0\",\"rest_total_"
          "588\":"
          "\"0\",\"rest_total_1688\":\"0\",\"rest_total_5000\":\"0\",\"rest_"
          "total_"
          "10000\":\"0\",\"rest_total_100000\":\"0\",\"rest_total_50000\":"
          "\"0\","
          "\"rest_total_160000\":\"0\"},\"is_set_young\":\"0\"}}",
          klib::sha_256_raw("zG2nSeEfSHfvTCHy5LCcqtBbQehKNLXn"), iv)) ==
      "IT+LcNazRBcK54/"
      "p1lMtcyRwpZ01VQ4tFr6GBslpnwMezmEBbIYc3GokHiTGB6XV/"
      "I3oWkrqLXB3DTQJUIlvLRRHe2GmNqGS8xHXeyq8BgBLCBxLcIFRtK+V6/"
      "Y1ovce7ie8h4t59PU0nHYbr8Lh7gq0yZ3DRd5oJ8go4QPgtTTPdfXCWIGMIbW"
      "ot0HwcYIGiCxE7RcvSEBdti5MBTmVHeF81cQYmB9SOPnxt4KOjH557P2Y6Pya"
      "nzcVO2BKpGvCgcVSHmkwA4xX1JjH8zjZ7miuFmnNKDrDacmx3AcxxwbtJpog+"
      "loA/b4hnDny3oCKkriy0eHkc5atGAwg/"
      "HFkArM2wUXZtUHuRWekImGlapISnp1fVpgvf2WKf5ENt8/"
      "LJUY8kX56wE8tea6feekOO1U1fiQyGHf4yceoeZTXfKHs/NXb9/"
      "YbStpsSaMvLujmbR0j2sBpMLyAqUAZF/1tEMlo7od/"
      "SS+AXj+T7R7daLFAKHLfb+gTqB/"
      "WffPV0SeG7dcDLKL7dvno0MEBvDP1RKBG9ACxWHAGq94GlBQcimH9xNAmAGeQ"
      "uyh9R7TmWVtAgSfW1q1tIpU22yytgXW3grdl0e1kqFr61PxXPiKEwFBlSUA/"
      "OJgFswAOayx6aXVdazd05w7QcXyexzka6DPKjMUi/"
      "Cw88uk6ovHmSffnx7hzoFz7qcGWO5rHuILxIin1HJjbNOqb/"
      "7IEnsjIUJxTp4V3ula9DcR3VUbSDFFo0oV98SNTSbSe8MBpTX5AREIN4Yr5SS"
      "vUji7Mm0EjlwOgIgzgcXZgCEGQty3aI1mj6luaCxWAe4YoJhJrr2ZHZxWPgjC"
      "we7dOSGWQHGAEEF9RiqX81yAqay13uYThwgMwIAxzA4TrevdoTgDuo/"
      "L2dvoVQ3UItVrAGFBZhFL6Xhi0izYjpXbpVWg0lo9M/"
      "rgBdysSYGrDTYXq0wVmEhJaPhCw9bwKqCK/"
      "uf1Ad+bqktuwCouikVmIfTJxV8qlBeoklFtrUuS93Zmyzq5Cj2CDnEH6+"
      "8j0ypE/"
      "wVnq1UuIPEtn9xDfVXSoxm8QcSAJ728jrRLn7g7QWCrB+"
      "eGvCmVqzo31GaF6I642OhFTyuLUmsLuR8aONaMOueuiFQL/"
      "dxdeP+KILutwIH0Mtg8jS0bWUlN14+"
      "LJLzuqlE3j7Cz6HV29CI9ri35WTDvn1TKqF0rama5bRt5Jp+"
      "WRrXXEELbTIwjkaNfY9bBMM75B8ewIaOpOWXPaAI1vLejz6r5aSAsz26bGMWZ"
      "J3A8s+OtcaqeIcTTtjB9N/+qVGyViyOJb/"
      "YHS4cYH+"
      "pCj2wZR4s1m3TrdLNSw5VGBBPzLUQtBMSungUNS7knFBHbyQMwtoD8tRsHdGC"
      "DCoiAbWOCwUQFeld9MpcuDGRE93gUo8Wz4GHjsm8V+"
      "WrsD7sW5bRxngbdrUSiGJhbkHh+I+rMqNDkEGGZ5eGM/"
      "s3ZbTuChHMCAh75IY1an6E2QJddEOBRfszEtWcGcy+u9ACV/"
      "hXhhg2mw2KUT2QnyJ9en9jPkjEOd04V8ja3Aoqk3chTTQJksU8D/"
      "XLOyDgEkWNaX5I4WgTtkOeLyEQVg2yzrAwjgayCXWj71JDe4");
}

TEST_CASE("aes_256_cbc_decrypt", "[util]") {
  std::vector<std::uint8_t> iv;
  iv.resize(16, 0);

  REQUIRE(
      klib::aes_256_cbc_decrypt(
          klib::base64_decode(
              "IT+LcNazRBcK54/"
              "p1lMtcyRwpZ01VQ4tFr6GBslpnwMezmEBbIYc3GokHiTGB6XV/"
              "I3oWkrqLXB3DTQJUIlvLRRHe2GmNqGS8xHXeyq8BgBLCBxLcIFRtK+V6/"
              "Y1ovce7ie8h4t59PU0nHYbr8Lh7gq0yZ3DRd5oJ8go4QPgtTTPdfXCWIGMIbW"
              "ot0HwcYIGiCxE7RcvSEBdti5MBTmVHeF81cQYmB9SOPnxt4KOjH557P2Y6Pya"
              "nzcVO2BKpGvCgcVSHmkwA4xX1JjH8zjZ7miuFmnNKDrDacmx3AcxxwbtJpog+"
              "loA/b4hnDny3oCKkriy0eHkc5atGAwg/"
              "HFkArM2wUXZtUHuRWekImGlapISnp1fVpgvf2WKf5ENt8/"
              "LJUY8kX56wE8tea6feekOO1U1fiQyGHf4yceoeZTXfKHs/NXb9/"
              "YbStpsSaMvLujmbR0j2sBpMLyAqUAZF/1tEMlo7od/"
              "SS+AXj+T7R7daLFAKHLfb+gTqB/"
              "WffPV0SeG7dcDLKL7dvno0MEBvDP1RKBG9ACxWHAGq94GlBQcimH9xNAmAGeQ"
              "uyh9R7TmWVtAgSfW1q1tIpU22yytgXW3grdl0e1kqFr61PxXPiKEwFBlSUA/"
              "OJgFswAOayx6aXVdazd05w7QcXyexzka6DPKjMUi/"
              "Cw88uk6ovHmSffnx7hzoFz7qcGWO5rHuILxIin1HJjbNOqb/"
              "7IEnsjIUJxTp4V3ula9DcR3VUbSDFFo0oV98SNTSbSe8MBpTX5AREIN4Yr5SS"
              "vUji7Mm0EjlwOgIgzgcXZgCEGQty3aI1mj6luaCxWAe4YoJhJrr2ZHZxWPgjC"
              "we7dOSGWQHGAEEF9RiqX81yAqay13uYThwgMwIAxzA4TrevdoTgDuo/"
              "L2dvoVQ3UItVrAGFBZhFL6Xhi0izYjpXbpVWg0lo9M/"
              "rgBdysSYGrDTYXq0wVmEhJaPhCw9bwKqCK/"
              "uf1Ad+bqktuwCouikVmIfTJxV8qlBeoklFtrUuS93Zmyzq5Cj2CDnEH6+"
              "8j0ypE/"
              "wVnq1UuIPEtn9xDfVXSoxm8QcSAJ728jrRLn7g7QWCrB+"
              "eGvCmVqzo31GaF6I642OhFTyuLUmsLuR8aONaMOueuiFQL/"
              "dxdeP+KILutwIH0Mtg8jS0bWUlN14+"
              "LJLzuqlE3j7Cz6HV29CI9ri35WTDvn1TKqF0rama5bRt5Jp+"
              "WRrXXEELbTIwjkaNfY9bBMM75B8ewIaOpOWXPaAI1vLejz6r5aSAsz26bGMWZ"
              "J3A8s+OtcaqeIcTTtjB9N/+qVGyViyOJb/"
              "YHS4cYH+"
              "pCj2wZR4s1m3TrdLNSw5VGBBPzLUQtBMSungUNS7knFBHbyQMwtoD8tRsHdGC"
              "DCoiAbWOCwUQFeld9MpcuDGRE93gUo8Wz4GHjsm8V+"
              "WrsD7sW5bRxngbdrUSiGJhbkHh+I+rMqNDkEGGZ5eGM/"
              "s3ZbTuChHMCAh75IY1an6E2QJddEOBRfszEtWcGcy+u9ACV/"
              "hXhhg2mw2KUT2QnyJ9en9jPkjEOd04V8ja3Aoqk3chTTQJksU8D/"
              "XLOyDgEkWNaX5I4WgTtkOeLyEQVg2yzrAwjgayCXWj71JDe4"),
          klib::sha_256_raw("zG2nSeEfSHfvTCHy5LCcqtBbQehKNLXn"), iv) ==
      "{\"code\":\"100000\",\"data\":{\"login_token\":"
      "\"06d3b540ecde7843d79fa0c790b4c968\",\"user_code\":"
      "\"9827638bc3c6ae0a43174f2a2d25d35b\",\"reader_info\":{\"reader_id\":"
      "\"9986391\",\"account\":\"\\u4e66\\u5ba287999639162\",\"is_bind\":\"1\","
      "\"is_bind_qq\":\"0\",\"is_bind_weixin\":\"0\",\"is_bind_huawei\":\"0\","
      "\"is_bind_apple\":\"0\",\"phone_num\":\"15041557811\",\"phone_crypto\":"
      "\"HOcCgi\\/"
      "crmKmnAKvlSoZbQ==\",\"mobileVal\":\"1\",\"email\":\"\",\"license\":\"\","
      "\"reader_name\":\"\\u4e66\\u5ba287999639162\",\"avatar_url\":\"\","
      "\"avatar_thumb_url\":\"\",\"base_status\":\"1\",\"exp_lv\":\"4\",\"exp_"
      "value\":\"697\",\"gender\":\"1\",\"vip_lv\":\"0\",\"vip_value\":\"0\","
      "\"is_author\":\"0\",\"is_uploader\":\"0\",\"book_age\":\"1\",\"category_"
      "prefer\":[],\"used_decoration\":[{\"decoration_type\":\"1\","
      "\"decoration_url\":\"https:\\/\\/app.hbooker.com\\/resources\\/image\\/"
      "decoration\\/"
      "pendant_82.png\",\"decoration_id\":\"82\",\"reader_decoration_id\":"
      "\"2631577\"}],\"rank\":\"0\",\"ctime\":\"2021-06-04 "
      "16:43:55\"},\"prop_info\":{\"rest_gift_hlb\":\"10\",\"rest_hlb\":\"10\","
      "\"rest_yp\":\"0\",\"rest_recommend\":\"2\",\"rest_total_blade\":\"0\","
      "\"rest_month_blade\":\"0\",\"rest_total_100\":\"0\",\"rest_total_588\":"
      "\"0\",\"rest_total_1688\":\"0\",\"rest_total_5000\":\"0\",\"rest_total_"
      "10000\":\"0\",\"rest_total_100000\":\"0\",\"rest_total_50000\":\"0\","
      "\"rest_total_160000\":\"0\"},\"is_set_young\":\"0\"}}");
}

TEST_CASE("folder_size", "[util]") {
  REQUIRE(std::filesystem::exists("folder1"));
  REQUIRE(klib::folder_size("folder1") == 38);
}

TEST_CASE("same_folder", "[util]") {
  REQUIRE(std::filesystem::exists("folder1"));
  REQUIRE(std::filesystem::exists("folder2"));

  REQUIRE(klib::same_folder("folder1", "folder2"));
}

TEST_CASE("execute_command", "[util]") {
  std::string command = "gcc -v";
  REQUIRE_NOTHROW(klib::execute_command(command));
}

TEST_CASE("wait_for_child_process", "[util]") {
  for (std::int32_t i = 0; i < 3; ++i) {
    auto pid = fork();
    REQUIRE(pid >= 0);

    if (pid == 0) {
      std::ofstream ofs(std::to_string(i) + ".txt");
      ofs << std::to_string(i) << std::flush;

      std::exit(EXIT_SUCCESS);
    }
  }

  klib::wait_for_child_process();
  for (std::int32_t i = 0; i < 3; ++i) {
    auto file_name = std::to_string(i) + ".txt";

    REQUIRE(std::filesystem::exists(file_name));
    REQUIRE(klib::read_file(file_name, false) == std::to_string(i));
    std::filesystem::remove(file_name);
  }
}

TEST_CASE("splicing_url", "[util]") {
  REQUIRE(klib::splicing_url("www.example.com",
                             {{"a", "b"}, {"c", "书客123abc"}}) ==
          "www.example.com?a=b&c=%E4%B9%A6%E5%AE%A2123abc");
}
