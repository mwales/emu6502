#include<iostream>

#include<stdio.h>
#include<stdlib.h>

int main(int argc, char* argv[])
{
   if (argc != 2)
   {
      std::cerr << "Usage error" << std::endl;
      std::cerr << "Usage: " << argv[0] << " file.bin address" << std::endl;
      return 1;
   }




   return 0;
}

