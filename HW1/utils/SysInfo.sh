#!/bin/bash
# Title: Displaying system information using shell scripting
# Author: Nachiket Kelkar
#Purpose: For Advanced Embedded Software Development class Homework assignment 1

echo "System Information using shell scripting"
echo "1. User Information"
users
echo ""

echo "2. Operating system Type/Brand"
uname
lsb_release -a
echo ""

echo "3. OS Distribution"
lsb_release -d
echo ""

echo "4. OS Version"
lsb_release -r
echo ""

echo "5. Kernel Version"
uname -r
echo ""

echo "6. Kernel gcc version build"
gcc --version
echo ""

echo "7. Kernel build time"
uname -v
echo ""

echo "8. System Architecture Information"
uname -i
echo ""

echo "9. Information on file system memory"
df -H
echo ""

