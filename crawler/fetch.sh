#!/bin/sh
for id in 85632 53339; do
  printf "" > /tmp/mccme
  for page in {1..40}; do
    phantomjs submissions.js $id $page >> /tmp/mccme 2>&1 
  done
  cat /tmp/mccme | python3 compare.py
done
