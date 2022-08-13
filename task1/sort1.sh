#!/bin/bash
# LC_ALL=C forces applications such as awk to use the default language for output and sorting to be byte-wise
LC_ALL=C awk 'length($0) >= 3 && length($0) <= 15' ../input.txt | awk '/^[a-zA-Z]+$/' | sort -k 1.3 -s | uniq > output-bash.txt
