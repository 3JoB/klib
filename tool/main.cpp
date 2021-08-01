#include <fstream>
#include <iostream>

#include "klib/archive.h"

int main() {
  klib::archive::compress("error.tar.gz", klib::archive::Algorithm::Gzip, true);
}
